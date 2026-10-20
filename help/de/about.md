# MovieMetaM

Version {{version}} · Qt {{qt}}

Editor für die Metadaten von Filmdateien: Titel, Besetzung, Regie, Beschreibung, Genre, Altersfreigabe und Cover – mit Filmdaten aus The Movie Database (TMDB).

## Entwickler

Michael Speckenbach · SpeckenbachNet

Webseite: [software.speckenbach.net/moviemetam](https://software.speckenbach.net/moviemetam)  
Kontakt: [moviemetam@speckenbach.net](mailto:moviemetam@speckenbach.net)

## Unterstützen

Gefällt dir MovieMetaM? Spendier mir einen Kaffee: [paypal.me/michaelspeckenbach](https://paypal.me/michaelspeckenbach)

## Speichern

MovieMetaM schreibt die Daten wie die meisten Tag-Editoren **direkt in die Filmdatei**. Das geht schnell, auch bei großen Dateien und auf Netzlaufwerken. Wird das Schreiben unterbrochen (Absturz, Stromausfall, Netzwerkverbindung weg), kann die Datei beschädigt werden. Bewahre von wichtigen Dateien eine Kopie auf.

Für mehr Sicherheit schalte in den Einstellungen **Sicheres Speichern** ein: Die Daten werden dann in eine Kopie geschrieben, die Kopie wird geprüft, und erst danach ersetzt sie die Datei. Das braucht Platz für eine zweite Kopie der Datei und dauert bei großen Dateien und auf Netzlaufwerken deutlich länger.

Nach dem Speichern liest MovieMetaM die Datei noch einmal und prüft Titel, Beschreibung, Altersfreigabe und Cover.

## Lizenz

© 2026 Michael Speckenbach

MovieMetaM ist freie Software: Sie können sie unter den Bedingungen der **GNU General Public License**, Version 3 oder (nach Ihrer Wahl) jeder späteren Version, weitergeben und/oder verändern. Das Programm wird ohne jede Gewährleistung bereitgestellt. Den Lizenztext finden Sie unter [gnu.org/licenses](https://www.gnu.org/licenses/).

Quellcode: [github.com/SpeckenbachNet/MovieMetaM](https://github.com/SpeckenbachNet/MovieMetaM)

## Filmdaten

![The Movie Database](:/images/tmdb.svg)

MovieMetaM sucht Filmdaten über die API von [The Movie Database (TMDB)](https://www.themoviedb.org). Dieses Produkt verwendet die TMDB-API, ist aber nicht von TMDB unterstützt oder zertifiziert.

## Danksagungen

- **Qt** – The Qt Company, LGPL v3
- **TagLib** – schreibt die Tags in die Filmdateien, LGPL v2.1 oder MPL 1.1
- **Lucide** – Symbole, ISC-Lizenz
- **Material Icons** – Google, Apache License 2.0
- Weitere Symbole aus **churchWorshipM** und von **SVG Repo**
