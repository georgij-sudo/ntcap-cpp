#include "MainWindow.h"

#include <QMenuBar>
#include <QAction>
#include <QTimer>
#include <QMutexLocker>
#include <QInputDialog>

#include "CellularCanvas.h"
#include "AutomatonController.h"
#include "menu_bar/SavePlanesAction.h"
#include "menu_bar/LoadPlanesAction.h"

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
    bool wasPlaying = m_playing;
    if (m_playing)
        m_controller->stop();

    m_controller->resetPlanes(planes);
    m_canvas->setPlanes(m_controller->snapshot(), 0);
    m_canvas->refresh();

    if (wasPlaying)
        m_controller->start();

    updatePlayPauseState(wasPlaying);
}

// ── private: menu construction ──────────────────────────

void MainWindow::buildMenuBar()
{
    buildPlanesMenu();
    m_recipesMenu = addPlaceholderMenu(tr("Recipes"));
    buildSimulationMenu();
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

void MainWindow::buildSimulationMenu()
{
    m_simulationMenu = menuBar()->addMenu(tr("Simulation"));

    // ── Play / Pause ──
    m_playPauseAction = m_simulationMenu->addAction(tr("Pause"));
    m_playPauseAction->setShortcut(QKeySequence(Qt::Key_P));
    connect(m_playPauseAction, &QAction::triggered,
            this,              &MainWindow::togglePlayPause);

    // ── Step Forward ──
    m_stepForwardAction = m_simulationMenu->addAction(tr("Step Forward"));
    m_stepForwardAction->setShortcut(QKeySequence(Qt::Key_N));
    m_stepForwardAction->setEnabled(true);          // paused initially
    connect(m_stepForwardAction, &QAction::triggered,
            this,                &MainWindow::stepForward);

    m_simulationMenu->addSeparator();

    // ── Set step delay (ms) ──
    QAction* delayAction = m_simulationMenu->addAction(tr("Set step delay (ms)\u2026"));
    connect(delayAction, &QAction::triggered,
            this,        &MainWindow::promptStepDelay);

    // ── Skip rendering every N frames ──
    QAction* skipAction = m_simulationMenu->addAction(tr("Skip rendering every N frames\u2026"));
    connect(skipAction, &QAction::triggered,
            this,       &MainWindow::promptSkipRendering);

    m_simulationMenu->addSeparator();

    // ── Fit Plane to Window ──
    QAction* fitAction = m_simulationMenu->addAction(tr("Fit Plane to Window"));
    fitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(fitAction, &QAction::triggered,
            this,      &MainWindow::fitPlaneToWindow);
}

QMenu* MainWindow::addPlaceholderMenu(const QString& title)
{
    QMenu* menu = menuBar()->addMenu(title);
    QAction* placeholder = menu->addAction(tr("Coming soon\u2026"));
    placeholder->setEnabled(false);
    return menu;
}

// ── private: simulation slots ───────────────────────────

void MainWindow::togglePlayPause()
{
    if (m_playing) {
        m_controller->stop();
        updatePlayPauseState(false);
    } else {
        m_controller->start();
        updatePlayPauseState(true);
    }
}

void MainWindow::stepForward()
{
    if (m_playing)
        return;                                     // защита на всякий случай

    m_controller->stepOnce();
}

void MainWindow::promptStepDelay()
{
    bool ok = false;
    int ms = QInputDialog::getInt(
        this,
        tr("Step delay"),
        tr("Delay between steps (ms):"),
        static_cast<int>(m_controller->stepDelayMs()),  // текущее значение
        0,          // min
        60000,      // max (1 минута)
        1,          // step
        &ok);

    if (ok)
        m_controller->setStepDelay(static_cast<size_t>(ms));
}

void MainWindow::promptSkipRendering()
{
    bool ok = false;
    int n = QInputDialog::getInt(
        this,
        tr("Render interval"),
        tr("Render every N-th frame (1 = every frame):"),
        static_cast<int>(m_controller->renderInterval()),  // текущее значение
        1,          // min
        100000,     // max
        1,          // step
        &ok);

    if (ok)
        m_controller->setRenderInterval(static_cast<size_t>(n));
}

void MainWindow::fitPlaneToWindow()
{
    if (!m_canvas || m_fieldWidth == 0 || m_fieldHeight == 0)
        return;

    m_canvas->fitToWindow();
}

void MainWindow::updatePlayPauseState(bool playing)
{
    m_playing = playing;
    m_playPauseAction->setText(playing ? tr("Pause") : tr("Play"));
    m_stepForwardAction->setEnabled(!playing);
}

} // namespace ntcap