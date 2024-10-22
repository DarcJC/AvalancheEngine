import re
import random
import string
from functools import cached_property
from typing import Optional, Literal, Annotated
from pathlib import Path

import clang.cindex
import tomli
from pydantic import BaseModel, Field

from .hash import FNV1a


class GeneratorConfig(BaseModel):
    default_factory: Annotated[Optional[str], Field(None)]


class CommonBase:
    decl_cursor: clang.cindex.Cursor

    def __init__(self, cursor_of_decl: clang.cindex.Cursor):
        self.decl_cursor = cursor_of_decl

    @cached_property
    def type(self) -> clang.cindex.Type:
        return self.decl_cursor.type.get_canonical()

    @cached_property
    def metadata(self) -> Optional[dict]:
        if self.decl_cursor.raw_comment is None:
            return None
        return parse_comment(self.decl_cursor.raw_comment)

    @cached_property
    def namespace(self) -> str:
        return extract_namespace(self.type.spelling)[0]

    @cached_property
    def fully_qualified_name(self) -> str:
        return self.type.spelling

    @cached_property
    def display_name(self) -> str:
        return self.decl_cursor.spelling

    @cached_property
    def camel_case_name(self):
        fullname = self.fully_qualified_name
        # split by '::'
        parts = fullname.split('::')
        # Capitalizing the first char
        capitalized_parts = [part.capitalize() for part in parts]
        # Join parts
        camel_case_string = ''.join(capitalized_parts)
        return camel_case_string

    @cached_property
    def metaclass_name(self) -> str:
        return f'{self.camel_case_name}MetaClass__internal__'

    @cached_property
    def metastorage_name(self) -> str:
        return f"{self.metaclass_name}__MetaStorage"

    @cached_property
    def type_hash(self) -> int:
        return FNV1a().hash_64_fnv1a(self.fully_qualified_name)

    @cached_property
    def current_type_spelling(self) -> str:
        return self.decl_cursor.type.spelling

    @cached_property
    def access_specifier(self) -> Literal['invalid', 'public', 'protected', 'private', 'none']:
        if self.decl_cursor.access_specifier == clang.cindex.AccessSpecifier.INVALID:
            return 'invalid'
        if self.decl_cursor.access_specifier == clang.cindex.AccessSpecifier.PUBLIC:
            return 'public'
        if self.decl_cursor.access_specifier == clang.cindex.AccessSpecifier.PROTECTED:
            return 'protected'
        if self.decl_cursor.access_specifier == clang.cindex.AccessSpecifier.PRIVATE:
            return 'private'
        if self.decl_cursor.access_specifier == clang.cindex.AccessSpecifier.NONE:
            return 'none'

    @cached_property
    def source_location_text(self) -> str:
        location: clang.cindex.SourceLocation = self.decl_cursor.location
        return f"{location.file}({location.line}:{location.column})"


class Method(CommonBase):
    parent_class: 'Class'

    def __init__(self, cursor_of_decl: clang.cindex.Cursor, parent_class: 'Class'):
        super().__init__(cursor_of_decl)
        self.parent_class = parent_class

    @cached_property
    def return_type(self) -> clang.cindex.Type:
        return self.decl_cursor.result_type

    @cached_property
    def params(self) -> list[clang.cindex.Cursor]:
        result = []
        for child in self.decl_cursor.get_children():
            if child.kind == clang.cindex.CursorKind.PARM_DECL:
                result.append(child)
        return result

    @cached_property
    def param_typenames(self) -> list[str]:
        return [p.type.get_canonical().spelling for p in self.params]

    @cached_property
    def metaclass_name(self) -> str:
        return f'{self.parent_class.camel_case_name}_of_{self.display_name}MetaMethod__internal__'

    def __repr__(self) -> str:
        return f"{'static ' if self.decl_cursor.is_static_method() else ''}{self.return_type.spelling}({', '.join([param.type.spelling for param in self.params])})"


