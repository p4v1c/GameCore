#include "../headers/GameListWidget.h"
#include <QVBoxLayout>
#include <QDir>
#include <QDebug>
#include <QLabel>
#include "../headers/EmulatorConfig.h"
#include "../headers/Constants.h"
#include <QKeyEvent>
#include <SDL.h>

GameListWidget::GameListWidget(QWidget *parent) : QWidget(parent), lastAxisValue(0) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(50, 50, 50, 50);
    layout->setSpacing(20);

    // Initialisation de la QListWidget
    gameList = new QListWidget(this);
    gameList->setStyleSheet(
        "QListWidget { background-color: #3C3C3C; color: #E0E0E0; font-size: 24px; border: none; }"
        "QListWidget::item:selected { background-color: #007AFF; color: white; }"
    );
    gameList->setFocusPolicy(Qt::NoFocus); // On gère le focus manuellement

    layout->addWidget(gameList, 0, Qt::AlignCenter);
    setLayout(layout);
    setFocusPolicy(Qt::StrongFocus);
}

void GameListWidget::loadGamesFor(const QString &emulatorName) {
    this->selectedEmulatorName = emulatorName;
    gameList->clear(); // Vider la liste avant de charger de nouveaux jeux

    // Récupérer la configuration de l'émulateur
    QMap<QString, EmulatorConfig::Config> configs = EmulatorConfig::getEmulatorConfigs();
    if (!configs.contains(emulatorName)) {
        qWarning() << "Configuration non trouvée pour l'émulateur:" << emulatorName;
        QListWidgetItem *item = new QListWidgetItem();
        item->setText("Configuration non trouvée.");
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        gameList->addItem(item);
        return;
    }

    const EmulatorConfig::Config &config = configs.value(emulatorName);
    scanRoms(config.romsPath, config.extensions);
}

void GameListWidget::scanRoms(const QString &path, const QStringList &extensions) {
    QDir dir(path);
    if (!dir.exists()) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText("Aucun jeu trouvé. Le dossier n'existe pas ou n'est pas accessible.");
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        gameList->addItem(item);
        return;
    }

    QDir::Filters filters = QDir::Files | QDir::Readable | QDir::NoDotAndDotDot;
    QStringList files = dir.entryList(extensions, filters, QDir::Name);

    if (files.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText("Aucun jeu trouvé dans " + dir.path());
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags); // Rendre l'item non sélectionnable
        gameList->addItem(item);
    } else {
        gameList->clear();
        for (const QString &file : files) {
            gameList->addItem(file);
        }
        if (gameList->count() > 0) {
            gameList->setCurrentRow(0); // Sélectionner le premier item par défaut
        }
    }
}

void GameListWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        emit goBackToCarousel();
    }
}

void GameListWidget::handleControllerButton(int button) {
    if (gameList->count() == 0) return;

    if (button == SDL_CONTROLLER_BUTTON_B) { // Bouton Rond (O) de la PS4
        emit goBackToCarousel();
    } else if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
        gameList->setCurrentRow((gameList->currentRow() + 1) % gameList->count());
    } else if (button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
        gameList->setCurrentRow((gameList->currentRow() - 1 + gameList->count()) % gameList->count());
    }
}

void GameListWidget::handleControllerAxis(int axis, int value) {
    if (gameList->count() == 0 || (axis != SDL_CONTROLLER_AXIS_LEFTY && axis != SDL_CONTROLLER_AXIS_RIGHTY))
        return;

    const int THRESHOLD = 16000;

    // Mouvement vers le bas (stick Y)
    if (value > THRESHOLD && lastAxisValue <= THRESHOLD) {
        gameList->setCurrentRow((gameList->currentRow() + 1) % gameList->count());
    }
    // Mouvement vers le haut (stick Y)
    else if (value < -THRESHOLD && lastAxisValue >= -THRESHOLD) {
        gameList->setCurrentRow((gameList->currentRow() - 1 + gameList->count()) % gameList->count());
    }

    lastAxisValue = value;
}
