# MovieMetaM – Entwickler-Dokumentation

Stand: Version 0.4.10 (Oktober 2026). Diese Doku ist vorerst auf Deutsch. Code, Kommentare und
die Quelltexte der Übersetzungen sind Englisch.

---

## 1. Überblick

MovieMetaM ist ein Editor für die Metadaten von Filmdateien (Qt 6 Widgets, C++17, CMake). Die
App liest Titel, Besetzung, Regie, Beschreibung, Genre, Altersfreigabe und Cover aus MP4-Dateien
und sucht Filmdaten bei **TMDB** (The Movie Database).

**Stand**

| Funktion | Stand |
|---|---|
| Dateien hinzufügen (Dialog, Drag and Drop), Liste mit Statussymbol | fertig |
| MP4-Metadaten und Cover lesen (`iTunMOVI`, Cover-Box) | fertig |
| Formular: Titel, Studio, Beschreibung, Datum, Genre, Altersfreigabe, Tags, Cover | fertig |
| Altersfreigaben nach dem System der Datei (46 Länder) | fertig |
| TMDB-Suche mit Postern, Genreliste | fertig |
| TMDB-Treffer übernehmen (Tabs „Data“ und „Cover“) | offen |
| Speichern in die MP4-Datei | offen (Entscheidung: eigener Writer oder TagLib) |
| Hilfeseite | ein Thema („About MovieMetaM“) |

Unterstützt sind bisher nur MP4 und M4V (`MediaInfoM::supportedSuffixes()`). Andere Dateien
werden mit einer Meldung übersprungen.

**Ablauf**

```
 Dateien hinzufügen ──► Liste (Statussymbol grün/gelb) ──Auswahl──► Formular
                                                                      │
        Suchbegriff ──► TMDB-Ergebnisse ──Doppelklick──► (offen) übernehmen
                                                                      │
                                                  (offen) Speichern in die MP4-Datei
```

Die Oberfläche hat drei Spalten: Filmliste (links), Filminfo (Mitte), TMDB-Suche (rechts, Tabs
Ergebnisse / Daten / Cover). Dazu die Seiten Einstellungen und Hilfe (`stackedWidget`, jede mit
eigener Werkzeugleiste, `showPage()` wechselt).

---

## 2. Einrichten und Bauen

### Voraussetzungen

- **Qt 6** (auf Linux 6.12) mit den Modulen Widgets, Network, Svg, SvgWidgets, LinguistTools.
- CMake ≥ 3.16, C++17.
- **`tmdb_token.txt`** im Projektordner (nicht in Git, eine Zeile: der „API Read Access Token“
  von TMDB). CMake liest sie in das Define `TMDB_TOKEN`. Ohne die Datei läuft die App, aber die
  Suche nicht (CMake warnt). Nach dem ersten Anlegen der Datei CMake neu laufen lassen.

### Bauen

Am einfachsten: `CMakeLists.txt` in **Qt Creator** öffnen, Kit wählen, bauen und starten.

Auf der Kommandozeile:

```
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<Qt-Verzeichnis>
cmake --build build/release
```

Hinweise:

- **Neue Quelldateien zuerst anlegen, dann in `CMakeLists.txt` eintragen.** Qt Creator
  konfiguriert sofort neu; fehlt eine Datei, ist sein Build-Ordner kaputt.
- Defines aus `CMakeLists.txt`: `APP_NAME`, `APP_VERSION`, `COMPANY_NAME`, `TMDB_TOKEN`.
- Windows: `app.rc.in` (Icon `ressources/icons/app_icon.ico` und Versionsinfo). macOS: das
  Bundle-Icon `app_icon.icns`.
- Die Version steht in `CMakeLists.txt` (`PROJECT_VERSION`) und landet als `APP_VERSION` im
  Code.

---

## 3. Projektaufbau

| Datei | Inhalt |
|---|---|
| `main.cpp` | Einstellungen, Übersetzer, Setupdialog beim ersten Start, Hauptfenster |
| `mainwin.*`, `initialize.cpp` | Hauptfenster: Filmliste, Filminfo, TMDB-Suche; Seiten Einstellungen und Hilfe |
| `mediainfom.*` | liest MP4-Boxen: Metadaten, `iTunMOVI`-XML, Cover |
| `moviedata.*` | Daten eines Films, Hilfsfunktionen (`getItunExtc()`, Säubern von Texten) |
| `tagwidget.*`, `tagdelegate.*` | Tag-Eingabe (Besetzung, Regie, Drehbuch, Produktion) |
| `imagelabelm.*` | Cover: zeichnet sich selbst, Drag and Drop hinein und hinaus |
| `tableviewm.*` | Filmliste (`QListView`) mit Dateiablage; Klicks auf leere Fläche ändern die Auswahl nicht |
| `twolinedelegate.*` | Einträge von Filmliste und Suchergebnissen |
| `posterpreview.*` | vergrößertes Poster (200 × 300), wenn die Maus auf einem Ergebnis ruht |
| `setupdialog.*` | Dialog beim ersten Start |
| `helppage.*` | Hilfeseite (Markdown in `help/<de|en>/`, `Help.qrc`) |
| `languages.*` | Sprachen der App und der Filmdaten |
| `agerating.*` | Altersfreigaben als Daten (JSON je Land) |
| `toastm.*` | kurze Meldung über dem Inhalt |
| `themecolors.h` | gemeinsame Farben und Formen |
| `toolbarm.*`, `searchfield.*` | Kopien aus `../libraries` (Master dort) |

