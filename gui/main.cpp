#include <QApplication>
#include "CellularCanvas.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "AutomatonController.h"
#include "Parity.h"
#include "Planes.h"
#include "TorusTopology.h"

using namespace ntcap;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    constexpr size_t W = 400;
    constexpr size_t H = 400;

    // Начальное состояние: одна плоскость.
    Planes initialPlanes(W, H, 1);
    initialPlanes.data()[H / 2 * W + W / 2] = 1;

    // ── Автомат (Parity + тор) ──────────────────────────
    auto parity = std::make_unique<Parity>(
        initialPlanes,                              // ещё не перемещён
        std::make_unique<TorusTopology>(W, H)
    );

    // ── Симуляция (владеет Planes, работает в отдельном потоке) ──
    AutomatonController controller(
        std::move(initialPlanes),
        std::move(parity)
    );

    // ── Канвас ──────────────────────────────────────────
    CellularCanvas canvas(W, H);
    canvas.setPlanes(controller.snapshot(), 0);
    canvas.setPlanesMutex(controller.snapshotMutex());
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
    QObject::connect(&controller, &AutomatonController::stepped,
                     &canvas,     &CellularCanvas::refresh,
                     Qt::QueuedConnection);

    canvas.show();
    controller.start();

    const int result = app.exec();

    controller.stop();
    return result;
}