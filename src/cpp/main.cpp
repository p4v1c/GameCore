
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "../headers/Carousel.h"
#include "../headers/Constants.h"
#include "../headers/ControllerManager.h"
#include "../headers/Emulator.h"
#include "../headers/EmulatorWidget.h"
#include "../headers/GameListWidget.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // ----- FENÊTRE PRINCIPALE -----
  QWidget window;
  window.setWindowTitle("RetroLauncher");
  window.showFullScreen();
  window.setStyleSheet("background-color: #2E2E2E;");

  QVBoxLayout *mainLayout = new QVBoxLayout(&window);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // ----- CRÉATION DE LA BARRE DE STATUT -----
  QStatusBar *statusBar = new QStatusBar(&window);
  statusBar->setStyleSheet(
      "color: white; font-size: 16px; background: transparent; padding: 5px;");
  statusBar->setSizeGripEnabled(false);
  mainLayout->addWidget(statusBar);
  statusBar->showMessage("En attente de manette...",
                         0); // 🔹 affichage par défaut

  // ----- INITIALISATION DU MANAGER DE MANETTE -----
  ControllerManager controllerManager;
  controllerManager.initialize();

  QObject::connect(&controllerManager, &ControllerManager::controllerConnected,
                   statusBar, [statusBar](const QString &name) {
                     statusBar->showMessage(
                         QString("Manette '%1' détectée !").arg(name), 5000);
                   });
  QObject::connect(
      &controllerManager, &ControllerManager::controllerDisconnected, statusBar,
      [statusBar]() { statusBar->showMessage("Manette déconnectée.", 5000); });

  QTimer controllerTimer;
  QObject::connect(&controllerTimer, &QTimer::timeout, &controllerManager,
                   &ControllerManager::processEvents);
  controllerTimer.start(16);

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

  // ----- FADE-IN SPLASH -----
  QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
  fadeIn->setDuration(SPLASH_FADE_IN_DURATION);
  fadeIn->setStartValue(0.0);
  fadeIn->setEndValue(1.0);
  fadeIn->start();

  // ----- FADE-OUT SPLASH PUIS CAROUSEL + GAME LIST -----
  QTimer::singleShot(SPLASH_DISPLAY_DURATION, [opacityEffect, &mainLayout,
                                               &window, &controllerManager,
                                               statusBar]() {
    QPropertyAnimation *fadeOut =
        new QPropertyAnimation(opacityEffect, "opacity");
    fadeOut->setDuration(SPLASH_FADE_OUT_DURATION);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->start();

    QTimer::singleShot(SPLASH_FADE_OUT_DURATION, [&mainLayout, &window,
                                                  &controllerManager,
                                                  statusBar]() {
      // Nettoyer le layout principal sauf la status bar
      QLayoutItem *item;
      while ((item = mainLayout->takeAt(0)) != nullptr) {
        if (item->widget() != statusBar) {
          delete item->widget();
        }
        delete item;
      }

      // ----- CAROUSEL & GAMELIST -----
      Carousel *carousel = new Carousel(&window);
      carousel->setFocusPolicy(Qt::StrongFocus);
      carousel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
      carousel->setMaximumWidth(
          1200); // 🔹 limite largeur pour éviter les espaces énormes

      GameListWidget *gameListWidget = new GameListWidget(&window);

      QStackedWidget *stackedWidget = new QStackedWidget(&window);
      stackedWidget->addWidget(carousel);
      stackedWidget->addWidget(gameListWidget);

      // Connexion des signaux Carousel -> GameList
      QObject::connect(
          carousel, &Carousel::switchToGameList, stackedWidget,
          [stackedWidget, gameListWidget](const QString &emulatorName) {
            stackedWidget->setCurrentWidget(gameListWidget);
            gameListWidget->loadGamesFor(emulatorName);
            gameListWidget->setFocus();
          });

      // Connexion des signaux GameList -> Carousel
      QObject::connect(gameListWidget, &GameListWidget::goBackToCarousel,
                       stackedWidget, [stackedWidget, carousel]() {
                         stackedWidget->setCurrentWidget(carousel);
                         carousel->setFocus();
                       });

      // Gestion des entrées manette selon widget actif
      QObject::connect(
          &controllerManager, &ControllerManager::buttonPressed, stackedWidget,
          [stackedWidget](int button) {
            if (QWidget *currentWidget = stackedWidget->currentWidget()) {
              if (Carousel *carousel =
                      qobject_cast<Carousel *>(currentWidget)) {
                carousel->handleControllerButton(button);
              } else if (GameListWidget *gameListWidget =
                             qobject_cast<GameListWidget *>(currentWidget)) {
                gameListWidget->handleControllerButton(button);
              }
            }
          });

      QObject::connect(
          &controllerManager, &ControllerManager::axisMoved, stackedWidget,
          [stackedWidget](int axis, int value) {
            if (QWidget *currentWidget = stackedWidget->currentWidget()) {
              if (Carousel *carousel =
                      qobject_cast<Carousel *>(currentWidget)) {
                carousel->handleControllerAxis(axis, value);
              } else if (GameListWidget *gameListWidget =
                             qobject_cast<GameListWidget *>(currentWidget)) {
                gameListWidget->handleControllerAxis(axis, value);
              }
            }
          });

      // Placement centré du stackedWidget
      mainLayout->addStretch();
      mainLayout->addWidget(stackedWidget, 0, Qt::AlignCenter);
      mainLayout->addStretch();

      mainLayout->addWidget(statusBar); // status bar reste en bas

      carousel->scanEmulators("../emu");
      carousel->setFocus();
    });
  });

  return app.exec();
}
