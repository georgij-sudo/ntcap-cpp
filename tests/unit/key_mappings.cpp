#include <gtest/gtest.h>

#include <algorithm>
#include <string_view>
#include <vector>

#include "KeyMappingsData.h"

namespace {

using ntcap::KeyMappingAction;
using ntcap::KeyMappingGroup;
using ntcap::KeyMappingRow;

std::vector<std::string_view> keysOf(KeyMappingGroup group)
{
    std::vector<std::string_view> keys;
    for (const KeyMappingRow& row : ntcap::keyMappingRows(group)) {
        keys.push_back(row.key);
    }
    return keys;
}

bool contains(const std::vector<std::string_view>& keys, std::string_view key)
{
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

} // namespace

TEST(KeyMappings, ExposesGroupsInOrder)
{
    const std::span<const KeyMappingGroup> groups = ntcap::keyMappingGroups();

    ASSERT_EQ(groups.size(), 3u);
    EXPECT_EQ(groups[0], KeyMappingGroup::Simulation);
    EXPECT_EQ(groups[1], KeyMappingGroup::Canvas);
    EXPECT_EQ(groups[2], KeyMappingGroup::Mouse);
}

TEST(KeyMappings, SimulationGroupListsWorkingBindings)
{
    const std::vector<std::string_view> keys = keysOf(KeyMappingGroup::Simulation);

    EXPECT_EQ(keys.size(), 3u);
    EXPECT_TRUE(contains(keys, "P"));
    EXPECT_TRUE(contains(keys, "N"));
    EXPECT_TRUE(contains(keys, "Ctrl+0"));
}

TEST(KeyMappings, CanvasGroupListsWorkingBindings)
{
    const std::vector<std::string_view> keys = keysOf(KeyMappingGroup::Canvas);

    EXPECT_EQ(keys.size(), 8u);
    EXPECT_TRUE(contains(keys, "Ctrl++"));
    EXPECT_TRUE(contains(keys, "Ctrl+-"));
    EXPECT_TRUE(contains(keys, "Left"));
    EXPECT_TRUE(contains(keys, "Right"));
    EXPECT_TRUE(contains(keys, "Up"));
    EXPECT_TRUE(contains(keys, "Down"));
    EXPECT_TRUE(contains(keys, "M"));
    EXPECT_TRUE(contains(keys, "Shift+M"));
}

TEST(KeyMappings, MouseGroupListsWorkingGestures)
{
    const std::vector<std::string_view> gestures = keysOf(KeyMappingGroup::Mouse);

    EXPECT_EQ(gestures.size(), 2u);
    EXPECT_TRUE(contains(gestures, "Wheel"));
    EXPECT_TRUE(contains(gestures, "Left drag"));
}

TEST(KeyMappings, DisabledBindingsAreNotDocumented)
{
    for (KeyMappingGroup group : ntcap::keyMappingGroups()) {
        const std::vector<std::string_view> keys = keysOf(group);
        // F11 Fullscreen is bound to a disabled action, so it must not appear.
        EXPECT_FALSE(contains(keys, "F11"));
    }
}

TEST(KeyMappings, RowsAreNeverEmptyAndHaveUniqueKeysPerGroup)
{
    for (KeyMappingGroup group : ntcap::keyMappingGroups()) {
        const std::vector<std::string_view> keys = keysOf(group);

        for (std::string_view key : keys) {
            EXPECT_FALSE(key.empty());
        }

        std::vector<std::string_view> sorted = keys;
        std::sort(sorted.begin(), sorted.end());
        EXPECT_EQ(std::adjacent_find(sorted.begin(), sorted.end()), sorted.end());
    }
}

TEST(KeyMappings, EveryRowCarriesAnAction)
{
    for (KeyMappingGroup group : ntcap::keyMappingGroups()) {
        for (const KeyMappingRow& row : ntcap::keyMappingRows(group)) {
            // The action enum is exhaustive, so a valid row can only ever
            // reference one of the declared values.
            const auto value = static_cast<int>(row.action);
            EXPECT_GE(value, static_cast<int>(KeyMappingAction::PlayPause));
            EXPECT_LE(value, static_cast<int>(KeyMappingAction::PanByDragging));
        }
    }
}
