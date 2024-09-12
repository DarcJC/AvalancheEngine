from typing import Optional

import typer
from typer import Option

from .binding import generate_binding

cli_app = typer.Typer(name="Avalanche Build Tool", pretty_exceptions_enable=False)

def main():
    cli_app()


@cli_app.command("clean", help="Clean up generated codes")
def clean(binary_dir: str = Option(help="CMake binary directory")):
    print(f"Cleaning {binary_dir}...")


@cli_app.command("binding", help="Generating binding codes")
def binding_generation(
        binary_dir: str = Option(help="CMake binary directory"),
        input_header: str = Option(help="Path to header file to be parse"),
        out_header: str = Option(help="Path to generated header file will be place"),
        out_source: str = Option(help="Path to generated source file will be place"),
        include_path: Optional[str] = Option(None, help="Include paths to parse file"),
):
    include_paths = [] if include_path is None else include_path.split(";")
    generate_binding(binary_dir, input_header, out_header, out_source, include_paths)


@cli_app.command("module-list", help="Generating module list")
def module_list_generation(
        output_file: str = Option(help="Output path of generated header"),
        modules: str = Option(help="List of modules"),
):
    modules = modules.split(';')
    template = f"""
#pragma once

namespace avalanche::generated {{
    struct enabled_modules {{
        inline static const char* value[] {{
            {',\n\t\t\t'.join(f'"{module}"' for module in modules)}
        }};
    }};
}};
"""
    with open(output_file, 'w') as  f:
        f.truncate()
        f.write(template)


if __name__ == '__main__':
    main()
