# MovieMetaM – Entwickler-Dokumentation

Stand: Version 0.7.10 (Oktober 2026). Diese Doku ist vorerst auf Deutsch. Code, Kommentare und
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
| TMDB-Treffer übernehmen (Tab „Data“: Daten, Poster-Auswahl, Altersfreigabe, „Apply“) | fertig |
| Speichern in die MP4-Datei (TagLib) | fertig |
| Hilfeseite | Themen von links nach rechts: Filmliste, Filminfo, Filmsuche, Speichern und Einstellungen fertig; „About MovieMetaM“ |

Unterstützt sind bisher nur MP4 und M4V (`MediaInfoM::supportedSuffixes()`). Andere Dateien
werden mit einer Meldung übersprungen. Eine MP4, die sich nicht lesen lässt (kaputt oder
abgeschnitten, `MediaInfoM::isReadableMp4()`: die oberste Box-Ebene muss eine vollständige
`moov`-Box enthalten), kommt nicht in die Liste; eine Fehlermeldung nennt ihre Anzahl. Eine MP4
ohne Tags ist lesbar und kommt hinein.

**Ablauf**

```
 Dateien hinzufügen ──► Liste (Statussymbol grün/gelb) ──Auswahl──► Formular
                                                                      │
        Suchbegriff ──► TMDB-Ergebnisse ──Doppelklick──► Tab „Data“ ──Apply──► Film
                                                                      │
                                                  Speichern in die MP4-Datei (TagLib)
```

Die Oberfläche hat drei Spalten: Filmliste (links), Filminfo (Mitte), TMDB-Suche (rechts, Tabs
Ergebnisse / Daten). Dazu die Seiten Einstellungen und Hilfe (`stackedWidget`, jede mit
eigener Werkzeugleiste, `showPage()` wechselt). Die Einstellungsseite besteht aus Karten nach
Thema in zwei Spalten (`initializeSettingsPage()`), jede Einstellung ein Block aus Überschrift,
Feld und dem, was dazugehört: links Sprachen, Altersfreigabe (mit der Vorschau darunter) und
Speichern, rechts Eigene Genres (mit „Nur eigene Genres zeigen“ darunter) und Übernehmen.
Ausprobiert und verworfen: eine lange Karte (wird zur losen Sammlung) und eine Themenliste links
(man sieht nie alles auf einmal). Die alte Karte „General“ der .ui-Datei ist nur noch der Halter
der Widgets und wird beim Start gelöscht.

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
- Ausgaben: kein `qDebug` im Code, `qWarning` nur für echte Fehler. Alle Builds außer Debug
  setzen `QT_NO_DEBUG_OUTPUT` und `QT_NO_WARNING_OUTPUT`: Im Systemlog des Benutzers landet
  nichts von uns; was er wissen muss, zeigt die App (Meldungen).
- Windows: `app.rc.in` (Icon `ressources/icons/app_icon.ico` und Versionsinfo). macOS: das
  Bundle-Icon `app_icon.icns`. Die `Info.plist` des Bundles ist `ressources/macos/Info.plist.in` (Qts
  Vorlage plus `CFBundleLocalizations` de/en); sonst bleibt der native Öffnen-Dialog des Mac auf
  Englisch, er folgt der Systemsprache, nicht der App-Sprache.
- Die Version steht in `CMakeLists.txt` (`PROJECT_VERSION`) und landet als `APP_VERSION` im
  Code.

---

## 3. Projektaufbau

| Datei | Inhalt |
|---|---|
| `main.cpp` | Einstellungen, Übersetzer, Setupdialog beim ersten Start, Hauptfenster |
| `mainwin.*`, `initialize.cpp` | Hauptfenster: Filmliste, Filminfo, TMDB-Suche; Seiten Einstellungen und Hilfe |
| `mp4writer.*` | schreibt die Daten mit TagLib in die MP4-Datei, direkt oder über eine Kopie (Abschnitt 4) |
| `mediainfom.*` | liest MP4-Boxen: Metadaten, `iTunMOVI`-XML, Cover |
| `moviedata.*` | Daten eines Films, Hilfsfunktionen (`getItunExtc()`, Säubern von Texten) |
| `tagwidget.*` | Tag-Eingabe (Besetzung, Regie, Drehbuch, Produktion); beim Überfahren zeigt ein Tag rechts ein ×, nur ein Klick darauf entfernt ihn (Entf-/Rücktaste: der gewählte Tag; danach – auch nach dem Verschieben – ist kein Tag gewählt; Esc, ein Klick außerhalb der Liste – auch auf eine leere Fläche – oder Tab heben die Auswahl auf), ein Klick auf den Text wählt aus oder zieht zum Umsortieren; ein zu langer Name wird mit „…“ gekürzt (kein waagerechter Bildlauf, `TagDelegate::sizeHint` liefert nur die Höhe), der Tooltip zeigt dann den ganzen Namen (`TagModel`) |
| `imagelabelm.*` | Cover: zeichnet sich selbst, Drag and Drop hinein und hinaus, entfernen mit × (Hover) oder Auswählen + Entf |
| `tableviewm.*` | Filmliste (`QListView`) mit Dateiablage (Ordner werden in `addFilesToModel` aufgelöst: MP4/M4V samt Unterordnern in natürlicher Reihenfolge, ohne versteckte Dateien wie `._x.mp4`); Entf/Rücktaste entfernt die Auswahl (wie der Knopf −, `removeRequested`); Klicks auf leere Fläche ändern die Auswahl nicht |
| `twolinedelegate.*` | Einträge von Filmliste und Suchergebnissen |
| `posterpicker.*` | großes Poster im Tab „Data“ mit Abzeichen „n / m“; ein Klick öffnet eine Liste der Poster |
| `posterpreview.*` | vergrößertes Poster (200 × 300), wenn die Maus auf einem Ergebnis ruht |
| `setupdialog.*` | Dialog beim ersten Start |
| `helppage.*` | Hilfeseite (Markdown in `help/<de|en>/`, `Help.qrc`) |
| `languages.*` | Sprachen der App und der Filmdaten |
| `agerating.*` | Altersfreigaben als Daten (JSON je Land) |
| `bubble.*` | `Bubble`: abgerundete Sprechblase (eigenes Fenster, Farben der Felder); `TooltipFilter` zeigt **alle** Tooltips der App als Blase (statt der eckigen des Systems), installiert in `main.cpp` |
| `infoicon.*` | kleines Info-Symbol, das einen längeren Text in einer `Bubble` über dem Symbol zeigt (beim Überfahren und per Klick); `InfoIcon::savingNote()` ist der Hinweis zum direkten Schreiben |
| `fileopener.*` | Filme von außen: Kommandozeile, zweite Instanz, `QFileOpenEvent` (siehe „Dateien von außen“) |
| `toastm.*` | kurze Meldung über dem Inhalt |
| `themecolors.h` | gemeinsame Farben und Formen |
| `toolbarm.*`, `searchfield.*` | Kopien aus `../libraries` (Master dort) |

