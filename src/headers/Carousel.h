#ifndef CAROUSEL_H
#define CAROUSEL_H

#include "Emulator.h"
#include "EmulatorWidget.h"
#include <QKeyEvent>
#include <QWidget>
#include <vector>

class Carousel : public QWidget {
  Q_OBJECT

public:
  explicit Carousel(QWidget *parent = nullptr);
  void scanEmulators(const QString &path);

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void updateSelection();

private:
  void updateVisibleWidgets(); // Ajout de la déclaration de la fonction
  std::vector<EmulatorWidget *> emuWidgets;
  int selectedIndex;
};

#endif // CAROUSEL_H
