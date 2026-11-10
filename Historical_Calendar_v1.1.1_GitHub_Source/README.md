# Historical Calendar for Progressive Career

Calendar addon for Swofty's Progressive Career in F1 25. See [addon/README.md](addon/README.md) for installation and the optional circuits.

## Build on Windows

1. Install the MSVC x64 C++ build tools and Python 3.
2. Check out revision `61d1588cd186c2b37c9ea18af7c1d4a9661816bb` of [SwoftyLauncher](https://github.com/Swofty-Developments/SwoftyLauncher) for its SDK and native headers.
3. Open an x64 Native Tools Command Prompt, set `SWOFTY_SDK_ROOT` to that checkout and run `build.cmd` from this repository.

The package appears under `build/`. The calendar operation files and the corresponding C++ arrays are checked in. Building the release only requires the source files and SDK. The game and any optional track addons are needed when testing in the launcher.

The bundled Swofty header is covered by the [SDK licence](addon/LICENSE-Swofty-SDK.txt).
