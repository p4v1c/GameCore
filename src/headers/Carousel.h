#ifndef CAROUSEL_H
#define CAROUSEL_H

#include "ControllerManager.h"
#include "EmulatorWidget.h"
#include <QWidget>
#include <vector>

class Carousel : public QWidget {
  Q_OBJECT

public:
  explicit Carousel(QWidget *parent = nullptr);
  void scanEmulators(const QString &path);

public slots:
  void handleControllerButton(int button);
  void handleControllerAxis(int axis, int value);

signals:
  void switchToGameList(const QString &emulatorName);

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  void updateVisibleWidgets();
  void updateSelection();

  std::vector<EmulatorWidget *> emuWidgets;
  int selectedIndex;
  int lastAxisValue;
};

#endif // CAROUSEL_H