class ClassField(CommonBase):
    parent_class: 'Class'

    def __init__(self, cursor_of_decl: clang.cindex.Cursor, parent_class: 'Class'):
        super().__init__(cursor_of_decl)
        self.parent_class = parent_class

    @cached_property
    def metaclass_name(self) -> str:
        return f'{self.parent_class.camel_case_name}_of_{self.display_name}MetaField__internal__'


class Class(CommonBase):
    def __init__(self, cursor_of_decl: clang.cindex.Cursor):
        super().__init__(cursor_of_decl)

    @cached_property
    def kind(self) -> Literal['class', 'struct', 'union']:
        prefix: Literal['class', 'struct', 'union'] = "class"
        if self.decl_cursor.kind == clang.cindex.CursorKind.CLASS_DECL:
            prefix = "class"
        elif self.decl_cursor.kind == clang.cindex.CursorKind.STRUCT_DECL:
            prefix = "struct"
        elif self.decl_cursor.kind == clang.cindex.CursorKind.UNION_DECL:
            prefix = "union"
        return prefix

    @cached_property
    def base_classes(self) -> set[clang.cindex.Cursor]:
        result = []
        for child in self.decl_cursor.get_children():
            if child.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER:
                result.append(child)
        return set(result)

    @cached_property
    def base_classes_flatten(self) -> set[clang.cindex.Cursor]:
        result = []
        for child in self.decl_cursor.get_children():
            if child.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER:
                result.append(child)
                result.extend(get_base_classes_of_type(child))
        return set(result)

    @cached_property
    def derived_from_object(self) -> bool:
        return 'avalanche::Object' in [cursor.type.get_canonical().spelling for cursor in self.base_classes_flatten]

    @cached_property
    def fields(self) -> list[ClassField]:
        result = []
        for cursor in self.decl_cursor.get_children():
            cursor: clang.cindex.Cursor
            if cursor.kind == clang.cindex.CursorKind.FIELD_DECL:
                result.append(ClassField(cursor, self))
        return result

    @cached_property
    def public_fields(self) -> list[ClassField]:
        return [field for field in self.fields if field.access_specifier == 'public' and field.metadata is not None]

    @cached_property
    def methods(self) -> list[Method]:
        result = []
        for cursor in self.decl_cursor.get_children():
            cursor: clang.cindex.Cursor
            if cursor.kind == clang.cindex.CursorKind.CXX_METHOD:
                result.append(Method(cursor, self))
        return result

    @cached_property
    def public_methods(self) -> list[Method]:
        return [method for method in self.methods if method.access_specifier == 'public' and method.metadata is not None]


