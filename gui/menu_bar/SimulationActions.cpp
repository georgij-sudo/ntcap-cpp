#include "SimulationActions.h"

#include <QMenu>
#include <QAction>
#include <QInputDialog>

#include "AutomatonController.h"
#include "CellularCanvas.h"

namespace ntcap {

// ── ctor ────────────────────────────────────────────────

SimulationActions::SimulationActions(AutomatonController* controller,
                                     CellularCanvas*      canvas,
                                     size_t fieldWidth,
                                     size_t fieldHeight,
                                     QWidget* dialogParent,
                                     QObject* parent)
    : QObject(parent)
    , m_controller(controller)
    , m_canvas(canvas)
    , m_dialogParent(dialogParent)
    , m_fieldWidth(fieldWidth)
    , m_fieldHeight(fieldHeight)
{
    m_menu = new QMenu(tr("Simulation"), dialogParent);

    // ── Play / Pause ──
    m_playPauseAction = m_menu->addAction(tr("Pause"));
    m_playPauseAction->setShortcut(QKeySequence(Qt::Key_P));
    connect(m_playPauseAction, &QAction::triggered,
            this,              &SimulationActions::togglePlayPause);

    // ── Step Forward ──
    m_stepForwardAction = m_menu->addAction(tr("Step Forward"));
    m_stepForwardAction->setShortcut(QKeySequence(Qt::Key_N));
    m_stepForwardAction->setEnabled(true);
    connect(m_stepForwardAction, &QAction::triggered,
            this,                &SimulationActions::stepForward);

    m_menu->addSeparator();

    // ── Set step delay (ms) ──
    QAction* delayAction = m_menu->addAction(tr("Set step delay (ms)\u2026"));
    connect(delayAction, &QAction::triggered,
            this,        &SimulationActions::promptStepDelay);

    // ── Skip rendering every N frames ──
    QAction* skipAction = m_menu->addAction(tr("Skip rendering every N frames\u2026"));
    connect(skipAction, &QAction::triggered,
            this,       &SimulationActions::promptSkipRendering);

    m_menu->addSeparator();

    // ── Fit Plane to Window ──
    QAction* fitAction = m_menu->addAction(tr("Fit Plane to Window"));
    fitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(fitAction, &QAction::triggered,
            this,      &SimulationActions::fitPlaneToWindow);
}

// ── public ──────────────────────────────────────────────

QMenu* SimulationActions::menu() const noexcept { return m_menu; }
bool   SimulationActions::isPlaying() const noexcept { return m_playing; }

void SimulationActions::play()
{
    if (!m_playing) {
        m_controller->start();
        updatePlayPauseState(true);
    }
}

void SimulationActions::pause()
{
    if (m_playing) {
        m_controller->stop();
        updatePlayPauseState(false);
    }
}

// ── private slots ───────────────────────────────────────

void SimulationActions::togglePlayPause()
{
    if (m_playing)
        pause();
    else
        play();
}

void SimulationActions::stepForward()
{
    if (m_playing)
        return;

    m_controller->stepOnce();
}

void SimulationActions::promptStepDelay()
{
    bool ok = false;
    int ms = QInputDialog::getInt(
        m_dialogParent,
        tr("Step delay"),
        tr("Delay between steps (ms):"),
        static_cast<int>(m_controller->stepDelayMs()),
        0,      // min
        60000,  // max
        1,      // step
        &ok);

    if (ok)
        m_controller->setStepDelay(static_cast<size_t>(ms));
}

void SimulationActions::promptSkipRendering()
{
    bool ok = false;
    int n = QInputDialog::getInt(
        m_dialogParent,
        tr("Render interval"),
        tr("Render every N-th frame (1 = every frame):"),
        static_cast<int>(m_controller->renderInterval()),
        1,       // min
        100000,  // max
        1,       // step
        &ok);

    if (ok)
        m_controller->setRenderInterval(static_cast<size_t>(n));
}

void SimulationActions::fitPlaneToWindow()
{
    if (!m_canvas || m_fieldWidth == 0 || m_fieldHeight == 0)
        return;

    m_canvas->fitToWindow();
}

// ── private ─────────────────────────────────────────────

void SimulationActions::updatePlayPauseState(bool playing)
{
    m_playing = playing;
    m_playPauseAction->setText(playing ? tr("Pause") : tr("Play"));
    m_stepForwardAction->setEnabled(!playing);
    emit playStateChanged(playing);
}

} // namespace ntcap