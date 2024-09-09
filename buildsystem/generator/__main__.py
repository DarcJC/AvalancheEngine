import typer
from typer import Option

cli_app = typer.Typer(name="Avalanche Build Tool")

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
):
    print(input_header)


if __name__ == '__main__':
    main()
