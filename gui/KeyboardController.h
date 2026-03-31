#pragma once

#include <QObject>
#include <QPointer>
#include <QShortcut>
#include <QList>

namespace ntcap {

class CellularCanvas;

/**
 * Маппинг клавиш → действия над CellularCanvas.
 *
 * Не перехватывает события виджета.
 * Создаёт QShortcut-ы, привязанные к целевому виджету.
 * При уничтожении контроллера шорткаты удаляются.
 */
class KeyboardController final : public QObject {
    Q_OBJECT

public:
    /**
     * @param canvas  целевой виджет (не принимает владение).
     * @param parent  родительский QObject.
     */
    explicit KeyboardController(CellularCanvas* canvas,
                                QObject* parent = nullptr);
    ~KeyboardController() override;

private:
    /** Создаёт шорткат, привязывает к canvas, сохраняет в m_shortcuts. */
    QShortcut* addShortcut(const QKeySequence& key,
                           CellularCanvas* canvas,
                           void (CellularCanvas::*slot)());

    QList<QPointer<QShortcut>> m_shortcuts;
};

} // namespace ntcap