### Hauptfenster und Auswahl

- Die Filme liegen in `m_loadedMovies` (Arbeitsstand) und `m_originalMovies` (Zustand beim
  Laden). Der Schlüssel ist der Dateipfad (`Qt::UserRole` des Listeneintrags).
- Der Film im Formular wechselt mit `QItemSelectionModel::currentChanged`
  (`onCurrentMovieChanged`), also auch mit den Pfeiltasten.
- `displayMovie()` füllt das Formular (mit blockierten Signalen, damit kein „geändert“
  entsteht). Jede Eingabe löst `onDataChanged()` aus: `validateAndRefreshChanges()` vergleicht
  das Formular mit dem Original und setzt das Statussymbol.
- Ein Film ohne Datum zeigt im Feld den Standard 1972-01-01; das gilt nicht als Änderung. Das
  Genre ist ein bearbeitbares Auswahlfeld: ein Genre, das nicht in der TMDB-Liste steht,
  bleibt als Text erhalten.

---

## 4. MP4 lesen (`mediainfom.*`)

- `extractMetadata()` wählt nach dem Dateityp den Leser; bisher nur MP4.
- `parseBoxes()` geht die Boxen der Datei durch, ein Besucher (`std::function`) reagiert auf die
  gewünschten. `readBoxHeader()` liest Größe und Typ einer Box (auch die erweiterte 64-Bit-Größe).
- Die Metadaten stehen in `ilst`; unbekannte Namen (`name`-Box) liest `readProprietaryKey()`.
- Besetzung, Regie, Drehbuch, Produktion und Studio liegen als XML (`iTunMOVI`) in der Datei:
  `parseItunMoviXml()` und `parseNameArray()`.
- Die Altersfreigabe steht im Eintrag `iTunEXTC` (Abschnitt 6).
- Das Cover (`covr`) liest `extractCoverImageMp4()` über `findCoverBox()`.
- Fehler gehen über das Signal `errorOccurred` ins Log (`qWarning`).

**Schreiben** ist noch nicht umgesetzt (`saveMovieDetails()` und „Alle speichern“ sind leer).
Vorher ist eine Entscheidung nötig: eigener MP4-Writer (Offset-Tabellen `stco`/`co64` müssen
nachgeführt werden, wenn `moov` vor `mdat` liegt) oder eine Bibliothek wie TagLib (LGPL).

---

## 5. TMDB

- Der Token kommt zur Bauzeit aus `tmdb_token.txt`; `startApiRequest()` setzt ihn als
  Authorization-Header. Fehlt er, zeigt die App „No TMDB token“ als Meldung.
- Anfragen (alle mit `language=<Sprache der Filmdaten>`):
  - Suche: `https://api.themoviedb.org/3/search/movie` (mit `QUrlQuery` gebaut, damit `&`, `#`
    usw. im Titel richtig kodiert werden).
  - Genreliste: `https://api.themoviedb.org/3/genre/movie/list`.
  - Poster: `https://image.tmdb.org/t/p/w500<poster_path>`.
- Die Antworten laufen über `onApiRequestFinished()`; `fillSearchTableView()` und
  `fillGenreComboBox()` füllen Liste und Auswahlfeld.
- Die Poster laden einzeln nach. `m_searchGeneration` verwirft Poster einer älteren Suche. Ein
  Eintrag speichert die TMDB-ID (`UserRole`) und das Poster in voller Auflösung (`kPosterRole`)
  für die Vorschau.
- Nach jeder Suche zeigt ein Toast die Zahl der Treffer („%n movie(s) found“, oder „Showing 20 of
  143 movies“, denn TMDB liefert nur eine Seite mit höchstens 20 Treffern; `total_results` ist die
  Gesamtzahl). Eine Suche ohne Treffer leert die Liste und zeigt „No movies found“.
- Das TMDB-Logo und der Hinweis stehen in der Hilfe und in der Suchspalte (die Bedingung von
  TMDB für die Nutzung der Daten).

---

