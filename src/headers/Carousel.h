#ifndef CAROUSEL_H
#define CAROUSEL_H

#include <QWidget>
#include <vector>

#include "../headers/Emulator.h"
#include "../headers/EmulatorWidget.h" // Assurez-vous que cette inclusion est correcte si EmulatorWidget existe

class Carousel : public QWidget {
Q_OBJECT // <<-- INDISPENSABLE POUR LES SLOTS ET SIGNAUX

    public : explicit Carousel(QWidget *parent = nullptr);
  void scanEmulators(const QString &path);

protected:
  void keyPressEvent(QKeyEvent *event) override;

public slots: // <<-- DOIT ÊTRE PRÉSENT ET CORRECTEMENT PLACÉ
  void handleControllerButton(int button);
  void handleControllerAxis(int axis, int value);

private:
  void updateVisibleWidgets();
  void updateSelection();

  std::vector<EmulatorWidget *> emuWidgets;
  int selectedIndex;
};

#endif // CAROUSEL_H
