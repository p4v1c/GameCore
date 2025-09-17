#include "../headers/Emulator.h"
#include <QDebug>
#include <QDir>

void EmulatorManager::scanEmulators(const QString &directory) {
  emulators.clear();
  QDir dir(directory);
  if (!dir.exists()) {
    qDebug() << "Dossier émulateurs inexistant:" << directory;
    return;
  }

  // On parcourt les dossiers
  QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &entry : entries) {
    Emulator emu;
    emu.name = entry;
    emu.path = dir.filePath(entry);

    QString lower = entry.toLower();

    if (lower.contains("yuzu")) {
      emu.platform = "Switch";
      if (!emu.icon.load("../assets/logos/switch.png")) {
        qDebug() << "Impossible de charger yuzu.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("dolphin")) {
      emu.platform = "GameCube";
      if (!emu.icon.load("../assets/logos/GameCube.png")) {
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("snes")) {
      emu.platform = "SNES";
      if (!emu.icon.load("../assets/logos/snes.png")) {
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else {
      emu.platform = "Unknown";
      emu.icon = QPixmap(128, 128);
      emu.icon.fill(Qt::gray);
    }

    emulators.push_back(emu);
  }
}

const std::vector<Emulator> &EmulatorManager::getEmulators() const {
  return emulators;
}
