#ifndef EMULATOR_H
#define EMULATOR_H

#include <QPixmap>
#include <QString>
#include <vector>

struct Emulator {
  QString name;
  QString path;
  QString platform; // Switch, SNES, etc.
  QPixmap icon;     // Icône

  // Nouveau constructeur pour initialiser les membres
  Emulator(const QString &name, const QString &path, const QString &platform,
           const QPixmap &icon)
      : name(name), path(path), platform(platform), icon(icon) {}

  // Ajout d'un constructeur par défaut
  Emulator() = default;
};

class EmulatorManager {
public:
  void scanEmulators(const QString &directory);
  const std::vector<Emulator> &getEmulators() const;

private:
  std::vector<Emulator> emulators;
};

#endif // EMULATOR_H
