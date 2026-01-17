# 🗂️ Organisateur de Fichiers - Pro Edition

Un utilitaire **multiplateforme moderne** développé en **C** avec la bibliothèque **SDL2** pour organiser automatiquement vos dossiers encombrés en les catégorisants par type de fichier.

> **Version 2.0** - Interface redesignée avec confirmations, résultats détaillés et historique

## ✨ Nouvelles Fonctionnalités (v2.0)

- 🎨 **Interface ultra-moderne** : Thème sombre élégant avec gradient et ombres portées
- ✅ **Fenêtre de confirmation** : Validez avant d'organiser les fichiers
- 📊 **Résultats détaillés** : Breakdown complet des fichiers organisés par catégorie
- 🔄 **Bouton Réinitialiser** : Réinitialisez la sélection rapidement
- ⏱️ **Historique** : Accédez aux 10 derniers dossiers organisés
- 🎯 **State Machine** : Gestion fluide des états de l'application (Idle, Confirmation, Organizing, Results)
- 🔔 **Notifications Toast** : Messages en fade-out avec couleurs codifiées
- 🎮 **Animations fluides** : Hover effects et transitions visuelles

## 🎯 Fonctionnalités Principales

*   **Sélection de Dossier Native** : Boîte de dialogue native du système d'exploitation
*   **Organisation Automatique** : Scan et déplacement intelligent des fichiers
*   **Gestion des Erreurs Robuste** : Messages d'erreur détaillés et gestion des cas limites
*   **Support 25+ Extensions** : Documents, Images, Vidéos, Musique, Archives + Autres
*   **Multiplateforme** : Windows, Linux, macOS

## 📂 Catégories d'Organisation

| Catégorie | Dossier | Extensions |
| :--- | :--- | :--- |
| **📄 Documents** | `Documents/` | PDF, DOCX, TXT, XLSX, PPTX, DOC, ODT, RTF |
| **🖼️ Images** | `Images/` | JPG, PNG, GIF, BMP, SVG, WEBP, ICO |
| **🎬 Vidéos** | `Videos/` | MP4, MKV, AVI, MOV, WMV, FLV, WEBM, M4V |
| **🎵 Musique** | `Music/` | MP3, WAV, FLAC, AAC, OGG, WMA, M4A, OPUS |
| **📦 Archives** | `Archives/` | ZIP, TAR, GZ, RAR, 7Z, BZ2, XZ, ISO |
| **⚙️ Autres** | `Autres/` | Toutes les autres extensions |

## 🚀 Installation & Compilation

### Prérequis

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-ttf-dev libgtk-3-dev

# Fedora
sudo dnf install gcc cmake SDL2-devel SDL2_ttf-devel gtk3-devel

# macOS (Homebrew)
brew install cmake sdl2 sdl2_ttf gtk+3
```

### Étapes de Compilation

```bash
# 1. Cloner le dépôt
git clone https://github.com/juniorsems/Organisateur-de-Fichiers-Multiplatforme.git
cd Organisateur-de-Fichiers-Multiplatforme

# 2. Créer et compiler
mkdir build && cd build
cmake ..
make

# 3. Lancer l'application
./FileOrganizer
```

L'exécutable `FileOrganizer` sera généré dans le dossier `build/`.

## 🏗️ Architecture du Code

```
file_organizer/
├── src/
│   ├── main.c              # Interface SDL + State Machine
│   ├── organizer.c         # Logique d'organisation + stats
│   └── cwalk.c             # Manipulation de chemins
├── include/
│   └── organizer.h         # Structures et interfaces
├── assets/
│   ├── font.ttf            # Police principale
│   ├── Inter.ttf           # Police alternative
│   └── Roboto.ttf          # Police alternative
├── external/
│   ├── cwalk.h             # Lib chemins
│   ├── tinydir.h           # Lib parcours dossiers
│   └── nfd-src/            # Native File Dialog
├── CMakeLists.txt
└── README.md
```

## 📦 Dépendances

| Bibliothèque | Rôle |
| :--- | :--- |
| **SDL2** | Fenêtrage et rendering |
| **SDL2_ttf** | Rendu de texte |
| **NFDe (Native File Dialog Extended)** | Sélecteur de dossier natif |
| **TinyDir** | Parcours de répertoires cross-platform |
| **cwalk** | Manipulation de chemins de fichiers |

## 🎮 Utilisation

1. **Lancer l'application** : `./FileOrganizer`
2. **Cliquer sur "📁 Choisir un dossier"**
3. **Sélectionner le dossier à organiser** (ex: Téléchargements)
4. **Confirmer dans la boîte de dialogue**
5. **Voir les résultats détaillés** avec le breakdown par catégorie

### Raccourcis Clavier
- **ESC** : Annuler la confirmation ou fermer les résultats

## 🔒 Sécurité & Robustesse

- ✅ Vérifications NULL exhaustives
- ✅ Gestion des débordements de buffer
- ✅ Validation des chemins (MAX_PATH_LENGTH: 2048)
- ✅ Gestion des fichiers sans extension
- ✅ Messages d'erreur détaillés
- ✅ Compatibilité multiplateforme

## 📊 Améliorations Récentes (v2.0)

### 🎨 Interface
- Fenêtre plus grande (1000x700)
- Palette de couleurs enrichie (15+ couleurs)
- Ombres portées sophistiquées
- Bordures colorées dynamiques
- Emojis intuitifs

### 🔧 Fonctionnalités
- État de confirmation avant organisation
- Panel de résultats détaillés par catégorie
- Historique des 10 derniers dossiers
- Système de notifications Toast
- Gestion d'états fluide (State Machine)

### 🛡️ Robustesse
- +20 extensions supportées
- Meilleure gestion des erreurs
- Tracking des statistiques par catégorie
- Validation complète des entrées

## 📝 Licence

Ce projet est open-source. Libre d'utilisation et de modification.

## 🤝 Contribution

Les contributions sont bienvenues ! N'hésitez pas à :
- Signaler des bugs
- Proposer des améliorations
- Envoyer des pull requests

## 📧 Support

Pour toute question ou problème, veuillez ouvrir une issue sur GitHub.

---

**Développeur [Junior Semassa]**