### Hauptfenster und Auswahl

- Die Filme liegen in `m_loadedMovies` (Arbeitsstand) und `m_originalMovies` (Zustand beim
  Laden). Der Schlüssel ist der Dateipfad (`Qt::UserRole` des Listeneintrags).
- Der Film im Formular wechselt mit `QItemSelectionModel::currentChanged`
  (`onCurrentMovieChanged`), also auch mit den Pfeiltasten. Dort wird auch das Suchfeld mit
  `listTitle()` gefüllt (`showTitleInSearch()`, auch beim Tippen im Titelfeld und nach „Übernehmen“,
  nur in diese Richtung) (Titel des Films, sonst Dateiname bis zum ersten Punkt; kein Jahr, keine
  Bereinigung von Namen wie „1080p“ – die einfache Fassung, mit dem Nutzer entschieden). Die erste
  Zeile des Listeneintrags ist derselbe Text und folgt dem aktuellen Titel (`updateListEntry()` aus
  `onDataChanged`, also beim Tippen und beim Übernehmen).
- `displayMovie()` füllt das Formular (mit blockierten Signalen, damit kein „geändert“
  entsteht). Jede Eingabe löst `onDataChanged()` aus: `validateAndRefreshChanges()` vergleicht
  das Formular mit dem Original und setzt das Statussymbol.
- Ein Film ohne Datum zeigt im Feld den Standard 1972-01-01; das gilt nicht als Änderung. Das
  Genre ist ein bearbeitbares Auswahlfeld: ein Genre, das nicht in der TMDB-Liste steht,
  bleibt als Text erhalten.

### Dateien von außen (`fileopener.*`)

Filme kommen auch von außerhalb in die Liste: „Öffnen mit“, auf das Programmsymbol gezogen oder
als Argument der Kommandozeile. Auf jedem System kommen sie anders an, alle Wege enden in
`MainWin::openFiles()` und damit in `addFilesToModel` (Ordner werden aufgelöst, der erste neue
Film wird gewählt).

| System | Weg | Voraussetzung |
|---|---|---|
| Linux | Argumente (`%F` in der .desktop-Datei) | `ressources/linux/net.speckenbach.MovieMetaM.desktop`, Icon, `setDesktopFileName` |
| Windows | Argumente (Ziehen auf die .exe oder eine Verknüpfung) | für einen Eintrag in „Öffnen mit“ Registry-Einträge, die der Installer schreibt (`tools/MovieMetaM.iss`, Abschnitt 11) |
| macOS | `QFileOpenEvent`, eines je Datei | `CFBundleDocumentTypes` in `Info.plist.in` (MP4, M4V, Ordner) |

- **Eine Instanz:** Unter Linux und Windows startet das System sonst eine zweite Instanz. Sie
  verbindet sich mit dem `QLocalServer` der ersten (Name `MovieMetaM-<Benutzer>`, das Socket liegt
  unter Linux im gemeinsamen Temp-Ordner), schickt die Pfade (`QDataStream`) und beendet sich. Ein
  Socket, das ein Absturz hinterlassen hat, wird vor dem Starten des Servers entfernt. Die Prüfung
  steht ganz am Anfang von `main()`, noch vor dem Einrichtungsdialog. macOS startet die App nur
  einmal, dort gibt es keinen Server.
- **Sammeln:** Dateien, die kurz nacheinander ankommen, werden 100 ms gesammelt und als eine
  Liste geliefert (`FileOpener::deliver()`). Der Mac schickt ein Ereignis je Datei, Windows startet
  bei „Öffnen mit“ oft einen Prozess je Datei; ohne das stünde die Auswahl am Ende auf dem letzten
  Film und die Meldungen erschienen mehrfach.
- **Vor dem Fenster:** Was vor dem Hauptfenster ankommt (Start per Doppelklick auf dem Mac), wartet
  bis `FileOpener::start()`. Beim Speichern ist das Fenster gesperrt, `openFiles()` wartet dann auf
  das Ende (`finished` des Watchers).
- **Getestet:** Linux (Kommandozeile, „Öffnen mit“ unter niri, Starter unter Plasma, drei Instanzen
  gleichzeitig) und macOS (Dock-Symbol, mehrere Filme). Nicht getestet: GNOME, Windows, das Warten
  beim Speichern. Auf Windows ist ein Fehler gemeldet (vor dem Installer), siehe Punkt 16 in
  `CLAUDE.md`.

---

## 4. MP4 lesen (`mediainfom.*`)

- `extractMetadata()` wählt nach dem Dateityp den Leser; bisher nur MP4.
- `parseBoxes()` geht die Boxen der Datei durch, ein Besucher (`std::function`) reagiert auf die
  gewünschten. `readBoxHeader()` liest Größe und Typ einer Box (auch die erweiterte 64-Bit-Größe).
