#pragma once
#include <QMainWindow>
#include <QMenu>
#include <functional>
#include "Planes.h"

namespace ntcap {

class CellularCanvas;
class AutomatonController;
class SavePlanesAction;
class LoadPlanesAction;
class SimulationActions;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(size_t fieldWidth, size_t fieldHeight,
                        AutomatonController* controller,
                        QWidget* parent = nullptr);
    ~MainWindow() override = default;

    CellularCanvas*      canvas()     const noexcept;
    AutomatonController* controller() const noexcept;

protected:
    void showEvent(QShowEvent* event) override;

private:
    // ── menu construction ───────────────────────────────
    void   buildMenuBar();
    void   buildPlanesMenu();
    QMenu* addPlaceholderMenu(const QString& title);

    // ── planes helpers ──────────────────────────────────
    Planes snapshotCopy() const;
    void   applyLoadedPlanes(const Planes& planes);

    // ── data ────────────────────────────────────────────
    CellularCanvas*      m_canvas     = nullptr;
    AutomatonController* m_controller = nullptr;

    size_t m_fieldWidth  = 0;
    size_t m_fieldHeight = 0;

    // menus / actions
    QMenu*             m_planesMenu        = nullptr;
    QMenu*             m_recipesMenu       = nullptr;
    SavePlanesAction*  m_saveAction        = nullptr;
    LoadPlanesAction*  m_loadAction        = nullptr;
    SimulationActions* m_simulationActions = nullptr;

    bool m_firstShow = true;
};

} // namespace ntcap