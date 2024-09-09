from .process import CxxHeaderFileProcessor


def generate_binding(
        binary_dir: str,
        input_header: str,
        out_header: str,
        out_source: str,
        include_paths: list[str],
):
    processor = CxxHeaderFileProcessor(filepath=input_header, include_paths=include_paths)
    processor.parse()
    processor.save_outputs(out_header_path=out_header, out_source_path=out_source)


__all__ = ['generate_binding']
