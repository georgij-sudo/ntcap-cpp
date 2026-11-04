#pragma once

// Single source of truth for the "Key Mappings" reference window.
//
// MAINTENANCE RULE: whenever a keyboard or mouse binding is added, removed or
// changed in KeyboardController, MouseController or SimulationActions, the
// corresponding row here MUST be updated in the same change, so the reference
// never drifts from the actual behaviour.
//
// Only bindings that actually work are listed. Disabled actions (for example
// the "Fullscreen" action bound to F11 in MainWindow) must stay out of this
// table.
//
// This header is deliberately Qt-free so it can be unit-tested without a
// QApplication.

#include <cstddef>
#include <span>
#include <string_view>

namespace ntcap {

enum class KeyMappingGroup {
    Simulation,
    Canvas,
    Mouse,
};

enum class KeyMappingAction {
    PlayPause,
    StepForward,
    FitPlaneToWindow,
    ZoomIn,
    ZoomOut,
    PanLeft,
    PanRight,
    PanUp,
    PanDown,
    NextDisplayMode,
    PreviousDisplayMode,
    ZoomAtCursor,
    PanByDragging,
};

struct KeyMappingRow {
    std::string_view key;    // gesture as shown to the user, e.g. "Ctrl++"
    KeyMappingAction action;
};

// Groups in display order.
std::span<const KeyMappingGroup> keyMappingGroups() noexcept;

// Rows of one group.
std::span<const KeyMappingRow> keyMappingRows(KeyMappingGroup group) noexcept;

} // namespace ntcap
