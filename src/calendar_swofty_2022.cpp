#include <atomic>
#include <charconv>
#include <mutex>

#include <swofty/game/game.hpp>
#include <swofty/game/career.hpp>
#include <swofty/career_wizard.hpp>
#include <swofty/db.hpp>
#include <swofty/systems/career.hpp>
#include <swofty/systems/ui.hpp>

#include "calendar_preset.hpp"

namespace {
using namespace historical_calendar;
namespace game = swofty::game;

constexpr std::string_view progressive_group = "f1_career_sw_progressive";
constexpr std::string_view starting_season = "swofty.swofty-seasons-progressive/start_season";
#ifndef DEJW_PUBLIC_RELEASE
constexpr std::string_view finale_prefix = "dejw.swofty-historical-calendars/rollover_";
#endif
constexpr uint8_t full_season_preset = 0;
constexpr uint8_t historical_preset = 1;
constexpr char stock_list[] = "F1_2026_full";
constexpr char rollover_list[] = "F1_2026_full_plus";
constexpr uint32_t stock_count = 24;
#ifdef DEJW_PUBLIC_RELEASE
constexpr int first_year = 2015;
#else
constexpr int first_year = 2019;
#endif
constexpr int last_year = 2026;
std::atomic<bool> season_started{false};
struct CareerSeason {
    void *entity = nullptr;
    int year = 0;
    int index = 0;
};
std::mutex transition_mutex;
CareerSeason previous;

#include "calendar_swofty_2022_data.hpp"
#ifdef DEJW_PUBLIC_RELEASE
#include "calendar_public_early_data.hpp"
#endif

struct Calendar {
    const char *list;
    std::span<const uint8_t> events;
    std::span<const uint8_t> tracks;
    std::span<const char *const> track_codes{};
};
constexpr std::array calendars{
#ifdef DEJW_PUBLIC_RELEASE
    Calendar{list_2015, ids_2015, tracks_2015, track_codes_2015},
    Calendar{list_2016, ids_2016, tracks_2016, track_codes_2016},
    Calendar{list_2017, ids_2017, tracks_2017, track_codes_2017},
    Calendar{list_2018, ids_2018, tracks_2018, track_codes_2018},
#endif
    Calendar{list_2019, ids_2019, tracks_2019, track_codes_2019},
    Calendar{list_2020, ids_2020, tracks_2020, track_codes_2020},
    Calendar{list_2021, ids_2021, tracks_2021, track_codes_2021},
    Calendar{list_2022, ids_2022, tracks_2022},
    Calendar{list_2023, ids_2023, tracks_2023},
    Calendar{list_2024, ids_2024, tracks_2024},
    Calendar{list_2025, ids_2025, tracks_2025},
#ifdef DEJW_PUBLIC_RELEASE
    Calendar{list_2026, public_ids_2026, public_tracks_2026},
#else
    Calendar{list_2026, ids_2026, tracks_2026},
#endif
};
static_assert(calendars.size() == last_year - first_year + 1);

// Keep the map keys in place. Each row keeps exclusive ownership of one whole
// vector (allocator, begin, end, capacity), even while the calendars are swapped.
using ListVector = std::array<uintptr_t, 4>;
static_assert(sizeof(ListVector) + sizeof(uintptr_t) == list_row_size);

bool writable(uintptr_t address, size_t size)
{
    MEMORY_BASIC_INFORMATION region{};
    if (!address || VirtualQuery(reinterpret_cast<void *>(address), &region, sizeof(region)) != sizeof(region) ||
        region.State != MEM_COMMIT || (region.Protect & PAGE_GUARD) ||
        !(region.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))
        return false;
    const auto end = reinterpret_cast<uintptr_t>(region.BaseAddress) + region.RegionSize;
    return end > address && size <= end - address;
}

uintptr_t historical_preset_row(uintptr_t component)
{
    uintptr_t config{};
    PresetTable table{};
    if (!mem::read(component + config_offset, config) || !config || !mem::read(config, table) || !table.begin ||
        table.end <= table.begin || (table.end - table.begin) % sizeof(Preset) ||
        (table.end - table.begin) / sizeof(Preset) > 32)
        return 0;
    for (auto address = table.begin; address < table.end; address += sizeof(Preset)) {
        Preset row{};
        std::string name;
        if (mem::read(address, row) && row.type == historical_preset && row.list_name &&
            mem::c_string(reinterpret_cast<uintptr_t>(row.list_name), name, 64) && name == rollover_list)
            return address;
    }
    return 0;
}

struct RolloverCalendar {
    uintptr_t available = 0, source = 0, preset = 0;
    ListVector original{}, replacement{};
    const char *preset_list_name = nullptr;
    uint32_t count = 0;