- Die Metadaten stehen in `ilst`; unbekannte Namen (`name`-Box) liest `readProprietaryKey()`.
- Besetzung, Regie, Drehbuch, Produktion und Studio liegen als XML (`iTunMOVI`) in der Datei:
  `parseItunMoviXml()` und `parseNameArray()`.
- Die Altersfreigabe steht im Eintrag `iTunEXTC` (Abschnitt 6).
- **Inhaltshinweis** (`rtng`, Apples „content advisory“, 1 Byte): 0 keiner, 2 „clean“, 1 oder 4
  „explicit“. Das Formular hat dafür die Auswahl „Content Advisory“ (None / Clean / Explicit; ein
  anderer Wert bleibt als Eintrag „Other (n)“ erhalten). `readIntegerDataBox()` liest ihn.
  Geschrieben wird er nur, wenn er nicht 0 ist oder schon in der Datei steht (Explicit als 1;
  AtomicParsley liest 1 als „Explicit Content“). Apple-Geräte zeigen ein „E“ und werten ihn in
  der Kindersicherung aus; andere Player (Jellyfin, Plex, VLC) meist nicht.
- **Format:** `readVideoSize()` liest Breite und Höhe aus dem `tkhd` der ersten Videospur (16.16
  Festkomma, die letzten 8 Bytes der Box; die Spur erkennt es am `hdlr` „vide“). Das Formular zeigt
  sie mittig unter dem Cover, nur zur Information: ein Chip „720p“ im Look der Tags
  (Akzentfarbe), daneben „1280 × 720“; ohne Größe bleibt die Zeile leer, behält aber ihren Platz. `hdvd` (Apples HD-Etikett:
  0 SD, 1 720p, 2 1080p, 3 4K) setzt `Mp4Writer` beim Speichern aus der Größe, nach der Breite
  (`MovieData::hdFlag()`; ein zugeschnittener Breitwandfilm mit 1920 × 800 ist 1080p).
- Das Cover (`covr`) liest `extractCoverImageMp4()` über `findCoverBox()`.
- Fehler gehen über das Signal `errorOccurred` ins Log (`qWarning`).

**Schreiben** (`Mp4Writer`, `mp4writer.*`) macht die Bibliothek TagLib. Wie sie eingebunden ist,
steht im Abschnitt „TagLib (Bibliothek)“ weiter unten.

- „Save“ speichert den ausgewählten Film (`saveMovieDetails()`), „Save all“ alle geänderten
  (`onSaveAllButtonClicked()`); beide rufen `startSaving()`. `updateSaveButtons()` schaltet die
  Knöpfe.
- **Im Thread:** `startSaving()` kopiert die Daten der Filme in `SaveJob`s (ohne `QPixmap`: das
  gehört zum GUI-Thread, die Cover gehen als `QImage`), sperrt das Fenster
  (`centralWidget()->setEnabled(false)`, damit sich unter dem Thread nichts ändert) und startet
  `QtConcurrent::run`. Der Thread meldet den Fortschritt mit einem Toast („Saving 2 of 5 ...“).
  Wenn er fertig ist, läuft `savingFinished()` im GUI-Thread: Der gespeicherte Stand wird zum
  neuen Original (`m_originalMovies`), die Symbole werden grün, ein Toast meldet das Ergebnis.
  `closeEvent()` wartet auf einen laufenden Speichervorgang.
- **Zwei Wege** (Einstellung `General/SafeSave`, im Dialog „Safe saving“ mit Info-Symbol, dessen Sprechblase den Hinweis zeigt; gefragt beim ersten Start im Setupdialog und später in den Einstellungen):
  - **Direkt (Standard):** TagLib schreibt in die Datei selbst. Schnell, auch bei großen Dateien
    und auf Netzlaufwerken (bei einer 4,5-GB-Datei übers Netz Sekunden). Wird das Schreiben
    unterbrochen, kann die Datei beschädigt sein; die Hilfe sagt das.
  - **Sicher:** `Mp4Writer::save()` prüft den freien Platz, kopiert die Datei neben das Original
    (`name.tmp.mp4`; die Endung muss bleiben, der Leser erkennt das Format daran), schreibt dort
    und ersetzt erst nach der Prüfung das Original (es geht zuerst nach `.bak` und wird danach
    gelöscht). Bei jedem Fehler bleibt das Original unberührt. Der Preis ist die Kopie der ganzen
    Datei: lokal etwa die Zeit des Kopierens (4,5 GB: 12 s), übers Netz Minuten (4,5 GB: 5 min).
  - In beiden Wegen liest `verify()` die Datei danach mit `MediaInfoM` (Titel, Beschreibung,
    Altersfreigabe, Cover). Die Mediendaten prüft es nicht (bei GB-Dateien zu langsam).
- Geschrieben werden `©nam`, `ldes` und `desc` (auf 255 Zeichen gekürzt), `©gen`, `©day`
  (ISO), die freien Einträge `iTunEXTC` (`System|Code|Score|`) und `iTunMOVI` (Besetzung, Regie,
  Drehbuch, Produktion, Studio) sowie `covr`. `iTunMOVI` baut `buildItunMovi()` im Layout von
  Apple (Zeilenumbrüche und Tabs, so liest es unser Leser); `codirectors` des alten Eintrags
  bleiben. `©ART` und `aART` werden nicht angefasst. Ein unverändertes Cover wird nicht neu
  kodiert. Ein leerer Wert entfernt den Eintrag.
- Die Schlüssel mit dem ©-Zeichen müssen für TagLib UTF-8-Strings sein (`key("\xC2\xA9nam")`);
  als Latin-1 schreibt TagLib stillschweigend einen falschen Eintrag.
