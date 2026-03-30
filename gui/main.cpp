#include <QApplication>
#include <QTimer>

#include "CellularCanvas.h"
#include "Planes.h"
#include "TorusTopology.h"
#include "VonNeumannNeighbourhood.h"
#include "TableRule.h"

using namespace ntcap;

// ─── Правило: «Чётность» (Parity / XOR rule) ──────────────────────
// Окрестность фон Неймана, 2 состояния.
// Индекс в таблице = center + north + east + south + west  (от 0 до 5).
// Новое состояние = 1, если сумма нечётна (XOR-подобное поведение).
static constexpr TableRule<6> parityRule(
    0,  // sum=0 → 0
    1,  // sum=1 → 1
    0,  // sum=2 → 0
    1,  // sum=3 → 1
    0,  // sum=4 → 0
    1   // sum=5 → 1
);

// ─── Один шаг симуляции ─────────────────────────────────────────────
void step(Planes& planes, TorusTopology& topo)
{
    const size_t W = planes.width();
    const size_t H = planes.height();

    // Плоскость 0 — текущее поколение, плоскость 1 — следующее.
    const std::span<const uint8_t> cur = planes.view(0);
    // Получаем указатель на начало плоскости 1 для записи.
    uint8_t* next = planes.data() + planes.size();  // plane 1

    for (size_t y = 0; y < H; ++y) {
        for (size_t x = 0; x < W; ++x) {
            const size_t idx = y * W + x;

            // Нормализуем координаты соседей через тор.
            UCoord2D nc = topo.normalize({static_cast<ptrdiff_t>(x),
                                          static_cast<ptrdiff_t>(y) - 1});
            UCoord2D ec = topo.normalize({static_cast<ptrdiff_t>(x) + 1,
                                          static_cast<ptrdiff_t>(y)});
            UCoord2D sc = topo.normalize({static_cast<ptrdiff_t>(x),
                                          static_cast<ptrdiff_t>(y) + 1});
            UCoord2D wc = topo.normalize({static_cast<ptrdiff_t>(x) - 1,
                                          static_cast<ptrdiff_t>(y)});

            VonNeumannN neighborhood = VonNeumannNBuilder()
                .center(cur[idx])
                .north (cur[nc.y * W + nc.x])
                .east  (cur[ec.y * W + ec.x])
                .south (cur[sc.y * W + sc.x])
                .west  (cur[wc.y * W + wc.x])
                .build();

            // Индекс = сумма всех клеток окрестности.
            const size_t ruleIndex = neighborhood.count();
            next[idx] = parityRule(ruleIndex);
        }
    }

    // Копируем плоскость 1 → плоскость 0.
    std::memcpy(planes.data(), next, planes.size());
}

// ─── main ───────────────────────────────────────────────────────────
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    constexpr size_t W = 400;
    constexpr size_t H = 400;

    Planes planes(W, H, 2);  // 2 плоскости: текущая + буфер
    TorusTopology topo(W, H);

    // Начальное состояние: одна живая клетка по центру.
    planes.data()[H / 2 * W + W / 2] = 1;

    CellularCanvas canvas(W, H);
    canvas.setPlanes(&planes, /*planeIndex=*/0);
    canvas.setWindowTitle("Parity CA (Von Neumann)");
    canvas.show();

    // Таймер симуляции: ~60 шагов/сек.
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        step(planes, topo);
        canvas.refresh();
    });
    timer.start(16);  // ~60 FPS

    return app.exec();
}