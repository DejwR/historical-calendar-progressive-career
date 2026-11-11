#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

#include <windows.h>
#include <swofty/mem.hpp>

namespace historical_calendar {

// F1 25 build 25251167: the TrackAuthenticWeekendData reader writes a boolean
// at TrackDataTrack +0xfd (0x142e47a6c). The weekend builders read that byte.
class HistoricalSprints {
    static constexpr uintptr_t format_offset = 0xfd;
    struct ChangedTrack {
        uintptr_t address;
        uintptr_t record;
        uint8_t original;
        uint8_t written;
    };
    std::vector<ChangedTrack> changed_;

    static bool writable_byte(uintptr_t address)
    {
        MEMORY_BASIC_INFORMATION region{};
        return VirtualQuery(reinterpret_cast<void *>(address), &region, sizeof(region)) == sizeof(region) &&
            region.State == MEM_COMMIT && !(region.Protect & (PAGE_GUARD | PAGE_NOACCESS)) &&
            (region.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
    }

public:
    static constexpr bool applies(bool enabled, int year)
    {
        return enabled && year >= 2015 && year <= 2020;
    }

    static constexpr bool has_sprint(int year, std::string_view track)
    {
        constexpr std::array<std::array<std::string_view, 6>, 6> venues{{
            {"silverstone", "monza", "brazil"},
            {"imola", "austria", "brazil"},
            {"baku", "austria", "spa_francorchamps", "losail", "texas", "brazil"},
            {"shanghai", "miami", "austria", "texas", "brazil", "losail"},
            {"shanghai", "miami", "spa_francorchamps", "texas", "brazil", "losail"},
            {"shanghai", "miami", "montreal", "silverstone", "zandvoort", "singapore"},
        }};
        if (year < 2021 || year > 2026 || track.empty())
            return false;
        const auto &season = venues[year - 2021];
        return std::ranges::find(season, track) != season.end();
    }

    bool set_format(void *track, bool sprint)
    {
        const auto address = reinterpret_cast<uintptr_t>(track);
        const auto desired = static_cast<uint8_t>(sprint);
        uintptr_t record{};
        uint8_t format{};
        // Query the pages directly before reading. The SDK's cached readability
        // result can still describe memory freed during a career transition.
        if (!address || !writable_byte(address) || !writable_byte(address + format_offset) ||
            !swofty::mem::read(address, record) ||
            !swofty::mem::read(address + format_offset, format) || format > 1 || format == desired)
            return false;
        const auto existing = std::ranges::find_if(changed_, [address](const ChangedTrack &entry) {
            return entry.address == address;
        });
        if (existing == changed_.end())
            changed_.push_back({address, record, format, desired});
        else {
            if (existing->record != record || format != existing->written)
                existing->original = format;
            existing->record = record;
            existing->written = desired;
        }
        *reinterpret_cast<uint8_t *>(address + format_offset) = desired;
        return true;
    }

    void restore()
    {
        for (const auto &entry : changed_) {
            uintptr_t record{};
            uint8_t format{};
            if (writable_byte(entry.address) && writable_byte(entry.address + format_offset) &&
                swofty::mem::read(entry.address, record) && record == entry.record &&
                swofty::mem::read(entry.address + format_offset, format) && format == entry.written)
                *reinterpret_cast<uint8_t *>(entry.address + format_offset) = entry.original;
        }
        changed_.clear();
    }
};

}
