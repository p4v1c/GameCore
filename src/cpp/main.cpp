#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
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

  // ----- STATUS BAR -----
  QStatusBar *statusBar = new QStatusBar(&window);
  statusBar->setStyleSheet(
      "color: white; font-size: 16px; background: transparent; padding: 5px;");
  statusBar->setSizeGripEnabled(false);
  mainLayout->addWidget(statusBar);
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

  window.setLayout(mainLayout);
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

            // NOUVEAU: Le jeu est lancé immédiatement
            statusBar->showMessage("Lancement du jeu...", 5000);
            isGameRunning = true;

            currentProcess = new QProcess();

            QStringList argsList;
            argsList << emulatorArgs.split(" ", Qt::SkipEmptyParts);
            argsList << gamePath;

            qDebug() << "Lancement du jeu avec la commande:" << emulatorPath
                     << argsList;
            currentProcess->start(emulatorPath, argsList);

            // NOUVEAU: La fenêtre se cache après un délai de 5 secondes
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

      QObject::connect(
          &controllerManager, &ControllerManager::buttonPressed, stackedWidget,
          [stackedWidget, &isGameRunning, &currentProcess](int button) {
            if (button == SDL_CONTROLLER_BUTTON_GUIDE) {
              qDebug() << "Bouton Guide pressé.";
              if (isGameRunning && currentProcess) {
                QString emulatorPath = currentProcess->program();

                if (emulatorPath.contains("flatpak", Qt::CaseInsensitive)) {
                  qDebug() << "L'émulateur est une application Flatpak. "
                              "Tentative de récupération de l'ID...";
                  QString flatpakId;
                  for (const QString &arg : currentProcess->arguments()) {
                    if (arg.startsWith("org.")) {
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
                    QStringList args = currentProcess->arguments();
                    if (!args.isEmpty()) {
                      QFileInfo fileInfo(args.last());
                      QString romName = fileInfo.fileName();
                      if (!romName.isEmpty()) {
                        QProcess::startDetached(
                            "pkill", QStringList() << "-f" << romName);
                        qDebug() << "Commande lancée: pkill -f " << romName;
                      } else {
                        qDebug() << "Impossible de déterminer le nom de la ROM "
                                    "pour la terminaison.";
                      }
                    }
                  }
                } else {
                  qDebug() << "L'émulateur n'est pas une application Flatpak. "
                              "Utilisation de 'pkill'.";
                  QStringList args = currentProcess->arguments();
                  if (!args.isEmpty()) {
                    QFileInfo fileInfo(args.last());
                    QString romName = fileInfo.fileName();
                    if (!romName.isEmpty()) {
                      QProcess::startDetached("pkill", QStringList()
                                                           << "-f" << romName);
                      qDebug() << "Commande lancée: pkill -f " << romName;
                    } else {
                      qDebug() << "Impossible de déterminer le nom de la ROM "
                                  "pour la terminaison.";
                    }
                  }
                }
              } else {
                qDebug() << "Aucun jeu n'est en cours, pas de terminaison "
                            "nécessaire.";
              }
            }
            if (isGameRunning)
              return;
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
          [stackedWidget, &isGameRunning](int axis, int value) {
            if (isGameRunning)
              return;
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
