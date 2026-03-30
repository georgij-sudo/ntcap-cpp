#pragma once

#include <QObject>
#include <QTimer>

#include "Planes.h"
#include "TorusTopology.h"
#include "TableRule.h"

namespace ntcap {

/**
 * Невидимый контроллер, владеющий состоянием клеточного автомата
 * и управляющий его рабочим циклом.
 *
 * Окрестность фон Неймана, 2 состояния → TableRule<6> (сумма 0…5).
 * Топология — тор.
 *
 * После каждого шага испускает сигнал stepped().
 */
class AutomatonController final : public QObject {
    Q_OBJECT

public:
    /**
     * @param planes    двухплоскостной Planes (plane 0 — текущее, plane 1 — буфер).
     *                  Принимается по значению (move). Внешний код задаёт
     *                  начальное состояние в plane 0 до передачи.
     * @param topology  топология тора.
     * @param rule      таблица правил (индекс = сумма окрестности).
     * @param parent    родительский QObject.
     */
    explicit AutomatonController(Planes planes,
                                 TorusTopology topology,
                                 TableRule<6> rule,
                                 QObject* parent = nullptr);

    /** Указатель на внутренний Planes (plane 0 — актуальное поколение). */
    const Planes* planes() const noexcept;

    /** Номер текущего поколения (0 = начальное состояние). */
    size_t generation() const noexcept;

public slots:
    /** Вычисляет одно поколение и испускает stepped(). */
    void step();

    /** Запускает таймер (периодический вызов step()). */
    void start();

    /** Останавливает таймер. */
    void stop();

    /** Задаёт интервал таймера в миллисекундах. */
    void setInterval(int ms);

signals:
    /** Испускается после каждого завершённого шага. */
    void stepped();

private:
    Planes        m_planes;
    TorusTopology m_topology;
    TableRule<6>  m_rule;
    size_t        m_generation = 0;
    QTimer        m_timer;
};

} // namespace ntcap