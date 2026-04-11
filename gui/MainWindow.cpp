 #include "MainWindow.h"

#include <QMenuBar>
#include <QAction>
#include <QTimer>

#include "CellularCanvas.h"
#include "AutomatonController.h"
#include "menu_bar/SavePlanesAction.h"
#include "menu_bar/LoadPlanesAction.h"
#include "menu_bar/SimulationActions.h"
#include "menu_bar/ColorMapPresetsAction.h"
#include "menu_bar/ColorMapFromFileAction.h"
#include "ColorMapPresetsAction.h"
#include "ColorMapPresetsDialog.h"

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

    // ── Color Map… ──
    m_colorMapPresetsAction = new ColorMapPresetsAction(
        m_canvas, this, this);
    m_viewMenu->addAction(m_colorMapPresetsAction->action());

    // ── Color Map from file… ──
    m_colorMapFromFileAction = new ColorMapFromFileAction(
        m_canvas, this, this);
    m_viewMenu->addAction(m_colorMapFromFileAction->action());
}

QMenu* MainWindow::addPlaceholderMenu(const QString& title)
{
    QMenu* menu = menuBar()->addMenu(title);
    QAction* placeholder = menu->addAction(tr("Coming soon\u2026"));
    placeholder->setEnabled(false);
    return menu;
}

} // namespace ntcap