#include "../headers/Carousel.h"
#include "../headers/Emulator.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QtMath>
#include <SDL.h>

const int VISIBLE_WIDGETS_COUNT = 5;

Carousel::Carousel(QWidget *parent)
    : QWidget(parent), selectedIndex(0), axisMovedRight(false),
      axisMovedLeft(false) {
  setStyleSheet("background-color: #2E2E2E;");
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setSpacing(10);
  layout->setContentsMargins(50, 0, 50, 0);

  setFocusPolicy(Qt::StrongFocus);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Carousel::scanEmulators(const QString &path) {
  EmulatorManager emuManager;
  emuManager.scanEmulators(path);

  qDeleteAll(emuWidgets);
  emuWidgets.clear();

  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());
  for (const Emulator &emu : emuManager.getEmulators()) {
    EmulatorWidget *w = new EmulatorWidget(emu, this);
    emuWidgets.push_back(w);
    layout->addWidget(w);
  }

  updateVisibleWidgets();
  updateSelection();
}

void Carousel::keyPressEvent(QKeyEvent *event) {
  if (emuWidgets.empty())
    return;

  if (event->key() == Qt::Key_Right) {
    selectedIndex = (selectedIndex + 1) % emuWidgets.size();
  } else if (event->key() == Qt::Key_Left) {
    selectedIndex = (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
  } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    if (selectedIndex >= 0 &&
        static_cast<size_t>(selectedIndex) < emuWidgets.size()) {
      emit switchToGameList(emuWidgets[selectedIndex]->getEmulatorName());
    }
  } else {
    QWidget::keyPressEvent(event);
    return;
  }
  updateVisibleWidgets();
  updateSelection();
}

void Carousel::handleControllerButton(int button) {
  if (emuWidgets.empty())
    return;

  if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
    selectedIndex = (selectedIndex + 1) % emuWidgets.size();
  } else if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) {
    selectedIndex = (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
  } else if (button == SDL_CONTROLLER_BUTTON_A) { // Bouton X PS4
    if (selectedIndex >= 0 &&
        static_cast<size_t>(selectedIndex) < emuWidgets.size()) {
      emit switchToGameList(emuWidgets[selectedIndex]->getEmulatorName());
    }
  }
  updateVisibleWidgets();
  updateSelection();
}

void Carousel::handleControllerAxis(int axis, int value) {
  if (emuWidgets.empty())
    return;

  const int THRESHOLD = 16000;
  const int NEUTRAL_ZONE = 8000;

  // On ne gère que l'axe horizontal
  if (axis != SDL_CONTROLLER_AXIS_LEFTX && axis != SDL_CONTROLLER_AXIS_RIGHTX)
    return;

  // Stick vers la droite
  if (value > THRESHOLD) {
    if (!axisMovedRight) {
      selectedIndex = (selectedIndex + 1) % emuWidgets.size();
      updateVisibleWidgets();
      updateSelection();
      axisMovedRight = true;
      axisMovedLeft = false;
    }
  }
  // Stick vers la gauche
  else if (value < -THRESHOLD) {
    if (!axisMovedLeft) {
      selectedIndex =
          (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
      updateVisibleWidgets();
      updateSelection();
      axisMovedLeft = true;
      axisMovedRight = false;
    }
  }
  // Stick revenu au centre
  else if (qAbs(value) < NEUTRAL_ZONE) {
    axisMovedLeft = false;
    axisMovedRight = false;
  }
}

void Carousel::updateVisibleWidgets() {
  int halfVisible = VISIBLE_WIDGETS_COUNT / 2;
  int start = selectedIndex - halfVisible;
  int end = selectedIndex + halfVisible;

  if (selectedIndex < halfVisible) {
    start = 0;
    end = VISIBLE_WIDGETS_COUNT - 1;
  } else if (selectedIndex >=
             static_cast<int>(emuWidgets.size() - halfVisible)) {
    start = emuWidgets.size() - VISIBLE_WIDGETS_COUNT;
    end = emuWidgets.size() - 1;
  }

  for (EmulatorWidget *widget : emuWidgets) {
    widget->hide();
  }

  for (int i = start; i <= end; ++i) {
    if (i >= 0 && i < emuWidgets.size()) {
      emuWidgets[i]->show();
    }
  }
}

void Carousel::updateSelection() {
  for (size_t i = 0; i < emuWidgets.size(); ++i) {
    EmulatorWidget *widget = emuWidgets[i];
    if (static_cast<int>(i) == selectedIndex) {
      widget->getIconLabel()->setStyleSheet(
          "border: 4px solid #4C9EFF; background-color:#444444; border-radius: "
          "15px;");
    } else {
      widget->getIconLabel()->setStyleSheet(
          "border: 4px solid transparent; background-color:#444444; "
          "border-radius: 15px;");
    }
  }
}
