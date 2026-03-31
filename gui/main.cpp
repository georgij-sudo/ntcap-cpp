#include <QApplication>

#include "CellularCanvas.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "AutomatonController.h"
#include "Planes.h"
#include "TorusTopology.h"
#include "TableRule.h"

using namespace ntcap;

static constexpr TableRule<6> parityRule(
    0, 1, 0, 1, 0, 1
);

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    constexpr size_t W = 400;
    constexpr size_t H = 400;

    // Начальное состояние: одна плоскость.
    Planes initialPlanes(W, H, 1);
    initialPlanes.data()[H / 2 * W + W / 2] = 1;

    // ── Симуляция (владеет Planes, работает в отдельном потоке) ──
    AutomatonController automaton(
        std::move(initialPlanes),
        TorusTopology(W, H),
        parityRule
    );
    automaton.setRenderInterval(8);

    // ── Канвас ──────────────────────────────────────────
    CellularCanvas canvas(W, H);
    canvas.setPlanes(automaton.snapshot(), 0);
    canvas.setPlanesMutex(automaton.snapshotMutex());
    canvas.setWindowTitle("Parity CA (threaded)");

    QMap<uint8_t, QRgb> colorMap;
    colorMap[0] = qRgb(20,  20,  30);
    colorMap[1] = qRgb(0,   200, 120);
    canvas.setColorMap(colorMap);
    canvas.setDefaultColor(qRgb(255, 0, 0));

    // ── Ввод ────────────────────────────────────────────
    KeyboardController keyboard(&canvas);
    MouseController    mouse(&canvas);

    // ── Связи ───────────────────────────────────────────
    QObject::connect(&automaton, &AutomatonController::stepped,
                     &canvas,    &CellularCanvas::refresh,
                     Qt::QueuedConnection);

    canvas.show();
    automaton.start();

    const int result = app.exec();

    automaton.stop();
    return result;
}