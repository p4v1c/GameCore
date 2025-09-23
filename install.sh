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

# --- Installation des dépendances ---
echo "=== Installation Qt, SDL2, Make, Flatpak, SSH, build-essential ==="
sudo apt update
sudo apt install -y \
  qtbase5-dev qtbase5-dev-tools qtchooser qt5-qmake qttools5-dev qttools5-dev-tools \
  libsdl2-dev make flatpak openssh-server samba build-essential

# --- Installation i3 et configuration auto-start ---
echo "=== Installation i3 et configuration auto-start ==="
sudo apt install -y i3 dmenu xinit

# --- Installation de feh et configuration du fond d'écran ---
sudo apt install -y feh
sudo -u "$USER_NAME" bash -c "echo 'exec --no-startup-id feh --bg-scale $GAMECORE_PATH/background.png' >> /home/$USER_NAME/.config/i3/config"


# Création du fichier .xinitrc pour lancer i3
sudo -u "$USER_NAME" bash -c "echo 'exec i3' > /home/$USER_NAME/.xinitrc"

# Modification de .bash_profile pour lancer startx automatiquement sur TTY1
BASH_PROFILE="/home/$USER_NAME/.bash_profile"
sudo -u "$USER_NAME" bash -c "cat >> $BASH_PROFILE <<'EOF'

# Auto-start X et i3 sur TTY1
if [[ -z \$DISPLAY ]] && [[ \$(tty) == /dev/tty1 ]]; then
    startx
fi
EOF"

echo "✅ i3 installé et configuré pour démarrer automatiquement au login TTY1."

# --- RetroArch via Flatpak ---
echo "=== Installation RetroArch ==="
if ! flatpak remote-list | grep -q flathub; then
  sudo flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
fi
flatpak install -y flathub org.libretro.RetroArch || true

# --- Instructions Ryujinx & RPCS3 ---
echo
echo "=== Ryujinx & RPCS3 ==="
echo "Télécharge manuellement :"
echo "  - Ryujinx : https://git.ryujinx.app/ryubing/ryujinx/-/releases"
echo "  - RPCS3   : https://rpcs3.net/download"

# --- Autostart GameCore sous i3 ---
echo "=== Configuration autostart GameCore ==="
AUTOSTART_DIR="/home/$USER_NAME/.config/autostart"
sudo -u "$USER_NAME" mkdir -p "$AUTOSTART_DIR"

DESKTOP_FILE="$AUTOSTART_DIR/GameCore.desktop"
sudo -u "$USER_NAME" bash -c "cat > $DESKTOP_FILE <<EOF
[Desktop Entry]
Type=Application
Name=GameCore
Exec=$GAMECORE_PATH/build/GameCore
WorkingDirectory=$GAMECORE_PATH
X-GNOME-Autostart-enabled=true
EOF"

# --- Compilation GameCore ---
echo "=== Compilation de GameCore ==="
if [ ! -d "$GAMECORE_PATH/build" ]; then
  sudo -u "$USER_NAME" mkdir -p "$GAMECORE_PATH/build"
fi
cd "$GAMECORE_PATH/build"
sudo -u "$USER_NAME" qmake compile.pro
sudo -u "$USER_NAME" make -j"$(nproc)"

# --- Samba configuration ---
echo "=== Configuration Samba ==="
sudo cp /etc/samba/smb.conf /etc/samba/smb.conf.bak.$(date +%s)
sudo bash -c "cat >> /etc/samba/smb.conf <<EOF

[partage_prive]
   comment = Rom
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

sudo systemctl restart smbd nmbd

# --- SSH ---
sudo systemctl enable ssh
sudo systemctl start ssh

# --- Fin ---
echo
echo "=== Installation terminée ==="
echo "GameCore compilé et autostart configuré (fichier GameCore.desktop)."
echo "Samba configuré (utilisateur : $USER_NAME)."
echo "SSH activé."
echo "RetroArch installé via Flatpak."
echo "i3 installé et configuré pour démarrage automatique."
echo
echo "➡️ Tu peux maintenant supprimer le dossier source '../GameCore' si tu veux libérer de l’espace."
echo
read -rp "Redémarrer maintenant ? (y/N) " REBOOT
if [[ "$REBOOT" =~ ^([yY][eE][sS]|[yY])$ ]]; then
  sudo systemctl reboot
else
  echo "➡️ Tu peux redémarrer plus tard avec : sudo systemctl reboot"
fi
