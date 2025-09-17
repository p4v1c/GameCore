#ifndef EMULATOR_H
#define EMULATOR_H

#include <QPixmap>
#include <QString>
#include <vector>

struct Emulator {
  QString name;     // Nom du dossier/émulateur
  QString path;     // Chemin complet
  QString platform; // Switch, SNES, etc.
  QPixmap icon;     // Icône
};

class EmulatorManager {
public:
  void scanEmulators(const QString &directory);
  const std::vector<Emulator> &getEmulators() const;

private:
  std::vector<Emulator> emulators;
};

#endif // EMULATOR_H
