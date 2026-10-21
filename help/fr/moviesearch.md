# Recherche de films

À droite, vous trouvez les données de films sur TMDB (The Movie Database) et vous les reprenez dans le film sélectionné. MovieMetaM a besoin d’une connexion Internet pour cela.

## Rechercher

Saisissez un titre dans le champ de recherche et appuyez sur Entrée. Les résultats apparaissent dans l’onglet **Résultats**, chacun avec affiche, titre et année. Un court message indique combien de films ont été trouvés. Les 20 premiers résultats sont affichés ; si TMDB en connaît davantage, il indique par exemple « 20 films affichés sur 134 », et vous affinez la recherche.

La langue dans laquelle TMDB cherche et répond est la **langue des données du film** dans les réglages.

## Regarder les affiches

Laissez la souris un instant sur un résultat et l’affiche apparaît en plus grand.

## Regarder les données

Un double-clic sur un résultat charge ses données dans l’onglet **Données** : titre, date de sortie, studio, genre, classification par âge, description, acteurs, réalisateurs, scénario et producteurs. L’affiche est affichée au-dessus.

Un clic sur l’affiche ouvre une liste de jusqu’à 30 affiches de TMDB. Chacune indique la langue du texte imprimé (ou « Sans texte »), la taille en pixels et la note. Cliquez sur l’une pour la choisir. En haut à droite de l’affiche, vous voyez laquelle c’est, par exemple « 2 / 14 ».

![Onglet Données avec l’affiche et la liste des affiches](:/images/help/search.png)

MovieMetaM reprend la classification par âge selon ces règles :

- Si le fichier enregistré a déjà une classification, par exemple « FSK 12 », MovieMetaM prend de TMDB la classification du même pays (ici l’Allemagne). Ainsi, un film reste dans son système.
- Si le fichier n’en a pas encore, ou si vous avez modifié la classification dans le programme (par exemple en appliquant des données), le pays des réglages est utilisé, même après que vous l’avez changé.
- Si TMDB n’a rien pour ce pays, la classification du film reste telle quelle.

## Appliquer

**Appliquer** écrit les données dans le film sélectionné et prend l’affiche choisie comme jaquette. Ce que TMDB ne connaît pas reste tel quel – les valeurs vides ne changent rien.

> Ensuite, le film a un point jaune dans la liste. Rien n’est encore enregistré, la manière d’enregistrer est expliquée sous **Enregistrement**.

Si aucun film n’est sélectionné, un court message vous le dit.
