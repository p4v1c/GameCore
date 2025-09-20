#ifndef GAMELISTWIDGET_H
#define GAMELISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "ControllerManager.h"

class GameListWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameListWidget(QWidget *parent = nullptr);
    void loadGamesFor(const QString &emulatorName);

public slots:
    void handleControllerButton(int button);
    void handleControllerAxis(int axis, int value);

signals:
    void goBackToCarousel();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void scanRoms(const QString &path, const QStringList &extensions);
    QListWidget *gameList;
    QString selectedEmulatorName;
    int lastAxisValue;
};

#endif // GAMELISTWIDGET_H
