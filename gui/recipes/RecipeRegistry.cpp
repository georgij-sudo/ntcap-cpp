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

namespace ntcap {

QList<RecipeDescriptor> RecipeRegistry::builtIn()
{
    QList<RecipeDescriptor> list;

    list.append(RecipeDescriptor{
        QStringLiteral("Life"),
        QStringLiteral(
            "<h2>Conway's Game of Life</h2>"
            "<p>Classic totalistic rule B3/S23 on Moore neighbourhood.</p>"
            "<h3>Rule</h3>"
            "<p>Birth: exactly 3 alive neighbours.<br>"
            "Survival: 2 or 3 alive neighbours.<br>"
            "Otherwise: death.</p>"
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
        QStringLiteral("Parity"),
        QStringLiteral(
            "<h2>Parity</h2>"
            "<p>Cell flips when the number of alive Moore neighbours is odd.</p>"
            "<p>Requires <b>1</b> plane.</p>"),
        1,
        nullptr,
        [](Planes& p) -> std::unique_ptr<Automaton> {
            return std::make_unique<Parity>(p);
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
            "<p>Reversible two-plane automaton (Von Neumann).</p>"
            "<h3>Rule</h3>"
            "<p>"
            "count = Σ VN(plane₀) + center<br>"
            "new₀ = (count ≠ 0 ∧ count ≠ 5) ? 1 : 0<br>"
            "new₀ = new₀ ⊕ plane₁[i]<br>"
            "new₁ = plane₀[i]"
            "</p>"
            "<p><b>Tip:</b> Use <i>Planes → Swap Planes (0 ↔ 1)</i> "
            "to reverse time.</p>"
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