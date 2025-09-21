# GameCore


## Manual Installation

- Prepare Opt directory :
```sh
chmod 666 /opt
cp ../GameCore/ /opt
sudo chown -R p4v1c:p4v1c /opt/GameCore/
```

- Install QT :
```sh
sudo apt update
sudo apt install qtbase5-dev qtbase5-dev-tools qtchooser qt5-qmake qttools5-dev qttools5-dev-tools build-essential
```

- Install SDL :
```sh
sudo apt update
sudo apt install libsdl2-dev
```
- Install Make:

```sh
sudo apt update
sudo apt install make
```

- Install Flatpak :
```sh
sudo apt install flatpak
```

- Install retro-arch :
```sh
flatpak install flathub org.libretro.RetroArch
```

- Install Ryujinx :

https://git.ryujinx.app/ryubing/ryujinx/-/releases
====================

- Install RPCS3 :

https://rpcs3.net/download
====================

- GameCore as service :

**Create .service file**
```sh
sudo nano /etc/systemd/system/GameCore.service
```

** Content **
```sh
[Unit]
Description=GameCore
After=network.target

[Service]
ExecStart=/opt/GameCore/build/GameCore
User=p4v1c
Restart=always

[Install]
WantedBy=graphical.target
```

** Restart Service**
```sh
sudo systemctl enable GameCore.service
sudo systemctl start GameCore.service
```

- Setup ssh server :
```sh
sudo apt update
sudo apt install openssh-server
```

- Setup smbserver :

**Installez Samba**
```sh
sudo apt install samba
```

**Modify Samba config**
```sh
sudo nano /etc/samba/smb.conf
```
**Content**
```sh
[partage_prive]
comment = Rom
path = /opt/GameCore/emu/
browseable = yes
read only = no
writable = yes
create mask = 0755
valid users = p4v1c
```

**Creds**
```sh
sudo smbpasswd -a p4v1c
```

**Restart service**
```sh
sudo systemctl restart smbd
sudo systemctl restart nmbd
```

- Disable login screen

**Modify that file**
```sh
sudo nano /etc/gdm3/custom.conf
```

**Content**
```sh
[daemon]
# Activer la connexion automatique
AutomaticLoginEnable = true
AutomaticLogin = p4v1c
```

- Reboot :
```sh
systemctl reboot
```

## Automatic Installation
```sh
chmod +x install.sh
./install
```

⚠️ BIOS / firmware recommandé ou obligatoire

Dolphin → pour GameCube, souvent ok sans BIOS, mais pour Wii et certaines fonctionnalités (Wii NAND, IOS) il faut le dump officiel.

SwanStation → PS1 : un BIOS SCPH officiel est recommandé pour compatibilité maximale.

melonDS → pour Nintendo DS, un BIOS + firmware est nécessaire pour 100 % des jeux.

Mupen64Plus → N64 : certains plugins demandent le BIOS, mais beaucoup de jeux fonctionnent sans.

LRPS2 → PS2 : BIOS PS2 obligatoire pour lancer les jeux.

Ryujinx

RPCS3

- Doc For the firmwares :

http://emulation.gametechwiki.com/index.php/Emulator_Files
