#include <QApplication>
#include <QGraphicsDropShadowEffect> // Inclure le header pour QGraphicsDropShadowEffect
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "../headers/Constants.h"
#include "../headers/Emulator.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // ----- FENETRE PRINCIPALE -----
  QWidget window;
  window.setWindowTitle("RetroLauncher");
  window.resize(1000, 700);
  window.setStyleSheet("background-color: #2E2E2E;"); // gris mat

  QVBoxLayout *mainLayout = new QVBoxLayout(&window);

  // ----- SPLASH "GAME CORE" -----
  QLabel *gameCoreLabel = new QLabel();
  gameCoreLabel->setAlignment(Qt::AlignCenter);

  QString htmlText = "<span style='color:" + QString(COLOR_GAME) +
                     "; font-size:80px; font-weight:bold;'>Game</span> "
                     "<span style='color:" +
                     QString(COLOR_CORE) +
                     "; font-size:80px; font-weight:bold;'>Core</span>";
  gameCoreLabel->setText(htmlText);

  QGraphicsOpacityEffect *opacityEffect =
      new QGraphicsOpacityEffect(gameCoreLabel);
  gameCoreLabel->setGraphicsEffect(opacityEffect);
  opacityEffect->setOpacity(0.0);

  mainLayout->addStretch();
  mainLayout->addWidget(gameCoreLabel, 0, Qt::AlignCenter);
  mainLayout->addStretch();

  window.setLayout(mainLayout);
  window.show();

  // ----- FADE-IN FADE-OUT SPLASH -----
  QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
  fadeIn->setDuration(SPLASH_FADE_IN_DURATION);
  fadeIn->setStartValue(0.0);
  fadeIn->setEndValue(1.0);
  fadeIn->start();

  QTimer::singleShot(SPLASH_DISPLAY_DURATION, [opacityEffect, &mainLayout,
                                               &window]() {
    QPropertyAnimation *fadeOut =
        new QPropertyAnimation(opacityEffect, "opacity");
    fadeOut->setDuration(SPLASH_FADE_OUT_DURATION);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->start();

    // ----- APRES LE SPLASH : AFFICHAGE DES EMULATEURS -----
    QTimer::singleShot(SPLASH_FADE_OUT_DURATION, [&mainLayout, &window]() {
      // Clear le layout du splash
      QLayoutItem *item;
      while ((item = mainLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
      }

      // ----- SCAN DES EMULATEURS -----
      EmulatorManager emuManager;
      emuManager.scanEmulators("../emu"); // Remplace par ton dossier

      // Container principal pour tous les émulateurs
      QWidget *emuContainer = new QWidget();
      QHBoxLayout *emuLayout = new QHBoxLayout(emuContainer);
      emuLayout->setSpacing(50); // gros espacement entre émulateurs
      emuLayout->setContentsMargins(20, 20, 20, 20);

      for (const Emulator &emu : emuManager.getEmulators()) {
        // Widget global de l'émulateur
        QWidget *emuWidget = new QWidget();
        emuWidget->setFixedSize(200, 250);
        QVBoxLayout *emuVBox = new QVBoxLayout(emuWidget);
        emuVBox->setContentsMargins(0, 0, 0, 0);
        emuVBox->setSpacing(10);

        // ----- Nom de l'émulateur -----

        QLabel *nameLabel = new QLabel(emu.platform); // "Nintendo Switch", etc.
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet(
            "color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 "
            "#BBBBBB, stop:1 #DDDDDD);"
            "font-weight: bold;"
            "font-size: 28px;"
            "font-family: 'Arial';");
        emuVBox->addWidget(nameLabel, 0, Qt::AlignHCenter);

        // ----- Logo / Icône -----
        QLabel *iconLabel = new QLabel();
        iconLabel->setPixmap(emu.icon.scaled(150, 150, Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setStyleSheet("border-radius: 15px;"
                                 "background-color: #444444;");
        emuVBox->addWidget(iconLabel, 0, Qt::AlignCenter);

        emuVBox->addStretch();

        emuLayout->addWidget(emuWidget, 0, Qt::AlignCenter);
      }

      mainLayout->addStretch();
      mainLayout->addWidget(emuContainer, 0, Qt::AlignCenter);
      mainLayout->addStretch();

      window.setLayout(mainLayout);
    });
  });

  return app.exec();
}