- **Beenden mit ungespeicherten Änderungen:** `closeEvent` bringt das Formular auf den Stand
  (`validateAndRefreshChanges()`), zählt die Filme mit Status „modified“ und fragt: „Alle
  speichern“ / „Verwerfen“ / „Abbrechen“ (Standard). „Alle speichern“ ignoriert das Schließen,
  speichert wie gewohnt im Thread und schließt in `savingFinished()` von selbst
  (`m_closeAfterSaving`), aber nur ohne Fehler.
- **Entfernen** (Knopf unter der Filmliste): Ist einer der gewählten Filme geändert, fragt der
  Dialog wie beim Beenden: „Speichern und entfernen“ / „Entfernen“ / „Abbrechen“ (Standard).
  „Speichern und entfernen“ speichert nur die geänderten Filme im Thread und merkt sich die Liste
  in `m_removeAfterSaving`; `savingFinished()` entfernt sie dann über `removeMovies()`, aber nur
  ohne Fehler.
- Offen: andere Formate als MP4.

### TagLib (Bibliothek)

TagLib ist eine fertige C++-Bibliothek zum Lesen und Schreiben von Audio- und Video-Tags. Wir
nutzen nur den MP4-Teil (`TagLib::MP4::File`, `Tag`, `Item`, `CoverArt`). Eine Bibliothek ist
kein Programm, das man startet: Ihr Quelltext wird übersetzt und in die App gebunden, die App
ruft ihre Funktionen auf wie die von Qt.

- **Version, Lizenz:** TagLib 2.3.2, LGPL 2.1 oder MPL 1.1 (wir verwenden die LGPL-Seite; sie ist
  mit unserer GPL v3 vereinbar). Zur Weitergabe der App gehören die Lizenztexte (`COPYING.LGPL`
  im TagLib-Archiv); die Hilfe nennt TagLib.
- **Wie sie in den Build kommt (`CMakeLists.txt`):** Weg „Download beim Bauen“, damit das
  Repository klein bleibt (TagLib liegt nicht in Git). `FetchContent_Declare(taglib ...)` nennt
  die feste Adresse des Release-Archivs
  `https://github.com/taglib/taglib/releases/download/v2.3.2/taglib-2.3.2.tar.gz` und seine
  SHA-256-Prüfsumme. Beim ersten CMake-Lauf lädt CMake das Archiv in den Build-Ordner
  (`build/.../_deps/taglib-src`), prüft die Summe und baut TagLib mit. Weitere Läufe laden nichts
  mehr, solange der Build-Ordner bleibt. Das **erste Bauen braucht also Netz**, die fertige App
  nicht: TagLib ist als statische Bibliothek in sie eingebaut. Bevor `FetchContent_MakeAvailable`
  läuft, schalten `BUILD_SHARED_LIBS`, `BUILD_TESTING`, `BUILD_EXAMPLES` und `BUILD_BINDINGS`
  auf `OFF`.
- **Include-Verzeichnisse:** TagLib setzt seine Include-Pfade nur für den eigenen Build (nicht an
  das Ziel `tag` gebunden). Deshalb liest `CMakeLists.txt` sie nach dem Einbinden aus
  (`get_directory_property(... INCLUDE_DIRECTORIES)`), fügt sie unserem Ziel hinzu und dazu das
  Verzeichnis mit `taglib_config.h`. Im Code heißen die Header deshalb flach
  (`#include <mp4file.h>`), nicht `<taglib/mp4file.h>`.
- **Gelinkt wird** das Ziel `tag`. TagLib nutzt zlib, wenn es vorhanden ist (optional).
- **Auf eine neue Version wechseln:** In `CMakeLists.txt` die Adresse (`v2.3.2` und den Dateinamen)
  und die Prüfsumme ändern. Die Summe berechnet man vom heruntergeladenen Archiv mit
  `shasum -a 256 taglib-x.y.z.tar.gz`. Danach CMake neu laufen lassen (bei einem alten
  Build-Ordner den Ordner `_deps` löschen).
- **Ohne Netz bauen:** Das entpackte TagLib-Verzeichnis angeben mit
  `-DFETCHCONTENT_SOURCE_DIR_TAGLIB=<Pfad>` (dann lädt CMake nichts), oder mit
  `-DFETCHCONTENT_FULLY_DISCONNECTED=ON`, wenn `_deps` schon gefüllt ist.
- **Versuche:** Mein Prototyp (TagLib bauen, in eine Kopie schreiben, wieder lesen, Mediendaten
  per Prüfsumme vergleichen, mit `moov` hinten und vorn) liegt in `_dev/` (nicht in Git). Er
  zeigt: TagLib ändert die Datei direkt und braucht dafür nur Millisekunden; wächst das `moov`
  über den vorhandenen Platz und liegt es vor den Mediendaten, verschiebt TagLib sie und führt
  die Offset-Tabellen (`stco`/`co64`) nach. Die lange Zeit des sicheren Wegs ist allein das Kopieren.
- **Threads:** TagLib selbst ist unkritisch. `Mp4Writer` benutzt keine GUI-Klassen (daher die
  `QImage`-Parameter statt `QPixmap`).

---

## 5. TMDB

- Der Token kommt zur Bauzeit aus `tmdb_token.txt`; `startApiRequest()` setzt ihn als
  Authorization-Header. Ein eigener Token aus den Einstellungen (`TMDB/Token`, Karte „TMDB“:
  verdecktes Feld mit Auge, darunter welcher Token gilt) hat Vorrang. Fehlen beide, zeigt die
  App „No TMDB token“ als Meldung.
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
- **Daten übernehmen** (Doppelklick auf ein Ergebnis, Tab „Data“): `onSearchTableViewDoubleClicked()`
  braucht einen ausgewählten Film (sonst „Select a movie first“) und schickt zwei Anfragen:
  - `/3/movie/{id}?append_to_response=credits,release_dates`: Titel, Beschreibung, Datum, Studio,
    Genre, Besetzung (die ersten 20), Regie, Drehbuch, Produktion und die Freigaben je Land
    (`fillDetailsTab()`).
  - `/3/movie/{id}/images?include_image_language=<Sprache>,en,null`: die Poster
    (`fillPosters()`, höchstens `kPosterLimit` = 30; Vorschaubilder `w185`, großes Poster `w342`,
    Cover beim Übernehmen `w780`). `m_detailsGeneration` verwirft Antworten eines älteren
    Doppelklicks. Eine neue Suche schaltet auf den Tab „Results“ zurück.
