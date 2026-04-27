// gui/TrailPostProcessor.h
#pragma once

#include "PostProcessor.h"

namespace ntcap {

/**
 * @brief Эффект затухающего следа (afterglow).
 *
 * Для каждого пикселя: trail = max(current, previous_trail * decay).
 * Живые клетки дают полную яркость, мёртвые — экспоненциально затухают.
 *
 * @param decay  коэффициент затухания (0, 1). По умолчанию 0.85 (~15 кадров).
 */
class TrailPostProcessor final : public PostProcessor {
public:
    explicit TrailPostProcessor(double decay = 0.85);

    void process(QImage& image) override;
    void reset() override;
    QString name() const override;

    /**
     * @brief Установить коэффициент затухания.
     * @throws std::invalid_argument если decay вне (0, 1).
     */
    void setDecay(double decay);
    double decay() const noexcept;

private:
    double m_decay;
    QImage m_trail;
};

} // namespace ntcap