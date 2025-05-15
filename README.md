![Linux-64 Build Status](https://github.com/Fighter19/CnC_Generals_Zero_Hour/actions/workflows/ubuntu.yml/badge.svg)
![Windows-32 Build Status](https://github.com/Fighter19/CnC_Generals_Zero_Hour/actions/workflows/win32.yml/badge.svg)

# Command & Conquer Generals (inc. Zero Hour) Source Code

This repository includes source code for Command & Conquer Generals, and its expansion pack Zero Hour.

It contains adjustments to allow building with modern toolchains,
on a wider variety of systems. As such it has been tested on Windows x86, Windows x64,
Linux x64 and Linux ARM64.

Focus of the repository is on allowing the game to operate smoothly on Linux-based setups.

The main focus of this repository is *NOT* backwards compatibility,
but feature development, as well as bug fixing.

Cross-play between 64-bit versions of this fork is also a goal.
Toolings, such as the WorldBuilder or other build tools are not the focus of this repository.

Patches aside from the focus are still appreciated, but issue reports regarding those,
might be left open.

## Features

What's working:
- Campaigns and Skirmish for Zero Hour
- Most sound effects
- Support for modern toolchains
- Works on a Raspberry Pi 5 (with a few graphics glitches)
- Support for Linux x64 and ARM64
- Support for SDL3
- Animated cursors

What's not working:
- Generals base game only
- Music tracks and longer voice lines
- Multiplayer
- Probably a variety of other bugs

## Running the Game

To run the game, you need to have the original game files. You can use them from a CD installation or use the Steam version.
1. Obtain an executable either by downloading the latest artifact from the CI or by building it yourself.
2. Add the library path of the game to your `LD_LIBRARY_PATH` environment variable. This is required for the game to find its shared libraries, such as DXVK or SDL3. (libdxvk_d3d8.so needs to be loadable)
   ```sh
   cd [folder-with-RTS-executable]
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/lib
   ```
3. Set the current working directory to the folder containing the game files.
    ```sh
    cd [folder-with-game-files] # e.g. ~/.steam/steam/steamapps/common/Command & Conquer Generals - Zero Hour
    [path-to-executable]/RTS -win
    ```

Omit the `-win` flag to run the game in fullscreen mode.

## Dependencies

To build this project on a Debian or Ubuntu machine, install the following dependencies:

Install the required build tools and libraries by running the following commands:

```sh
sudo apt install nasm autoconf automake libtool pkg-config libltdl-dev
sudo apt install ninja-build
sudo apt install python3-jinja2
```

Make sure to install VCPKG, if you haven't already.
```sh
cd [your-workspace]
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
export VCPKG_ROOT=$(pwd)
./bootstrap-vcpkg.sh
export PATH="$PATH:$VCPKG_ROOT"
```

Make sure to export the PATH variable, each time you need to run vcpkg.
This is usually only required during the configuration via CMake.


## Compiling

To use the compiled binaries, you must own the game. The C&C Ultimate Collection is available for purchase on [EA App](https://www.ea.com/en-gb/games/command-and-conquer/command-and-conquer-the-ultimate-collection/buy/pc) or [Steam](https://store.steampowered.com/bundle/39394/Command__Conquer_The_Ultimate_Collection/).

To compile the source code, follow these steps:

1. Clone the repository:
   ```sh
   git clone https://github.com/Fighter19/CnC_Generals_Zero_Hour.git
2. Change to the project directory:
   ```sh
   cd CnC_Generals_Zero_Hour
   ```

From here on, you can use either VSCode with the CMake extension, or the command line.

### Building with CMake
3. Create a build directory:
   ```sh
   mkdir build
   cd build
   ```
4. Configure the project:
   ```sh
   cmake --preset=default -DVCPKG_INSTALL_OPTIONS="--allow-unsupported" -DSAGE_USE_SDL3=ON ../
    ```
5. Build the project:
    ```sh
    cmake --build . --config Release
    ```

You can use the `--preset` flag to specify a preset defined in the `CMakePresets.json` file. On our CI we build the Linux package with `linux64-deploy` and the Windows package with `win32-deploy`.

### Building with Visual Studio Code

Building with Visual Studio Code is straight-forward. Opening the project with the C++ Extension Pack installed,
should automatically execute vcpkg to install the dependencies.
You can then use the CMake extension to build the project.

## Contributing

This repository is currently accepting contributions (pull requests, issues, etc).


## License

This repository and its contents are licensed under the GPL v3 license, with additional terms applied. Please see [LICENSE.md](LICENSE.md) for details.
