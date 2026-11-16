# Historical Calendar for Progressive Career

Historical calendars for 2013-2026 in Swofty Progressive Career for F1 25.

[Download 1.4.8](downloads/Historical_Calendar_for_Progressive_Career_v1.4.8.swoftyaddon)

Steam: tested. EA app: compatibility pending confirmation. Requires Swofty Launcher 1.9.58 or later, Windows, Progressive Career and the F1 2026 game season.

See [usage and limitations](addon/README.md), [changelog](CHANGELOG.md), and [compatibility audit](COMPATIBILITY.md).
Sprint overrides are currently unavailable. The 2013 calendar requires Progressive Career to expose a 2013 start and is untested in-game. The optional fast-rollover test switch remains disabled by default.

## Build

Install MSVC x64 C++ build tools and Python 3. Use a SwoftyLauncher SDK checkout with the required exported bindings and headers. Open an x64 Native Tools Command Prompt, set SWOFTY_SDK_ROOT to that checkout, and run build.cmd.

Output is written to build/. The included release binaries are the same binaries tested in 1.4.8-beta.3. Game files and optional track assets are not distributed in this repository.
The bundled Swofty header is covered by [the SDK licence](addon/LICENSE-Swofty-SDK.txt).
