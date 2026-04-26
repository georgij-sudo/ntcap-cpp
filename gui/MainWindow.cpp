#include "MainWindow.h"

#include "TrailPostProcessor.h"
#include "AntiflickerPostProcessor.h"
#include "GrayscalePostProcessor.h"
#include "StatusBarManager.h"

#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QTimer>
#include <QActionGroup>

#include "CellularCanvas.h"
#include "AutomatonController.h"
#include "menu_bar/SavePlanesAction.h"
#include "menu_bar/LoadPlanesAction.h"
#include "menu_bar/SimulationActions.h"
#include "menu_bar/ColorMapPresetsAction.h"
#include "menu_bar/ColorMapFromFileAction.h"
#include "ColorMapPresetsAction.h"
#include "ColorMapPresetsDialog.h"

#include "DisplayStrategy.h"
#include "SinglePlaneStrategy.h"
#include "BitfieldStrategy.h"
#include "SaturatingSumStrategy.h"
#include "NormalizedSumStrategy.h"

namespace ntcap {

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

    if (m_controller) {
        connect(m_controller, &AutomatonController::planesReset,
                this,         &MainWindow::onPlanesReset);
    }

    buildInfoBar();

    setWindowTitle("Parity CA (threaded)");
    resize(1280, 720);
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

Planes MainWindow::snapshotCopy() const
{
    auto& tb = m_controller->tripleBuffer();
    tb.consume();
    return tb.readBuffer();
}

void MainWindow::applyLoadedPlanes(const Planes& planes)
{
    bool wasPlaying = m_simulationActions->isPlaying();
    if (wasPlaying)
        m_simulationActions->pause();

    m_canvas->setTripleBuffer(&m_controller->tripleBuffer(), 0);
    m_canvas->setFieldSize(planes.width(), planes.height());
    m_canvas->refresh();

    if (wasPlaying)
        m_simulationActions->play();
}

void MainWindow::buildMenuBar()
{
    buildPlanesMenu();
    m_recipesMenu = addPlaceholderMenu(tr("Recipes"));

    m_simulationActions = new SimulationActions(
        m_controller, m_canvas,
        m_fieldWidth, m_fieldHeight,
        this, this);
    menuBar()->addMenu(m_simulationActions->menu());

    buildViewMenu();
}

void MainWindow::buildPlanesMenu()
{
    m_planesMenu = menuBar()->addMenu(tr("Planes"));

    m_loadAction = new LoadPlanesAction(this, this);
    m_planesMenu->addAction(m_loadAction->action());
    connect(m_loadAction, &LoadPlanesAction::planesLoaded,
            this,         &MainWindow::applyLoadedPlanes);

    m_saveAction = new SavePlanesAction(
        [this]() -> Planes { return snapshotCopy(); },
        this, this);
    m_planesMenu->addAction(m_saveAction->action());
}

void MainWindow::buildViewMenu()
{
    m_viewMenu = menuBar()->addMenu(tr("View"));

    QAction* fullscreen = m_viewMenu->addAction(tr("Fullscreen"));
    fullscreen->setShortcut(QKeySequence(Qt::Key_F11));
    fullscreen->setEnabled(false);

    QAction* theme = m_viewMenu->addAction(tr("Theme\u2026"));
    theme->setEnabled(false);

    m_viewMenu->addSeparator();

    m_displayModeMenu = m_viewMenu->addMenu(tr("Display Mode\u2026"));

    size_t initialNumPlanes = 1;
    if (m_controller)
        initialNumPlanes = m_controller->tripleBuffer().readBuffer().numPlanes();
    rebuildDisplayModeMenu(initialNumPlanes);

    m_viewMenu->addSeparator();

    m_colorMapPresetsAction = new ColorMapPresetsAction(
        m_canvas, this, this);
    m_viewMenu->addAction(m_colorMapPresetsAction->action());

    m_colorMapFromFileAction = new ColorMapFromFileAction(
        m_canvas, this, this);
    m_viewMenu->addAction(m_colorMapFromFileAction->action());

    buildPostProcessingMenu();
}

QMenu* MainWindow::addPlaceholderMenu(const QString& title)
{
    QMenu* menu = menuBar()->addMenu(title);
    QAction* placeholder = menu->addAction(tr("Coming soon\u2026"));
    placeholder->setEnabled(false);
    return menu;
}

void MainWindow::rebuildDisplayModeMenu(size_t numPlanes)
{
    if (numPlanes == 0)
        numPlanes = 1;

    m_displayModeMenu->clear();
    m_displayActions.clear();

    QActionGroup* group = new QActionGroup(this);
    group->setExclusive(true);

    std::vector<DisplayStrategyPtr> strategies;
    strategies.reserve(numPlanes + 3);

    for (size_t i = 0; i < numPlanes; ++i) {
        strategies.push_back(std::make_shared<SinglePlaneStrategy>(i));
    }
    strategies.push_back(std::make_shared<BitfieldStrategy>());
    strategies.push_back(std::make_shared<SaturatingSumStrategy>());
    strategies.push_back(std::make_shared<NormalizedSumStrategy>());

    m_canvas->setDisplayStrategies(std::move(strategies));

    const auto& avail = m_canvas->availableDisplayStrategies();
    for (size_t i = 0; i < avail.size(); ++i) {
        QAction* a = m_displayModeMenu->addAction(avail[i]->name());
        a->setCheckable(true);
        a->setActionGroup(group);
        const size_t idx = i;
        connect(a, &QAction::triggered, this, [this, idx]() {
            m_canvas->setDisplayStrategyIndex(idx);
        });
        m_displayActions.push_back(a);
    }

    const size_t current = m_canvas->displayStrategyIndex();
    if (current < m_displayActions.size())
        m_displayActions[current]->setChecked(true);

    connect(m_canvas, &CellularCanvas::displayStrategyChanged,
            this,      &MainWindow::updateDisplayModeActions);
}

void MainWindow::onPlanesReset(const Planes& planes)
{
    rebuildDisplayModeMenu(planes.numPlanes());
}

void MainWindow::updateDisplayModeActions(size_t activeIndex)
{
    for (size_t i = 0; i < m_displayActions.size(); ++i) {
        m_displayActions[i]->setChecked(i == activeIndex);
    }
}

void MainWindow::buildPostProcessingMenu()
{
    m_postProcessingMenu = m_viewMenu->addMenu(tr("Post-processing"));

    m_trailAction = m_postProcessingMenu->addAction(tr("Trail / Afterglow"));
    m_trailAction->setCheckable(true);
    m_trailAction->setChecked(false);
    connect(m_trailAction, &QAction::toggled,
            m_canvas, &CellularCanvas::setTrailEnabled);

    m_antiflickerAction = m_postProcessingMenu->addAction(tr("Antiflicker"));
    m_antiflickerAction->setCheckable(true);
    m_antiflickerAction->setChecked(false);
    connect(m_antiflickerAction, &QAction::toggled,
            m_canvas, &CellularCanvas::setAntiflickerEnabled);

    m_grayscaleAction = m_postProcessingMenu->addAction(tr("Grayscale"));
    m_grayscaleAction->setCheckable(true);
    m_grayscaleAction->setChecked(false);
    connect(m_grayscaleAction, &QAction::toggled,
            m_canvas, &CellularCanvas::setGrayscaleEnabled);
}

void MainWindow::buildInfoBar()
{
    m_infoBar = new QToolBar(tr("Info"), this);
    m_infoBar->setMovable(false);
    m_infoBar->setFloatable(false);
    m_infoBar->setAllowedAreas(Qt::TopToolBarArea);
    m_infoBar->toggleViewAction()->setEnabled(false);
    addToolBar(Qt::TopToolBarArea, m_infoBar);

    m_statusBarManager = new StatusBarManager(
        m_infoBar,
        m_controller,
        m_canvas,
        m_simulationActions,
        this);
}

} // namespace ntcap