- **Apply** (`applyTmdbMovie()`) nimmt alles Gefundene in den ausgewählten Film (leere Werte ändern
  nichts), zeigt ihn neu und setzt den Status auf „geändert“. Das gewählte Poster lädt danach und
  wird als Cover gesetzt, falls noch derselbe Film ausgewählt ist. Gespeichert wird nicht. Mit der
  Einstellung `General/FillEmptyOnly` („Nur leere Felder füllen“, Standard aus) werden nur Felder
  gefüllt, die noch leer sind, auch Cover und Altersfreigabe; ein nie gesetztes Datum steht im
  Formular als 1972-01-01 und zählt als leer.
- **Altersfreigabe von TMDB:** `findTmdbRating()` nimmt das Land aus `ratingCountry()`: das System der
  **gespeicherten** Datei, solange der Film es noch hat, sonst das Land der Einstellungen (auch bei
  einer übernommenen oder gewählten, noch nicht gespeicherten Freigabe; beim Speichern der
  Einstellungen wird der Reiter „Daten“ neu aufgebaut; `UK` heißt bei TMDB `GB`) und dort den Eintrag nach „Heimvideo vor
  Kino“ (Typ 5, 4, 3, 2, 6, 1). `AgeRatings::findEntry()` ordnet den Text einer Stufe zu (erst
  `key`, dann `code`, ohne Groß-/Kleinschreibung und Leerzeichen). Passt nichts, bleibt die
  Freigabe unverändert. Die Texte von TMDB passen nicht in jedem Land zu unseren `key`; bei
  Abweichungen ist die Zuordnung zu ergänzen.
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

- **Sprache der App**: Einstellung `language` (Code wie `de`, `pt_BR`, `zh_CN`), gilt nach einem
  Neustart. Es gibt die Sprachen mit einer eingebauten `:/i18n/MovieMetaM_<Code>.qm` (Englisch
  immer, die Tabelle `kAppLanguages` in `languages.cpp` nennt die möglichen). Welche Sprache
  gilt, entscheidet `Languages::resolveAppLanguage()`: der Code selbst, sonst dieselbe
  Sprachfamilie (`pt_PT` → `pt_BR`, `zh_TW` → `zh_CN`), sonst Englisch. Dieselbe Funktion nutzen
  Übersetzer, Hilfe und Altersfreigaben. Eine **neue Sprache**: der Code in `APP_LANGUAGES` in
  `CMakeLists.txt` und in `kAppLanguages`, die Hilfetexte in `help/<Code>/` (`Help.qrc`;
  fehlt ein Thema, kommt der englische Text). Die Ländernamen der Altersfreigaben
  sind normale Übersetzungstexte (Kontext `Country`, der englische Name ist der Schlüssel; die Liste
  steht in der erzeugten `countrynames.cpp`, nachgeschlagen wird in `agerating.cpp`). Qt hat keine
  übersetzten Ländernamen, deshalb stehen sie in den `.ts`-Dateien.
  **Entwurfssprachen** (Französisch und Spanisch mit Hilfe, Italienisch, Portugiesisch (Brasilien),
  Chinesisch (vereinfacht), Japanisch, Koreanisch nur die Oberfläche): von Claude übersetzt, von keinem Muttersprachler geprüft;
  die Einträge der `.ts` behalten `type="unfinished"` (kommen trotzdem in die `.qm`). Die
  Hilfetexte benutzen die Begriffe der Oberfläche in fetter Schrift, sie müssen zusammenpassen.
  In der Sprachliste steht hinter einem Entwurf das Wort dafür in der eigenen Sprache (Feld `draft`
  in `kAppLanguages`), daneben erklärt ein Info-Symbol, was das heißt (Start-Dialog und
  Einstellungen). Ist eine Sprache geprüft: `unfinished` entfernen, `draft` auf `nullptr`.
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
  `{{qt}}`); die Sprache folgt dem Übersetzer. Überschriften stehen im `accentGrey()` (Anteil 0,6, mehr Kontrast als in der App), unter dem
  Titel steht eine Linie, ein Zitat (`> …`) wird zu einem Hinweiskasten (Tabelle mit Streifen in der
  Akzentfarbe, `quotesToNotes()`); bei Wechsel hell/dunkel wird der Text neu aufgebaut.
  Screenshots (PNG in `ressources/images/help/`, `Images.qrc`) sind englisch für beide Sprachen und
  Retina-Aufnahmen vom Mac; `scaleImages()` zeigt sie in halber Pixelgröße (`kScreenshotScale` in
  `helppage.h`). Keine Filmposter (Rechte): die Cover darin sind frei oder selbst gemacht. Bilder: Filmliste (Zustand),
  Filminfo (Namen mit ×, Cover mit × und Videogröße), Filmsuche (Posterliste).
- Meldungen im Log (`qDebug`, `qWarning`, `errorOccurred`) sind Englisch und werden nicht mit
  `tr()` übersetzt.
