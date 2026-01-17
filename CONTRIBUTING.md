# 🤝 Guide de Contribution

Merci de votre intérêt pour contribuer au projet **Organisateur de Fichiers** ! 

## Comment Contribuer

### 🐛 Signaler un Bug

1. Vérifiez que le bug n'a pas déjà été signalé dans les [Issues](https://github.com/juniorsems/Organisateur-de-Fichiers-Multiplatforme/issues)
2. Ouvrez une nouvelle issue avec :
   - Une description claire du problème
   - Les étapes pour reproduire
   - Votre système d'exploitation et version
   - Les messages d'erreur éventuels

### 💡 Proposer une Amélioration

1. Ouvrez une issue avec le tag `enhancement`
2. Décrivez clairement l'amélioration proposée
3. Expliquez pourquoi cela améliorerait le projet

### 🔧 Soumettre du Code

1. **Fork** le projet
2. **Créez une branche** pour votre feature (`git checkout -b feature/MaFeature`)
3. **Committez vos changements** (`git commit -m 'Ajoute MaFeature'`)
4. **Poussez vers la branche** (`git push origin feature/MaFeature`)
5. **Ouvrez une Pull Request**

## Standards de Code

- Utilisez le standard C11
- Respectez le format de code existant
- Commentez le code complexe
- Testez sur au moins Linux

## Compilation Locale pour Développement

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
./FileOrganizer
```

## Questions ?

N'hésitez pas à ouvrir une issue avec le tag `question` ou à nous contacter directement !

Merci de votre contribution ! 🌟
