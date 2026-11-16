# Historical Calendar 1.4.8

Public package prepared from the tested 1.4.8-beta.3 binaries. Steam tested; EA app compatibility pending confirmation.

## Audit

1.4.7 already registers its hooks through launcher bindings:
`Game_SeasonLength_ApplyPreset`, `Career_ScheduleEntry_Finish`, and
`Game_TrackData_GetTrack`. Its typed `Historical_FinishSchedule` declaration
is an alias for an existing launcher binding, not a private address resolver.

However, preset validation also compared the resolved function against the
old executable RVA and read a fixed body RVA. Sprint validation read two
additional fixed executable addresses and then wrote a private track field.
Those checks stopped the features after the game update.

## Implemented

- Preset discovery uses `api->resolve("Game_SeasonLength_ApplyPreset")`.
- Before mixin installation, follow the resolved entry's relative jump and
  verify the existing body-layout signature. No fixed executable RVA remains.
- Preserve named-list, expected event sequence, preset, writable-memory and
  saved-calendar checks. A failed layout check disables calendar writes.
- Remove the private sprint memory writer and its unnecessary track hook.
- Mark both sprint controls unavailable; existing saved settings cannot turn
  the removed writer back on. The game's sprint rules remain active.
- Remove the old exact game-build manifest restriction; require launcher 1.9.58.
- Keep Steam-only support declaration and Progressive Career dependency.

## Validation

MSVC x64 release DLL and optional-track helper compiled successfully.
Compiler emitted two warnings in existing SDK/Windows headers (C4067, C5105).
The resolved-entry body signature was checked against the installed updated
Steam executable and matched. Archive CRC validation passed during packaging.
Subsequent Steam in-game tests confirmed 2018 creation and 2018-to-2019 rollover, and 2014 creation and 2014-to-2015 rollover. 2013 and EA remain untested.

Test on a NEW 2018 career: select Full Season, check 21 rounds and their order,
then start the first weekend. Also test a subsequent season transition.
This build does not repair the already-created 24-round save.

## Remaining launcher API request (not submitted)

Please expose a verified track authentic-weekend/sprint field accessor or a
core operation to set/restore a track's authentic weekend format per career.
The SDK available here declares TrackDataTrack but leaves this field in padding.
The previous addon used TrackDataTrack +0xfd, verified by the old build's
format writer at 0x142e47a6c and reader at 0x140ad65eb. These are historical
evidence only; they are not addresses to use on the updated executable.
Central verification and cross-build maintenance are required before restoring
the two sprint options without private field assumptions.

Calendar code still uses validated local structure layouts (config, preset
table and event vectors). Launcher-resolved function bindings do not make these
structures automatically version-independent. A future SDK preset/calendar
operation would remove this remaining local layout dependency.

## References consulted

- https://f1.swofty.net/reference/addons/hooking.html
- https://f1.swofty.net/reference/std/mixin.html
- https://f1.swofty.net/reference/game/
- https://f1.swofty.net/std/systems/career.html
- Installed launcher 1.9.58 bindings, including bindings.steam-26566308.json.

The public coverage/design page describes shipping patterns as bindings, but
that does not establish that our missing sprint field is a maintained exported
launcher binding. No new binding was invented or submitted automatically.
