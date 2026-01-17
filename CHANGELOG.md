# 📋 Changelog

Toutes les modifications notables de ce projet seront documentées dans ce fichier.

## [2.0] - 17 Janvier 2026

### ✨ Ajouté
- 🎨 Interface SDL ultra-moderne avec thème sombre élégant
- ✅ Fenêtre de confirmation avant d'organiser les fichiers
- 📊 Panel de résultats détaillés avec breakdown par catégorie
- 🔄 Bouton Réinitialiser pour réinitialiser la sélection
- ⏱️ Historique des 10 derniers dossiers organisés
- 🎯 State Machine pour gestion fluide des états (Idle, Confirmation, Organizing, Results)
- 🔔 Système de notifications Toast avec fade-out
- 🎮 Animations fluides avec hover effects et transitions
- 🛡️ Support de 25+ extensions de fichiers
- 📈 Tracking détaillé des statistiques par catégorie
- 🔒 Vérifications de sécurité exhaustives

### 🔧 Amélioré
- Interface utilisateur agrandie (1000x700)
- Palette de couleurs enrichie (15+ couleurs)
- Ombres portées sophistiquées et bordures colorées
- Gestion des erreurs avec messages détaillés
- Validation complète des chemins et extensions

### 🐛 Corrigé
- Fuite mémoire dans render_text()
- Débordement de buffer sur chemins longs
- Gestion NULL des extensions manquantes
- Chemin des polices fragile en fonction du répertoire d'exécution

### 📦 Dépendances
- SDL2 >= 2.0
- SDL2_ttf >= 2.0
- GTK3 (Linux)
- NFD Extended

## [1.0] - Initial Release

### ✨ Ajouté
- Interface SDL basique
- Organisation automatique par extension
- Support multiplateforme
- Sélecteur de dossier natif
- 5 catégories principales + Autres

---

**Note:** Les versions futures incluront :
- Support du Drag & Drop
- Mode batch pour traiter plusieurs dossiers
- Configuration des catégories personnalisées
- Interface alternative en ligne de commande
