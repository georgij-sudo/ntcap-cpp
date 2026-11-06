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
#include "TrailPostProcessor.h"
#include "AntiflickerPostProcessor.h"
#include "GrayscalePostProcessor.h"

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

    /**
     * @brief Field width in cells.
     */
    size_t fieldWidth() const noexcept;

    /**
     * @brief Field height in cells.
     */
    size_t fieldHeight() const noexcept;

    /**
     * @brief Number of completed paint events (for render FPS).
     */
    uint64_t paintCount() const noexcept;

    void setPanStep(qreal pixels) noexcept;
    void setZoomFactor(qreal factor) noexcept;

    QSize sizeHint()        const override;
    QSize minimumSizeHint() const override;

    void setDisplayStrategies(std::vector<DisplayStrategyPtr> strategies) noexcept;
    void setDisplayStrategyIndex(size_t idx) noexcept;
    size_t displayStrategyIndex() const noexcept;
    const std::vector<DisplayStrategyPtr>& availableDisplayStrategies() const noexcept;

    // ── Post-processing API ─────────────────────────────

    void setTrailEnabled(bool enabled) noexcept;
    bool isTrailEnabled() const noexcept;
    void setTrailDecay(double decay);

    void setAntiflickerEnabled(bool enabled) noexcept;
    bool isAntiflickerEnabled() const noexcept;
    void setAntiflickerAlpha(double alpha);

    void setGrayscaleEnabled(bool enabled) noexcept;
    bool isGrayscaleEnabled() const noexcept;

    void resetPostProcessors() noexcept;

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

    void cycleDisplayModeNext() noexcept;
    void cycleDisplayModePrev() noexcept;

signals:
    void displayStrategyChanged(size_t index);

protected:
    void paintEvent(QPaintEvent* event)   override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void    initDefaults();
    void    rebuildColorTable();
    void    rebuildImage();
    void    applyPostProcessing();
    void    renderLastFrame();
    void    resetStatefulPostProcessors();
    QPointF widgetToField(const QPointF& widgetPos) const;

    size_t                m_fieldWidth   = 0;
    size_t                m_fieldHeight  = 0;

    TripleBuffer<Planes>* m_triple       = nullptr;
    size_t                m_planeIndex   = 0;
    bool                  m_hasFrame     = false;

    QMap<uint8_t, QRgb>   m_colorMap;
    QRgb                  m_defaultColor = qRgb(255, 255, 255);

    QImage                m_image;
    QImage                m_processedImage;

    qreal   m_zoom       = 1.0;
    QPointF m_offset     = {0.0, 0.0};
    qreal   m_panStep    = 20.0;
    qreal   m_zoomFactor = 1.25;
    bool    m_fitPending = true;

    std::vector<DisplayStrategyPtr> m_strategies;
    size_t                          m_strategyIndex = 0;

    TrailPostProcessor       m_trailProcessor;
    AntiflickerPostProcessor m_antiflickerProcessor;
    GrayscalePostProcessor   m_grayscaleProcessor;

    bool m_trailEnabled       = false;
    bool m_antiflickerEnabled = false;
    bool m_grayscaleEnabled   = false;

    uint64_t m_paintCount = 0;
};

} // namespace ntcap