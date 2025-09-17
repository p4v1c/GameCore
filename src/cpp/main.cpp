#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout> // Peut être nécessaire si Carousel utilise QHBoxLayout directement
#include <QIcon>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "../headers/Carousel.h"
#include "../headers/Constants.h"
#include "../headers/Emulator.h"
#include "../headers/EmulatorWidget.h"

// Note : Si vous utilisez toujours le ControllerManager et la QStatusBar,
// vous devrez les réintégrer comme dans les versions précédentes.
// Ce code est une version simplifiée sans ces éléments.

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // ----- FENETRE PRINCIPALE -----
  QWidget window;
  window.setWindowTitle("RetroLauncher");
  window.showFullScreen();
  window.setStyleSheet("background-color: #2E2E2E;");

  QVBoxLayout *mainLayout = new QVBoxLayout(&window);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

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

    // ----- APRES LE SPLASH : AFFICHAGE DU CAROUSEL -----
    QTimer::singleShot(SPLASH_FADE_OUT_DURATION, [&mainLayout, &window]() {
      // Nettoyer l'ancien contenu du layout (le splash screen)
      QLayoutItem *item;
      while ((item = mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) { // S'assurer que l'item contient un widget avant
                              // de le supprimer
          delete item->widget();
        }
        delete item;
      }

      Carousel *carousel = new Carousel(&window);
      carousel->setFocusPolicy(
          Qt::StrongFocus); // Pour recevoir les événements clavier
      carousel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      mainLayout->addStretch();
      mainLayout->addWidget(carousel, 0, Qt::AlignCenter);
      mainLayout->addStretch();

      carousel->scanEmulators("../emu"); // Charger les émulateurs
      carousel->setFocus(); // Donner le focus au carousel pour la navigation
                            // clavier
    });
  });

  return app.exec();
}
