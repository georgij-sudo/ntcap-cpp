#include "AutomatonController.h"
#include <cstring>
#include <algorithm>

namespace ntcap {

AutomatonController::AutomatonController(Planes planes,
                                         std::unique_ptr<Automaton> automaton,
                                         QObject* parent)
    : QObject(parent)
    , m_current(std::move(planes))
    , m_automaton(std::move(automaton))
    , m_triple([this]() {
          return Planes(m_current.width(),
                        m_current.height(),
                        m_current.numPlanes());
      })
{
    m_automaton->setCurrent(m_current);
    cacheAutomatonName();
    publishSnapshot();
}

AutomatonController::~AutomatonController()
{
    stop();
}

size_t AutomatonController::generation() const noexcept {
    return m_generation.load(std::memory_order_relaxed);
}
size_t AutomatonController::renderInterval() const noexcept {
    return m_renderInterval.load(std::memory_order_relaxed);
}
size_t AutomatonController::stepDelayMs() const noexcept {
    return m_stepDelayMs.load(std::memory_order_relaxed);
}
bool AutomatonController::isRunning() const noexcept {
    return m_running.load();
}

QString AutomatonController::automatonName() const noexcept
{
    return m_cachedAutomatonName;
}

size_t AutomatonController::fieldWidth() const noexcept
{
    return m_current.width();
}

size_t AutomatonController::fieldHeight() const noexcept
{
    return m_current.height();
}

size_t AutomatonController::numPlanes() const noexcept
{
    return m_current.numPlanes();
}

void AutomatonController::cacheAutomatonName()
{
    m_cachedAutomatonName = m_automaton
        ? m_automaton->name()
        : QStringLiteral("None");
}

void AutomatonController::setRenderInterval(size_t n) {
    m_renderInterval.store(n < 1 ? 1 : n, std::memory_order_relaxed);
}
void AutomatonController::skipSteps(size_t n) {
    m_skipRemaining.fetch_add(n, std::memory_order_relaxed);
}
void AutomatonController::setStepDelay(size_t ms) {
    m_stepDelayMs.store(ms, std::memory_order_relaxed);
}

void AutomatonController::setAutomaton(std::unique_ptr<Automaton> automaton)
{
    Q_ASSERT(!m_running.load());
    m_automaton = std::move(automaton);
    m_automaton->setCurrent(m_current);
    cacheAutomatonName();
}

void AutomatonController::resetPlanes(Planes planes)
{
    Q_ASSERT(!m_running.load());
    m_current = std::move(planes);
    m_generation = 0;
    m_automaton->setCurrent(m_current);
    cacheAutomatonName();
    emit planesReset(m_current);
    publishSnapshot();
}

void AutomatonController::start()
{
    if (m_running.load())
        return;
    if (!m_automaton)
        return;
    m_running.store(true);
    QObject::connect(&m_thread, &QThread::started,
                     this,      &AutomatonController::run,
                     Qt::DirectConnection);
    m_thread.start();
}

void AutomatonController::stop()
{
    if (!m_running.load())
        return;
    m_running.store(false);
    m_thread.quit();
    m_thread.wait();
    disconnect(&m_thread, &QThread::started,
               this,      &AutomatonController::run);
}

void AutomatonController::stepOnce()
{
    Q_ASSERT(!m_running.load());
    m_automaton->step();
    m_automaton->commit();
    m_generation.fetch_add(1, std::memory_order_relaxed);
    publishSnapshot();
}

void AutomatonController::run()
{
    while (m_running.load(std::memory_order_relaxed)) {
        const size_t delay = m_stepDelayMs.load(std::memory_order_relaxed);
        if (delay > 0) {
            constexpr size_t kChunkMs = 50;
            size_t waited = 0;
            while (waited < delay
                   && m_running.load(std::memory_order_relaxed)) {
                const size_t chunk = std::min(kChunkMs, delay - waited);
                QThread::msleep(static_cast<unsigned long>(chunk));
                waited += chunk;
            }
            if (!m_running.load(std::memory_order_relaxed))
                break;
        }

        m_automaton->step();
        m_automaton->commit();
        const size_t gen =
            m_generation.fetch_add(1, std::memory_order_relaxed) + 1;

        if (!shouldRender(gen))
            continue;

        publishSnapshot();
    }
}

bool AutomatonController::shouldRender(size_t gen) noexcept
{
    size_t skip = m_skipRemaining.load(std::memory_order_relaxed);
    while (skip > 0) {
        if (m_skipRemaining.compare_exchange_weak(
                skip, skip - 1, std::memory_order_relaxed))
            return false;
    }
    const size_t interval = m_renderInterval.load(std::memory_order_relaxed);
    return interval <= 1 || (gen % interval) == 0;
}

void AutomatonController::publishSnapshot()
{
    const size_t totalBytes =
        m_current.size() * m_current.numPlanes();

    std::memcpy(m_triple.writeBuffer().data(),
                m_current.data(),
                totalBytes);
    m_triple.publish();
}

} // namespace ntcap