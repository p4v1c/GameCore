#include "../headers/Carousel.h"
#include <QDebug>
#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QtMath>
#include <SDL.h>

const int VISIBLE_WIDGETS_COUNT = 7;

Carousel::Carousel(QWidget *parent) : QWidget(parent), selectedIndex(0) {
  setStyleSheet("background-color: #2E2E2E;");
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setSpacing(50);
  layout->setContentsMargins(50, 0, 50, 0);

  setFocusPolicy(Qt::StrongFocus);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Carousel::scanEmulators(const QString &path) {
  qDeleteAll(emuWidgets);
  emuWidgets.clear();

  QDir emuDir(path);
  if (!emuDir.exists()) {
    qWarning() << "Le dossier des émulateurs n'existe pas:" << path;
    return;
  }

  QFileInfoList subDirs =
      emuDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

  for (const QFileInfo &dirInfo : subDirs) {
    QString emulatorName = dirInfo.fileName();
    QString emulatorPath = dirInfo.absoluteFilePath();
    QString iconPath = emulatorPath + "/icon.png";

    // This line now works because we added the constructor in Emulator.h
    Emulator emu(emulatorName, emulatorPath, iconPath);

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
    updateVisibleWidgets();
  } else if (event->key() == Qt::Key_Left) {
    selectedIndex = (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
    updateVisibleWidgets();
  }
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
  if (emuWidgets.empty())
    return;

  const int THRESHOLD = 16000;

  if (axis == SDL_CONTROLLER_AXIS_LEFTX) {
    static bool axisInMotion = false;
    if (value > THRESHOLD && !axisInMotion) {
      selectedIndex = (selectedIndex + 1) % emuWidgets.size();
      updateVisibleWidgets();
      axisInMotion = true;
    } else if (value < -THRESHOLD && !axisInMotion) {
      selectedIndex =
          (selectedIndex - 1 + emuWidgets.size()) % emuWidgets.size();
      updateVisibleWidgets();
      axisInMotion = true;
    } else if (qAbs(value) <= THRESHOLD) {
      axisInMotion = false;
    }
  }
}

void Carousel::updateVisibleWidgets() {
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());
  if (!layout)
    return;

  for (EmulatorWidget *widget : emuWidgets) {
    widget->setVisible(false);
  }

  int start;
  int end;

  if (static_cast<int>(emuWidgets.size()) <= VISIBLE_WIDGETS_COUNT) {
    start = 0;
    end = emuWidgets.size() - 1;
  } else {
    int halfVisible = VISIBLE_WIDGETS_COUNT / 2;
    start = selectedIndex - halfVisible;
    end = selectedIndex + halfVisible;

    if (start < 0) {
      start = 0;
      end = VISIBLE_WIDGETS_COUNT - 1;
    } else if (end >= static_cast<int>(emuWidgets.size())) {
      end = emuWidgets.size() - 1;
      start = emuWidgets.size() - VISIBLE_WIDGETS_COUNT;
    }
  }

  // Use size_t for the loop counter to fix the second warning
  for (size_t i = start; i <= end; ++i) {
    emuWidgets[i]->setVisible(true);
  }

  updateSelection();
}

void Carousel::updateSelection() {
  // Use size_t for the loop counter to fix the third warning
  for (size_t i = 0; i < emuWidgets.size(); ++i) {
    EmulatorWidget *widget = emuWidgets[i];
    if (i == static_cast<size_t>(selectedIndex)) {
      widget->setStyleSheet("border: 4px solid #4C9EFF; border-radius: 15px;");
    } else {
      widget->setStyleSheet(
          "border: 4px solid transparent; border-radius: 15px;");
    }
  }
}
