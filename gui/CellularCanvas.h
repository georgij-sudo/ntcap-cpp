#pragma once
#include <QWidget>
#include <QImage>
#include <QMap>
#include <QRgb>
#include <QPointF>
#include <vector>
#include <memory>
#include "Planes.h"
#include "TripleBuffer.h"
#include "DisplayStrategy.h"

namespace ntcap {

class CellularCanvas final : public QWidget {
    Q_OBJECT
public:
    explicit CellularCanvas(QWidget* parent = nullptr);
    CellularCanvas(size_t fieldWidth, size_t fieldHeight,
                   QWidget* parent = nullptr);

    void setFieldSize(size_t width, size_t height);

    void setTripleBuffer(TripleBuffer<Planes>* tb,
                         size_t planeIndex = 0) noexcept;

    void setColorMap(const QMap<uint8_t, QRgb>& colorMap) noexcept;
    void setDefaultColor(QRgb color) noexcept;

    qreal   zoom()   const noexcept;
    QPointF offset() const noexcept;
    void setPanStep(qreal pixels) noexcept;
    void setZoomFactor(qreal factor) noexcept;

    QSize sizeHint()        const override;
    QSize minimumSizeHint() const override;

    /**
     * @brief Установить список доступных стратегий отображения.
     *
     * Порядок стратегий будет использоваться при циклическом переключении.
     * Если текущий индекс стратегии >= нового числа стратегий, индекс
     * будет сброшен в 0 (Single Plane 0 при классическом порядке).
     */
    void setDisplayStrategies(std::vector<DisplayStrategyPtr> strategies) noexcept;

    /**
     * @brief Установить выбранную стратегию по индексу в списке, если индекс
     *        корректен. Если индекс вне диапазона — игнорируем.
     */
    void setDisplayStrategyIndex(size_t idx) noexcept;

    /**
     * @brief Вернуть индекс выбранной стратегии из ранее установленного списка.
     */
    size_t displayStrategyIndex() const noexcept;

    /**
     * @brief Вернуть список доступных стратегий (shared pointers).
     */
    const std::vector<DisplayStrategyPtr>& availableDisplayStrategies() const noexcept;

public slots:
    void refresh();
    void zoomIn();
    void zoomOut();
    void panLeft();
    void panRight();
    void panUp();
    void panDown();
    void zoomAt(const QPointF& widgetAnchor, qreal newZoom);
    void panByPixels(qreal dx, qreal dy);
    void fitToWindow();

    /** Переключить режим отображения на следующий (по кругу). */
    void cycleDisplayModeNext() noexcept;
    /** Переключить режим отображения на предыдущий (по кругу). */
    void cycleDisplayModePrev() noexcept;

signals:
    /**
     * @brief Сигнал, оповещающий о смене выбранной стратегии (индекс в списке).
     */
    void displayStrategyChanged(size_t index);

protected:
    void paintEvent(QPaintEvent* event)   override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void    initDefaults();
    void    rebuildColorTable();
    void    rebuildImage();
    QPointF widgetToField(const QPointF& widgetPos) const;

    size_t                m_fieldWidth   = 0;
    size_t                m_fieldHeight  = 0;
    TripleBuffer<Planes>* m_triple       = nullptr;
    size_t                m_planeIndex   = 0;

    QMap<uint8_t, QRgb>   m_colorMap;
    QRgb                  m_defaultColor = qRgb(255, 255, 255);
    QImage                m_image;

    qreal   m_zoom       = 1.0;
    QPointF m_offset     = {0.0, 0.0};
    qreal   m_panStep    = 20.0;
    qreal   m_zoomFactor = 1.25;
    bool    m_fitPending = true;

    // display strategies:
    std::vector<DisplayStrategyPtr> m_strategies;
    size_t                          m_strategyIndex = 0;
};

} // namespace ntcap