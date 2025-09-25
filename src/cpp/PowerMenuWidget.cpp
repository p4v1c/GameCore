#include "../headers/PowerMenuWidget.h"
#include <QDebug>
#include <QListWidgetItem>
#include <QVBoxLayout>

PowerMenuWidget::PowerMenuWidget(QWidget *parent)
    : QWidget(parent), lastAxisValue(0) {
  // Le menu est un QListWidget
  menuList = new QListWidget(this);
  menuList->setStyleSheet("QListWidget {"
                          "    background-color: rgba(30, 30, 30, 200);"
                          "    color: white;"
                          "    font-size: 28px;"
                          "    border: 2px solid #555555;"
                          "    border-radius: 15px;"
                          "    padding: 0px;"
                          "}"
                          "QListWidget::item {"
                          "    padding: 15px;"
                          "    text-align: center;"
                          "    border-bottom: none;"
                          "}"
                          "QListWidget::item:selected {"
                          "    background-color: #007AFF;"
                          "    border-radius: 10px;"
                          "}");

  menuList->setSpacing(0);
  menuList->setUniformItemSizes(true);
  menuList->setFocusPolicy(Qt::NoFocus);

  // Ajout des options au menu
  menuList->addItem(tr("Redémarrer"));
  menuList->item(0)->setTextAlignment(Qt::AlignCenter);

  menuList->addItem(tr("Éteindre"));
  menuList->item(1)->setTextAlignment(Qt::AlignCenter);
  menuList->setCurrentRow(0);

  // Ajuster la hauteur du widget à la taille des items
  int itemHeight = menuList->sizeHintForRow(0);
  int totalHeight = itemHeight * menuList->count() + 2 * menuList->frameWidth();
  menuList->setFixedHeight(totalHeight);
  menuList->setFixedWidth(400);

  // Layout sans marges
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(menuList, 0, Qt::AlignCenter);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);

  // Connexion des signaux
  connect(menuList, &QListWidget::itemClicked, this,
          [this](QListWidgetItem *item) {
            if (item->text() == tr("Redémarrer")) {
              onRestartSelected();
            } else if (item->text() == tr("Éteindre")) {
              onShutdownSelected();
            }
          });

  // Le focus doit être sur ce widget, pas sur la liste
  setFocusPolicy(Qt::StrongFocus);
  menuList->setFocusPolicy(Qt::NoFocus);
}

void PowerMenuWidget::handleControllerButton(int button) {
  if (menuList->count() == 0)
    return;

  if (button == SDL_CONTROLLER_BUTTON_A) {
    if (menuList->currentRow() == 0) {
      onRestartSelected();
    } else if (menuList->currentRow() == 1) {
      onShutdownSelected();
    }
    emit hideMenu();
  } else if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
    menuList->setCurrentRow((menuList->currentRow() + 1) % menuList->count());
  } else if (button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
    menuList->setCurrentRow((menuList->currentRow() - 1 + menuList->count()) %
                            menuList->count());
  }
}

void PowerMenuWidget::handleControllerAxis(int axis, int value) {
  if (menuList->count() == 0)
    return;

  const int THRESHOLD = 16000;

  if (axis != SDL_CONTROLLER_AXIS_LEFTY && axis != SDL_CONTROLLER_AXIS_RIGHTY)
    return;

  if (value > THRESHOLD && lastAxisValue <= THRESHOLD) {
    menuList->setCurrentRow((menuList->currentRow() + 1) % menuList->count());
  } else if (value < -THRESHOLD && lastAxisValue >= -THRESHOLD) {
    menuList->setCurrentRow((menuList->currentRow() - 1 + menuList->count()) %
                            menuList->count());
  }

  lastAxisValue = value;
}

void PowerMenuWidget::onShutdownSelected() {
  qDebug() << "Simulation: Arrêt du système...";
  // QProcess::startDetached("shutdown", QStringList() << "now");
}

void PowerMenuWidget::onRestartSelected() {
  qDebug() << "Simulation: Redémarrage du système...";
  // QProcess::startDetached("reboot", QStringList());
}
