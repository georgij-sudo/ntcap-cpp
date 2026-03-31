#include <QApplication>

#include "CellularCanvas.h"
#include "KeyboardController.h"
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

    Planes initialPlanes(W, H, 2);
    initialPlanes.data()[H / 2 * W + W / 2] = 1;

    // ── Симуляция ───────────────────────────────────────
    AutomatonController automaton(
        std::move(initialPlanes),
        TorusTopology(W, H),
        parityRule
    );
    automaton.setInterval(16);

    // ── Канвас ──────────────────────────────────────────
    CellularCanvas canvas(W, H);
    canvas.setPlanes(automaton.planes(), 0);
    canvas.setWindowTitle("Parity CA — wheel/Ctrl+±: zoom, arrows/drag: pan");

    QMap<uint8_t, QRgb> colorMap;
    colorMap[0] = qRgb(20,  20,  30);
    colorMap[1] = qRgb(0,   200, 120);
    canvas.setColorMap(colorMap);
    canvas.setDefaultColor(qRgb(255, 0, 0));

    // ── Клавиатура ──────────────────────────────────────
    KeyboardController keyboard(&canvas);

    // ── Связи ───────────────────────────────────────────
    QObject::connect(&automaton, &AutomatonController::stepped,
                     &canvas,    &CellularCanvas::refresh);

    canvas.show();
    automaton.start();

    return app.exec();
}