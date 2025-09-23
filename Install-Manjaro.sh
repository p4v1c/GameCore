#!/usr/bin/env bash
set -euo pipefail

# === Fonctions utilitaires ===
msg() { echo -e "\n\033[1;32m==> $*\033[0m"; }
err() { echo -e "\n\033[1;31m[ERREUR]\033[0m $*" >&2; exit 1; }

# === PROMPTS ===
read -rp "Nom de l'utilisateur système (ex: p4v1c) : " USER_NAME
read -rp "Chemin d'installation de GameCore (ex: /opt/GameCore) : " GAMECORE_PATH
read -rsp "Mot de passe Samba pour l'utilisateur $USER_NAME : " SMB_PASS
echo

# === Variables ===
GAMECORE_PARENT_DIR=$(dirname "$GAMECORE_PATH")
AUTOSTART_DIR="/home/$USER_NAME/.config/autostart"

msg "Résumé des paramètres"
echo "Utilisateur : $USER_NAME"
echo "GameCore path : $GAMECORE_PATH"
sleep 1

# === Vérification utilisateur ===
if ! id "$USER_NAME" >/dev/null 2>&1; then
  read -rp "L'utilisateur '$USER_NAME' n'existe pas. Créer maintenant ? (y/N) " CREATE_USER
  if [[ "$CREATE_USER" =~ ^([yY][eE][sS]|[yY])$ ]]; then
    sudo useradd -m -s /bin/bash "$USER_NAME"
    msg "Utilisateur $USER_NAME créé."
  else
    err "Impossible de continuer sans utilisateur."
  fi
fi

# === Répertoire GameCore ===
msg "Préparation du répertoire $GAMECORE_PARENT_DIR"
sudo mkdir -p "$GAMECORE_PARENT_DIR"
sudo chmod 755 "$GAMECORE_PARENT_DIR"

if [ -d "../GameCore" ]; then
  msg "Copie de ../GameCore vers $GAMECORE_PATH"
  sudo cp -r ../GameCore/. "$GAMECORE_PATH"
else
  msg "⚠️ ../GameCore introuvable. Création dossier vide."
  sudo mkdir -p "$GAMECORE_PATH"
fi
sudo chown -R "${USER_NAME}:${USER_NAME}" "$GAMECORE_PATH"

# === Nettoyage fichiers 'example' ===
msg "Suppression des fichiers contenant 'example'"
sudo find "$GAMECORE_PATH" -iname "*example*" -exec rm -rf {} +

# === Paquets système ===
msg "Mise à jour système et installation drivers/headers"
sudo pacman -Syu --noconfirm

# === Wallpaper ===
WALLPAPER_PATH="$GAMECORE_PATH/background.png"
if [ -f "$WALLPAPER_PATH" ]; then
  msg "Configuration du fond d'écran pour $USER_NAME"
  sudo -u "$USER_NAME" DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/$(id -u $USER_NAME)/bus \
    gsettings set org.gnome.desktop.background picture-uri "file://$WALLPAPER_PATH"
  sudo -u "$USER_NAME" DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/$(id -u $USER_NAME)/bus \
    gsettings set org.gnome.desktop.background picture-options "scaled"
else
  echo "⚠️ Le fichier $WALLPAPER_PATH n'existe pas. Pas de fond d'écran configuré."
fi

# Détecter headers noyau
KERNEL=$(uname -r)
if pacman -Ss "^linux-headers$" >/dev/null 2>&1; then
  HEADERS="linux-headers"
elif [[ $KERNEL == *zen* ]]; then
  HEADERS="linux-zen-headers"
else
  HEADERS="linux-headers"
fi

sudo pacman -S --noconfirm amd-ucode mesa xf86-video-amdgpu vulkan-radeon lib32-vulkan-radeon \
  "$HEADERS" base-devel git

# === Dépendances générales ===
msg "Installation dépendances"
sudo pacman -S --noconfirm qt5-base qt5-tools sdl2 make flatpak samba openssh feh \
  cpupower thermald polybar lm_sensors antimicrox radeontop

# === Services CPU & thermald ===
msg "Activation services CPU et thermald"
sudo systemctl enable --now cpupower.service
sudo cpupower frequency-set -g performance || true
sudo systemctl enable --now thermald.service || true

# === Extensions GNOME ===
msg "Nettoyage extensions GNOME"
gnome-extensions list | grep -q dash-to-dock@micxgx.gmail.com && \
  gnome-extensions disable dash-to-dock@micxgx.gmail.com || true

# === RetroArch ===
msg "Installation RetroArch via Flatpak"
if ! flatpak remote-list | grep -q flathub; then
  sudo flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
fi
flatpak install -y flathub org.libretro.RetroArch || true

# === Compilation GameCore ===
msg "Compilation GameCore"
sudo -u "$USER_NAME" mkdir -p "$GAMECORE_PATH/build"
cd "$GAMECORE_PATH/build"

if command -v qmake6 >/dev/null; then
  QMAKE_BIN=qmake6
else
  QMAKE_BIN=qmake
fi
sudo -u "$USER_NAME" $QMAKE_BIN compile.pro
sudo -u "$USER_NAME" make -j"$(nproc)"

# === Autostart GameCore (après compilation) ===
msg "Configuration autostart"
sudo -u "$USER_NAME" mkdir -p "$AUTOSTART_DIR"
DESKTOP_FILE="$AUTOSTART_DIR/GameCore.desktop"
sudo -u "$USER_NAME" tee "$DESKTOP_FILE" >/dev/null <<EOF
[Desktop Entry]
Type=Application
Name=GameCore
Exec=$GAMECORE_PATH/build/GameCore
WorkingDirectory=$GAMECORE_PATH/build
Terminal=false
StartupNotify=false
EOF

# === Samba ===
msg "Configuration Samba"
sudo cp /etc/samba/smb.conf /etc/samba/smb.conf.bak.$(date +%s)
sudo tee -a /etc/samba/smb.conf >/dev/null <<EOF

[ROMS]
   comment = Dossier ROMS
   path = $GAMECORE_PATH/emu/
   browseable = yes
   read only = no
   writable = yes
   create mask = 0755
   valid users = $USER_NAME
EOF

sudo mkdir -p "$GAMECORE_PATH/emu/"
sudo chown -R "$USER_NAME:$USER_NAME" "$GAMECORE_PATH/emu/"
printf "%s\n%s\n" "$SMB_PASS" "$SMB_PASS" | sudo smbpasswd -s -a "$USER_NAME"
sudo systemctl restart smb.service nmb.service || true

# === SSH ===
msg "Activation SSH"
sudo systemctl enable --now sshd

# === Fin ===
msg "Installation terminée"
echo "GameCore compilé et autostart configuré."
echo "Samba configuré (utilisateur : $USER_NAME)."
echo "SSH activé."
echo "RetroArch installé via Flatpak."
echo "Drivers AMD + Vulkan installés."
echo "Pour activer HideTopBar après redémarrage :"
echo "  gnome-extensions enable hidetopbar@mathieu.bidon.ca"

read -rp "Redémarrer maintenant ? (y/N) " REBOOT
if [[ "$REBOOT" =~ ^([yY][eE][sS]|[yY])$ ]]; then
  sudo systemctl reboot
else
  echo "➡️ Tu peux redémarrer plus tard avec : sudo systemctl reboot"
fi