    bool prepare(uintptr_t component, int year, const Calendar &calendar)
    {
        if (!supported_preset_layout())
            return false;
        const auto available_row = named_list_row(component, rollover_list);
        const auto source_row = named_list_row(component, calendar.list);
        const auto preset_row = historical_preset_row(component);
        if (!available_row || !source_row || available_row == source_row || !preset_row)
            return false;
        const auto available_vector = available_row + sizeof(uintptr_t);
        const auto source_vector = source_row + sizeof(uintptr_t);
        ListVector current{}, other{};
        Preset current_preset{};
        if (!mem::read(available_vector, current) || !mem::read(source_vector, other) ||
            !mem::read(preset_row, current_preset))
            return false;
        if (available)
            return available == available_vector && source == source_vector && preset == preset_row &&
                current == replacement && other == original && current_preset.max_tracks == count &&
                calendar_matches(current[1], current[2], calendar.events);

        if (!calendar_matches(current[1], current[2], ids_2023) ||
            !calendar_matches(other[1], other[2], calendar.events) ||
            current[1] == other[1] || current_preset.max_tracks != ids_2023.size() ||
            !writable(available_vector, sizeof(current)) || !writable(source_vector, sizeof(other)) ||
            !writable(preset_row, sizeof(uint32_t)))
            return false;

        available = available_vector;
        source = source_vector;
        preset = preset_row;
        original = current;
        replacement = other;
        preset_list_name = current_preset.list_name;
        count = static_cast<uint32_t>(calendar.events.size());
        std::memcpy(reinterpret_cast<void *>(available), &replacement, sizeof(replacement));
        std::memcpy(reinterpret_cast<void *>(source), &original, sizeof(original));
        *reinterpret_cast<uint32_t *>(preset) = count;
        swofty::log("[swofty-historical-calendar] Historical Calendar: switched available events and limit to %d (%u races)",
                    year, count);
        return true;
    }

