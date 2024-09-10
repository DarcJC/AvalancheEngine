import re
from functools import lru_cache
from typing import Optional

import clang.cindex
import tomli


class CxxHeaderFileProcessor:
    index: clang.cindex.Index
    translation_unit: clang.cindex.TranslationUnit

    _include_paths: list[str]
    _filepath: str
    _out_header: str
    _out_source: str

    def __init__(self, filepath: str, include_paths: list[str]):
        self.index = clang.cindex.Index.create()
        self._filepath = filepath
        self._include_paths = include_paths
        self._out_header = '''#pragma once
#pragma warning (disable: 4244)
\n#include <array>\n#include <tuple>
#include "class.h"\n#include "polyfill.h"
#include "container/vector.hpp"\n#include "container/shared_ptr.hpp"\n#include "container/unique_ptr.hpp"\n'''
        self._out_source = ''

    def parse(self):
        with open(file=self._filepath, mode='rb') as f:
            source_content = f.read().decode(encoding='utf-8')
        clang_args = ['-x', 'c++', '-std=c++20', '-Wno-pragma-once-outside-header', '-DDURING_BUILD_TOOL_PROCESS=1']
        # Remove empty item
        self._include_paths.remove('')
        for path in self._include_paths:
            clang_args.append(f'-I{path}')
        self.translation_unit = clang.cindex.TranslationUnit.from_source(self._filepath, args=clang_args, unsaved_files=[( self._filepath, source_content )], options=0, index=self.index)
        self.traverse_ast_and_process(self.translation_unit.cursor)

    def save_outputs(self, out_header_path: str, out_source_path: str):
        self._out_header = f'{self._out_header}\n#pragma warning (default: 4244)'
        self._out_source = f'#include "{out_header_path}"\n{self._out_source}'

        with open(out_header_path, "w") as f:
            f.write(self._out_header)

        with open(out_source_path, "w") as f:
            f.write(self._out_source)

    @staticmethod
    def parse_comment(raw_comment: str) -> Optional[dict]:
        matches = re.search(r'@avalanche::begin(.*?)@avalanche::end', raw_comment, re.DOTALL)
        if not matches:
            return None
        toml_text = matches.group(1).replace("///", "").strip()
        return tomli.loads(toml_text)

    def append_to_header(self, text: str):
        self._out_header += text

    def append_to_source(self, text: str):
        self._out_source += text

    def traverse_ast_and_process(self, current_node: clang.cindex.Cursor, depth: int = 0):
        if depth > 5:
            return

        for child in current_node.get_children():
            self.traverse_ast_and_process(child, depth=depth+1)

        if current_node.kind == clang.cindex.CursorKind.CLASS_DECL or current_node.kind == clang.cindex.CursorKind.STRUCT_DECL:
            if current_node.raw_comment is not None:
                metadata = self.parse_comment(current_node.raw_comment)
                if metadata is None:
                    return

                self.append_to_header(generate_metadata_struct(current_node.displayname, metadata))


BASE_TYPE_MAPS = {
    int: 'int32_t',
    float: 'float',
    bool: 'bool'
}


def generate_fields(metadata: dict) -> str:
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

    return result


def generate_metadata_struct(name: str, metadata: dict) -> str:
    template = f"""
    namespace avalanche::generated {{
        struct {name}MetadataType : metadata_tag {{
            {generate_fields(metadata)}
        }};
    }}
    """

    return template