## 6. Altersfreigaben (`agerating.*`)

- Die Systeme sind **Daten**: eine JSON-Datei je Land in `ressources/ratings/<Code>.json`
  (`Ratings.qrc`). Land (en/de), System, Id und Stufen: `key` (Text im Abzeichen), `code` (wird
  in die Datei geschrieben), `score`, `label` (Text in der Auswahl), `description` (Tooltip),
  Farbe, Form und Stil.
- **Die JSON-Dateien und `Ratings.qrc` sind erzeugt, nicht von Hand zu ändern.** Quelle ist die
  Tabelle `age_ratings.xlsx` (nicht in Git, im kDrive-Ordner `Development/QT`). Nach einer
  Änderung der Tabelle:

  ```
  python3 tools/ratings_from_xlsx.py ~/kDrive/Development/QT/age_ratings.xlsx
  ```

  (braucht `openpyxl`). Stand: 46 Länder, 266 Stufen.
- **Grundsatz** (mit dem Entwickler entschieden): Alle Player lesen den einen Eintrag `iTunEXTC`
  (`Präfix|Code|Score|Grund|`). Der Code ist die Form, die Medienserver kennen (Jellyfin:
  `Emby.Server.Implementations/Localization/Ratings/*.json`, z. B. DE „FSK 12“, nicht Apples
  „Ab 12 Jahren“). Präfix und Score wie bei Apple/Subler (`Resources/Ratings.json`). Wo ein Land
  ein eigenes System für Heimvideo hat, gilt es vor dem Kino (Kanada: CHVRS).
- **Abzeichen:** `createAgeRatingIcon()` zeichnet Formen (Quadrat, Kreis, Raute, Dreieck,
  Achteck, Sechseck, Stern) in den Stilen gefüllt, Umriss (JP, SK, TR), hell (US, IS) und dunkel
  (FI). Die Schrift wird kleiner, bis der Code passt. Gezeichnet wird in der Auflösung des
  Bildschirms.
- **Anzeige eines Films** (`showMovieRating()`): Das System in der Datei (Präfix von `iTunEXTC`,
  `AgeRatings::countryOfSystem()`) bestimmt, welche Stufen die Auswahl zeigt. Das System der
  Einstellungen gilt nur für Filme ohne Altersfreigabe. Gewählt wird nach Code, sonst nach
  Score (ältere Apple-Texte). Ein unbekanntes System oder eine unbekannte Stufe bleibt
  erhalten: Die Auswahl zeigt einen Extra-Eintrag „Präfix: Code“ unter dem leeren Eintrag, ohne
  „geändert“. „Geändert“ heißt: System oder Score weichen ab.
- Die Texte der Altersfreigaben (z. B. „ab 12 Jahren“) sind amtliche Bezeichnungen, die in die
  Datei geschrieben werden: sie werden nicht übersetzt.

---

## 7. Sprachen und Übersetzungen

Es gibt **zwei** Sprachen (`languages.*`):

- **Sprache der App** (Deutsch / Englisch): Einstellung `language`, gilt nach einem Neustart.
- **Sprache der Filmdaten** für TMDB (Genreliste und Suche, z. B. `ja-JP`): Einstellung
  `General/DataLanguage`, gilt sofort. Die Liste sind TMDBs primäre Übersetzungen, die Namen
  kommen aus `QLocale`.

Beide folgen dem System, bis sie im Setupdialog oder in den Einstellungen gewählt werden. Ein
japanischer Nutzer bekommt so die englische App mit japanischen Filmdaten.

**Regeln**

- Quelltexte in `tr()` und in `mainwin.ui` sind **Englisch**; Deutsch steht in
  `i18n/MovieMetaM_de.ts`.
- `MovieMetaM_de.ts` muss vollständig sein (0 unfertige Einträge). `MovieMetaM_en.ts` füllt nur
  die `<numerusform>` bei `%n`-Texten.
- Texte, die nicht übersetzt werden, bekommen in der `.ui` `notr="true"`.
- Nach Änderungen an Texten: `lupdate -no-obsolete` (oder das Ziel `update_translations`), dann
  alle unfertigen deutschen Einträge füllen.
- Die `.qm`-Dateien baut `qt_add_translations` und bettet sie ein (`:/i18n`); sie liegen nicht
  in Git.
- Die Hilfetexte liegen als Markdown in `help/<de|en>/<Thema>.md` (Platzhalter `{{version}}`,
  `{{qt}}`); die Sprache folgt dem Übersetzer.
- Meldungen im Log (`qDebug`, `qWarning`, `errorOccurred`) sind Englisch und werden nicht mit
  `tr()` übersetzt.

---

## 8. Einstellungen (QSettings)