class CxxHeaderFileProcessor:
    index: clang.cindex.Index
    translation_unit: clang.cindex.TranslationUnit

    _include_paths: list[str]
    _filepath: str
    _out_header: str
    _out_source: str
    _random_id: str
    _classes: list[Class]

    _registered_classes: list[Class]

    def __init__(self, filepath: str, include_paths: list[str]):
        self._random_id = ''.join(random.choices(string.ascii_letters, k=16))
        self.index = clang.cindex.Index.create()
        self._filepath = filepath
        self._include_paths = include_paths
        self._out_header = f'''#pragma once
#if !defined(DURING_BUILD_TOOL_PROCESS)
#pragma warning (disable: 4244)
\n#include <array>\n#include <tuple>
#include "class.h"\n#include "metaspace.h"\n#include "field.h"\n#include "method.h"\n#include "dynamic_container.h"
#include "polyfill.h"
#include "container/vector.hpp"\n#include "container/shared_ptr.hpp"\n#include "container/unique_ptr.hpp"\n
'''
        self._out_source = f'#include <cassert>\n#include "{self._filepath}"\n\nusing namespace avalanche;'
        self._classes = []
        self._registered_classes = []

    @cached_property
    def processing_file(self):
        return Path(self._filepath).resolve()

    def parse(self):
        with open(file=self._filepath, mode='rb') as f:
            source_content = f.read().decode(encoding='utf-8')
        clang_args = ['-x', 'c++', '-std=c++23', '-Wno-pragma-once-outside-header', '-DDURING_BUILD_TOOL_PROCESS=1']
        # Remove empty item
        self._include_paths.remove('')
        for path in self._include_paths:
            clang_args.append(f'-I{path}')
            self.translation_unit = clang.cindex.TranslationUnit.from_source(self._filepath, args=clang_args, unsaved_files=[( self._filepath, source_content )], options=clang.cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES, index=self.index)
        self.traverse_ast_and_process(self.translation_unit.cursor)
        for clazz in self._classes:
            self.generate_class_info(clazz)
        self.append_to_source(self.generate_metaspace_storage())

    def save_outputs(self, out_header_path: str, out_source_path: str):
        self._out_header = f'{self._out_header}\n#pragma warning (default: 4244)\n#endif // !defined(DURING_BUILD_TOOL_PROCESS)\n'
        self._out_source = f'#include "{out_header_path}"\n{self._out_source}\n'
        with open(out_header_path, "w") as f:
            f.truncate()
            f.write(self._out_header)

        with open(out_source_path, "w") as f:
            f.truncate()
            f.write(self._out_source)

    def append_to_header(self, text: str):
        self._out_header += text

    def append_to_source(self, text: str):
        self._out_source += text

    def traverse_ast_and_process(self, current_node: clang.cindex.Cursor, depth: int = 0):
        for child in current_node.get_children():
            self.traverse_ast_and_process(child, depth=depth+1)

        if current_node.kind != clang.cindex.CursorKind.CLASS_DECL and current_node.kind != clang.cindex.CursorKind.STRUCT_DECL:
            return

        if not current_node.is_definition():
            return

        source_location: clang.cindex.SourceLocation = current_node.location
        current_file_path = Path(source_location.file.name).resolve()

        if current_file_path != self.processing_file:
            return

        current_class = Class(current_node)
        self._classes.append(current_class)

    def generate_class_info(self, current_class: Class):
        if current_class.metadata is None and not current_class.derived_from_object:
            return

        self.append_to_header(generate_constant_class_name(current_class))
        self.append_to_source(generate_metadata_struct(current_class, current_class))
        self.append_to_source(generate_metaclass(current_class))
        self._registered_classes.append(current_class)

    def generate_metaspace_storage(self):
        template = f'''
avalanche::MetaSpaceProxy {self._random_id}_create_metaspace_internal__() {{
    auto result = avalanche::MetaSpace::get().create();
    {'\n\t'.join([f'result->register_class(new {clazz.metaclass_name}());' for clazz in self._registered_classes])}
    return result;
}}
static avalanche::MetaSpaceProxy G_{self._random_id}_METASPACE_ = {self._random_id}_create_metaspace_internal__();
'''
        return template


BASE_TYPE_MAPS = {
    int: 'int32_t',
    float: 'float',
    bool: 'bool'
}


def extract_namespace(spelling: str) -> (str, str):
    last_double_colon = spelling.rfind("::")
    if last_double_colon != -1:
        return spelling[:last_double_colon], spelling[last_double_colon+2:]
    return "", spelling


def flatten_dict(d, parent_key='', sep='.'):
    items = []
    for k, v in d.items():
        new_key = f"{parent_key}{sep}{k}" if parent_key else k
        if isinstance(v, dict):
            items.extend(flatten_dict(v, new_key, sep=sep).items())
        else:
            items.append((new_key, v))
    return dict(items)


def generate_metadata_container(value: any) -> str:
    result = ""
    value_type = type(value)
    if value_type is int or value_type is float:
        result += f"static const {BASE_TYPE_MAPS[value_type]} s_ = {value};"
    elif value_type is bool:
        result += f"static const {BASE_TYPE_MAPS[value_type]} s_ = {'true' if value else 'false'};"
    elif isinstance(value, list):
        result += f'static const std::string_view s_ = "{';'.join(map(lambda x: str(x), value))}";'
    elif isinstance(value, str):
        result += f'static const std::string_view s_ = "{value}";'

    return result


