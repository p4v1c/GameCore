#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QStatusBar>
#include <QStackedWidget> // Ajout de l'inclusion pour QStackedWidget

#include "../headers/Carousel.h"
#include "../headers/GameListWidget.h" // Ajout de l'inclusion pour GameListWidget
#include "../headers/EmulatorConfig.h" // Ajout de l'inclusion pour EmulatorConfig
#include "../headers/Constants.h"
#include "../headers/Emulator.h"
#include "../headers/EmulatorWidget.h"
#include "../headers/ControllerManager.h"

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
    statusBar->setStyleSheet("color: white; font-size: 16px; background: transparent; padding: 5px;");
    statusBar->setSizeGripEnabled(false);
    mainLayout->addWidget(statusBar);
    statusBar->showMessage("En attente de manette...", 0);

    // ----- INITIALISATION DU MANAGER DE MANETTE -----
    ControllerManager controllerManager;
    controllerManager.initialize();

    QObject::connect(&controllerManager, &ControllerManager::controllerConnected,
                     statusBar, [statusBar](const QString &name) {
        statusBar->showMessage("Manette connectée : " + name, 5000);
    });
    QObject::connect(&controllerManager, &ControllerManager::controllerDisconnected,
                     statusBar, [statusBar]() {
        statusBar->showMessage("Manette déconnectée.", 5000);
    });

    QTimer *eventTimer = new QTimer(&app);
    QObject::connect(eventTimer, &QTimer::timeout, &controllerManager,
                     &ControllerManager::processEvents);
    eventTimer->start(16); // Environ 60 FPS

    // ----- ÉCRAN DE SPLASH ----
    QLabel *splashLabel = new QLabel(&window);
    splashLabel->setAlignment(Qt::AlignCenter);
    splashLabel->setPixmap(QPixmap("../assets/images/splash.png")
                               .scaled(window.size() * 0.8, Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));
    mainLayout->addWidget(splashLabel, 0, Qt::AlignCenter);

    // Effet de fade-out pour le splash screen
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(splashLabel);
    splashLabel->setGraphicsEffect(opacityEffect);
    QPropertyAnimation *fadeOut = new QPropertyAnimation(opacityEffect, "opacity");
    fadeOut->setDuration(SPLASH_FADE_OUT_DURATION);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->start();

    // ----- APRÈS LE SPLASH : AFFICHAGE DES WIDGETS PRINCIPAUX -----
    QTimer::singleShot(SPLASH_FADE_OUT_DURATION, [&mainLayout, &window, &controllerManager, statusBar]() {
        QLayoutItem *item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if (item->widget() != statusBar) {
                delete item->widget();
            }
            delete item;
        }

        // Crée le QStackedWidget
        QStackedWidget *stackedWidget = new QStackedWidget(&window);
        stackedWidget->setStyleSheet("background-color: #2E2E2E;");

        // Crée les deux widgets
        Carousel *carousel = new Carousel(&window);
        GameListWidget *gameListWidget = new GameListWidget(&window);

        // Ajoute les widgets au QStackedWidget
        stackedWidget->addWidget(carousel); // Index 0
        stackedWidget->addWidget(gameListWidget); // Index 1

        // Connexion des signaux
        QObject::connect(carousel, &Carousel::switchToGameList,
                         gameListWidget, &GameListWidget::loadGamesFor);

        QObject::connect(carousel, &Carousel::switchToGameList,
                         stackedWidget, [stackedWidget, gameListWidget]() {
            stackedWidget->setCurrentWidget(gameListWidget);
            gameListWidget->setFocus();
        });

        QObject::connect(gameListWidget, &GameListWidget::goBackToCarousel,
                         stackedWidget, [stackedWidget, carousel]() {
            stackedWidget->setCurrentWidget(carousel);
            carousel->setFocus();
        });

        // Connexion des signaux de la manette
        QObject::connect(&controllerManager, &ControllerManager::buttonPressed,
                         carousel, &Carousel::handleControllerButton);
        QObject::connect(&controllerManager, &ControllerManager::axisMoved,
                         carousel, &Carousel::handleControllerAxis);

        QObject::connect(&controllerManager, &ControllerManager::buttonPressed,
                         gameListWidget, &GameListWidget::handleControllerButton);
        QObject::connect(&controllerManager, &ControllerManager::axisMoved,
                         gameListWidget, &GameListWidget::handleControllerAxis);

        mainLayout->addWidget(stackedWidget);
        mainLayout->addWidget(statusBar);

        carousel->scanEmulators("../emu");
        carousel->setFocus();
    });

    return app.exec();
}
