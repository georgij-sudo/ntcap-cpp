#pragma once

#include "RecipePreset.h"
#include <QList>
#include <QString>

namespace ntcap {

/**
 * @brief Catalogue of built-in per-recipe initialisation presets.
 *
 * Stateless — every call builds and returns a fresh list.
 */
class RecipePresetRegistry {
public:
    RecipePresetRegistry() = delete;

    /**
     * @brief Return presets for the recipe identified by Automaton::name().
     * @param recipeName  exact string returned by Automaton::name().
     * @return preset list (empty if recipe is unknown).
     */
    static QList<RecipePreset> presetsFor(const QString& recipeName);

private:
    static QList<RecipePreset> lifePresets();
    static QList<RecipePreset> lifeHistoryPresets();
    static QList<RecipePreset> parityPresets();
    static QList<RecipePreset> tubeWormsPresets();
    static QList<RecipePreset> stableTubeWormsPresets();
    static QList<RecipePreset> timeTunnelPresets();
    static QList<RecipePreset> cyclicRankPresets();
    static QList<RecipePreset> brianBrainPresets();
    static QList<RecipePreset> histogramPresets();
    static QList<RecipePreset> scarvesPresets();
};

} // namespace ntcap