def generate_metadata_struct(current_class: CommonBase, parent_class: Class) -> str:
    metadata = current_class.metadata or {}
    template = f"""
namespace avalanche::generated {{
    struct {current_class.metastorage_name} : public IMetadataKeyValueStorage {{
        
        [[nodiscard]] Class* get_declaring_class() const override {{
            return Class::for_name(class_name_v<{parent_class.fully_qualified_name}>);
        }}
        
        void keys(size_t& o_num_keys, std::string_view const* & o_keys) const override {{
            {
            f'''
            static constexpr std::string_view svs[] = {{ 
                {',\n\t\t\t\t'.join([f'std::string_view("{key}")' for key in metadata.keys()])}
            }};\n'''
            if len(metadata) != 0 else "static constexpr std::string_view* svs = nullptr;\n"
            }
            o_num_keys = { len(metadata) };
            o_keys = svs;
        }}
        
        [[nodiscard]] const DynamicContainerBase* get(std::string_view key) const override {{
            { 
            ' '.join([f'''if (key == "{key}") {{ 
                {generate_metadata_container(metadata[key])}
                static GenericDynamicContainer<decltype(s_)> result(s_);
                return &result;
            }}''' for key in metadata.keys()])
            }
            return nullptr;
        }}
        
    }};
}} // namespace avalanche::generated
    """

    return template


def generate_constant_class_name(current_class: Class) -> str:
    namespace, class_name = extract_namespace(spelling=current_class.fully_qualified_name)
    template = f"""
namespace {namespace} {{
    {current_class.kind} {class_name};
}} // namespace {namespace}
template <>
struct avalanche::class_name<{current_class.fully_qualified_name}> {{
    static constexpr const char* value = "{current_class.fully_qualified_name}";
    static constexpr bool primitive = false;
}};
"""
    return template


def parse_comment(raw_comment: str) -> Optional[dict]:
    matches = re.search(r'@avalanche::begin(.*?)@avalanche::end', raw_comment, re.DOTALL)
    if not matches:
        if '@reflect' not in raw_comment:
            return None
        return {}
    toml_text = matches.group(1).replace("///", "").strip()
    return flatten_dict(tomli.loads(toml_text))


def get_base_classes_of_type(cursor: clang.cindex.Cursor) -> list[clang.cindex.Cursor]:
    if not cursor:
        return []

    result = []
    for child in cursor.get_children():
        if child.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER:
            result.append(child)
            result.extend(get_base_classes_of_type(child))
        elif child.kind == clang.cindex.CursorKind.TEMPLATE_REF:
            definition_cursor: clang.cindex.Cursor = child.get_definition()
            for tchild in definition_cursor.get_children():
                if tchild.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER:
                    result.append(tchild)
                    result.extend(get_base_classes_of_type(tchild))
    return result


def generate_fields_class(current_class: Class) -> str:
    result = ""
    for field in current_class.fields:
        if field.metadata is None:
            continue
        if field.access_specifier != 'public':
            raise ValueError(f'{field.source_location_text}: error: Field "{field.display_name}" access specifier expected "public", found "{field.access_specifier}".')
        result += f"""
class {field.metaclass_name} : public avalanche::Field {{
public:
    Chimera get(Chimera object) const override {{
        assert(*get_declaring_class() == *object.get_class()); // Invalid instance type
        auto* obj = static_cast<{current_class.fully_qualified_name}*>(object.memory());
        Class* clazz = Class::for_name(class_name_v<std::remove_pointer_t<std::decay_t<{field.fully_qualified_name}>>>);
        ScopedStruct* container = new FieldProxyStruct(&obj->{field.display_name}, clazz);
        return Chimera {{ container, true }};
    }}
    
    [[nodiscard]] Class* get_declaring_class() const override {{
        return Class::for_name(class_name_v<{current_class.fully_qualified_name}>);
    }}
    
    [[nodiscard]] const char* get_name() const override {{
        return "{field.display_name}";
    }}
    
    [[nodiscard]] const IMetadataKeyValueStorage* get_metadata() const override {{
        static avalanche::generated::{field.metastorage_name} s{{}};
        return &s;
    }}
}};
"""

    return result

