#include "../headers/EmulatorConfig.h"
#include "../headers/Constants.h"

QMap<QString, EmulatorConfig::Config> EmulatorConfig::getEmulatorConfigs() {
    QMap<QString, Config> configs;

    // Configuration pour le NES (FCEUX)
    Config nesConfig;
    nesConfig.emulatorName = "nes";
    nesConfig.romsPath = ROMS_DIR + "nes/";
    nesConfig.extensions << "*.nes" << "*.zip";
    configs.insert("fceux", nesConfig); // Utiliser le nom de l'exécutable/dossier comme clé

    // Configuration pour la SNES (BSNES)
    Config snesConfig;
    snesConfig.emulatorName = "snes";
    snesConfig.romsPath = ROMS_DIR + "snes/";
    snesConfig.extensions << "*.sfc" << "*.smc" << "*.zip";
    configs.insert("bsnes", snesConfig);

    // Configuration pour la GBA (mGBA)
    Config gbaConfig;
    gbaConfig.emulatorName = "gba";
    gbaConfig.romsPath = ROMS_DIR + "gba/";
    gbaConfig.extensions << "*.gba" << "*.zip";
    configs.insert("mgba", gbaConfig);

    // Ajoutez d'autres configurations ici de la même manière
    // Pour chaque nouvel émulateur, ajoutez un nouveau bloc Config.

    return configs;
}
