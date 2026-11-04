#include "KeyMappingsData.h"

namespace ntcap {
namespace {

// Sources of truth for these rows:
//   shell/KeyboardController.cpp  — zoom, pan, display-mode cycling
//   shell/SimulationActions.cpp   — play/pause, step forward, fit to window
//   shell/MouseController.cpp     — wheel zoom, left-drag pan
constexpr KeyMappingRow kSimulationRows[] = {
    { "P",      KeyMappingAction::PlayPause },
    { "N",      KeyMappingAction::StepForward },
    { "Ctrl+0", KeyMappingAction::FitPlaneToWindow },
};

// Ctrl+= is registered in KeyboardController as an alias of Ctrl++ and is not
// listed twice here.
constexpr KeyMappingRow kCanvasRows[] = {
    { "Ctrl++",  KeyMappingAction::ZoomIn },
    { "Ctrl+-",  KeyMappingAction::ZoomOut },
    { "Left",    KeyMappingAction::PanLeft },
    { "Right",   KeyMappingAction::PanRight },
    { "Up",      KeyMappingAction::PanUp },
    { "Down",    KeyMappingAction::PanDown },
    { "M",       KeyMappingAction::NextDisplayMode },
    { "Shift+M", KeyMappingAction::PreviousDisplayMode },
};

constexpr KeyMappingRow kMouseRows[] = {
    { "Wheel",     KeyMappingAction::ZoomAtCursor },
    { "Left drag", KeyMappingAction::PanByDragging },
};

constexpr KeyMappingGroup kGroups[] = {
    KeyMappingGroup::Simulation,
    KeyMappingGroup::Canvas,
    KeyMappingGroup::Mouse,
};

} // namespace

std::span<const KeyMappingGroup> keyMappingGroups() noexcept
{
    return kGroups;
}

std::span<const KeyMappingRow> keyMappingRows(KeyMappingGroup group) noexcept
{
    switch (group) {
    case KeyMappingGroup::Simulation:
        return kSimulationRows;
    case KeyMappingGroup::Canvas:
        return kCanvasRows;
    case KeyMappingGroup::Mouse:
        return kMouseRows;
    }

    return {};
}

} // namespace ntcap
