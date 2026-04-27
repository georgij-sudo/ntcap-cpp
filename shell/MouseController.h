#pragma once

#include <QObject>
#include <QPointF>
#include <QWheelEvent>
#include <QMouseEvent>

namespace ntcap {

class CellularCanvas;

/**
 * Обработка мыши для CellularCanvas.
 *
 * Устанавливает eventFilter на целевой виджет.
 * - Колесо: зум к курсору.
 * - ЛКМ + перетаскивание: панорамирование.
 */
class MouseController final : public QObject {
    Q_OBJECT

public:
    explicit MouseController(CellularCanvas* canvas,
                             QObject* parent = nullptr);
    ~MouseController() override;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    bool handleWheel(QWheelEvent* event);
    bool handleMousePress(QMouseEvent* event);
    bool handleMouseMove(QMouseEvent* event);
    bool handleMouseRelease(QMouseEvent* event);

    CellularCanvas* m_canvas;
    bool            m_panning = false;
    QPointF         m_panLastPos;
};

} // namespace ntcap