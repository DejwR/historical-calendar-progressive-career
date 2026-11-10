# Historical Calendar for Progressive Career 1.3.1

By Dejw. Requires Swofty's Progressive Career. Update it to 0.4.0 or later for the 2015 start.
Choose a starting year in Progressive Career and use Full Season.
At the season transition, confirm the Historical Calendar preset if the game shows the calendar picker.

This package requires Swofty Launcher 1.8.36 or later on Windows, F1 25 build 25251167 and the F1 2026 game season.
The public release supports the Steam copy. EA app is unsupported in this release.
Each Grand Prix selector starts with a standard circuit. To use an optional circuit, select it and enable its
track addon in the same launcher profile, then rebuild and restart from the launcher. If the track addon is
missing or disabled, the build reports a warning and uses the default circuit. Choose the circuits before
creating your career and keep them stable for that save. A calendar already saved in a career does not get
rebuilt by changing an option.

| Grand Prix | Default circuit | Optional circuits and addon IDs |
|---|---|---|
| Tuscan Grand Prix (2020) | Suzuka | Jerez (`pajko.jerez`), Mugello (`mod.mugello`) |
| France Grand Prix (2018, 2019, 2021, 2022) | Madrid | Paul Ricard (`twilight.paul-ricard`) |
| Portugal Grand Prix (2020, 2021) | Melbourne | Portimão (`mod.portimao`) |
| Russia Grand Prix (2015, 2016, 2017, 2018, 2019, 2020, 2021) | Miami | Sochi (`mod.sochi`) |
| Germany Grand Prix (2016, 2018, 2019) | Zandvoort | Hockenheim (`mod.hockenheim`) |
| Turkey Grand Prix (2020, 2021) | Singapore | Istanbul Park (`track.istanbul_park`) |
| Sakhir Grand Prix (2020) | Jeddah | Bahrain Outer (`track.bahrain_outer`) |
| Eifel Grand Prix (2020) | Zandvoort | Nürburgring (`mod.nurburgring`) |
| Malaysia Grand Prix (2015, 2016, 2017) | Lusail | Sepang (`mod.bahrain24`) |
| Bahrain Grand Prix (2026) | Bahrain | Sepang (`mod.bahrain24`) |

Circuit choices keep the same round positions and season lengths.
After updating from 1.2.1, reselect any optional circuits previously enabled with a toggle.
Bahrain Grand Prix (2026) controls round 16 of the 23-round 2026 Progressive Career calendar.
It is independent of the Malaysia Grand Prix selector for 2015–2017.

Thanks to Swofty for Progressive Career and the launcher SDK, and to the creators of the optional tracks.
Obtain each track separately from its publisher. This package contains the calendar changes and its two binaries.

## Changelog

- 1.3.1 Updates Progressive Career to the revised 23-round 2026 calendar. Removes Jeddah and moves the
  Bahrain or Sepang choice to round 16, after Baku and before Singapore.
- 1.3.0 Replaces track toggles with Grand Prix circuit selectors. Uses Madrid for France, Miami for Russia
  and Jeddah for Sakhir by default. Adds an independent Bahrain or Sepang choice for round 4 of 2026.
- 1.2.1 Adds a single circuit choice for the 2020 Tuscan Grand Prix.
- 1.2.0 Adds an optional Mugello for the 2020 Tuscany round. Jerez stays available as an alternative.

### 1.1.2 changes since 1.0.0

- Added the complete 19-round 2015 calendar and extended the supported starting years to 2015–2026. Update Progressive Career to 0.4.0 or later to start in 2015.
- Added the 2015 Malaysian and Russian rounds to the existing Sepang and Sochi options. With the options off, Losail and Jeddah fill those slots.
- Added the Steam platform badge in Addon Browser. This release supports Steam and requires Swofty Launcher 1.8.36 or later. EA app support is experimental and available separately for testing.
- Added the **Source code** link to the project's GitHub repository in Addon Browser.

- 1.0.0 Adds the 2016, 2017 and 2018 calendars and an optional Sepang toggle.