def generate_methods_class(current_class: Class) -> str:
    result = ""
    for method in current_class.methods:
        if method.metadata is None:
            continue
        if method.access_specifier != 'public':
            raise ValueError(f'{method.source_location_text}: error: Method "{method.display_name}" access specifier expected "public", found "{method.access_specifier}".')
        result += f"""
class {method.metaclass_name} : public avalanche::Method {{
public:
    [[nodiscard]] Class* get_declaring_class() const override {{
        return Class::for_name(class_name_v<{current_class.fully_qualified_name}>);
    }}
    
    [[nodiscard]] const char* get_name() const override {{
        return "{method.display_name}";
    }}
    
    [[nodiscard]] uint64_t arg_hash() const override {{
        return arg_package_hash_v<{",".join([f"std::remove_cvref_t<{name}>" for name in method.param_typenames])}>;
    }}
    
    [[nodiscard]] const IMetadataKeyValueStorage* get_metadata() const override {{
        static avalanche::generated::{method.metastorage_name} s{{}};
        return &s;
    }}
}};
"""
    return result

def generate_metaclass(current_class: Class) -> str:
    template = f"""
{"".join([generate_metadata_struct(field, current_class) for field in current_class.public_fields])}
{"".join([generate_metadata_struct(method, current_class) for method in current_class.public_methods])}

{generate_fields_class(current_class)}

{generate_methods_class(current_class)}

class {current_class.metaclass_name} : public avalanche::Class {{
public:
    std::string_view full_name() const override {{
        return full_name_str();
    }}
    
    const std::string& full_name_str() const override {{
        static std::string value("{current_class.fully_qualified_name}");
        return value;
    }}
    
    size_t hash() const override {{
        return {current_class.type_hash}ULL;
    }}
    
    void base_classes(int32_t& num_result, const char* const*& out_data) const override {{
        constexpr int32_t num_base_classes = {len(current_class.base_classes_flatten)};
        { f'''static constexpr const char*{ f""" base_classes_name[] {{
            {',\n\t\t\t'.join([f'"{base_class.type.get_canonical().spelling}"' for base_class in current_class.base_classes_flatten])}
        }}""" if current_class.base_classes_flatten else '* base_classes_name = nullptr'};''' }
        num_result = num_base_classes;
        out_data = base_classes_name;
    }}
    
    [[nodiscard]] bool is_derived_from_object() const override {{
        return {'true' if current_class.derived_from_object else 'false'};
    }}
    
    void fields(int32_t& num_result, const avalanche::Field* const*& out_data) const override {{
        constexpr int32_t num_fields = {len(current_class.public_fields)};
        {'\n\t\t'.join([f'static {field.metaclass_name} {field.metaclass_name}_inst{{}};' for field in current_class.public_fields])}
        { f"""static constexpr const Field*{ f""" fields[] {{
            {',\n\t\t\t'.join([f'&{field.metaclass_name}_inst' for field in current_class.public_fields])}
        }}""" if current_class.public_fields else "* fields = nullptr" };"""}
        num_result = num_fields;
        out_data = fields;
    }}
    
    void methods(int32_t& num_result, const Method* const*& out_data) const override {{
        constexpr int32_t num_methods = {len(current_class.public_methods)};
        {'\n\t\t'.join([f'static {method.metaclass_name} {method.metaclass_name}_inst{{}};' for method in current_class.public_methods])}
        { f"""static constexpr const Method*{ f""" methods[] {{
            {',\n\t\t\t'.join([f'&{method.metaclass_name}_inst' for method in current_class.public_methods])}
        }}""" if current_class.public_methods else "* methods = nullptr" };"""}
        num_result = num_methods;
        out_data = methods;
    }}
    
    [[nodiscard]] const IMetadataKeyValueStorage* get_metadata() const override {{
        static avalanche::generated::{current_class.metastorage_name} s{{}};
        return &s;
    }}
}};
"""
    return template
