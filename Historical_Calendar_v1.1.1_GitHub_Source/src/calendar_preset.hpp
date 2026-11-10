#pragma once

#include <array>
#include <span>

#include <swofty/mem.hpp>

namespace historical_calendar {
namespace mem = swofty::mem;

// Layouts read by Game_SeasonLength_ApplyPreset on the supported game build.
constexpr uintptr_t config_offset = 0x290;
constexpr uintptr_t entity_offset = 0x278;
constexpr uintptr_t track_lists_offset = 0xf58;
constexpr uintptr_t list_rows_begin = 0xb8;
constexpr uintptr_t list_rows_end = 0xc0;
constexpr uintptr_t active_begin = 0x2b0;
constexpr uintptr_t active_end = 0x2b8;
constexpr uintptr_t active_count = 0x2c8;
constexpr uintptr_t list_row_size = 0x28;
constexpr uintptr_t entry_size = 0x28;

struct Preset {
    uint32_t max_tracks;
    uint32_t padding;
    const char *list_name;
    uintptr_t label;
    uint8_t type;
    std::array<uint8_t, 0x17> other;
};
static_assert(sizeof(Preset) == 0x30 && offsetof(Preset, type) == 0x18);

struct PresetTable {
    std::array<uint8_t, 0xc0> other;
    uintptr_t begin;
    uintptr_t end;
};
static_assert(offsetof(PresetTable, begin) == 0xc0 && offsetof(PresetTable, end) == 0xc8);

bool calendar_matches(uintptr_t begin, uintptr_t end, std::span<const uint8_t> expected)
{
    if (!begin || end <= begin || end - begin != expected.size() * entry_size)
        return false;
    for (size_t i = 0; i < expected.size(); ++i) {
        uint8_t event{};
        if (!mem::read(begin + i * entry_size + 8, event) || event != expected[i])
            return false;
    }
    return true;
}

uintptr_t named_list_row(uintptr_t component, std::string_view name)
{
    uintptr_t entity{}, lists{}, begin{}, end{};
    if (!mem::read(component + entity_offset, entity) || !entity ||
        !mem::read(entity + track_lists_offset, lists) || !lists ||
        !mem::read(lists + list_rows_begin, begin) || !mem::read(lists + list_rows_end, end) ||
        !begin || end <= begin || (end - begin) % list_row_size || (end - begin) / list_row_size > 128)
        return 0;

    for (auto row = begin; row < end; row += list_row_size) {
        uintptr_t label{};
        std::string text;
        if (mem::read(row, label) && label && mem::c_string(label, text, 64) && text == name)
            return row;
    }
    return 0;
}

bool named_list_matches(uintptr_t component, std::string_view name, std::span<const uint8_t> events)
{
    const auto row = named_list_row(component, name);
    uintptr_t begin{}, end{};
    return row && mem::read(row + 0x10, begin) && mem::read(row + 0x18, end) &&
        calendar_matches(begin, end, events);
}

constexpr std::array<uint8_t, 23> preset_body_signature{
    0x41, 0x57, 0x41, 0x56, 0x41, 0x55, 0x41, 0x54, 0x56, 0x57, 0x55, 0x53,
    0x48, 0x83, 0xec, 0x48, 0x48, 0x8b, 0x81, 0x90, 0x02, 0x00, 0x00,
};

#ifdef DEJW_EA_TEST
bool preset_layout_verified = false;

bool verify_test_preset_layout()
{
    constexpr uintptr_t steam_entry_rva = 0xbc9fc0;
    constexpr uintptr_t ea_entry_rva = 0xbc8930;
    const auto base = swofty::api->module_base();
    const auto entry = swofty::api->resolve("Game_SeasonLength_ApplyPreset");
    const auto rva = base && entry >= base ? entry - base : 0;
    if (rva != steam_entry_rva && rva != ea_entry_rva) {
        swofty::log("[swofty-historical-calendar] EA/Steam test: unsupported ApplyPreset entry RVA 0x%llx",
                    static_cast<unsigned long long>(rva));
        return false;
    }

    // Read the original jump before registering the mixin. The two storefronts
    // put the body at different addresses, but it must use the same layout.
    std::array<uint8_t, 5> jump{};
    if (!mem::read(entry, jump) || jump[0] != 0xe9) {
        swofty::log("[swofty-historical-calendar] EA/Steam test: could not read the original ApplyPreset jump");
        return false;
    }
    int32_t displacement{};
    std::memcpy(&displacement, jump.data() + 1, sizeof(displacement));
    const auto body_rva = static_cast<int64_t>(rva + jump.size()) + displacement;
    if (body_rva <= 0)
        return false;
    const auto body = base + static_cast<uintptr_t>(body_rva);
    MEMORY_BASIC_INFORMATION region{};
    std::array<uint8_t, preset_body_signature.size()> actual{};
    preset_layout_verified = VirtualQuery(reinterpret_cast<void *>(body), &region, sizeof(region)) == sizeof(region) &&
        reinterpret_cast<uintptr_t>(region.AllocationBase) == base && mem::read(body, actual) &&
        actual == preset_body_signature;
    swofty::log("[swofty-historical-calendar] EA/Steam test: %s ApplyPreset entry RVA 0x%llx, body RVA 0x%llx, layout %s",
                rva == ea_entry_rva ? "EA app" : "Steam", static_cast<unsigned long long>(rva),
                static_cast<unsigned long long>(body_rva), preset_layout_verified ? "verified" : "rejected");
    return preset_layout_verified;
}
#endif

bool supported_preset_layout()
{
#ifdef DEJW_EA_TEST
    return preset_layout_verified;
#else
    const auto base = swofty::api->module_base();
    std::array<uint8_t, preset_body_signature.size()> actual{};
    return base && swofty::api->resolve("Game_SeasonLength_ApplyPreset") == base + 0xbc9fc0 &&
        mem::read(base + 0x98ccd50, actual) && actual == preset_body_signature;
#endif
}

class ScopedPreset {
    PresetTable table_{};
    std::array<Preset, 32> rows_{};
    void *volatile *slot_ = nullptr;
    void *old_table_ = nullptr;

public:
    ScopedPreset() = default;
    ScopedPreset(const ScopedPreset &) = delete;
    ScopedPreset &operator=(const ScopedPreset &) = delete;

