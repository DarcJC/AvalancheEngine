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


class Method:
    pass


class Class:
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
    def type_hash(self) -> int:
        return FNV1a().hash_64_fnv1a(self.fully_qualified_name)


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
#pragma warning (disable: 4244)
\n#include <array>\n#include <tuple>
#include "class.h"\n#include "metaspace.h"\n#include "polyfill.h"
#include "container/vector.hpp"\n#include "container/shared_ptr.hpp"\n#include "container/unique_ptr.hpp"\n
'''
        self._out_source = f'#include "{self._filepath}"'
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
        self._out_header = f'{self._out_header}\n#pragma warning (default: 4244)'
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
        self.append_to_header(generate_metadata_struct(current_class))
        self.append_to_source(generate_metaclass(current_class))
        self._registered_classes.append(current_class)

    def generate_metaspace_storage(self):
        template = f'''
avalanche::MetaSpaceProxy {self._random_id}_create_metaspace_internal__() {{
    auto result = avalanche::MetaSpace::get().create();
    {';'.join([f'result->register_class(new {clazz.metaclass_name}());' for clazz in self._registered_classes])}
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


def generate_fields(metadata: Optional[dict]) -> str:
    metadata = metadata or {}
    result = ""
    for (k, v) in metadata.items():
        value_type = type(v)
        if value_type is int or value_type is float:
            result += f"{BASE_TYPE_MAPS[value_type]} {k} = {v};\n"
        elif value_type is bool:
            result += f"{BASE_TYPE_MAPS[value_type]} {k} = {'true' if v else 'false'};\n"
        elif isinstance(v, dict):
            result += (f'struct {{\n'
                       f'   {generate_fields(v)}'
                       f'}} {k} {{}};\n')
        elif isinstance(v, list):
            result += f'std::tuple<{",".join(map(lambda x: BASE_TYPE_MAPS[type(x)], v))}> {k} = std::make_tuple({",".join(map(lambda x: str(int(x)) if type(x) == bool else str(x), v))});\n'
        elif isinstance(v, str):
            result += f'const char* {k} = "{v}";\n'

    return result


def generate_metadata_struct(current_class: Class) -> str:
    template = f"""
namespace avalanche::generated {{
    struct {current_class.display_name}MetadataType : metadata_tag {{
        {generate_fields(current_class.metadata)}
        
        [[nodiscard]] Class* get_declaring_class() const override {{
            return Class::for_name(class_name_v<{current_class.fully_qualified_name}>);
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
        return None
    toml_text = matches.group(1).replace("///", "").strip()
    return tomli.loads(toml_text)


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


def generate_metaclass(current_class: Class) -> str:
    template = f"""
class {current_class.metaclass_name} : public avalanche::Class {{
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
        static constexpr const char* base_classes_name[] {{
            {',\n\t\t\t'.join([f'"{base_class.type.get_canonical().spelling}"' for base_class in current_class.base_classes_flatten])}
        }};
        constexpr int32_t num_base_classes = sizeof(base_classes_name) / sizeof(const char*);
        num_result = num_base_classes;
        out_data = base_classes_name;
    }}
    
    [[nodiscard]] bool is_derived_from_object() const override {{
        return {'true' if current_class.derived_from_object else 'false'};
    }}
}};
"""
    return template
