#include "MainWindow.h"

#include "TrailPostProcessor.h"
#include "AntiflickerPostProcessor.h"
#include "GrayscalePostProcessor.h"
#include <QMenuBar>
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

    if (m_controller) {
        connect(m_controller, &AutomatonController::planesReset,
                this,         &MainWindow::onPlanesReset);
    }

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
    // consume() + копия из read-буфера — без мьютекса
    auto& tb = m_controller->tripleBuffer();
    tb.consume();                       // забираем свежее (если есть)
    return tb.readBuffer();             // копия (Planes copy ctor)
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

    buildViewMenu();
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

void MainWindow::buildViewMenu()
{
    m_viewMenu = menuBar()->addMenu(tr("View"));

    // ── Fullscreen (заглушка) ──
    QAction* fullscreen = m_viewMenu->addAction(tr("Fullscreen"));
    fullscreen->setShortcut(QKeySequence(Qt::Key_F11));
    fullscreen->setEnabled(false);

    // ── Theme (заглушка) ──
    QAction* theme = m_viewMenu->addAction(tr("Theme\u2026"));
    theme->setEnabled(false);

    m_viewMenu->addSeparator();

    // ── Display Mode… ──
    m_displayModeMenu = m_viewMenu->addMenu(tr("Display Mode\u2026"));

    size_t initialNumPlanes = 1;
    if (m_controller)
        initialNumPlanes = m_controller->tripleBuffer().readBuffer().numPlanes();
    rebuildDisplayModeMenu(initialNumPlanes);

    m_viewMenu->addSeparator();

    // ── Color Map… ──
    m_colorMapPresetsAction = new ColorMapPresetsAction(
        m_canvas, this, this);
    m_viewMenu->addAction(m_colorMapPresetsAction->action());

    // ── Color Map from file… ──
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

    // initial check
    const size_t current = m_canvas->displayStrategyIndex();
    if (current < m_displayActions.size())
        m_displayActions[current]->setChecked(true);

    // подписываемся на изменение из canvas (через клавиатуру/другие внешние события)
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

// ─── Новый метод: добавить в MainWindow.cpp ─────────────
// (Полная реализация)

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

} // namespace ntcap