    void restore()
    {
        if (!available)
            return;
        ListVector current{}, other{};
        if (mem::read(available, current) && mem::read(source, other) &&
            current == replacement && other == original && writable(available, sizeof(current)) && writable(source, sizeof(other))) {
            std::memcpy(reinterpret_cast<void *>(available), &original, sizeof(original));
            std::memcpy(reinterpret_cast<void *>(source), &replacement, sizeof(replacement));
        }
        Preset row{};
        if (mem::read(preset, row) && row.type == historical_preset && row.list_name == preset_list_name &&
            row.max_tracks == count && writable(preset, sizeof(uint32_t)))
            *reinterpret_cast<uint32_t *>(preset) = static_cast<uint32_t>(ids_2023.size());
        *this = {};
    }
};
RolloverCalendar rollover_calendar;

#ifndef DEJW_PUBLIC_RELEASE
bool finale_enabled(int year)
{
    if (year < first_year || year > 2024)
        return false;
    auto &options = swofty::ui::addon_options;
    const auto id = std::string(finale_prefix) + std::to_string(year);
    const auto option = options.get(options[id]);
    if (!option)
        swofty::log("[swofty-historical-calendar] Rollover test %d is unavailable; using the full calendar", year);
    return option && option->value.as_bool();
}
#endif

int wizard_year()
{
    // The new Progressive wizard loads a provisional career before applying
    // Full Season. SeasonStart is reported later, on entering Team HQ.
    if (season_started.load() || swofty::career::wizard::group() != progressive_group)
        return 0;
    auto &options = swofty::ui::addon_options;
    const auto season = options.get(options[starting_season]);
    if (!season)
        return 0;
    const auto year = season->value.as_string();
    if (year.size() != 4)
        return 0;
    int selected = 0;
    const auto end = year.data() + year.size();
    const auto parsed = std::from_chars(year.data(), end, selected);
    return parsed.ptr == end && parsed.ec == std::errc{} &&
        selected >= first_year && selected <= last_year ? selected : 0;
}

bool read_season(void *entity, uint32_t index, game::Season &season)
{
    if (!entity)
        return false;

    game::CareerEntity state{};
    game::Championship championship{};
    const auto championship_vtable = swofty::api->resolve("vt_Career_Championship");
    if (!championship_vtable || !mem::read(reinterpret_cast<uintptr_t>(entity), state))
        return false;
    const auto read_championship = [&](const void *address) {
        return address && mem::read(reinterpret_cast<uintptr_t>(address), championship) &&
            reinterpret_cast<uintptr_t>(championship.vtable) == championship_vtable;
    };
    if (!read_championship(state.current_championship) &&
        !read_championship(swofty::career::component("k_Career_ChampionshipDesc")))
        return false;

    const auto begin = reinterpret_cast<uintptr_t>(championship.seasons_begin);
    const auto end = reinterpret_cast<uintptr_t>(championship.seasons_end);
    game::ChampionshipSeasonEntry entry{};
    const auto season_vtable = swofty::api->resolve("vt_Career_Season");
    return championship.season_index == index && begin && end > begin && end - begin >= (index + 1) * sizeof(entry) &&
        season_vtable && mem::read(begin + index * sizeof(entry), entry) && entry.season &&
        mem::read(reinterpret_cast<uintptr_t>(entry.season), season) &&
        reinterpret_cast<uintptr_t>(season.vtable) == season_vtable;
}

int rollover_year()
{
    CareerSeason current;
    {
        const std::lock_guard lock(transition_mutex);
        current = previous;
    }
    if (!current.entity || current.entity != swofty::career::entity() || current.year < first_year ||
        current.year > last_year || current.index < 0 || current.index >= 60)
        return 0;
    game::Season season{};
    return read_season(current.entity, static_cast<uint32_t>(current.index + 1), season) &&
        season.weekends_begin == season.weekends_end ? std::min(current.year + 1, last_year) : 0;
}

int track_id(std::string_view code)
{
    int64_t count = 0;
    if (swofty::db::int_at("tracks._Track_instancecount_", &count) != swofty::db::Result::ok || count < 0 || count > 255)
        return -1;
    for (int64_t index = 0; index < count; ++index) {
        const auto row = "tracks.Track_" + std::to_string(index);
        const char *label = nullptr;
        if (swofty::db::string_at(row + "._metadata_", &label) != swofty::db::Result::ok || !label || code != label)
            continue;
        int64_t id = 0;
        if (swofty::db::int_at(row + ".statId", &id) == swofty::db::Result::ok && id >= 0 && id < 255)
            return static_cast<int>(id);
    }
    return -1;
}

#ifdef DEJW_PUBLIC_RELEASE
std::string schedule_field(std::string_view table, std::string_view row_type,
                           std::string_view name, std::string_view field)
{
    using namespace swofty::db;
    const auto prefix = std::string(table) + ".";
    int64_t count = 0;
    if (int_at(prefix + "_" + std::string(row_type) + "_instancecount_", &count) != Result::ok ||
        count < 0 || count > 512)
        return {};
    for (int64_t index = 0; index < count; ++index) {
        const auto row = prefix + std::string(row_type) + "_" + std::to_string(index);
        const char *label = nullptr;
        const char *value = nullptr;
        if (string_at(row + "._metadata_", &label) == Result::ok && label && name == label &&
            string_at(row + "." + std::string(field), &value) == Result::ok && value)
            return value;
    }
    return {};
}

int configured_track_id(int year, size_t index)
{
    char event_name[64]{};
    if (year == 2022)
        std::snprintf(event_name, sizeof(event_name), "rw_paul_ricard");
    else if (year == 2026)
        std::snprintf(event_name, sizeof(event_name), "%s", bahrain_event_2026);
    else
        std::snprintf(event_name, sizeof(event_name), "dejw_calendar_%d_%02zu", year, index + 1);

    const auto activity = schedule_field("schedule.all_track_events", "fixed_event", event_name, "activity");
    const auto track = schedule_field("schedule.activities", "activity_race_weekend", activity, "track");
    return track.empty() ? -1 : track_id(track);
}
#endif

void log_saved_calendar(void *entity, uint32_t index, int year, const Calendar &calendar)
{
    game::Season season{};
    if (!read_season(entity, index, season))
        return;
    const auto begin = reinterpret_cast<uintptr_t>(season.weekends_begin);
    const auto end = reinterpret_cast<uintptr_t>(season.weekends_end);
    if (!begin || end < begin || (end - begin) % sizeof(uintptr_t))
        return;
    const auto count = (end - begin) / sizeof(uintptr_t);
#ifdef DEJW_PUBLIC_RELEASE
    bool verified = count == calendar.tracks.size();
#else
    const bool finale = count == 1 && index == 0 && finale_enabled(year);
    bool verified = finale || count == calendar.tracks.size();
#endif
    bool unknown_track = false;
    for (size_t i = 0; verified && i < count; ++i) {
        uintptr_t weekend{};
        uint8_t track{};
#ifdef DEJW_PUBLIC_RELEASE
        const bool optional = (!calendar.track_codes.empty() && *calendar.track_codes[i]) ||
            (year == 2022 && calendar.events[i] == 1) ||
            (year == 2026 && i == bahrain_index_2026);
        int expected = optional ? configured_track_id(year, i) : calendar.tracks[i];
#else
        int expected = finale ? calendar.tracks.back() : calendar.tracks[i];
        if (!finale && !calendar.track_codes.empty() && *calendar.track_codes[i])
            expected = track_id(calendar.track_codes[i]);
#endif
        unknown_track |= expected < 0;
        verified = mem::read(begin + i * sizeof(weekend), weekend) && weekend &&
            mem::read(weekend + offsetof(game::Weekend, track_stat_id), track) && (expected < 0 || track == expected);
    }
    swofty::log("[swofty-historical-calendar] Team HQ %d: %u weekends (%s)", year, static_cast<unsigned>(count),
                !verified ? "calendar mismatch" : (unknown_track ? "count verified; some track IDs unavailable" : "verified"));
}
}

