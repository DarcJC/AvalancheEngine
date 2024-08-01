import argparse
from .config import GLOBAL_BASE_CONFIG


command_storage = {}


def subcommand(name: str, /, *, aliases: list[str] = None):
    def decorator(func):
        command_storage[name] = func
        if aliases:
            for alias in aliases:
                command_storage[alias] = func
        return func
    return decorator


def main():
    parser = argparse.ArgumentParser(description="Avalanche binding code generator")

    parser.add_argument('subcommand', help='Subcommand to run')
    parser.add_argument('-b', '--base-dir', type=str, required=True, help='Working directory for the generator')

    GLOBAL_BASE_CONFIG.args = parser.parse_args()
    GLOBAL_BASE_CONFIG.base_dir = GLOBAL_BASE_CONFIG.args.base_dir

    command = GLOBAL_BASE_CONFIG.args.subcommand
    if command not in command_storage:
        print(f"error: subcommand '{command}' not found")
        exit(1)
    else:
        try:
            command_storage[command]()
        except Exception as err:
            print(f"error: error occurred while performing '{command}':\n\t{err.with_traceback()}")
            exit(2)


@subcommand('clean')
def clean() -> int:
    pass


if __name__ == '__main__':
    main()