- **Qts eigene Texte** (Standardknöpfe „Ja“ / „Nein“ / „Abbrechen“, Dateidialog) stehen im Katalog
  `qtbase_<Sprache>.qm`. `CMakeLists.txt` bettet die Kataloge der App-Sprachen aus dem
  Übersetzungsordner von Qt als `:/i18n/qtbase_<Sprache>.qm` in die App ein; `main.cpp` lädt
  ihn von dort, sonst aus `QLibraryInfo::TranslationsPath`. So hat jedes Paket (Mac-Bundle,
  später Windows / Linux) die Texte, ohne dass ein Pfad im Bundle stimmen muss.

---

## 8. Einstellungen (QSettings)

`QSettings()` mit Organisation „SpeckenbachNet“, auf jedem System im INI-Format
(`QSettings::setDefaultFormat` in `main.cpp`).

| Schlüssel | Bedeutung |
|---|---|
| `language` | Sprache der App (Code, z. B. `de`) |
| `General/DataLanguage` | Sprache der Filmdaten für TMDB (z. B. `de-DE`) |
| `General/SafeSave` | `true`: beim Speichern erst in eine Kopie schreiben (sicher, aber langsam); Standard `false` |
| `General/LastFolder` | Ordner, aus dem zuletzt Filme hinzugefügt wurden (Öffnen-Dialog und Ablegen); der Dialog startet dort, sonst im Ordner „Filme“ des Benutzers |
| `General/FillEmptyOnly` | `true`: „Übernehmen“ füllt nur leere Felder (auch Cover und Altersfreigabe); Standard `false` |
| `General/CustomGenres` | eigene Genres (Liste), stehen in der Genre-Auswahl der Filminfo zuerst |
| `General/OnlyCustomGenres` | `true`: in der Genre-Auswahl nur die eigenen Genres, ohne die von TMDB; Standard `false` |
| `TMDB/Token` | eigener TMDB-Token („API Read Access Token“); leer bzw. fehlend: der eingebaute `TMDB_TOKEN`. Im Klartext in der INI |
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
- **Icons:** eigene schwarze Liniezeichnungen (24 × 24, Strich 2, runde Enden), in der Textfarbe
  getönt (ToolbarM, SearchField). Herkunft und Lizenz: `ressources/icons/LICENSES.md`. Die
  Stammkopie samt Regeln für neue Icons liegt in `../libraries/icons/`.
- **Gezeichnete Bilder** (Statussymbole, Poster, Abzeichen, Cover) in der Auflösung des
  Bildschirms (`devicePixelRatioF()`), sonst sind sie unscharf. Icons in ausgewählten Einträgen
  bekommen dasselbe Bild für `QIcon::Selected`.
- **Listen** als `QListView` mit einer Spalte (eine `QTableView` kann keine ganze Zeile runden).
  Die Einträge zeichnet `TwoLineDelegate`: Icon, Titel, kleinere gedimmte zweite Zeile (Text
  „Titel\nzweite Zeile“), Abstand 7/6 px, dünne Trennlinie.
- **Tooltips:** `TooltipFilter` (`bubble.cpp`, ein Filter auf `qApp`) fängt `QEvent::ToolTip` jedes
  Widgets ab, holt den Text (bei Listen den `ToolTipRole` des Eintrags unter der Maus) und zeigt
  eine `Bubble` unter dem Mauszeiger statt des Systemtooltips. Sie geht bei Mausbewegung (mehr
  als 24 px), Klick, Taste, Rad, Verlassen des Widgets und nach 12 s. Formatierung wird entfernt
  (reiner Text). Die Klasse `ToolbarM` aus `../libraries` bleibt unverändert.
- **Meldungen** als Toast (`ToastM`): kurz über der Spalte, zu der sie gehören, mit sanftem Ein-
  und Ausblenden; Klicks gehen hindurch. `MainWin::showColumnStatus(StatusColumn, Text, Level,
  Dauer)`; Hinweise nach der Länge des Textes (1 s + 35 ms je Zeichen, 1,5 bis 4 s), Fehler 5 s. Eine Statusleiste gibt es nicht.
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
- TagLib 2.3.2 (schreibt die Tags): LGPL 2.1 oder MPL 1.1, wird statisch eingebaut. Die Lizenztexte
  (`COPYING.LGPL`) gehören zu jeder Weitergabe der App; die Hilfe nennt TagLib.
- Icons: alle eigene Zeichnungen, GPL v3 (`ressources/icons/LICENSES.md`).
- TMDB: Logo (`ressources/images/tmdb.svg`) und Hinweis laut den Bedingungen von TMDB.
- Ressourcen: `Icons.qrc`, `Images.qrc`, `Ratings.qrc`, `Help.qrc`.

---

## 11. Versionen, Commits, Veröffentlichung

- Die **Version setzt der Entwickler** in `CMakeLists.txt` (`PROJECT_VERSION`).
- Commit-Nachricht: `0.x.y Kurzer Titel` und darunter eine Liste der Änderungen.
- Eine Version gilt für die App auf allen Systemen. Eine Änderung am Code der App erhöht sie, eine
  Änderung nur an Bau- oder Paketskripten nicht (Commit-Titel dann ohne Nummer).
- **Veröffentlichung (geplant, noch nicht gemacht):** ein Tag `vX.Y.Z` auf GitHub mit den
  Paketen aller Systeme, die es dann gibt. Die Pakete hätten feste Namen ohne Version, damit die
  Website auf `.../releases/latest/download/<Name>` verlinken kann.

### Gemeinsam für alle Pakete

- **Version:** beide Skripte lesen sie aus `CMakeLists.txt`.
- **Token:** `tmdb_token.txt` (Abschnitt 2) wird beim Konfigurieren in `TMDB_TOKEN` eingebaut und
  steckt in der Binary; er liegt nicht im Repository. Fehlt er, läuft die App, aber die Suche
  nicht. Das Mac-Skript warnt nur, das Windows-Skript bricht ab.
