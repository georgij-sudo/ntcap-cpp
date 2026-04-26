// gui/AntiflickerPostProcessor.h
#pragma once

#include "PostProcessor.h"

namespace ntcap {

/**
 * @brief Экспоненциальное скользящее среднее (EMA) между кадрами.
 *
 * result = alpha * current + (1 - alpha) * previous
 *
 * Гасит высокочастотное мерцание, типичное для клеточных автоматов.
 */
class AntiflickerPostProcessor final : public PostProcessor {
public:
    /**
     * @param alpha  коэффициент EMA, допустимый диапазон (0, 1]. По умолчанию 0.5.
     */
    explicit AntiflickerPostProcessor(double alpha = 0.5);

    void process(QImage& image) override;
    void reset() override;
    QString name() const override;

    /**
     * @brief Установить коэффициент EMA.
     * @throws std::invalid_argument если alpha вне (0, 1].
     */
    void setAlpha(double alpha);
    double alpha() const noexcept;

private:
    double m_alpha;
    QImage m_prev;
};

} // namespace ntcap