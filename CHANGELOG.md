# Historical Calendar for Progressive Career - 1.4.8

Changes since 1.4.7

- Restored calendar compatibility after the F1 25 update. Calendar function addresses now come from launcher bindings instead of checks against fixed executable addresses.
- Retained validation of calendar data and memory layouts before applying changes.
- Added 19-round calendars for 2013 and 2014, including automatic progression into subsequent supported seasons.
- Added the 2013 German Grand Prix circuit selector (Nurburgring or Zandvoort substitute) and Korean Grand Prix selector (Korea or Shanghai substitute).
- Extended the Malaysia selector to 2013-2014 and the Russia and Germany selectors to 2014.
- Included the optional Fast rollover test switch, disabled by default. On a new Full Season career it reduces the starting season to its finale; the next season uses its full calendar. Brazil is the 2013 finale; other supported seasons use Abu Dhabi.
- Updated the minimum launcher requirement to 1.9.58 and removed the old exact game-build restriction.
- Steam: tested. EA app: compatibility pending confirmation; no EA support claim is made until in-game testing is complete.

Current limitations

- Sprint overrides are unavailable pending a maintained launcher API. Both sprint settings currently have no effect; the game's sprint rules apply, including in pre-2021 seasons.
- The 2013 calendar is included but has not been tested in-game. Starting in 2013 requires that year to be exposed by Progressive Career. India uses Lusail as a substitute.
- Existing saved calendars are not rebuilt automatically. Choose optional circuits before creating a career.
- Launcher bindings resolve function addresses, but the addon still validates local data layouts. Future game updates can require another compatibility update.

Validation

- In-game confirmation on Steam: 2018 calendar creation and 2018-to-2019 rollover; 2014 calendar creation and 2014-to-2015 rollover.
- Release native binaries are byte-identical to the tested 1.4.8-beta.3 package; this release updates packaging and documentation.
- These checks do not establish EA compatibility or validate every supported season in-game.
