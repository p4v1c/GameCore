#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QGraphicsBlurEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProcess>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <SDL.h>

#include "../headers/Carousel.h"
#include "../headers/Constants.h"
#include "../headers/ControllerManager.h"
#include "../headers/Emulator.h"
#include "../headers/EmulatorWidget.h"
#include "../headers/GameListWidget.h"
#include "../headers/PowerMenuWidget.h"

QString runCommand(const QString &command) {
  QProcess process;
  process.start("bash", QStringList() << "-c" << command);
  process.waitForFinished();
  return QString(process.readAllStandardOutput()).trimmed();
}

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
  window.setLayout(mainLayout);

  // ----- STATUS BAR -----
  QStatusBar *statusBar = new QStatusBar(&window);
  statusBar->setStyleSheet(
      "color: white; font-size: 16px; background: transparent; padding: 5px;");
  statusBar->setSizeGripEnabled(false);
  statusBar->showMessage("En attente de manette...", 0);

  // Widget permanent à droite pour IP + Disk
  QLabel *sysInfoLabel = new QLabel(statusBar);
  sysInfoLabel->setStyleSheet("color: lightgray; font-size: 16px;");
  QString ip = runCommand("hostname -I | awk '{print $1}'");
  QString disk = runCommand("df -h / | awk 'NR==2{print $3\"/\"$2}'");
  sysInfoLabel->setText(QString("IP: %1 | Disk: %2").arg(ip).arg(disk));
  statusBar->addPermanentWidget(sysInfoLabel);

  // ----- MANAGER DE MANETTE -----
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

  window.show();

  QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
  fadeIn->setDuration(SPLASH_FADE_IN_DURATION);
  fadeIn->setStartValue(0.0);
  fadeIn->setEndValue(1.0);
  fadeIn->start();

  bool isGameRunning = false;
  QProcess *currentProcess = nullptr;

  // ----- FADE-OUT SPLASH PUIS CAROUSEL + GAME LIST -----
  QTimer::singleShot(SPLASH_DISPLAY_DURATION, [opacityEffect, &mainLayout,
                                               &window, &controllerManager,
                                               statusBar, &isGameRunning,
                                               &currentProcess]() {
    QPropertyAnimation *fadeOut =
        new QPropertyAnimation(opacityEffect, "opacity");
    fadeOut->setDuration(SPLASH_FADE_OUT_DURATION);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->start();

    QTimer::singleShot(SPLASH_FADE_OUT_DURATION, [&mainLayout, &window,
                                                  &controllerManager, statusBar,
                                                  &isGameRunning,
                                                  &currentProcess]() {
      QLayoutItem *item;
      while ((item = mainLayout->takeAt(0)) != nullptr) {
        if (item->widget() != statusBar) {
          delete item->widget();
        }
        delete item;
      }

      QWidget *centralWidget = new QWidget(&window);
      QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
      centralLayout->setContentsMargins(0, 0, 0, 0);

      // Création de la barre d'en-tête pour l'heure
      QHBoxLayout *headerLayout = new QHBoxLayout();
      QLabel *clockLabel = new QLabel(centralWidget);
      clockLabel->setStyleSheet("color: white; font-size: 24px; padding-left: "
                                "20px; padding-top: 10px;");
      clockLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

      headerLayout->addWidget(clockLabel);
      headerLayout->addStretch();

      centralLayout->addLayout(headerLayout);
      centralLayout->addStretch();

      Carousel *carousel = new Carousel(&window);
      carousel->setFocusPolicy(Qt::StrongFocus);
      carousel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      carousel->setMaximumWidth(1200);

      GameListWidget *gameListWidget = new GameListWidget(&window);

      QStackedWidget *stackedWidget = new QStackedWidget(&window);
      stackedWidget->addWidget(carousel);
      stackedWidget->addWidget(gameListWidget);
      stackedWidget->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Expanding);

      centralLayout->addWidget(stackedWidget, 0, Qt::AlignCenter);

      centralLayout->addStretch();

      QObject::connect(
          carousel, &Carousel::switchToGameList, stackedWidget,
          [stackedWidget, gameListWidget](const QString &emulatorName) {
            stackedWidget->setCurrentWidget(gameListWidget);
            gameListWidget->loadGamesFor(emulatorName);
            gameListWidget->setFocus();
          });

      QObject::connect(gameListWidget, &GameListWidget::goBackToCarousel,
                       stackedWidget, [stackedWidget, carousel]() {
                         stackedWidget->setCurrentWidget(carousel);
                         carousel->setFocus();
                       });

      QObject::connect(
          gameListWidget, &GameListWidget::launchGame, &window,
          [&window, &isGameRunning, &currentProcess,
           statusBar](const QString &emulatorPath, const QString &emulatorArgs,
                      const QString &gamePath) {
            qDebug() << "Signal launchGame reçu.";
            statusBar->showMessage("Lancement du jeu...", 5000);
            isGameRunning = true;
            currentProcess = new QProcess();
            QStringList argsList;
            argsList << emulatorArgs.split(" ", Qt::SkipEmptyParts);
            argsList << gamePath;
            qDebug() << "Lancement du jeu avec la commande:" << emulatorPath
                     << argsList;
            currentProcess->start(emulatorPath, argsList);

            QTimer::singleShot(5000, [&window]() {
              qDebug() << "Délai de 5 secondes écoulé. Masquage de la fenêtre.";
              window.hide();
            });

            QObject::connect(
                currentProcess,
                QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [&window, &isGameRunning, &currentProcess](int exitCode) {
                  qDebug() << "Le processus principal a terminé avec le code:"
                           << exitCode;
                  isGameRunning = false;
                  window.showFullScreen();
                  window.setFocus();
                  currentProcess->deleteLater();
                  currentProcess = nullptr;
                });
          });

      // Nouveau : Ajout du menu d'alimentation et de l'effet de flou
      PowerMenuWidget *powerMenu = new PowerMenuWidget(&window);
      powerMenu->hide();
      powerMenu->setParent(&window);
      powerMenu->setGeometry(0, 0, window.width(), window.height());

      // Appliquer l'effet de flou au widget central, pas à la fenêtre entière
      QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
      blurEffect->setBlurRadius(0);
      centralWidget->setGraphicsEffect(blurEffect);

      // Nouveau : Connexion des signaux de la manette pour le menu
      // d'alimentation
      QObject::connect(
          &controllerManager, &ControllerManager::buttonPressed,
          [&window, powerMenu, blurEffect, stackedWidget, &isGameRunning,
           &currentProcess, centralWidget](int button) {
            // Le bouton Back (ou Guide) ouvre/ferme le menu, et gère le flou
            if (button == SDL_CONTROLLER_BUTTON_BACK ||
                button == SDL_CONTROLLER_BUTTON_GUIDE) {
              if (powerMenu->isHidden()) {
                powerMenu->show();
                QPropertyAnimation *anim =
                    new QPropertyAnimation(blurEffect, "blurRadius");
                anim->setDuration(300);
                anim->setStartValue(0);
                anim->setEndValue(30);
                anim->start();
              } else {
                QPropertyAnimation *anim =
                    new QPropertyAnimation(blurEffect, "blurRadius");
                anim->setDuration(300);
                anim->setStartValue(30);
                anim->setEndValue(0);
                QObject::connect(anim, &QPropertyAnimation::finished,
                                 [powerMenu]() { powerMenu->hide(); });
                anim->start();
              }
            }
            // Si le menu est visible, on passe l'événement au menu pour le
            // gérer
            else if (!powerMenu->isHidden()) {
              // C'est ici que le bouton 'O' est géré pour cacher le menu
              if (button == SDL_CONTROLLER_BUTTON_B) {
                QPropertyAnimation *anim =
                    new QPropertyAnimation(blurEffect, "blurRadius");
                anim->setDuration(300);
                anim->setStartValue(30);
                anim->setEndValue(0);
                QObject::connect(anim, &QPropertyAnimation::finished,
                                 [powerMenu]() { powerMenu->hide(); });
                anim->start();
              } else {
                powerMenu->handleControllerButton(button);
              }
            }
            // Si un jeu est en cours, le bouton Guide sert de raccourci pour le
            // quitter
            else if (isGameRunning && currentProcess) {
              if (button == SDL_CONTROLLER_BUTTON_GUIDE) {
                qDebug() << "Bouton Guide pressé pour quitter le jeu.";
                currentProcess->terminate();
              }
            }
            // Sinon, on transmet l'événement au widget de la pile (Carousel ou
            // GameListWidget)
            else {
              if (QWidget *currentWidget = stackedWidget->currentWidget()) {
                if (Carousel *carousel =
                        qobject_cast<Carousel *>(currentWidget)) {
                  carousel->handleControllerButton(button);
                } else if (GameListWidget *gameListWidget =
                               qobject_cast<GameListWidget *>(currentWidget)) {
                  gameListWidget->handleControllerButton(button);
                }
              }
            }
          });

      QObject::connect(
          &controllerManager, &ControllerManager::axisMoved,
          [powerMenu, stackedWidget, &isGameRunning](int axis, int value) {
            if (isGameRunning)
              return;
            if (!powerMenu->isHidden()) {
              powerMenu->handleControllerAxis(axis, value);
            } else {
              if (QWidget *currentWidget = stackedWidget->currentWidget()) {
                if (Carousel *carousel =
                        qobject_cast<Carousel *>(currentWidget)) {
                  carousel->handleControllerAxis(axis, value);
                } else if (GameListWidget *gameListWidget =
                               qobject_cast<GameListWidget *>(currentWidget)) {
                  gameListWidget->handleControllerAxis(axis, value);
                }
              }
            }
          });

      // Reconnexion de l'horloge
      QTimer *clockTimer = new QTimer(centralWidget);
      QObject::connect(clockTimer, &QTimer::timeout, [clockLabel]() {
        clockLabel->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
      });
      clockTimer->start(1000);

      mainLayout->addWidget(centralWidget);
      mainLayout->addWidget(statusBar);

      carousel->scanEmulators("../emu");
      carousel->setFocus();
    });
  });

  return app.exec();
}
