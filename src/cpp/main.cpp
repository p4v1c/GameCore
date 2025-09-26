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

  QLabel *sysInfoLabel = new QLabel(statusBar);
  sysInfoLabel->setStyleSheet("color: lightgray; font-size: 16px;");
  QString ip = runCommand("hostname -I | awk '{print $1}'");
  QString disk = runCommand("df -h / | awk 'NR==2{print $3\"/\"$2}'");
  sysInfoLabel->setText(QString("IP: %1 | Disk: %2").arg(ip).arg(disk));
  statusBar->addPermanentWidget(sysInfoLabel);

  // ----- MANAGER DE MANETTE -----
  ControllerManager controllerManager;
  controllerManager.initialize();

  bool controllerFound = false;
  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    if (SDL_IsGameController(i)) {
      SDL_GameController *ctrl = SDL_GameControllerOpen(i);
      if (ctrl) {
        QString name = QString::fromUtf8(SDL_GameControllerName(ctrl));
        statusBar->showMessage(QString("Manette '%1' détectée !").arg(name),
                               5000);
        SDL_GameControllerClose(ctrl);
        controllerFound = true;
        break;
      }
    }
  }
  if (!controllerFound) {
    statusBar->showMessage("En attente de manette...", 0);
  }

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
            statusBar->showMessage("Lancement du jeu...", 5000);
            isGameRunning = true;
            currentProcess = new QProcess();
            QStringList argsList;
            argsList << emulatorArgs.split(" ", Qt::SkipEmptyParts);
            argsList << gamePath;
            currentProcess->start(emulatorPath, argsList);

            QTimer::singleShot(5000, [&window]() { window.hide(); });

            QObject::connect(
                currentProcess,
                QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [&window, &isGameRunning, &currentProcess](int exitCode) {
                  isGameRunning = false;
                  window.showFullScreen();
                  window.setFocus();
                  currentProcess->deleteLater();
                  currentProcess = nullptr;
                });
          });

      PowerMenuWidget *powerMenu = new PowerMenuWidget(&window);
      powerMenu->hide();
      powerMenu->setParent(&window);
      powerMenu->setGeometry(0, 0, window.width(), window.height());

      QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
      blurEffect->setBlurRadius(0);
      centralWidget->setGraphicsEffect(blurEffect);

      QObject::connect(
          &controllerManager, &ControllerManager::buttonPressed,
          [&window, powerMenu, blurEffect, stackedWidget, &isGameRunning,
           &currentProcess, centralWidget](int button) {
            // Si jeu en cours : ignorer tous les boutons sauf GUIDE
            if (isGameRunning && currentProcess) {
              if (button == SDL_CONTROLLER_BUTTON_GUIDE) {
                QString emulatorPath = currentProcess->program();
                QString romName;
                QStringList args = currentProcess->arguments();
                if (!args.isEmpty()) {
                  QFileInfo fileInfo(args.last());
                  romName = fileInfo.fileName();
                }

                if (emulatorPath.contains("flatpak", Qt::CaseInsensitive)) {
                  qDebug() << "L'émulateur est une application Flatpak. "
                              "Tentative de récupération de l'ID...";
                  QString flatpakId;
                  for (const QString &arg : currentProcess->arguments()) {
                    if (arg.startsWith("org.") || arg.startsWith("io.") ||
                    arg.startsWith("net.") || arg.startsWith("info.")){
                      flatpakId = arg;
                      break;
                    }
                  }

                  if (!flatpakId.isEmpty()) {
                    QProcess::startDetached(
                        "flatpak", QStringList() << "kill" << flatpakId);
                    qDebug() << "Commande lancée: flatpak kill " << flatpakId;
                  } else {
                    qDebug() << "Impossible de trouver l'ID Flatpak dans les "
                                "arguments. Utilisation de pkill en dernier "
                                "recours.";
                    if (!romName.isEmpty()) {
                      QString firstWord = romName.split(" ", Qt::SkipEmptyParts).first();
                      QProcess::startDetached("pkill", QStringList() << "-f" << firstWord);
                      qDebug() << "Commande lancée: pkill -f " << firstWord;
                    } else {
                      qDebug() << "Impossible de déterminer le nom de la ROM "
                                  "pour la terminaison.";
                    }
                  }
                } else {
                  qDebug() << "L'émulateur n'est pas une application Flatpak. "
                              "Utilisation de 'pkill'.";
                  if (!romName.isEmpty()) {
                    QString firstWord = romName.split(" ", Qt::SkipEmptyParts).first();
                    QProcess::startDetached("pkill", QStringList() << "-f" << firstWord);
                    qDebug() << "Commande lancée: pkill -f " << firstWord;
                  } else {
                    qDebug() << "Impossible de déterminer le nom de la ROM "
                                "pour la terminaison.";
                  }
                }
              }
              return;
            }

            // Gestion PowerMenu
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
            } else if (!powerMenu->isHidden()) {
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
            } else {
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
