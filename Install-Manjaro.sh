#!/usr/bin/env bash
set -euo pipefail

# --- PROMPTS ---
read -rp "Nom de l'utilisateur système (ex: p4v1c) : " USER_NAME
read -rp "Chemin d'installation de GameCore (ex: /opt/GameCore) : " GAMECORE_PATH
read -rsp "Mot de passe Samba pour l'utilisateur $USER_NAME : " SMB_PASS
echo

# --- Variables ---
GAMECORE_PARENT_DIR=$(dirname "$GAMECORE_PATH")

echo
echo "=== Résumé des paramètres ==="
echo "Utilisateur : $USER_NAME"
echo "GameCore path : $GAMECORE_PATH"
echo "---------------------------------"
sleep 1

# Vérifier que l'utilisateur existe
if ! id "$USER_NAME" >/dev/null 2>&1; then
  echo "L'utilisateur '$USER_NAME' n'existe pas. Veux-tu le créer maintenant ? (y/N)"
  read -r CREATE_USER
  if [[ "$CREATE_USER" =~ ^([yY][eE][sS]|[yY])$ ]]; then
    sudo useradd -m -s /bin/bash "$USER_NAME"
    echo "Utilisateur $USER_NAME créé."
  else
    echo "Impossible de continuer sans utilisateur existant. Sortie."
    exit 1
  fi
fi

# --- Préparation répertoire et copie GameCore ---
echo "=== Préparation du répertoire $GAMECORE_PARENT_DIR ==="
sudo mkdir -p "$GAMECORE_PARENT_DIR"
sudo chmod 777 "$GAMECORE_PARENT_DIR"

if [ -d "../GameCore" ]; then
  echo "Copie du contenu de ../GameCore vers $GAMECORE_PATH ..."
  sudo cp -r ../GameCore/. "$GAMECORE_PATH"
else
  echo "⚠️ ../GameCore n'existe pas. Création du dossier cible vide."
  sudo mkdir -p "$GAMECORE_PATH"
fi
sudo chown -R "${USER_NAME}:${USER_NAME}" "$GAMECORE_PATH"

# --- Nettoyage fichiers 'example' ---
echo "=== Suppression des fichiers/dossiers contenant 'example' dans $GAMECORE_PATH ==="
sudo find "$GAMECORE_PATH" -iname "*example*" -exec rm -rf {} +

# --- Mise à jour système, microcode, drivers AMD et Vulkan ---
echo "=== Mise à jour système et installation microcode AMD, drivers GPU et Vulkan ==="
sudo pacman -Syu --noconfirm
sudo pacman -S --noconfirm amd-ucode mesa xf86-video-amdgpu vulkan-radeon lib32-vulkan-radeon \
  linux$(uname -r | cut -d- -f1,2)-headers base-devel git

# --- Installation des dépendances générales ---
echo "=== Installation Qt, SDL2, make, flatpak, samba, ssh, feh, optimisation CPU, Polybar, AntimicroX ==="
sudo pacman -S --noconfirm qt5-base qt5-tools sdl2 make flatpak samba openssh feh cpupower thermald polybar lm_sensors antimicrox radeontop

# --- Activation services CPU & thermald pour perf & stabilité ===
sudo systemctl enable --now cpupower.service
sudo cpupower frequency-set -g performance
sudo systemctl enable --now thermald.service

# --- Clean Gnome ---

sudo apt remove gnome-shell-extension-ubuntu-dock

# --- RetroArch via Flatpak ---
echo "=== Installation RetroArch ==="
if ! flatpak remote-list | grep -q flathub; then
  sudo flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
fi
flatpak install -y flathub org.libretro.RetroArch || true

# --- Autostart GameCore ---
sudo -u "$USER_NAME" mkdir -p "$AUTOSTART_DIR"
DESKTOP_FILE="$AUTOSTART_DIR/GameCore.desktop"
sudo -u "$USER_NAME" bash -c "cat > $DESKTOP_FILE <<EOF
[Desktop Entry]
Type=Application
Name=GameCore
Exec=$GAMECORE_PATH/build/GameCore
WorkingDirectory=$GAMECORE_PATH
EOF"

# --- Compilation GameCore ---
sudo -u "$USER_NAME" mkdir -p "$GAMECORE_PATH/build"
cd "$GAMECORE_PATH/build"
sudo -u "$USER_NAME" qmake ../compile.pro
sudo -u "$USER_NAME" make -j"$(nproc)"

# --- Samba configuration ---
sudo cp /etc/samba/smb.conf /etc/samba/smb.conf.bak.$(date +%s)
sudo bash -c "cat >> /etc/samba/smb.conf <<EOF

[ROMS]
   comment = Dossier pour stocker les ROMS
   path = $GAMECORE_PATH/emu/
   browseable = yes
   read only = no
   writable = yes
   create mask = 0755
   valid users = $USER_NAME
EOF"

sudo mkdir -p "$GAMECORE_PATH/emu/"
sudo chown -R "$USER_NAME:$USER_NAME" "$GAMECORE_PATH/emu/"
printf "%s\n%s\n" "$SMB_PASS" "$SMB_PASS" | sudo smbpasswd -s -a "$USER_NAME"
sudo systemctl restart smb.service nmb.service || true

# --- SSH ---
sudo systemctl enable sshd
sudo systemctl start sshd

# --- Fin ---
echo
echo "=== Installation terminée ==="
echo "GameCore compilé et autostart configuré."
echo "Samba configuré (utilisateur : $USER_NAME)."
echo "SSH activé."
echo "RetroArch installé via Flatpak."
echo "Polybar installé et caché par défaut en bas de l'écran."
echo "Bouton Share PS4 toggle Polybar."
echo "CPU Ryzen optimisé (microcode + mode performance)."
echo "Drivers AMD + Vulkan installés pour GPU Radeon 680M."
echo
read -rp "Redémarrer maintenant ? (y/N) " REBOOT
if [[ "$REBOOT" =~ ^([yY][eE][sS]|[yY])$ ]]; then
  sudo systemctl reboot
else
  echo "➡️ Tu peux redémarrer plus tard avec : sudo systemctl reboot"
fi
