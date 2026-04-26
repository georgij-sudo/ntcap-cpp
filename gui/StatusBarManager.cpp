#include "StatusBarManager.h"

#include <QToolBar>
#include <QLabel>
#include <QTimer>
#include <QString>

#include "AutomatonController.h"
#include "CellularCanvas.h"
#include "SimulationActions.h"
#include "DisplayStrategy.h"

namespace ntcap {

StatusBarManager::StatusBarManager(QToolBar*            toolbar,
                                   AutomatonController* controller,
                                   CellularCanvas*      canvas,
                                   SimulationActions*   simActions,
                                   QObject*             parent)
    : QObject(parent)
    , m_toolbar(toolbar)
    , m_controller(controller)
    , m_canvas(canvas)
    , m_simActions(simActions)
{
    buildLabels();

    m_playing = m_simActions->isPlaying();
    m_prevGeneration = m_controller->generation();
    m_prevPaintCount = m_canvas->paintCount();
    m_elapsed.start();

    connect(m_simActions, &SimulationActions::playStateChanged,
            this,         &StatusBarManager::onPlayStateChanged);

    connect(m_canvas, &CellularCanvas::displayStrategyChanged,
            this,     &StatusBarManager::onDisplayStrategyChanged);

    m_timer = new QTimer(this);
    m_timer->setInterval(250);
    connect(m_timer, &QTimer::timeout, this, &StatusBarManager::onTick);
    m_timer->start();

    updateAll();
}

void StatusBarManager::onTick()
{
    const qint64 elapsedMs = m_elapsed.restart();

    if (elapsedMs > 0) {
        const size_t   curGen   = m_controller->generation();
        const uint64_t curPaint = m_canvas->paintCount();

        const size_t   genDelta   = curGen   - m_prevGeneration;
        const uint64_t paintDelta = curPaint - m_prevPaintCount;

        const double seconds = static_cast<double>(elapsedMs) / 1000.0;
        m_simFps    = static_cast<double>(genDelta)   / seconds;
        m_renderFps = static_cast<double>(paintDelta) / seconds;

        m_prevGeneration = curGen;
        m_prevPaintCount = curPaint;
    }

    updateAll();
}

void StatusBarManager::onPlayStateChanged(bool playing)
{
    m_playing = playing;

    if (playing) {
        m_prevGeneration = m_controller->generation();
        m_prevPaintCount = m_canvas->paintCount();
        m_elapsed.restart();
        m_simFps    = 0.0;
        m_renderFps = 0.0;
    }

    updateAll();
}

void StatusBarManager::onDisplayStrategyChanged(size_t /*index*/)
{
    updateAll();
}

void StatusBarManager::buildLabels()
{
    m_stateLabel       = makeLabel(QStringLiteral(" ▶ Playing "));
    m_generationLabel  = makeLabel(QStringLiteral(" Gen: 00000000 "));
    m_simFpsLabel      = makeLabel(QStringLiteral(" Sim: 00000 g/s "));
    m_renderFpsLabel   = makeLabel(QStringLiteral(" Render: 000 fps "));
    m_recipeLabel      = makeLabel(QStringLiteral(" Recipe: TubeWorms "));
    m_displayModeLabel = makeLabel(QStringLiteral(" Mode: NormSum "));
    m_fieldSizeLabel   = makeLabel(QStringLiteral(" 0000×0000 ×00 "));
    m_cameraLabel      = makeLabel(QStringLiteral(" XY: -000.0, -000.0 "));
    m_zoomLabel        = makeLabel(QStringLiteral(" ×00.00 "));

    m_toolbar->addWidget(m_stateLabel);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_generationLabel);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_simFpsLabel);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_renderFpsLabel);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_recipeLabel);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_displayModeLabel);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_fieldSizeLabel);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_cameraLabel);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_zoomLabel);
}

void StatusBarManager::updateAll()
{
    if (m_playing) {
        m_stateLabel->setText(QStringLiteral("▶ Playing"));
    } else {
        m_stateLabel->setText(QStringLiteral("⏸ Paused"));
    }

    m_generationLabel->setText(
        QStringLiteral("Gen: %1").arg(m_controller->generation()));

    m_simFpsLabel->setText(
        QStringLiteral("Sim: %1 g/s").arg(
            static_cast<int>(m_simFps)));

    m_renderFpsLabel->setText(
        QStringLiteral("Render: %1 fps").arg(
            static_cast<int>(m_renderFps)));

    m_recipeLabel->setText(
        QStringLiteral("Recipe: %1").arg(m_controller->automatonName()));

    const auto& strategies = m_canvas->availableDisplayStrategies();
    const size_t idx = m_canvas->displayStrategyIndex();
    QString modeName = QStringLiteral("—");
    if (idx < strategies.size() && strategies[idx]) {
        modeName = strategies[idx]->name();
    }
    m_displayModeLabel->setText(
        QStringLiteral("Mode: %1").arg(modeName));

    m_fieldSizeLabel->setText(
        QStringLiteral("%1×%2 ×%3").arg(
            m_controller->fieldWidth()).arg(
            m_controller->fieldHeight()).arg(
            m_controller->numPlanes()));

    const QPointF off = m_canvas->offset();
    m_cameraLabel->setText(
        QStringLiteral("XY: %1, %2")
            .arg(off.x(), 0, 'f', 1)
            .arg(off.y(), 0, 'f', 1));

    m_zoomLabel->setText(
        QStringLiteral("×%1").arg(
            m_canvas->zoom(), 0, 'f', 2));
}

QLabel* StatusBarManager::makeLabel(const QString& minText)
{
    auto* label = new QLabel(m_toolbar);
    label->setMinimumWidth(label->fontMetrics().horizontalAdvance(minText));
    return label;
}

} // namespace ntcap