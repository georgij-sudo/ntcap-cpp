#pragma once

#include <QObject>

class QAction;
class QMenu;
class QWidget;

namespace ntcap {

class AutomatonController;
class CellularCanvas;

class SimulationActions final : public QObject {
    Q_OBJECT
public:
    explicit SimulationActions(AutomatonController* controller,
                               CellularCanvas*      canvas,
                               size_t fieldWidth,
                               size_t fieldHeight,
                               QWidget* dialogParent,
                               QObject* parent = nullptr);

    QMenu* menu()      const noexcept;
    bool   isPlaying() const noexcept;

    void play();
    void pause();

signals:
    void playStateChanged(bool playing);

private slots:
    void togglePlayPause();
    void stepForward();
    void promptStepDelay();
    void promptSkipRendering();
    void promptRandomSeed();
    void fitPlaneToWindow();

private:
    void updatePlayPauseState(bool playing);

    AutomatonController* m_controller   = nullptr;
    CellularCanvas*      m_canvas       = nullptr;
    QWidget*             m_dialogParent = nullptr;

    size_t m_fieldWidth  = 0;
    size_t m_fieldHeight = 0;

    QMenu*   m_menu              = nullptr;
    QAction* m_playPauseAction   = nullptr;
    QAction* m_stepForwardAction = nullptr;

    bool m_playing = true;
};

} // namespace ntcap