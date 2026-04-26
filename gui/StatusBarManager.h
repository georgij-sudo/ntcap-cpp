#pragma once

#include <QObject>
#include <QElapsedTimer>

class QToolBar;
class QLabel;
class QTimer;

namespace ntcap {

class AutomatonController;
class CellularCanvas;
class SimulationActions;

/**
 * @brief Manages a top info bar (QToolBar) with live simulation metrics.
 *
 * Displays: play state, generation, sim FPS, render FPS,
 * recipe name, display mode, field size, camera position, zoom.
 */
class StatusBarManager final : public QObject {
    Q_OBJECT
public:
    /**
     * @param toolbar        target QToolBar (not owned).
     * @param controller     automaton controller (not owned).
     * @param canvas         cellular canvas (not owned).
     * @param simActions     simulation actions for play/pause state (not owned).
     * @param parent         QObject parent.
     */
    explicit StatusBarManager(QToolBar*            toolbar,
                              AutomatonController* controller,
                              CellularCanvas*      canvas,
                              SimulationActions*   simActions,
                              QObject*             parent = nullptr);

private slots:
    void onTick();
    void onPlayStateChanged(bool playing);
    void onDisplayStrategyChanged(size_t index);

private:
    void buildLabels();
    void updateAll();
    QLabel* makeLabel(const QString& minText);

    QToolBar*            m_toolbar;
    AutomatonController* m_controller;
    CellularCanvas*      m_canvas;
    SimulationActions*   m_simActions;

    QTimer* m_timer = nullptr;

    QLabel* m_stateLabel       = nullptr;
    QLabel* m_generationLabel  = nullptr;
    QLabel* m_simFpsLabel      = nullptr;
    QLabel* m_renderFpsLabel   = nullptr;
    QLabel* m_recipeLabel      = nullptr;
    QLabel* m_displayModeLabel = nullptr;
    QLabel* m_fieldSizeLabel   = nullptr;
    QLabel* m_cameraLabel      = nullptr;
    QLabel* m_zoomLabel        = nullptr;

    QElapsedTimer m_elapsed;
    size_t   m_prevGeneration = 0;
    uint64_t m_prevPaintCount = 0;

    double m_simFps    = 0.0;
    double m_renderFps = 0.0;
    bool   m_playing   = false;
};

} // namespace ntcap