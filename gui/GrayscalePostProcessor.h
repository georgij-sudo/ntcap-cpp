// gui/GrayscalePostProcessor.h
#pragma once

#include "PostProcessor.h"

namespace ntcap {

/**
 * @brief Преобразует ARGB32-кадр в оттенки серого (ITU-R BT.601).
 *
 * Stateless — не хранит буферов между кадрами.
 */
class GrayscalePostProcessor final : public PostProcessor {
public:
    void process(QImage& image) override;
    void reset() override;
    QString name() const override;
};

} // namespace ntcap