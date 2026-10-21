# Speichern

Änderungen stehen zuerst nur in MovieMetaM. Die Filmdatei ändert sich erst, wenn du speicherst. Ein Film mit ungespeicherten Änderungen hat in der Filmliste einen gelben Punkt.

## Speichern

- **Speichern** (Werkzeugleiste) schreibt den ausgewählten Film.
- **Alle speichern** schreibt alle geänderten Filme.

Beide sind nur aktiv, wenn es etwas zu speichern gibt. Während MovieMetaM schreibt, ist das Fenster gesperrt, bei großen Dateien kann das dauern. Eine Meldung sagt, wie viele Filme gespeichert wurden, oder was schiefging, zum Beispiel wenn die Datei nicht beschreibbar ist.

Nach dem Speichern liest MovieMetaM die Datei noch einmal und prüft Titel, Beschreibung, Altersfreigabe und Cover. Stimmt etwas nicht, bekommst du eine Fehlermeldung.

## Direkt in die Datei

MovieMetaM schreibt die Daten wie die meisten Tag-Editoren **direkt in die Filmdatei**. Das geht schnell, auch bei großen Dateien und auf Netzlaufwerken.

> Wird das Schreiben unterbrochen (Absturz, Stromausfall, Netzwerkverbindung weg), kann die Datei beschädigt werden. Bewahre von wichtigen Dateien eine Kopie auf.

## Sicheres Speichern

Für mehr Sicherheit schalte in den Einstellungen **Sicheres Speichern** ein: Die Daten werden dann in eine Kopie geschrieben, die Kopie wird geprüft, und erst danach ersetzt sie die Datei. Das braucht Platz für eine zweite Kopie der Datei und dauert bei großen Dateien und auf Netzlaufwerken deutlich länger (eine Datei von 4,5 GB im Netzwerk etwa fünf Minuten).

## Beim Schließen und Entfernen

Hat ein Film ungespeicherte Änderungen, fragt MovieMetaM nach:

- **Beim Schließen des Fensters:** **Alle speichern** speichert und schließt dann von selbst (gab es einen Fehler, bleibt das Fenster offen). **Verwerfen** schließt ohne Speichern. **Abbrechen** lässt alles, wie es ist.
- **Beim Entfernen aus der Liste:** siehe **Filmliste**.
