from typing import Optional

import clang.cindex


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
        self._out_header = "#pragma once"
        self._out_source = ""

    def parse(self):
        with open(file=self._filepath, mode='r') as f:
            source_content = f.read()
        clang_args = ['-x', 'c++', '-std=c++20', '-Wno-pragma-once-outside-header', '-DDURING_BUILD_TOOL_PROCESS=1', '-v']
        # Remove empty item
        self._include_paths.remove('')
        for path in self._include_paths:
            clang_args.append(f'-I{path}')
        self.translation_unit = clang.cindex.TranslationUnit.from_source(self._filepath, args=clang_args, unsaved_files=[( self._filepath, source_content )], options=0, index=self.index)
        self.traverse_ast_and_process(self.translation_unit.cursor)

    def save_outputs(self, out_header_path: str, out_source_path: str):
        with open(out_header_path, "w") as f:
            f.write(self._out_header)
        with open(out_source_path, "w") as f:
            f.write(self._out_source)

    def traverse_ast_and_process(self, current_node: clang.cindex.Cursor):
        for child in current_node.get_children():
            self.traverse_ast_and_process(child)

        if current_node.kind == clang.cindex.CursorKind.CLASS_DECL:
            pass
