# Historical Calendar for Progressive Career

Calendar addon for Swofty's Progressive Career in F1 25. See [addon/README.md](addon/README.md) for installation and the optional circuits.

## Download

[Historical Calendar 1.4.0 for Swofty Launcher](downloads/Historical_Calendar_for_Progressive_Career_v1.4.0.swoftyaddon)

This release supports Steam on Windows and requires Swofty Launcher 1.8.36 or later.
See the [changelog](addon/README.md#changelog) for changes since 1.0.0.

## Sprint settings

Both options are enabled by default. Disable sprints (2015–2020) uses standard weekends in those seasons.
Historical sprint weekends (2021–2026) selects the year's sprint venues. Turning that option off restores
the game's programmed venues. Choose the options before generating a season or starting a new weekend.

Launcher builds and native policy tests passed. Gameplay logs confirm the 2019 standard-weekend override
and activation of the 2021 historical sprint policy. The saved session list and the 2020-to-2021 rollover
still need end-to-end gameplay verification.

## Build on Windows

1. Install the MSVC x64 C++ build tools and Python 3.
2. Check out revision `61d1588cd186c2b37c9ea18af7c1d4a9661816bb` of [SwoftyLauncher](https://github.com/Swofty-Developments/SwoftyLauncher) for its SDK and native headers.
3. Open an x64 Native Tools Command Prompt, set `SWOFTY_SDK_ROOT` to that checkout and run `build.cmd` from this repository.

The package appears under `build/`. The calendar operation files and the corresponding C++ arrays are checked in. Building the release only requires the source files and SDK. The game and any optional track addons are needed when testing in the launcher.

The bundled Swofty header is covered by the [SDK licence](addon/LICENSE-Swofty-SDK.txt).
