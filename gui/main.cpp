#include <QApplication>
#include "MainWindow.h"
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
        initialPlanes,
        std::make_unique<TorusTopology>(W, H));

    // ── Симуляция ───────────────────────────────────────
    AutomatonController controller(
        std::move(initialPlanes),
        std::move(parity));

    // ── Главное окно ────────────────────────────────────
    MainWindow window(W, H, &controller);
    CellularCanvas* canvas = window.canvas();

    canvas->setPlanes(controller.snapshot(), 0);
    canvas->setPlanesMutex(controller.snapshotMutex());

    QMap<uint8_t, QRgb> colorMap;
    colorMap[0] = qRgb(20,  20,  30);
    colorMap[1] = qRgb(0,   200, 120);
    canvas->setColorMap(colorMap);
    canvas->setDefaultColor(qRgb(255, 0, 0));

    // ── Ввод ────────────────────────────────────────────
    KeyboardController keyboard(canvas);
    MouseController    mouse(canvas);

    // ── Связи ───────────────────────────────────────────
    QObject::connect(&controller, &AutomatonController::stepped,
                     canvas,      &CellularCanvas::refresh,
                     Qt::QueuedConnection);

    window.show();
    controller.start();

    const int result = app.exec();
    controller.stop();
    return result;
}