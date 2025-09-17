#include "../headers/Carousel.h"
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QtMath>

// Définition du nombre d'émulateurs visibles à l'écran
const int VISIBLE_WIDGETS_COUNT = 5;

Carousel::Carousel(QWidget *parent) : QWidget(parent), selectedIndex(0) {
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

  // Vider la liste existante des widgets
  qDeleteAll(emuWidgets);
  emuWidgets.clear();

  // Créer tous les widgets et les stocker en mémoire
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
  } else {
    return; // Ne pas mettre à jour si la touche n'est pas une flèche
  }

  updateVisibleWidgets();
}


void Carousel::handleControllerButton(int button) {
    if (emuWidgets.empty())
        return;

    if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
        selectedIndex = (selectedIndex + 1) % emuWidgets.size();
        updateVisibleWidgets();
    } else if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) {
        selectedIndex = (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
        updateVisibleWidgets();
    }
}

void Carousel::handleControllerAxis(int axis, int value) {
    if (emuWidgets.empty() || axis != SDL_CONTROLLER_AXIS_LEFTX)
        return;

    const int THRESHOLD = 16000;

    // Mouvement vers la droite
    if (value > THRESHOLD && lastAxisValue <= THRESHOLD) {
        selectedIndex = (selectedIndex + 1) % emuWidgets.size();
        updateVisibleWidgets();
    }
    // Mouvement vers la gauche
    else if (value < -THRESHOLD && lastAxisValue >= -THRESHOLD) {
        selectedIndex = (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
        updateVisibleWidgets();
    }

    lastAxisValue = value;
}

void Carousel::updateVisibleWidgets() {
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());
  if (!layout)
    return;

  // Masquer tous les widgets avant de réafficher les visibles
  for (EmulatorWidget *widget : emuWidgets) {
    widget->hide();
    // Optionnel : Retirer du layout pour optimiser, mais hide() suffit souvent
    // layout->removeWidget(widget);
  }

  int halfVisible = VISIBLE_WIDGETS_COUNT / 2;
  int start = selectedIndex - halfVisible;
  int end = selectedIndex + halfVisible;

  // Gestion des cas extrêmes au début et à la fin de la liste
  if (start < 0) {
    start = 0;
    end = qMin(static_cast<int>(emuWidgets.size() - 1),
               VISIBLE_WIDGETS_COUNT - 1);
  } else if (end >= emuWidgets.size()) {
    end = emuWidgets.size() - 1;
    start =
        qMax(0, static_cast<int>(emuWidgets.size() - VISIBLE_WIDGETS_COUNT));
  }

  // Ajouter les widgets visibles au layout et les afficher
  for (int i = start; i <= end; ++i) {
    layout->addWidget(emuWidgets[i], 0, Qt::AlignCenter);
    emuWidgets[i]->show();
  }
  updateSelection();
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
