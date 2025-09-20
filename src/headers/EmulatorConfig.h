#ifndef EMULATORCONFIG_H
#define EMULATORCONFIG_H

#include <QString>
#include <QMap>
#include <QStringList>

class EmulatorConfig {
public:
    struct Config {
        QString emulatorName;
        QString romsPath;
        QStringList extensions;
    };

    static QMap<QString, Config> getEmulatorConfigs();
};

#endif // EMULATORCONFIG_H
