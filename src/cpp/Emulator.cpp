#include "../headers/Emulator.h"
#include <QDebug>
#include <QDir>
#include <QPixmap>

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

    if (lower.contains("citron")) {
      emu.platform = "Switch";
      if (!emu.icon.load("../assets/logos/switch.png")) {
        qDebug() << "Impossible de charger switch.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("dolphin")) {
      emu.platform = "GameCube/Wii";
      if (!emu.icon.load("../assets/logos/gamecube.png")) {
        qDebug() << "Impossible de charger gamecube.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("pcsx2")) {
      emu.platform = "PS2";
      if (!emu.icon.load("../assets/logos/ps2.png")) {
        qDebug() << "Impossible de charger ps2.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("rpcs3")) {
      emu.platform = "PS3";
      if (!emu.icon.load("../assets/logos/ps3.png")) {
        qDebug() << "Impossible de charger ps3.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("duckstation")) {
      emu.platform = "PS1";
      if (!emu.icon.load("../assets/logos/ps1.png")) {
        qDebug() << "Impossible de charger ps1.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("mgba")) {
      emu.platform = "Game Boy";
      if (!emu.icon.load("../assets/logos/gba.png")) {
        qDebug() << "Impossible de charger gba.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("ppsspp")) {
      emu.platform = "PSP";
      if (!emu.icon.load("../assets/logos/psp.png")) {
        qDebug() << "Impossible de charger psp.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("azahar")) {
      emu.platform = "3DS";
      if (!emu.icon.load("../assets/logos/3ds.png")) {
        qDebug() << "Impossible de charger 3ds.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("melonds")) {
      emu.platform = "DS";
      if (!emu.icon.load("../assets/logos/ds.png")) {
        qDebug() << "Impossible de charger ds.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    } else if (lower.contains("gopher64")) {
      emu.platform = "N64";
      if (!emu.icon.load("../assets/logos/n64.png")) {
        qDebug() << "Impossible de charger n64.png";
        emu.icon = QPixmap(128, 128);
        emu.icon.fill(Qt::gray);
      }
    }
    if (!emu.platform.isEmpty() && !emu.icon.isNull()) {
      emulators.push_back(emu);
    }
  }
}

const std::vector<Emulator> &EmulatorManager::getEmulators() const {
  return emulators;
}
