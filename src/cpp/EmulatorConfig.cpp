#include "../headers/EmulatorConfig.h"
#include "../headers/Constants.h"

QMap<QString, EmulatorConfig::Config> EmulatorConfig::getEmulatorConfigs() {
  QMap<QString, Config> configs;

  // Configuration pour la N64 (Mupen64Plus)
  Config gopher64Config;
  gopher64Config.emulatorName = "gopher64";
  gopher64Config.romsPath = ROMS_DIR + "gopher64/";
  gopher64Config.extensions << "*.n64" << "*.z64" << "*.v64" << "*.zip";
  configs.insert("gopher64", gopher64Config);

  // Configuration pour le GameCube/Wii (Dolphin)
  Config dolphinConfig;
  dolphinConfig.emulatorName = "dolphin";
  dolphinConfig.romsPath = ROMS_DIR + "dolphin/";
  dolphinConfig.extensions << "*.iso" << "*.gcm" << "*.wii" << "*.zip"
                           << "*.rvz";
  configs.insert("dolphin", dolphinConfig);

  // Configuration pour la DS (MelonDS)
  Config melondsConfig;
  melondsConfig.emulatorName = "melonds";
  melondsConfig.romsPath = ROMS_DIR + "melonds/";
  melondsConfig.extensions << "*.nds" << "*.zip";
  configs.insert("melonds", melondsConfig);

  // Configuration pour la 3DS (azahar)
  Config azaharConfig;
  azaharConfig.emulatorName = "azahar";
  azaharConfig.romsPath = ROMS_DIR + "azahar/";
  azaharConfig.extensions << "*.3ds" << "*.3dsx" << "*.cci" << "*.zip";
  configs.insert("azahar", azaharConfig);

  // Configuration pour la GBA (mGBA)
  Config mgbaConfig;
  mgbaConfig.emulatorName = "mgba";
  mgbaConfig.romsPath = ROMS_DIR + "mgba/";
  mgbaConfig.extensions << "*.gba" << "*.zip" << "*.gbc" << "*.gb";
  configs.insert("mgba", mgbaConfig);

  // Configuration pour la PS1 (DuckStation)
  Config duckstationConfig;
  duckstationConfig.emulatorName = "duckstation";
  duckstationConfig.romsPath = ROMS_DIR + "duckstation/";
  duckstationConfig.extensions << "*.bin" << "*.cue" << "*.iso" << "*.img"
                               << "*.zip";
  configs.insert("duckstation", duckstationConfig);

  // Configuration pour la PS2 (PCSX2)
  Config pcsx2Config;
  pcsx2Config.emulatorName = "pcsx2";
  pcsx2Config.romsPath = ROMS_DIR + "pcsx2/";
  pcsx2Config.extensions << "*.iso" << "*.bin" << "*.zip";
  configs.insert("pcsx2", pcsx2Config);

  // Configuration pour la PS3 (RPCS3)
  Config rpcs3Config;
  rpcs3Config.emulatorName = "rpcs3";
  rpcs3Config.romsPath = ROMS_DIR + "rpcs3/";
  rpcs3Config.extensions << "*.pkg" << "*.iso" << "*.zip";
  configs.insert("rpcs3", rpcs3Config);

  // Configuration pour la PSP (PPSSPP)
  Config ppssppConfig;
  ppssppConfig.emulatorName = "ppsspp";
  ppssppConfig.romsPath = ROMS_DIR + "ppsspp/";
  ppssppConfig.extensions << "*.iso" << "*.cso" << "*.zip";
  configs.insert("ppsspp", ppssppConfig);

  // Configuration pour la Switch (citon)
  Config citronConfig;
  citronConfig.emulatorName = "citron";
  citronConfig.romsPath = ROMS_DIR + "citron/";
  citronConfig.extensions << "*.nsp" << "*.xci" << "*.zip";
  configs.insert("citron", citronConfig);

  return configs;
}
