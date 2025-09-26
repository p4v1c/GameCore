# 🎮 GameCore : Instructions d'Installation et d'Utilisation

Ce guide vous aidera à configurer et à optimiser **GameCore** sur votre système.
Suivez les étapes ci-dessous pour une expérience de jeu fluide.

---

## 📝 Prérequis du Système

Avant de lancer l'installation, assurez-vous que votre système est configuré pour une utilisation optimale :

- **Désactiver la barre des tâches** : Pour une expérience plein écran immersive.
- **Désactiver la mise en veille automatique** : Pour éviter les interruptions pendant vos sessions de jeu.
- **Mettre à jour le fond d'écran** : Personnalisez votre environnement de bureau.
- **Connecter la manette en Bluetooth** : Assurez-vous que votre manette est jumelée et reconnue par le système avant de lancer l'application.

---

## 🚀 Installation Automatique

Le script `Install-Manjaro.sh` simplifie le processus d'installation pour les systèmes **Manjaro**.

```bash
chmod +x Install-Manjaro.sh
./Install-Manjaro.sh
```


## ⚠️ Étapes Cruciales avant le Redémarrage

Certaines étapes manuelles sont nécessaires pour que les émulateurs fonctionnent correctement.
Effectuez-les **impérativement avant de redémarrer votre machine** :

---

### 🔧 Firmwares et BIOS des émulateurs
Téléchargez les **firmwares** (BIOS, clés, etc.) requis et placez-les dans les dossiers correspondants de chaque émulateur.
C'est **indispensable** pour la compatibilité avec un grand nombre de jeux.

---

### 🎮 Configuration des jeux lourds (Citra / Citron)
Pour des jeux exigeants comme *The Legend of Zelda: Breath of the Wild (BOTW)* ou *Tears of the Kingdom (TOTK)*, il est fortement recommandé de :

- Les lancer une première fois pour générer les **shaders**.
- Installer des **mods d’optimisation** (comme *nx-optimizer*) afin d’améliorer les performances (ex : augmenter les FPS).


## 📂 BIOS et Firmwares Recommandés (ou Obligatoires)

La plupart des émulateurs modernes ne fonctionnent pas sans leurs fichiers système d'origine.
Voici un récapitulatif des besoins par émulateur :

---

### 🟦 Dolphin (GameCube, Wii)
- Le BIOS n'est pas toujours requis pour les jeux GameCube.
- Un **dump officiel de la NAND Wii** est nécessaire pour certaines fonctionnalités et pour l’émulation Wii.

---

### 🟥 DuckStation (PS1)
- Un **BIOS SCPH officiel** est vivement recommandé pour une compatibilité maximale.
- Permet également d’éviter les problèmes de performance.

---

### 🟨 melonDS (DS, DSi)
- Un **BIOS et un firmware** sont obligatoires pour garantir que 100% des jeux se lancent sans problème.
- Pour la **DSi**, un **dump de la NAND** est également requis.

---

### 🟩 gopher56 (N64)
- Fonctionne souvent **sans BIOS**.
- Certains plugins peuvent cependant en exiger un.

---

### 🟪 pscx2 (PS2)
- Un **BIOS PS2** est obligatoire pour démarrer les jeux.

---

### 🟧 Citron (3DS)
- Nécessite une **clé (prod.keys)** et un **firmware** pour décrypter et lancer les jeux.

---

### 🟦 RPCS3 (PS3)
- Un **BIOS PS3** est obligatoire pour lancer les jeux.

---

### 🟫 Cemu (Wii U)
- Nécessite une **clé de jeu** pour lancer les fichiers `.wux` et autres formats chiffrés.

---

📖 Pour obtenir ces fichiers, consultez la documentation officielle :
👉 [Emulator Files - gametechwiki](http://emulation.gametechwiki.com/index.php/Emulator_Files)


### Misc
```sh
/home/pavic/.var/app/org.azahar_emu.Azahar/config/azahar-emu/qt-config.ini
/home/pavic/.var/app/io.github.gopher64.gopher64/config/gopher64/config.json
/home/pavic/.var/app/io.mgba.mGBA/config/mgba/config.ini
/home/pavic/.var/app/net.kuribo64.melonDS/config/melonDS/melonDS.toml
/home/pavic/.var/app/org.DolphinEmu.dolphin-emu/GCPadNew.ini
/home/pavic/.var/app/org.ppsspp.PPSSPP/config/ppsspp/PSP/SYSTEM/ppsspp.ini
/home/pavic/.var/app/org.ppsspp.PPSSPP/config/ppsspp/PSP/SYSTEM/controls.ini

/home/pavic/.local/share/duckstation/settings.ini
```
