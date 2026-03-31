#include "KeyboardController.h"
#include "CellularCanvas.h"

namespace ntcap {

KeyboardController::KeyboardController(CellularCanvas* canvas,
                                         QObject* parent)
    : QObject(parent)
{
    // ── Зум ─────────────────────────────────────────────
    addShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus),  canvas, &CellularCanvas::zoomIn);
    addShortcut(QKeySequence(Qt::CTRL | Qt::Key_Equal), canvas, &CellularCanvas::zoomIn);
    addShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus), canvas, &CellularCanvas::zoomOut);

    // ── Панорамирование ─────────────────────────────────
    addShortcut(QKeySequence(Qt::Key_Left),  canvas, &CellularCanvas::panLeft);
    addShortcut(QKeySequence(Qt::Key_Right), canvas, &CellularCanvas::panRight);
    addShortcut(QKeySequence(Qt::Key_Up),    canvas, &CellularCanvas::panUp);
    addShortcut(QKeySequence(Qt::Key_Down),  canvas, &CellularCanvas::panDown);
}

KeyboardController::~KeyboardController()
{
    for (auto& ptr : m_shortcuts) {
        delete ptr.data();  // QPointer = null если canvas уже удалён
    }
}

QShortcut* KeyboardController::addShortcut(const QKeySequence& key,
                                            CellularCanvas* canvas,
                                            void (CellularCanvas::*slot)())
{
    // Шорткат привязан к canvas (контекст фокуса).
    auto* shortcut = new QShortcut(key, canvas);
    shortcut->setAutoRepeat(true);
    connect(shortcut, &QShortcut::activated, canvas, slot);
    m_shortcuts.append(shortcut);
    return shortcut;
}

} // namespace ntcap