`QSettings()` mit Organisation „SpeckenbachNet“, auf jedem System im INI-Format
(`QSettings::setDefaultFormat` in `main.cpp`).

| Schlüssel | Bedeutung |
|---|---|
| `language` | Sprache der App (Code, z. B. `de`) |
| `General/DataLanguage` | Sprache der Filmdaten für TMDB (z. B. `de-DE`) |
| `General/AgeRatingSystem` | Land des Altersfreigabe-Systems (für Filme ohne Freigabe) |
| `Window/Geometry` | Größe und Lage des Fensters |
| `Window/Splitter` | Breiten der drei Spalten |

Beim ersten Start fragt der Setupdialog (`setupdialog.*`) das System und beide Sprachen ab.
„Abbrechen“ beendet die App.

---

## 9. UI-Bausteine und Stil

- **Farben** aus der Palette (helles und dunkles Thema), keine festen Farben, wo sie auf
  Themenfarben treffen. Ausnahme: der rote Rand der Fehler-Meldung.
- **Akzentfarbe** des Systems (`palette(highlight)`) für ausgewählte Einträge (Filmliste,
  Ergebnisse, geöffnete Auswahlliste) und für Tags. Überschriften in `accentGrey()`.
- **Alles gerundet** mit Radius 6 (`FieldLook::radius`). Helfer in `themecolors.h`:
  `applyInputStyle`, `applySpinStyle`, `applyBoxStyle`, `applyComboStyle`, `applyTabStyle`.
  Auswahlfelder bekommen einen einfachen `QStyledItemDelegate` (der Standard des Menüs
  ignoriert die Regeln des Style Sheets); der Pfeil ist die graue `chevron_down.svg`.
- **Icons:** schwarze SVG-Zeichnungen, in der Textfarbe getönt (ToolbarM, SearchField). Herkunft
  und Lizenzen: `ressources/icons/LICENSES.md`.
- **Gezeichnete Bilder** (Statussymbole, Poster, Abzeichen, Cover) in der Auflösung des
  Bildschirms (`devicePixelRatioF()`), sonst sind sie unscharf. Icons in ausgewählten Einträgen
  bekommen dasselbe Bild für `QIcon::Selected`.
- **Listen** als `QListView` mit einer Spalte (eine `QTableView` kann keine ganze Zeile runden).
  Die Einträge zeichnet `TwoLineDelegate`: Icon, Titel, kleinere gedimmte zweite Zeile (Text
  „Titel\nzweite Zeile“), Abstand 7/6 px, dünne Trennlinie.
- **Meldungen** als Toast (`ToastM`): kurz über der Spalte, zu der sie gehören, mit sanftem Ein-
  und Ausblenden; Klicks gehen hindurch. `MainWin::showColumnStatus(StatusColumn, Text, Level,
  Dauer)`; Hinweise 3 s, Fehler 5 s. Eine Statusleiste gibt es nicht.
- **Spende:** Kaffee-Button in der Werkzeugleiste (orange auf normalem Button), öffnet
  `https://paypal.me/michaelspeckenbach`; derselbe Link steht in der Hilfe.
- **Drag and Drop unter Wayland:** Die Daten schon in `dragEnterEvent` lesen. Im `dropEvent`
  sind Daten aus einem Browser (Brave/Chromium) leer (siehe `ImageLabelM::readDragData`).
- **Gemeinsame Klassen** `ToolbarM` und `SearchField` haben ihren Master in
  `~/Development/QT/libraries/` (mit CHANGELOG und README). Zuerst dort ändern und die Version
  erhöhen, dann in die Projekte kopieren.

---

## 10. Ressourcen und Lizenzen

- Quelltext: GPL v3 (`LICENSE`). Kopfzeilen in den Quelldateien.
- Icons: `ressources/icons/LICENSES.md` (Speichern-Icons von Lucide, ISC). Die Herkunft einiger
  Icons ist noch zu klären.
- TMDB: Logo (`ressources/images/tmdb.svg`) und Hinweis laut den Bedingungen von TMDB.
- Ressourcen: `Icons.qrc`, `Images.qrc`, `Ratings.qrc`, `Help.qrc`.

---

## 11. Versionen, Commits, Veröffentlichung

- Die **Version setzt der Entwickler** in `CMakeLists.txt` (`PROJECT_VERSION`).
- Commit-Nachricht: `0.x.y Kurzer Titel` und darunter eine Liste der Änderungen.
- Einen Installer gibt es noch nicht.

### Werkzeuge

| Werkzeug | Zweck |
|---|---|
| `tools/ratings_from_xlsx.py` | erzeugt die JSON-Dateien und `Ratings.qrc` aus der Tabelle (Abschnitt 6) |
| `_dev/` (nicht im Repository) | Arbeitsmaterial zu den Altersfreigaben, Bildschirmfotos |
