#include "MouseController.h"
#include "CellularCanvas.h"

#include <QWheelEvent>
#include <QMouseEvent>
#include <QCursor>
#include <cmath>

namespace ntcap {

// ─── Конструктор / деструктор ───────────────────────────────────────

MouseController::MouseController(CellularCanvas* canvas,
                                   QObject* parent)
    : QObject(parent)
    , m_canvas(canvas)
{
    m_canvas->installEventFilter(this);
}

MouseController::~MouseController()
{
    if (m_canvas) {
        m_canvas->removeEventFilter(this);
    }
}

// ─── eventFilter ────────────────────────────────────────────────────

bool MouseController::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != m_canvas) {
        return false;
    }

    switch (event->type()) {
    case QEvent::Wheel:
        return handleWheel(static_cast<QWheelEvent*>(event));
    case QEvent::MouseButtonPress:
        return handleMousePress(static_cast<QMouseEvent*>(event));
    case QEvent::MouseMove:
        return handleMouseMove(static_cast<QMouseEvent*>(event));
    case QEvent::MouseButtonRelease:
        return handleMouseRelease(static_cast<QMouseEvent*>(event));
    default:
        return false;
    }
}

// ─── Колесо: зум к курсору ─────────────────────────────────────────

bool MouseController::handleWheel(QWheelEvent* event)
{
    constexpr qreal zoomBase = 1.1;

    const int degrees = event->angleDelta().y();
    if (degrees == 0) {
        return false;
    }

    const qreal steps  = degrees / 120.0;
    const qreal factor = std::pow(zoomBase, steps);

    m_canvas->zoomAt(event->position(), m_canvas->zoom() * factor);

    return true;
}

// ─── ЛКМ: панорамирование ──────────────────────────────────────────

bool MouseController::handleMousePress(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return false;
    }

    m_panning    = true;
    m_panLastPos = event->position();
    m_canvas->setCursor(Qt::ClosedHandCursor);
    return true;
}

bool MouseController::handleMouseMove(QMouseEvent* event)
{
    if (!m_panning) {
        return false;
    }

    const QPointF delta = event->position() - m_panLastPos;
    m_canvas->panByPixels(delta.x(), delta.y());
    m_panLastPos = event->position();
    return true;
}

bool MouseController::handleMouseRelease(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton || !m_panning) {
        return false;
    }

    m_panning = false;
    m_canvas->setCursor(Qt::ArrowCursor);
    return true;
}

} // namespace ntcap