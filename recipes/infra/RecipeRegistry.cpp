#include "RecipeRegistry.h"
#include "RecipeParameterWidget.h"
#include "CyclicRankParameterWidget.h"

#include "Life.h"
#include "LifeHistory.h"
#include "Parity.h"
#include "TubeWorms.h"
#include "StableTubeWorms.h"
#include "TimeTunnel.h"
#include "CyclicRank.h"
#include "BrianBrain.h"
#include "Histogram.h"
#include "Scarves.h"

namespace ntcap {

QList<RecipeDescriptor> RecipeRegistry::builtIn()
{
    QList<RecipeDescriptor> list;

    list.append(RecipeDescriptor{
        QStringLiteral("Life"),
        QStringLiteral(
            "<h2>Conway's Game of Life</h2>"
            "<p>Classic totalistic rule B3/S23 on Moore neighbourhood.</p>"
            "<p>Requires <b>1</b> plane.</p>"),
        1,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<Life>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("LifeHistory"),
        QStringLiteral(
            "<h2>Life with History</h2>"
            "<p>Conway's Life (B3/S23). Plane 1 stores previous generation.</p>"
            "<p>Requires <b>2</b> planes.</p>"),
        2,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<LifeHistory>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("BrianBrain"),
        QStringLiteral(
            "<h2>Brian's Brain</h2>"
            "<p>Three-state automaton (ready/firing/dying) on Moore neighbourhood. "
            "Encoded across 2 planes.</p>"
            "<p>Requires <b>2</b> planes.</p>"),
        2,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<BrianBrain>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("Parity"),
        QStringLiteral(
            "<h2>Parity</h2>"
            "<p>Cell is alive if sum of itself and Von Neumann neighbours is odd.</p>"
            "<p>Requires <b>1</b> plane.</p>"),
        1,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<Parity>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("Histogram"),
        QStringLiteral(
            "<h2>Histogram</h2>"
            "<p>Gravity-like falling particles with static wall barriers. "
            "Von Neumann neighbourhood.</p>"
            "<p>Requires <b>2</b> planes.</p>"),
        2,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<Histogram>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("Scarves"),
        QStringLiteral(
            "<h2>Scarves</h2>"
            "<p>XOR-based weaving pattern. First row evolves, "
            "others shift downward.</p>"
            "<p>Requires <b>2</b> planes.</p>"),
        2,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<Scarves>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("TubeWorms"),
        QStringLiteral(
            "<h2>TubeWorms</h2>"
            "<p>Three-plane automaton: body, alarm, countdown timer.</p>"
            "<p>Requires <b>3</b> planes.</p>"),
        3,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<TubeWorms>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("StableTubeWorms"),
        QStringLiteral(
            "<h2>StableTubeWorms</h2>"
            "<p>TubeWorms variant with a predefined stable alarm mask.</p>"
            "<p>Requires <b>3</b> planes.</p>"),
        3,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<StableTubeWorms>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("TimeTunnel"),
        QStringLiteral(
            "<h2>TimeTunnel</h2>"
            "<p>Reversible two-plane automaton (Von Neumann). "
            "Swap planes to reverse time.</p>"
            "<p>Requires <b>2</b> planes.</p>"),
        2,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<TimeTunnel>(p);
        }
    });

    list.append(RecipeDescriptor{
        QStringLiteral("CyclicRank"),
        QStringLiteral(
            "<h2>CyclicRank</h2>"
            "<p>Cyclic automaton with probabilistic Von Neumann rule.</p>"
            "<p>Requires <b>1</b> plane.</p>"),
        1,
        [](QWidget* parent) -> RecipeParameterWidget* {
            return new CyclicRankParameterWidget(parent);
        },
        nullptr
    });

    return list;
}

} // namespace ntcap