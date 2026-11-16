# Historical Calendar for Progressive Career 1.4.8

Historical calendars for 2013-2026 in F1 25 using Swofty's Progressive Career.

Requires Windows, Swofty Launcher 1.9.58 or later, Progressive Career and the F1 2026 game season.
Steam: tested. EA app: compatibility pending confirmation. Launcher binding resolution is used, but the EA version has not been tested. The manifest therefore retains only the verified Steam platform badge.

## Installation and use

Import the .swoftyaddon file directly into the launcher without extracting it. Enable it with Progressive Career. Choose your starting year and select Full Season.
Optional circuit addons must be enabled in the same profile. Configure Grand Prix selectors before creating a career, then rebuild and restart through the launcher. Missing optional tracks use their configured substitutes. Existing saved calendars are not rebuilt by changing settings.

## Current limitations

Sprint overrides are currently unavailable while a maintained launcher API is needed. The two sprint controls have no effect; game sprint rules apply even in seasons that historically had no sprints.
The 2013 calendar has 19 rounds but is untested in-game; Progressive Career must expose a 2013 start. India 2013 uses Lusail as a substitute. Optional Nurburgring and Korea selectors are included.

## Fast rollover test

Retained by request and disabled by default. For a NEW Full Season career only, it reduces the starting season to its finale (Brazil for 2013, Abu Dhabi otherwise). Finish it to test rollover into a full historical calendar. Leave it OFF before creating a normal career. It does not shorten existing saved seasons.

## Validation

Steam in-game checks confirmed 2018 creation and 2018-to-2019 rollover, plus 2014 creation and 2014-to-2015 rollover. Other season starts and EA compatibility are not all verified. Release binaries match 1.4.8-beta.3.

## Changelog



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

## Earlier releases


- 1.4.7 Synchronizes Full Season and deferred calendar reads for every starting
  year from 2015 to 2026, extending the earlier 2016-only protection.
  Restores the previous wizard list before changing year, career entity or preset,
  preventing calendars from being combined after backing out and trying again.
  Labels preset verification as provisional until the saved Team HQ calendar is checked.
- 1.4.6 Fixes the whole native addon being rejected by the launcher in 1.4.5.
  Uses the exported Career_ScheduleEntry_Finish binding for automatic rollover
  instead of the non-exported Game_Season_CompleteSeason analysis symbol.
  Restores calendar creation and sprint overrides, including the 21-round 2016 start.
- 1.4.5 Prepares the next historical calendar when the final weekend is completed,
  including automatic rollovers that skip the season-length selection screen.
  Addresses the 2015-to-2016 rollover loading the 22-round base calendar instead of 21 rounds.
  Keeps deferred schedule readers on the same list until the next season starts.
  Caches failed sprint checks until the next session and uses SDK region caching
  for sprint-apply reads, with guarded writes for stale or read-only objects.
- 1.4.2 Caches sprint checks per track object and session to avoid repeated VirtualQuery calls in
  Game_TrackData_GetTrack. Uses SDK region caching for cold reads and names tracks in sprint logs.
  Includes the 2016 calendar creation fix from 1.4.1.
- 1.4.1 Keeps the 2016 Full Season preset and its deferred calendar readers on the same 21-round
  event list until the new season has been created. Addresses the reported 45-round 2016 calendar.
  Test with a new 2016 career. Existing saved 45-round calendars require separate repair.
- 1.4.0 Adds two enabled-by-default sprint settings. One disables sprint weekends in
  Progressive Career seasons 2015-2020. The other sets historical sprint venues for 2021-2026,
  with the game's programmed venues restored when it is turned off.
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
