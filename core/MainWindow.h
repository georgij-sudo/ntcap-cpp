#pragma once

#include <QMainWindow>
#include <QMenu>
#include <functional>
#include <vector>
#include <memory>

#include "Planes.h"

class QToolBar;

namespace ntcap {

class CellularCanvas;
class AutomatonController;
class SavePlanesAction;
class LoadPlanesAction;
class SimulationActions;
class RecipeActions;
class InitializeActions;
class ColorMapPresetsAction;
class ColorMapFromFileAction;
class DisplayStrategy;
class StatusBarManager;
class HelpActions;

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

private slots:
    void applyLoadedPlanes(const Planes& planes);
    void onPlanesReset(const Planes& planes);
    void updateDisplayModeActions(size_t activeIndex);
    void onSwapPlanesTriggered();
    void onResetAllTriggered();

private:

    void   buildMenuBar();
    void   buildPlanesMenu();
    void   buildInitializeMenu();
    void   buildViewMenu();
    void   buildHelpMenu();
    void   buildPostProcessingMenu();
    void   buildInfoBar();
    QMenu* addPlaceholderMenu(const QString& title);
    void   rebuildDisplayModeMenu(size_t numPlanes);

    Planes snapshotCopy() const;

    CellularCanvas*      m_canvas     = nullptr;
    AutomatonController* m_controller = nullptr;

    size_t m_fieldWidth  = 0;
    size_t m_fieldHeight = 0;

    QMenu*             m_planesMenu        = nullptr;
    SavePlanesAction*  m_saveAction        = nullptr;
    LoadPlanesAction*  m_loadAction        = nullptr;
    SimulationActions* m_simulationActions = nullptr;
    QAction*           m_swapPlanesAction  = nullptr;
    QAction*           m_resetAllAction    = nullptr;

    InitializeActions* m_initializeActions = nullptr;

    RecipeActions*     m_recipeActions     = nullptr;

    HelpActions*       m_helpActions       = nullptr;

    QMenu*                   m_viewMenu              = nullptr;
    QMenu*                   m_displayModeMenu       = nullptr;
    QActionGroup*            m_displayActionGroup     = nullptr;
    ColorMapPresetsAction*   m_colorMapPresetsAction  = nullptr;
    ColorMapFromFileAction*  m_colorMapFromFileAction = nullptr;
    std::vector<QAction*>    m_displayActions;

    QMenu*   m_postProcessingMenu   = nullptr;
    QAction* m_trailAction          = nullptr;
    QAction* m_antiflickerAction    = nullptr;
    QAction* m_grayscaleAction      = nullptr;

    QToolBar*         m_infoBar           = nullptr;
    StatusBarManager* m_statusBarManager  = nullptr;

    bool m_firstShow = true;
};

} // namespace ntcap