- **Fest eingebaut** (kein Pfad im Paket muss stimmen): TagLib (statisch; wird beim ersten
  Konfigurieren per FetchContent geladen, braucht also einmal Netz), die Übersetzungen, Qts eigene
  Texte (`:/i18n/qtbase_<Sprache>.qm`), die Hilfe, Icons und Altersfreigaben als Ressourcen.
  Daneben liegen im Paket nur Qt-Bibliotheken und Plugins.
- **Einstellungen** (QSettings, Format INI auf jedem System) liegen außerhalb des Pakets und
  bleiben bei einem Update oder einer Deinstallation erhalten.
- Keines der Pakete ist bisher per Download geprüft (siehe die Hinweise unten).

### Mac

**Bauen:** `tools/build-mac.sh` (von überall aufrufbar), Ergebnis `build/MovieMetaM-mac.dmg` (fester Name ohne Version).
Voraussetzungen: Qt 6.12 in `~/Qt/6.12.0/macos` (anderes Qt:
`QT_DIR=/pfad/zu/Qt/6.x/macos tools/build-mac.sh`), CMake und Ninja aus der Qt-Installation
(`~/Qt/Tools`), `tmdb_token.txt`.

**Ablauf:**

1. Release-Build nach `build/release`, universal (arm64 + x86_64).
2. `macdeployqt` packt Qt-Frameworks und Plugins ins Bundle. Es nimmt auch das Plugin der
   virtuellen (Touch-)Tastatur mit, das QtQuick, QtQml und mehr nachzieht (etwa 27 MB). Schritt 2b
   entfernt, was eine Widgets-App nicht braucht (`QtOpenGL`, `QtQml*`, `QtQuick`,
   `QtVirtualKeyboard*`, `platforminputcontexts`) und bricht ab, wenn noch etwas im Bundle darauf
   zeigt (sonst startete die App nicht).
3. Ad-hoc-Signatur (`codesign --sign -`), danach die Prüfung `codesign --verify`.
4. DMG mit der App und einem Link auf „Programme“.

**Signatur:** ad hoc, ohne Developer ID und ohne Notarisierung (bewusst, das Programm kostet Geld).

**Hinweis für Benutzer, erster Start:**
- *Beobachtet:* Installation und Start ohne Probleme, mit dem selbst gebauten oder per Sync
  kopierten DMG (nicht heruntergeladen).
- *Ungeprüft:* ein aus dem Internet geladenes DMG (GitHub-Release). Dafür wird die Warnung von
  macOS erwartet; dann Rechtsklick auf die App → „Öffnen“ oder Systemeinstellungen → Datenschutz &
  Sicherheit. Beim ersten Release prüfen und diesen Absatz auf den echten Stand bringen.

### Windows

**Bauen:** Doppelklick auf `tools\build-windows.cmd` (oder `tools\build-windows` in der Konsole),
Ergebnis `build\MovieMetaM-windows-Setup.exe` (fester Name ohne Version). Die .cmd startet das PowerShell-Skript
`tools\build-windows.ps1` mit `-ExecutionPolicy Bypass`, weil Windows unsignierte Skripte sonst nicht
ausführt; das Fenster bleibt am Ende offen. Vorbild ist PhotoBoothM.
Voraussetzungen: Visual Studio 2022 Build Tools (MSVC, das Skript findet sie mit `vswhere`), Qt 6.12
für MSVC in `C:\Qt\6.12.0\msvc2022_64`, Qt Tools (CMake, Ninja) in `C:\Qt\Tools`, **Inno Setup 6**
(kostenlos, `winget install JRSoftware.InnoSetup`), `tmdb_token.txt`. Anderes Qt: `-QtDir
C:\Qt\6.x\msvc2022_64`, andere Tools: `-QtTools`.

**Ablauf:**

1. Release-Build mit MSVC und Ninja nach `build\windows-release` (bricht ab, wenn `tmdb_token.txt`
   fehlt oder leer ist: sonst ginge die Suche im Installer nicht).
2. `windeployqt` (`--release --no-compiler-runtime --no-system-d3d-compiler --no-opengl-sw
   --no-translations`) sammelt Qt-DLLs und Plugins (TLS über Schannel für TMDB, SVG, Bildformate)
   nach `build\windows-deploy`. Qts Übersetzungen sind in die exe eingebaut, die VC-Laufzeit
   bringt der Installer. Der Ordner läuft auch ohne Installation.
3. Inno Setup macht daraus den Installer (`tools\MovieMetaM.iss`; das Skript übergibt
   `AppVersion`, `SourceDir` und, wenn gefunden, `Redist`).

**Was der Installer tut:**

- Sprache des Systems (en, de, fr, es, it, pt_BR, ja, ko); passt keine, erscheint ein Auswahldialog
  (Englisch steht an erster Stelle).
- Nur 64 Bit, ab Windows 10; installiert nach `C:\Program Files\MovieMetaM` (braucht
  Administrator), Startmenü-Eintrag, Desktop-Symbol auf Wunsch.
- Die Visual-C++-Laufzeit (`vc_redist.x64.exe` aus der Visual-Studio-Installation) steckt im
  Installer und wird still nur installiert, wenn die vorhandene fehlt oder älter als 14.40 ist.
- Lizenz (GPL v3) als eigene Seite; `LICENSE.txt` und `LGPL-3.0.txt` liegen neben der exe.
- Die feste `AppId` sorgt dafür, dass eine neue Version die alte ersetzt; eine laufende App wird
  dafür geschlossen (sie fragt selbst nach ungespeicherten Änderungen).
- Deinstallieren entfernt die Programmdateien und die Registry-Einträge (unten); die Einstellungen
  (INI in `%APPDATA%`) bleiben.

**Registry („Öffnen mit“ für `.mp4` / `.m4v`):** Der Installer macht MovieMetaM nicht zum
Standardprogramm, der Videoplayer bleibt es. Die Einträge stehen unter `Software\Classes` (`HKA`,
bei dieser Administrator-Installation also für alle Benutzer):

