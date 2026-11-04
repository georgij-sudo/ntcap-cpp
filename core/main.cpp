#include <QApplication>
#include <QTimer>

#include "Version.h"

#include "MainWindow.h"
#include "CellularCanvas.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "AutomatonController.h"
#include "Planes.h"
#include "TorusTopology.h"
#include "StableTubeWorms.h"
#include "LifeHistory.h"

using namespace ntcap;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(
        QStringLiteral(NTCAP_APPLICATION_NAME));
    QCoreApplication::setApplicationVersion(
        QStringLiteral(NTCAP_VERSION_STRING));

    constexpr size_t W = 400;
    constexpr size_t H = 400;

    Planes initialPlanes(W, H, 2);

    const size_t planeSize = W * H;
    for (size_t i = 0; i < planeSize; ++i) {
        initialPlanes.data()[i] = static_cast<uint8_t>(std::rand() % 2);
    }

    auto worms = std::make_unique<LifeHistory>(initialPlanes);
    AutomatonController controller(std::move(initialPlanes), std::move(worms));

    controller.setStepDelay(32);

    MainWindow window(W, H, &controller);
    CellularCanvas* canvas = window.canvas();

    canvas->setTripleBuffer(&controller.tripleBuffer());

    auto* renderTimer = new QTimer(&window);
    renderTimer->setTimerType(Qt::PreciseTimer);
    renderTimer->setInterval(16);
    QObject::connect(renderTimer, &QTimer::timeout,
                     canvas,      &CellularCanvas::refresh);
    renderTimer->start();

    QMap<uint8_t, QRgb> colorMap;
    colorMap[0] = qRgb(20,  20,  30);
    colorMap[1] = qRgb(0,   200, 120);
    canvas->setColorMap(colorMap);
    canvas->setDefaultColor(qRgb(255, 0, 0));

    KeyboardController keyboard(canvas);
    MouseController    mouse(canvas);

    window.show();
    controller.start();

    const int result = app.exec();
    controller.stop();
    return result;
}