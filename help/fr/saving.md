# Enregistrement

Les modifications ne sont d’abord que dans MovieMetaM. Le fichier du film ne change que lorsque vous enregistrez. Un film avec des modifications non enregistrées a un point jaune dans la liste des films.

## Enregistrer

- **Enregistrer** (barre d’outils) écrit le film sélectionné.
- **Tout enregistrer** écrit tous les films modifiés.

Les deux ne sont actifs que lorsqu’il y a quelque chose à enregistrer. Pendant que MovieMetaM écrit, la fenêtre est verrouillée ; avec de gros fichiers, cela peut prendre un moment. Un message indique combien de films ont été enregistrés, ou ce qui s’est mal passé, par exemple si le fichier n’est pas accessible en écriture.

Après l’enregistrement, MovieMetaM relit le fichier et vérifie le titre, la description, la classification par âge et la jaquette. Si quelque chose ne correspond pas, vous recevez un message d’erreur.

## Directement dans le fichier

MovieMetaM écrit les données **directement dans le fichier du film**, comme la plupart des éditeurs de balises. C’est rapide, même pour de gros fichiers et sur les disques réseau.

> Si l’écriture est interrompue (plantage, coupure de courant, connexion réseau perdue), le fichier peut être endommagé. Conservez une copie des fichiers importants pour vous.

## Enregistrement sécurisé

Pour plus de sécurité, activez **Enregistrement sécurisé** dans les réglages : les données sont alors écrites dans une copie, la copie est vérifiée, et ce n’est qu’ensuite qu’elle remplace le fichier. Cela demande de la place pour une seconde copie du fichier et prend beaucoup plus de temps pour les gros fichiers et sur les disques réseau (un fichier de 4,5 Go sur le réseau prend environ cinq minutes).

## Fermer et retirer

Si un film a des modifications non enregistrées, MovieMetaM demande :

- **Fermeture de la fenêtre :** **Tout enregistrer** enregistre puis ferme de lui-même (en cas d’erreur, la fenêtre reste ouverte). **Ignorer** ferme sans enregistrer. **Annuler** laisse tout tel quel.
- **Retrait de la liste :** voir **Liste des films**.