| Schlüssel | Zweck |
|---|---|
| `MovieMetaM.Movie` (`DefaultIcon`, `shell\open\command`) | die ProgID: Name „MP4 Video“, Icon, Befehl `MovieMetaM.exe "%1"` |
| `.mp4\OpenWithProgids`, `.m4v\OpenWithProgids` | nehmen die ProgID in die „Öffnen mit“-Liste dieser Typen auf |
| `Applications\MovieMetaM.exe` (`shell\open\command`, `SupportedTypes`) | die App selbst: Anzeigename, Befehl, unterstützte Endungen |

**Signatur:** nicht signiert.

**Hinweis für Benutzer, erster Start:**
- *Beobachtet:* Installation und Start ohne Probleme, mit dem selbst gebauten Installer (nicht
  heruntergeladen).
- *Ungeprüft:* ein aus dem Internet geladener Installer (GitHub-Release). Dafür wird die Warnung
  von Windows SmartScreen erwartet; dann „Weitere Informationen“ → „Trotzdem ausführen“. Beim
  ersten Release prüfen und diesen Absatz auf den echten Stand bringen.

### Linux

**Bauen:** `tools/build-linux.sh` (von überall aufrufbar), Ergebnis
`build/MovieMetaM-linux.AppImage` (etwa 34 MB, fester Name ohne Version). Voraussetzungen:
nur **Podman** (ohne root) und beim ersten Lauf Netz. Qt und die Werkzeuge liegen danach in
`build/linux-cache` (etwa 1,8 GB, löschen = neu laden).

**Warum im Container:** Ein Programm läuft nur auf Systemen mit der gleichen oder einer neueren
glibc. Ein Build auf einem neuen System (Arch) startete auf Ubuntu LTS oder Debian nicht. Darum baut
das Skript in einem Container mit **Ubuntu 22.04** (glibc 2.35); das Paket verlangt höchstens
glibc 2.34.

**Ablauf** (im Container, das Skript startet sich dafür selbst mit `--inside`):

1. Compiler, Ninja und die Systembibliotheken, gegen die Qts Plugins linken, per `apt`.
2. **Qt 6.12** mit `aqtinstall` (Ubuntu 22.04 hat kein Qt 6.12), in den Cache. **CMake aus `pip`**,
   weil Qt 6.12 CMake ab 3.25 verlangt und Ubuntu 22.04 nur 3.22 hat.
3. linuxdeploy samt Qt- und AppImage-Plugin (von GitHub, `continuous`, in den Cache).
4. Release-Build nach `build/linux-release`.
5. `cmake --install` in `build/linux-appdir`. Was TagLibs Installationsregeln danebenlegen
   (Header, `taglib-config`), wird gelöscht; TagLib ist statisch eingebunden.
6. linuxdeploy kopiert Qt hinein (`--plugin qt`), mit den Wayland-Plugins (`libqwayland.so`; in Qt
   6.12 ein Plugin, vorher `-generic` und `-egl`). Dazu von Hand: das Plattform-Theme
   `libqxdgdesktopportal.so` (der Dateidialog des Desktops; `DEPLOY_PLATFORM_THEMES` nähme auch das
   GTK-Theme und damit ganz GTK) und `libOpenGL.so.0` (GLVND; linuxdeploy lässt sie weg, auf einem
   schlanken System ohne das Paket `libopengl0` startete die App nicht). Danach der AppImage-Schritt.

**Nicht im Paket** (kommt vom System): glibc, libstdc++, die Treiber (libGL, libEGL), X11 und
Wayland, fontconfig, freetype, D-Bus, `libssl.so.3` (Qt lädt es zur Laufzeit für HTTPS zu TMDB;
vorhanden ab Ubuntu 22.04 und Debian 12).

**Getestet:** gebaut (0.9.3); auf dem Entwicklungsrechner (Arch, Plasma, Wayland) gestartet, volle
Oberfläche mit Icons und Schrift, richtiger Desktop-Name. In einem Debian-12-Container (glibc 2.36,
`xcb` an Xvfb, ohne `libopengl0`) startet die App und läuft ohne Fehlermeldung. **Nicht getestet:**
Dateidialog, TMDB-Suche und Drag & Drop aus dem AppImage, niri, GNOME und ein echter älterer
Desktop (eine Debian-12-VM wäre dafür da).

**Hinweis für Benutzer:** Datei ausführbar machen (`chmod +x`) und starten. AppImages brauchen
FUSE 2 (Paket `libfuse2`, auf neueren Ubuntu-Versionen `libfuse2t64`); fehlt es, geht
`./MovieMetaM-linux.AppImage --appimage-extract-and-run`. (Allgemein bei AppImages, mit
unserer Datei nicht geprüft.) Das AppImage trägt sich nicht selbst ins Startmenü ein.

Weiter geplant: Flatpak. Ein PKGBUILD für das AUR hilft nur Arch-Benutzern, .deb / .rpm sind nicht
geplant (Punkt 15 in `CLAUDE.md`).

### Werkzeuge

| Werkzeug | Zweck |
|---|---|
| `tools/build-mac.sh` | baut das Mac-Paket (DMG), siehe „Mac“ |
| `tools/build-windows.cmd`, `tools/build-windows.ps1` | baut den Windows-Installer, siehe „Windows“ |
| `tools/build-linux.sh` | baut das Linux-AppImage im Podman-Container, siehe „Linux“ |
| `tools/MovieMetaM.iss` | Skript von Inno Setup für den Windows-Installer |
| `tools/ratings_from_xlsx.py` | erzeugt die JSON-Dateien, `Ratings.qrc` und `countrynames.cpp` aus der Tabelle (Abschnitt 6) |
| `_dev/` (nicht im Repository) | Arbeitsmaterial zu den Altersfreigaben, Bildschirmfotos |