    bool install(uintptr_t component, uint8_t preset, const char *original_list,
                 uint32_t original_count, const char *target,
                 std::span<const uint8_t> events)
    {
        uintptr_t original{};
        if (!supported_preset_layout() || !named_list_matches(component, target, events) ||
            !mem::read(component + config_offset, original) || !original || !mem::read(original, table_) ||
            !table_.begin || table_.end <= table_.begin || (table_.end - table_.begin) % sizeof(Preset))
            return false;
        const auto count = (table_.end - table_.begin) / sizeof(Preset);
        if (count > rows_.size())
            return false;

        bool replaced = false;
        for (size_t i = 0; i < count; ++i) {
            auto &row = rows_[i];
            if (!mem::read(table_.begin + i * sizeof(Preset), row))
                return false;
            std::string name;
            if (row.type == preset && row.list_name &&
                mem::c_string(reinterpret_cast<uintptr_t>(row.list_name), name, 64) && name == original_list) {
                if (replaced || row.max_tracks != original_count)
                    return false;
                row.list_name = target;
                row.max_tracks = static_cast<uint32_t>(events.size());
                replaced = true;
            }
        }
        if (!replaced)
            return false;

        // ApplyPreset only reads this table. The copy is visible to this component
        // during the synchronous call, without modifying the game's shared rows.
        const auto address = component + config_offset;
        MEMORY_BASIC_INFORMATION region{};
        if (address % alignof(void *) ||
            VirtualQuery(reinterpret_cast<void *>(address), &region, sizeof(region)) != sizeof(region) ||
            region.State != MEM_COMMIT || (region.Protect & PAGE_GUARD) ||
            !(region.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))
            return false;
        table_.begin = reinterpret_cast<uintptr_t>(rows_.data());
        table_.end = reinterpret_cast<uintptr_t>(rows_.data() + count);
        auto *slot = reinterpret_cast<void *volatile *>(address);
        auto *old_table = reinterpret_cast<void *>(original);
        if (InterlockedCompareExchangePointer(slot, &table_, old_table) != old_table)
            return false;
        slot_ = slot;
        old_table_ = old_table;
        return true;
    }

    ~ScopedPreset()
    {
        if (slot_)
            InterlockedCompareExchangePointer(slot_, old_table_, &table_);
    }
};

} // namespace historical_calendar
