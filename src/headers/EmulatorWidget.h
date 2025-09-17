#ifndef EMULATORWIDGET_H
#define EMULATORWIDGET_H

#include "Emulator.h"
#include <QLabel>
#include <QWidget>

class EmulatorWidget : public QWidget {
  Q_OBJECT

public:
  // Constructeur qui prend un objet Emulator et un parent (le widget conteneur)
  explicit EmulatorWidget(const Emulator &emu, QWidget *parent = nullptr);

  // Getters pour accéder aux labels de l'extérieur
  QLabel *getNameLabel() const;
  QLabel *getIconLabel() const;

private:
  QLabel *nameLabel;
  QLabel *iconLabel;
};

#endif // EMULATORWIDGET_H
