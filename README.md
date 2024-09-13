# Avalanche Engine

A WIP game engine project.

## Building from source

### Required Tools

#### Windows

<details>
    <summary>Windows Installation Guide</summary>

1. [Download](https://visualstudio.microsoft.com/zh-hans/downloads/) / Open Visual Studio Installer
2. Go `Single Component` tag, search for `clang` and **ensure** you have selected all options it showed.
3. Check installed Python version is **>= 3.12**, and **ensure** it has been added to your current Path.
4. Install pipx by `python -m pip install pipx` and then install poetry `pipx install poetry`.
</details>

#### Linux

<details>
    <summary>Linux Installation Guide</summary>

🚧 **TBD** 🚧

Installing Clang + Python>=3.12 + poetry, then it should be ok.
</details>

### Build

<details>
    <summary>Steps</summary>

1. Clone this project `git clone https://github.com/DarcJC/AvalancheEngine.git`.
2. Get into cloned directory `cd AvalancheEngine`.
3. Pull submodules `git submodule update --init --recursive`.
4. Setup python environment `poetry install`.
5. Run `poetry shell` to activate new environment or select your poetry environment in `CLion`.
6. (1) Select target and click build button if you are using `CLion`.
7. (2) Run `cmake -B cmake-build && cmake --build cmake-build`.
</details>
