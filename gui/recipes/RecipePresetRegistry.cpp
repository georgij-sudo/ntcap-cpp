#include "RecipePresetRegistry.h"

#include <cstring>
#include <random>
#include <utility>
#include <vector>
#include <cmath>

namespace ntcap {

namespace {

using Cells = std::vector<std::pair<int, int>>;

void stampCentered(Planes& planes, size_t planeIdx,
                   const Cells& cells, int patW, int patH,
                   uint8_t value = 1)
{
    auto fw = static_cast<int>(planes.width());
    auto fh = static_cast<int>(planes.height());
    int ox = (fw - patW) / 2;
    int oy = (fh - patH) / 2;
    size_t stride = planes.width();
    uint8_t* dst = planes.data() + planeIdx * planes.size();

    for (auto [cx, cy] : cells) {
        int fx = ox + cx;
        int fy = oy + cy;
        if (fx >= 0 && fx < fw && fy >= 0 && fy < fh)
            dst[static_cast<size_t>(fy) * stride + static_cast<size_t>(fx)] = value;
    }
}

void randomFill(Planes& planes, size_t planeIdx,
                double density, uint8_t maxVal)
{
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> coin(0.0, 1.0);
    std::uniform_int_distribution<int> val(0, static_cast<int>(maxVal));

    uint8_t* dst = planes.data() + planeIdx * planes.size();
    for (size_t i = 0; i < planes.size(); ++i) {
        if (coin(rng) < density)
            dst[i] = static_cast<uint8_t>(val(rng));
    }
}

void randomFillCircle(Planes& planes, size_t planeIdx,
                      double radiusFrac, double density, uint8_t maxVal)
{
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> coin(0.0, 1.0);
    std::uniform_int_distribution<int> val(0, static_cast<int>(maxVal));

    int fw = static_cast<int>(planes.width());
    int fh = static_cast<int>(planes.height());
    double cx = fw / 2.0;
    double cy = fh / 2.0;
    double rx = cx * radiusFrac;
    double ry = cy * radiusFrac;

    uint8_t* dst = planes.data() + planeIdx * planes.size();
    size_t stride = planes.width();

    for (int y = 0; y < fh; ++y) {
        for (int x = 0; x < fw; ++x) {
            double dx = (x - cx) / rx;
            double dy = (y - cy) / ry;
            if (dx * dx + dy * dy <= 1.0 && coin(rng) < density)
                dst[static_cast<size_t>(y) * stride + static_cast<size_t>(x)]
                    = static_cast<uint8_t>(val(rng));
        }
    }
}

void fillRow(Planes& planes, size_t planeIdx, size_t row, uint8_t value)
{
    uint8_t* dst = planes.data() + planeIdx * planes.size();
    size_t W = planes.width();
    for (size_t x = 0; x < W; ++x)
        dst[row * W + x] = value;
}

void randomFillFirstRowBand(Planes& planes, size_t planeIdx,
                            size_t from, size_t to)
{
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> coin(0, 1);
    uint8_t* dst = planes.data() + planeIdx * planes.size();
    for (size_t x = from; x < to; ++x)
        dst[x] = static_cast<uint8_t>(coin(rng));
}

Cells gliderCells()
{
    return {{1,0},{2,1},{0,2},{1,2},{2,2}};
}

Cells rPentominoCells()
{
    return {{1,0},{2,0},{0,1},{1,1},{1,2}};
}

Cells acornCells()
{
    return {{1,0},{3,1},{0,2},{1,2},{4,2},{5,2},{6,2}};
}

Cells diehardCells()
{
    return {{6,0},{0,1},{1,1},{1,2},{5,2},{6,2},{7,2}};
}

Cells gosperGunCells()
{
    return {
        {0,4},{0,5},{1,4},{1,5},
        {10,4},{10,5},{10,6},{11,3},{11,7},
        {12,2},{12,8},{13,2},{13,8},{14,5},
        {15,3},{15,7},{16,4},{16,5},{16,6},{17,5},
        {20,2},{20,3},{20,4},{21,2},{21,3},{21,4},
        {22,1},{22,5},{24,0},{24,1},{24,5},{24,6},
        {34,2},{34,3},{35,2},{35,3}
    };
}

Cells cross5Cells()
{
    return {{2,0},{2,1},{0,2},{1,2},{2,2},{3,2},{4,2},{2,3},{2,4}};
}

} // anonymous namespace

// ── dispatch ────────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::presetsFor(const QString& recipeName)
{
    if (recipeName == QStringLiteral("Life"))
        return lifePresets();
    if (recipeName == QStringLiteral("LifeHistory"))
        return lifeHistoryPresets();
    if (recipeName == QStringLiteral("Parity"))
        return parityPresets();
    if (recipeName == QStringLiteral("TubeWorms"))
        return tubeWormsPresets();
    if (recipeName == QStringLiteral("StableTubeWorms"))
        return stableTubeWormsPresets();
    if (recipeName == QStringLiteral("TimeTunnel"))
        return timeTunnelPresets();
    if (recipeName.startsWith(QStringLiteral("CyclicRank")))
        return cyclicRankPresets();
    if (recipeName == QStringLiteral("BrianBrain"))
        return brianBrainPresets();
    if (recipeName == QStringLiteral("Histogram"))
        return histogramPresets();
    if (recipeName == QStringLiteral("Scarves"))
        return scarvesPresets();
    return {};
}

// ── Life ────────────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::lifePresets()
{
    QList<RecipePreset> out;

    {
        Planes p(100, 100, 1);
        stampCentered(p, 0, gliderCells(), 3, 3);
        out.append({QStringLiteral("Glider"),
                    QStringLiteral("Smallest spaceship. Period 4, moves SE.\n"
                                   "Field: 100x100."),
                    std::move(p)});
    }
    {
        Planes p(152, 152, 1);
        stampCentered(p, 0, rPentominoCells(), 3, 3);
        out.append({QStringLiteral("R-pentomino"),
                    QStringLiteral("Methuselah: 5 cells, stabilises after 1103 generations.\n"
                                   "Field: 152x152."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 1);
        stampCentered(p, 0, acornCells(), 7, 3);
        out.append({QStringLiteral("Acorn"),
                    QStringLiteral("Methuselah: 7 cells, runs for 5206 generations.\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }
    {
        Planes p(148, 148, 1);
        stampCentered(p, 0, diehardCells(), 8, 3);
        out.append({QStringLiteral("Diehard"),
                    QStringLiteral("Vanishes completely after 130 generations.\n"
                                   "Field: 148x148."),
                    std::move(p)});
    }
    {
        Planes p(120, 80, 1);
        stampCentered(p, 0, gosperGunCells(), 36, 9);
        out.append({QStringLiteral("Gosper Glider Gun"),
                    QStringLiteral("First known finite pattern with unbounded growth.\n"
                                   "Period-30 gun. Field: 120x80."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 1);
        randomFill(p, 0, 0.25, 1);
        out.append({QStringLiteral("Random 25%"),
                    QStringLiteral("Random fill with 25% density.\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }

    return out;
}

// ── Life History ────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::lifeHistoryPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(100, 100, 2);
        stampCentered(p, 0, gliderCells(), 3, 3);
        out.append({QStringLiteral("Glider"),
                    QStringLiteral("Glider on plane 0; history plane empty.\n"
                                   "Field: 100x100, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(152, 152, 2);
        stampCentered(p, 0, rPentominoCells(), 3, 3);
        out.append({QStringLiteral("R-pentomino"),
                    QStringLiteral("R-pentomino on plane 0.\n"
                                   "Field: 152x152, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 2);
        randomFill(p, 0, 0.20, 1);
        out.append({QStringLiteral("Random 20%"),
                    QStringLiteral("Random fill on plane 0, 20% density.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }

    return out;
}

// ── Parity ──────────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::parityPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(200, 200, 1);
        stampCentered(p, 0, {{0,0}}, 1, 1);
        out.append({QStringLiteral("Single pixel"),
                    QStringLiteral("One cell in the centre.\n"
                                   "Produces a growing diamond (Sierpinski-like).\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 1);
        stampCentered(p, 0, cross5Cells(), 5, 5);
        out.append({QStringLiteral("Cross 5x5"),
                    QStringLiteral("Plus-sign seed in the centre.\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 1);
        randomFill(p, 0, 0.10, 1);
        out.append({QStringLiteral("Random 10%"),
                    QStringLiteral("Sparse random fill, 10% density.\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }

    return out;
}

// ── TubeWorms ───────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::tubeWormsPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(148, 148, 3);
        Cells block;
        for (int y = 0; y < 5; ++y)
            for (int x = 0; x < 5; ++x)
                block.push_back({x, y});
        stampCentered(p, 0, block, 5, 5);
        out.append({QStringLiteral("Central seed"),
                    QStringLiteral("5x5 block of worms (plane 0).\n"
                                   "Alarm and timer planes empty.\n"
                                   "Field: 148x148, 3 planes."),
                    std::move(p)});
    }
    {
        Planes p(148, 148, 3);
        randomFill(p, 0, 0.15, 1);
        out.append({QStringLiteral("Random worms 15%"),
                    QStringLiteral("Random fill on plane 0, 15% density.\n"
                                   "Field: 148x148, 3 planes."),
                    std::move(p)});
    }

    return out;
}

// ── StableTubeWorms ─────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::stableTubeWormsPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(148, 148, 3);
        Cells block;
        for (int y = 0; y < 5; ++y)
            for (int x = 0; x < 5; ++x)
                block.push_back({x, y});
        stampCentered(p, 0, block, 5, 5);
        out.append({QStringLiteral("Central seed"),
                    QStringLiteral("5x5 block of worms (plane 0).\n"
                                   "Field: 148x148, 3 planes."),
                    std::move(p)});
    }
    {
        Planes p(148, 148, 3);
        randomFill(p, 0, 0.15, 1);
        out.append({QStringLiteral("Random worms 15%"),
                    QStringLiteral("Random fill on plane 0, 15% density.\n"
                                   "Field: 148x148, 3 planes."),
                    std::move(p)});
    }

    return out;
}

// ── TimeTunnel ──────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::timeTunnelPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(200, 200, 2);
        stampCentered(p, 0, cross5Cells(), 5, 5);
        out.append({QStringLiteral("Central cross"),
                    QStringLiteral("Plus-sign on plane 0, plane 1 empty.\n"
                                   "Swap planes to reverse time.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 2);
        stampCentered(p, 0, {{0,0}}, 1, 1);
        out.append({QStringLiteral("Single pixel"),
                    QStringLiteral("One cell on plane 0.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 2);
        randomFill(p, 0, 0.20, 1);
        out.append({QStringLiteral("Random 20%"),
                    QStringLiteral("Random fill on plane 0, 20% density.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }

    return out;
}

// ── CyclicRank ──────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::cyclicRankPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(200, 200, 1);
        randomFillCircle(p, 0, 0.6, 0.80, 3);
        out.append({QStringLiteral("Random circle (4 states)"),
                    QStringLiteral("Circle ~60% of field, states 0..3, 80% density.\n"
                                   "Recommendation: statesCount = 4.\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 1);
        randomFillCircle(p, 0, 0.6, 0.80, 15);
        out.append({QStringLiteral("Random circle (16 states)"),
                    QStringLiteral("Circle ~60% of field, states 0..15, 80% density.\n"
                                   "Recommendation: statesCount = 16.\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 1);
        randomFill(p, 0, 1.0, 3);
        out.append({QStringLiteral("Full random (4 states)"),
                    QStringLiteral("Every cell gets a random state 0..3.\n"
                                   "Recommendation: statesCount = 4.\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 1);
        randomFill(p, 0, 1.0, 15);
        out.append({QStringLiteral("Full random (16 states)"),
                    QStringLiteral("Every cell gets a random state 0..15.\n"
                                   "Recommendation: statesCount = 16.\n"
                                   "Field: 200x200."),
                    std::move(p)});
    }

    return out;
}

// ── Brian's Brain ───────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::brianBrainPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(200, 200, 2);
        randomFill(p, 0, 0.20, 1);
        out.append({QStringLiteral("Random 20%"),
                    QStringLiteral("Random firing cells on plane 0, 20% density.\n"
                                   "Plane 1 empty (no dying cells initially).\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 2);
        randomFillCircle(p, 0, 0.5, 0.30, 1);
        out.append({QStringLiteral("Circle 30%"),
                    QStringLiteral("Circular seed ~50% radius, 30% density on plane 0.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 2);
        randomFill(p, 0, 0.50, 1);
        out.append({QStringLiteral("Random 50%"),
                    QStringLiteral("Dense random firing cells on plane 0, 50% density.\n"
                                   "Creates chaotic waves.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(100, 100, 2);
        Cells line;
        for (int x = 0; x < 20; ++x)
            line.push_back({x, 0});
        stampCentered(p, 0, line, 20, 1);
        out.append({QStringLiteral("Horizontal line"),
                    QStringLiteral("20-cell horizontal line in the centre.\n"
                                   "Field: 100x100, 2 planes."),
                    std::move(p)});
    }

    return out;
}

// ── Histogram ───────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::histogramPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(200, 200, 2);
        randomFill(p, 0, 0.20, 1);
        fillRow(p, 1, 0, 1);
        fillRow(p, 1, p.height() - 1, 1);
        out.append({QStringLiteral("Standard (20%)"),
                    QStringLiteral("Plane 0: random 20% particles.\n"
                                   "Plane 1: walls on top and bottom rows.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 2);
        randomFill(p, 0, 0.40, 1);
        fillRow(p, 1, 0, 1);
        fillRow(p, 1, p.height() - 1, 1);
        out.append({QStringLiteral("Dense (40%)"),
                    QStringLiteral("Plane 0: random 40% particles.\n"
                                   "Plane 1: walls on top and bottom rows.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(100, 100, 2);
        randomFill(p, 0, 0.10, 1);
        fillRow(p, 1, 0, 1);
        fillRow(p, 1, p.height() - 1, 1);
        out.append({QStringLiteral("Small sparse (10%)"),
                    QStringLiteral("Plane 0: random 10% particles.\n"
                                   "Plane 1: walls on top and bottom rows.\n"
                                   "Field: 100x100, 2 planes."),
                    std::move(p)});
    }

    return out;
}

// ── Scarves ─────────────────────────────────────────────

QList<RecipePreset> RecipePresetRegistry::scarvesPresets()
{
    QList<RecipePreset> out;

    {
        Planes p(200, 200, 2);
        size_t from = 2 * p.width() / 5;
        size_t to   = 3 * p.width() / 5;
        randomFillFirstRowBand(p, 0, from, to);
        randomFillFirstRowBand(p, 1, from, to);
        out.append({QStringLiteral("Standard"),
                    QStringLiteral("Both planes seeded in middle fifth of first row.\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(300, 300, 2);
        size_t from = 2 * p.width() / 5;
        size_t to   = 3 * p.width() / 5;
        randomFillFirstRowBand(p, 0, from, to);
        randomFillFirstRowBand(p, 1, from, to);
        out.append({QStringLiteral("Large"),
                    QStringLiteral("Same pattern, larger field.\n"
                                   "Field: 300x300, 2 planes."),
                    std::move(p)});
    }
    {
        Planes p(200, 200, 2);
        size_t from = p.width() / 4;
        size_t to   = 3 * p.width() / 4;
        randomFillFirstRowBand(p, 0, from, to);
        randomFillFirstRowBand(p, 1, from, to);
        out.append({QStringLiteral("Wide band"),
                    QStringLiteral("Wider initial band (middle half of first row).\n"
                                   "Field: 200x200, 2 planes."),
                    std::move(p)});
    }

    return out;
}

} // namespace ntcap