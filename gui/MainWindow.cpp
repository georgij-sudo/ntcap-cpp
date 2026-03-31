#include "MainWindow.h"

#include <QMenuBar>
#include <QAction>
#include <QTimer>

#include "CellularCanvas.h"

namespace ntcap {

MainWindow::MainWindow(size_t fieldWidth, size_t fieldHeight,
                       QWidget* parent)
    : QMainWindow(parent)
    , m_fieldWidth(fieldWidth)
    , m_fieldHeight(fieldHeight)
{
    m_canvas = new CellularCanvas(fieldWidth, fieldHeight, this);
    setCentralWidget(m_canvas);

    buildMenuBar();

    setWindowTitle("Parity CA (threaded)");
    resize(800, 600);
}

CellularCanvas* MainWindow::canvas() const noexcept
{
    return m_canvas;
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);

    if (m_firstShow) {
        m_firstShow = false;

        // В следующем тике event-loop геометрия виджета
        // уже финальная → setFieldSize пересчитает zoom корректно.
        QTimer::singleShot(0, this, [this] {
            m_canvas->setFieldSize(m_fieldWidth, m_fieldHeight);
        });
    }
}

void MainWindow::buildMenuBar()
{
    m_planesMenu     = addPlaceholderMenu(tr("Planes"));
    m_recipesMenu    = addPlaceholderMenu(tr("Recipes"));
    m_simulationMenu = addPlaceholderMenu(tr("Simulation"));
}

QMenu* MainWindow::addPlaceholderMenu(const QString& title)
{
    QMenu* menu = menuBar()->addMenu(title);

    QAction* placeholder = menu->addAction(tr("Coming soon\u2026"));
    placeholder->setEnabled(false);

    return menu;
}

} // namespace ntcap