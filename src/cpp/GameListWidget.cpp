#include "../headers/GameListWidget.h"
#include "../headers/Constants.h"
#include "../headers/EmulatorConfig.h"
#include <QDebug>
#include <QDir>
#include <QKeyEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <SDL.h>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

GameListWidget::GameListWidget(QWidget *parent)
    : QWidget(parent), lastAxisValue(0) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(50, 50, 50, 50);
  layout->setSpacing(20);

  // Initialisation de la QListWidget
  gameList = new QListWidget(this);
  gameList->setStyleSheet(
      "QListWidget {"
      "    background-color: #3C3C3C;"
      "    color: #E0E0E0;"
      "    font-size: 24px;"
      "    border: 2px solid #555555;"
      "    border-radius: 15px;"
      "}"
      "QListWidget::item {"
      "    background: transparent;"
      "    padding: 10px;"
      "}"
      "QListWidget::item:selected {"
      "    background-color: #007AFF;"
      "    color: white;"
      "    border-radius: 10px;"
      "}");
  gameList->setFocusPolicy(Qt::NoFocus);
  gameList->setFixedWidth(800);
  gameList->setMinimumHeight(400);

  layout->addWidget(gameList, 0, Qt::AlignCenter);
  setLayout(layout);
  setFocusPolicy(Qt::StrongFocus);
}

void GameListWidget::loadGamesFor(const QString &emulatorName) {
  this->selectedEmulatorName = emulatorName;
  gameList->clear();

  QMap<QString, EmulatorConfig::Config> configs =
      EmulatorConfig::getEmulatorConfigs();
  if (!configs.contains(emulatorName)) {
    qWarning() << "[GameListWidget] Configuration non trouvée pour l'émulateur:"
               << emulatorName;
    QListWidgetItem *item = new QListWidgetItem();
    item->setText("Configuration non trouvée.");
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::NoItemFlags);
    gameList->addItem(item);
    return;
  }

  const EmulatorConfig::Config &config = configs.value(emulatorName);
  qDebug() << "[GameListWidget] Chargement des jeux pour l'émulateur:"
           << emulatorName;
  qDebug() << "[GameListWidget] Chemin des ROMs:" << config.romsPath;
  qDebug() << "[GameListWidget] Extensions supportées:" << config.extensions;

  scanRoms(config.romsPath, config.extensions);
}

void GameListWidget::scanRoms(const QString &path,
                              const QStringList &extensions) {
  QDir dir(path);
  qDebug() << "[GameListWidget] Scan du dossier:" << path;

  if (!dir.exists()) {
    qWarning()
        << "[GameListWidget] Le dossier n'existe pas ou n'est pas accessible:"
        << path;
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(
        "Aucun jeu trouvé. Le dossier n'existe pas ou n'est pas accessible.");
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::NoItemFlags);
    gameList->addItem(item);
    return;
  }

  QDir::Filters filters = QDir::Files | QDir::Readable | QDir::NoDotAndDotDot;
  QStringList files = dir.entryList(extensions, filters, QDir::Name);

  qDebug() << "[GameListWidget] Fichiers trouvés:" << files.size();
  for (const QString &file : files) {
    qDebug() << "  ->" << file;
  }

  if (files.isEmpty()) {
    QListWidgetItem *item = new QListWidgetItem();
    item->setText("Aucun jeu trouvé dans " + dir.path());
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::NoItemFlags);
    gameList->addItem(item);
  } else {
    gameList->clear();
    for (const QString &file : files) {
      gameList->addItem(file);
    }
    if (gameList->count() > 0) {
      gameList->setCurrentRow(0);
    }
  }
}

void GameListWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) {
    emit goBackToCarousel();
  } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    launchSelectedGame();
  }
}

void GameListWidget::handleControllerButton(int button) {
  if (gameList->count() == 0)
    return;

  if (button == SDL_CONTROLLER_BUTTON_B) {
    emit goBackToCarousel();
  } else if (button == SDL_CONTROLLER_BUTTON_A) {
    launchSelectedGame();
  } else if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
    gameList->setCurrentRow((gameList->currentRow() + 1) % gameList->count());
  } else if (button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
    gameList->setCurrentRow((gameList->currentRow() - 1 + gameList->count()) %
                            gameList->count());
  }
}

void GameListWidget::handleControllerAxis(int axis, int value) {
  if (gameList->count() == 0 ||
      (axis != SDL_CONTROLLER_AXIS_LEFTY && axis != SDL_CONTROLLER_AXIS_RIGHTY))
    return;

  const int THRESHOLD = 16000;

  if (value > THRESHOLD && lastAxisValue <= THRESHOLD) {
    gameList->setCurrentRow((gameList->currentRow() + 1) % gameList->count());
  } else if (value < -THRESHOLD && lastAxisValue >= -THRESHOLD) {
    gameList->setCurrentRow((gameList->currentRow() - 1 + gameList->count()) %
                            gameList->count());
  }

  lastAxisValue = value;
}

void GameListWidget::launchSelectedGame() {
    if (gameList->currentRow() < 0) return;
    QString selectedGameName = gameList->currentItem()->text();

    QFile file("../config/settings.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Impossible d'ouvrir le fichier de configuration JSON: ../config/settings.json";
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject emulatorsConfig = doc.object();
    file.close();

    if (!emulatorsConfig.contains(selectedEmulatorName)) {
        qWarning() << "Configuration non trouvée pour l'émulateur:" << selectedEmulatorName;
        return;
    }

    QJsonObject emuConfig = emulatorsConfig.value(selectedEmulatorName).toObject();
    QString emuPath = emuConfig.value("path").toString();
    QString emuArgs = emuConfig.value("args").toString();

    QMap<QString, EmulatorConfig::Config> configs = EmulatorConfig::getEmulatorConfigs();
    QString romsPath = configs.value(selectedEmulatorName).romsPath;
    QString fullGamePath = QDir(romsPath).filePath(selectedGameName);

    // FIX : On passe les trois arguments au signal
    emit launchGame(emuPath, emuArgs, fullGamePath);
}
