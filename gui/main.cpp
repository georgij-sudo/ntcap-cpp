#include <QApplication>

#include "CellularCanvas.h"
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

    AutomatonController controller(
        std::move(initialPlanes),
        TorusTopology(W, H),
        parityRule
    );
    controller.setInterval(16);

    CellularCanvas canvas(W, H);
    canvas.setPlanes(controller.planes(), 0);
    canvas.setWindowTitle("Parity CA (Von Neumann)");

    // ── Пользовательская цветовая карта ─────────────────
    QMap<uint8_t, QRgb> colorMap;
    colorMap[0] = qRgb(20,  20,  30);    // мёртвая — тёмно-серый
    colorMap[1] = qRgb(0,   200, 120);   // живая   — зелёный
    canvas.setColorMap(colorMap);
    canvas.setDefaultColor(qRgb(255, 0, 0));  // неизвестное — красный (отладка)

    QObject::connect(&controller, &AutomatonController::stepped,
                     &canvas,     &CellularCanvas::refresh);

    canvas.show();
    controller.start();

    return app.exec();
}