#include "../headers/EmulatorConfig.h"
#include "../headers/Constants.h"

QMap<QString, EmulatorConfig::Config> EmulatorConfig::getEmulatorConfigs() {
  QMap<QString, Config> configs;

  // Configuration pour le NES (FCEUX)
  Config fceuxConfig;
  fceuxConfig.emulatorName = "fceux";
  fceuxConfig.romsPath = ROMS_DIR + "fceux/roms/";
  fceuxConfig.extensions << "*.nes" << "*.zip";
  configs.insert("fceux", fceuxConfig);

  // Configuration pour la SNES (BSNES)
  Config bsnesConfig;
  bsnesConfig.emulatorName = "bsnes";
  bsnesConfig.romsPath = ROMS_DIR + "bsnes/roms/";
  bsnesConfig.extensions << "*.sfc" << "*.smc" << "*.zip";
  configs.insert("bsnes", bsnesConfig);

  // Configuration pour la N64 (Mupen64Plus)
  Config mupenConfig;
  mupenConfig.emulatorName = "mupen64plus";
  mupenConfig.romsPath = ROMS_DIR + "mupen64plus/roms/";
  mupenConfig.extensions << "*.n64" << "*.z64" << "*.v64" << "*.zip";
  configs.insert("mupen64plus", mupenConfig);

  // Configuration pour le GameCube/Wii (Dolphin)
  Config dolphinConfig;
  dolphinConfig.emulatorName = "dolphin";
  dolphinConfig.romsPath = ROMS_DIR + "dolphin/roms/";
  dolphinConfig.extensions << "*.iso" << "*.gcm" << "*.wii" << "*.zip";
  configs.insert("dolphin", dolphinConfig);

  // Configuration pour la DS (MelonDS)
  Config melondsConfig;
  melondsConfig.emulatorName = "melonds";
  melondsConfig.romsPath = ROMS_DIR + "melonds/roms/";
  melondsConfig.extensions << "*.nds" << "*.zip";
  configs.insert("melonds", melondsConfig);

  // Configuration pour la 3DS (azahar)
  Config azaharConfig;
  azaharConfig.emulatorName = "azahar";
  azaharConfig.romsPath = ROMS_DIR + "azahar/roms/";
  azaharConfig.extensions << "*.3ds" << "*.3dsx" << "*.cci" << "*.zip";
  configs.insert("azahar", azaharConfig);

  // Configuration pour la GBA (mGBA)
  Config mgbaConfig;
  mgbaConfig.emulatorName = "mgba";
  mgbaConfig.romsPath = ROMS_DIR + "mgba/roms/";
  mgbaConfig.extensions << "*.gba" << "*.zip";
  configs.insert("mgba", mgbaConfig);

  // Configuration pour la PS1 (DuckStation)
  Config duckstationConfig;
  duckstationConfig.emulatorName = "duckstation";
  duckstationConfig.romsPath = ROMS_DIR + "duckstation/roms/";
  duckstationConfig.extensions << "*.bin" << "*.cue" << "*.iso" << "*.img"
                               << "*.zip";
  configs.insert("duckstation", duckstationConfig);

  // Configuration pour la PS2 (PCSX2)
  Config pcsx2Config;
  pcsx2Config.emulatorName = "pcsx2";
  pcsx2Config.romsPath = ROMS_DIR + "pcsx2/roms/";
  pcsx2Config.extensions << "*.iso" << "*.bin" << "*.zip";
  configs.insert("pcsx2", pcsx2Config);

  // Configuration pour la PS3 (RPCS3)
  Config rpcs3Config;
  rpcs3Config.emulatorName = "rpcs3";
  rpcs3Config.romsPath = ROMS_DIR + "rpcs3/roms/";
  rpcs3Config.extensions << "*.pkg" << "*.iso" << "*.zip";
  configs.insert("rpcs3", rpcs3Config);

  // Configuration pour la PSP (PPSSPP)
  Config ppssppConfig;
  ppssppConfig.emulatorName = "ppsspp";
  ppssppConfig.romsPath = ROMS_DIR + "ppsspp/roms/";
  ppssppConfig.extensions << "*.iso" << "*.cso" << "*.zip";
  configs.insert("ppsspp", ppssppConfig);

  // Configuration pour la Switch (Yuzu)
  Config yuzuConfig;
  yuzuConfig.emulatorName = "yuzu";
  yuzuConfig.romsPath = ROMS_DIR + "yuzu/roms/";
  yuzuConfig.extensions << "*.nsp" << "*.xci" << "*.zip";
  configs.insert("yuzu", yuzuConfig);

  return configs;
}
