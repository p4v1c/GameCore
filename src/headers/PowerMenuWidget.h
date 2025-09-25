#ifndef POWERMENUWIDGET_H
#define POWERMENUWIDGET_H

#include <QGraphicsBlurEffect>
#include <QListWidget>
#include <QProcess>
#include <QWidget>
#include <SDL.h>

class PowerMenuWidget : public QWidget {
  Q_OBJECT

public:
  explicit PowerMenuWidget(QWidget *parent = nullptr);
  void handleControllerAxis(int axis, int value);
  void handleControllerButton(int button);

signals:
  void hideMenu();

private slots:
  void onShutdownSelected();
  void onRestartSelected();

private:
  QListWidget *menuList;
  int lastAxisValue; // Ajout de cette ligne
};

#endif // POWERMENUWIDGET_H
