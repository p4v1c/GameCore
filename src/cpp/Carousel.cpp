#include "../headers/Carousel.h"
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QtMath>

Carousel::Carousel(QWidget *parent) : QWidget(parent), selectedIndex(0) {
  setStyleSheet("background-color: #2E2E2E;");
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setSpacing(20);
  layout->setContentsMargins(50, 0, 50, 0);

  setFocusPolicy(Qt::StrongFocus);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Carousel::scanEmulators(const QString &path) {
  EmulatorManager emuManager;
  emuManager.scanEmulators(path);

  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());
  if (!layout)
    return;

  qDeleteAll(emuWidgets);
  emuWidgets.clear();

  for (const Emulator &emu : emuManager.getEmulators()) {
    EmulatorWidget *w = new EmulatorWidget(emu, this);
    emuWidgets.push_back(w);
    layout->addWidget(w, 0, Qt::AlignCenter);
  }
  updateSelection();
}

void Carousel::keyPressEvent(QKeyEvent *event) {
  if (emuWidgets.empty())
    return;

  if (event->key() == Qt::Key_Right) {
    selectedIndex = (selectedIndex + 1) % emuWidgets.size();
    updateSelection();
  } else if (event->key() == Qt::Key_Left) {
    selectedIndex = (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
    updateSelection();
  }
}

void Carousel::updateSelection() {
  for (size_t i = 0; i < emuWidgets.size(); ++i) {
    EmulatorWidget *widget = emuWidgets[i];
    if (i == selectedIndex) {
      // Style de l'émulateur sélectionné
      widget->setStyleSheet("border: 4px solid #4C9EFF; border-radius: 15px;");
    } else {
      // Style par défaut pour les autres émulateurs
      widget->setStyleSheet(
          "border: 4px solid transparent; border-radius: 15px;");
    }
  }
}
