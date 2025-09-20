#ifndef GAMELISTWIDGET_H
#define GAMELISTWIDGET_H

#include "ControllerManager.h"
#include <QListWidget>
#include <QWidget>
#include <QKeyEvent>

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
    void launchGame(const QString &emulatorPath, const QString &emulatorArgs, const QString &gamePath);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void launchSelectedGame();

private:
    void scanRoms(const QString &path, const QStringList &extensions);
    QListWidget *gameList;
    QString selectedEmulatorName;
    int lastAxisValue;
};

#endif // GAMELISTWIDGET_H
