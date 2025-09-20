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
    : QWidget(parent), selectedIndex(0), lastAxisValue(0) {
  setStyleSheet("background-color: #2E2E2E;");
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setSpacing(50);
  layout->setContentsMargins(50, 0, 50, 0);

  setFocusPolicy(Qt::StrongFocus);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Carousel::scanEmulators(const QString &path) {
  EmulatorManager emuManager;
  emuManager.scanEmulators(path);

  qDeleteAll(emuWidgets);
  emuWidgets.clear();

  for (const Emulator &emu : emuManager.getEmulators()) {
    EmulatorWidget *w = new EmulatorWidget(emu, this);
    emuWidgets.push_back(w);
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
  } else if (button == SDL_CONTROLLER_BUTTON_A) { // Bouton Croix (X) de la PS4
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
  if (axis == SDL_CONTROLLER_AXIS_LEFTX || axis == SDL_CONTROLLER_AXIS_RIGHTX) {
    if (value > THRESHOLD && lastAxisValue <= THRESHOLD) {
      selectedIndex = (selectedIndex + 1) % emuWidgets.size();
    } else if (value < -THRESHOLD && lastAxisValue >= -THRESHOLD) {
      selectedIndex =
          (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
    }
    lastAxisValue = value;
  } else {
    lastAxisValue = 0;
    return;
  }

  updateVisibleWidgets();
  updateSelection();
}

void Carousel::updateVisibleWidgets() {
  QLayout *layout = this->layout();
  if (!layout)
    return;

  // Masquer tous les widgets et les retirer du layout
  for (EmulatorWidget *widget : emuWidgets) {
    layout->removeWidget(widget);
    widget->hide();
  }

  int halfVisible = VISIBLE_WIDGETS_COUNT / 2;
  int start = selectedIndex - halfVisible;
  int end = selectedIndex + halfVisible;

  // Gestion des cas extrêmes au début et à la fin de la liste
  if (start < 0) {
    start = 0;
    end = qMin(static_cast<int>(emuWidgets.size() - 1),
               VISIBLE_WIDGETS_COUNT - 1);
  } else if (end >= static_cast<int>(emuWidgets.size())) {
    end = static_cast<int>(emuWidgets.size() - 1);
    start =
        qMax(0, static_cast<int>(emuWidgets.size() - VISIBLE_WIDGETS_COUNT));
  }

  // Ajouter les widgets visibles au layout et les afficher
  for (int i = start; i <= end; ++i) {
    layout->addWidget(emuWidgets[i]);
    emuWidgets[i]->show();
  }

  updateSelection();
}

void Carousel::updateSelection() {
  for (int i = 0; i < static_cast<int>(emuWidgets.size()); ++i) {
    EmulatorWidget *widget = emuWidgets[i];
    if (i == selectedIndex) {
      // L'élément sélectionné est mis en avant
      widget->setGraphicsEffect(nullptr); // Réinitialiser l'effet
      widget->setFixedSize(QSize(250, 300));
      widget->setStyleSheet(
          "background-color: #4A4A4A; border: 2px solid #007AFF;");
    } else {
      // Les autres éléments sont plus petits
      widget->setFixedSize(QSize(200, 250));
      widget->setStyleSheet("background-color: #3C3C3C; border: none;");

      // Effet d'ombre pour les widgets non sélectionnés
      QGraphicsDropShadowEffect *shadowEffect =
          new QGraphicsDropShadowEffect(widget);
      shadowEffect->setBlurRadius(10);
      shadowEffect->setColor(QColor(0, 0, 0, 150));
      shadowEffect->setOffset(5, 5);
      widget->setGraphicsEffect(shadowEffect);
    }
  }
}
