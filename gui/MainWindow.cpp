 #include "MainWindow.h"

#include <QMenuBar>
#include <QAction>
#include <QTimer>
#include <QMutexLocker>

#include "CellularCanvas.h"
#include "AutomatonController.h"
#include "menu_bar/SavePlanesAction.h"
#include "menu_bar/LoadPlanesAction.h"
#include "menu_bar/SimulationActions.h"

namespace ntcap {

// ── ctor ────────────────────────────────────────────────

MainWindow::MainWindow(size_t fieldWidth, size_t fieldHeight,
                       AutomatonController* controller,
                       QWidget* parent)
    : QMainWindow(parent)
    , m_fieldWidth(fieldWidth)
    , m_fieldHeight(fieldHeight)
    , m_controller(controller)
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

AutomatonController* MainWindow::controller() const noexcept
{
    return m_controller;
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    if (m_firstShow) {
        m_firstShow = false;
        QTimer::singleShot(0, this, [this] {
            m_canvas->setFieldSize(m_fieldWidth, m_fieldHeight);
        });
    }
}

// ── private: planes helpers ─────────────────────────────

Planes MainWindow::snapshotCopy() const
{
    QMutexLocker lock(m_controller->snapshotMutex());
    return *m_controller->snapshot();
}

void MainWindow::applyLoadedPlanes(const Planes& planes)
{
    bool wasPlaying = m_simulationActions->isPlaying();
    if (wasPlaying)
        m_simulationActions->pause();

    m_controller->resetPlanes(planes);
    m_canvas->setPlanes(m_controller->snapshot(), 0);
    m_canvas->refresh();

    if (wasPlaying)
        m_simulationActions->play();
}

// ── private: menu construction ──────────────────────────

void MainWindow::buildMenuBar()
{
    buildPlanesMenu();
    m_recipesMenu = addPlaceholderMenu(tr("Recipes"));

    m_simulationActions = new SimulationActions(
        m_controller, m_canvas,
        m_fieldWidth, m_fieldHeight,
        this,   // dialogParent
        this);  // QObject parent
    menuBar()->addMenu(m_simulationActions->menu());
}

void MainWindow::buildPlanesMenu()
{
    m_planesMenu = menuBar()->addMenu(tr("Planes"));

    // ── Open ──
    m_loadAction = new LoadPlanesAction(this, this);
    m_planesMenu->addAction(m_loadAction->action());
    connect(m_loadAction, &LoadPlanesAction::planesLoaded,
            this,         &MainWindow::applyLoadedPlanes);

    // ── Save As ──
    m_saveAction = new SavePlanesAction(
        [this]() -> Planes { return snapshotCopy(); },
        this, this);
    m_planesMenu->addAction(m_saveAction->action());
}

QMenu* MainWindow::addPlaceholderMenu(const QString& title)
{
    QMenu* menu = menuBar()->addMenu(title);
    QAction* placeholder = menu->addAction(tr("Coming soon\u2026"));
    placeholder->setEnabled(false);
    return menu;
}

} // namespace ntcap