SWOFTY_WRAP(Game_SeasonLength_ApplyPreset, void, void *component, uint8_t preset)
{
    if (!component || preset > 2) {
        orig(component, preset);
        return;
    }
    const int rollover = rollover_year();
    const int year = rollover ? rollover : (preset == full_season_preset ? wizard_year() : 0);
    if (!year) {
        orig(component, preset);
        return;
    }

    const auto &calendar = calendars[year - first_year];
#ifdef DEJW_PUBLIC_RELEASE
    const auto expected = calendar.events;
#else
    const bool finale = !rollover && finale_enabled(year);
    const auto expected = finale ? std::span<const uint8_t>(finale_ids) : calendar.events;
#endif

    const auto address = reinterpret_cast<uintptr_t>(component);
    if (rollover && year != 2023) {
        bool ready;
        {
            const std::lock_guard lock(transition_mutex);
            ready = rollover_calendar.prepare(address, year, calendar);
        }
        if (!ready) {
            swofty::log("[swofty-historical-calendar] Could not switch the rollover screen to %d; using the game calendar", year);
            orig(component, preset);
            return;
        }
        orig(component, historical_preset);
#ifdef DEJW_PUBLIC_RELEASE
    } else if (year == 2023) {
#else
    } else if (year == 2023 && !finale) {
#endif
        if (!named_list_matches(address, rollover_list, expected)) {
            swofty::log("[swofty-historical-calendar] Could not validate the 2023 rollover list; using the game calendar");
            orig(component, preset);
            return;
        }
        orig(component, historical_preset);
    } else {
#ifdef DEJW_PUBLIC_RELEASE
        const auto *list = calendar.list;
#else
        const auto *list = finale ? finale_list : calendar.list;
#endif
        ScopedPreset replacement;
        if (!replacement.install(address, full_season_preset, stock_list, stock_count, list, expected)) {
            swofty::log("[swofty-historical-calendar] Could not validate %s or Full Season preset; using the game calendar", list);
            orig(component, preset);
            return;
        }
        orig(component, full_season_preset);
    }

    uintptr_t begin{}, end{};
    uint32_t count{};
    const bool verified = mem::read(address + active_begin, begin) && mem::read(address + active_end, end) &&
        mem::read(address + active_count, count) && count == expected.size() && calendar_matches(begin, end, expected);
#ifdef DEJW_PUBLIC_RELEASE
    const char *action = rollover ? "Rollover to" : "Starting";
#else
    const char *action = rollover ? "Rollover to" : (finale ? "Fast test" : "Starting");
#endif
    swofty::log("[swofty-historical-calendar] %s %d: %u of %u rounds (%s)", action, year, count,
                static_cast<unsigned>(expected.size()), verified ? "verified" : "check calendar in Team HQ");
}

SWOFTY_ON_LOAD
{
    using namespace swofty::career::events;
    swofty::events::on<SeasonStart>().after([](const SeasonStart &event, const swofty::events::Context &ctx) {
        season_started = true;
        const auto &career = ctx.career();
        const auto entity = career && career->data_group == progressive_group ? swofty::career::entity() : nullptr;
        {
            const std::lock_guard lock(transition_mutex);
            rollover_calendar.restore();
            previous = {entity, event.year, event.season_index};
        }
        if (entity && event.year >= first_year && event.year <= last_year && event.season_index >= 0)
            log_saved_calendar(entity, static_cast<uint32_t>(event.season_index), event.year,
                               calendars[event.year - first_year]);
    });
    swofty::events::on<Unloaded>().after([](const Unloaded &, const swofty::events::Context &) {
        const std::lock_guard lock(transition_mutex);
        rollover_calendar.restore();
        previous = {};
        season_started = false;
    });
}

#ifdef DEJW_EA_TEST
SWOFTY_ADDON(historical_calendar::verify_test_preset_layout)
#else
SWOFTY_ADDON()
#endif
