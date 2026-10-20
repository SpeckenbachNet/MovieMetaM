# MovieMetaM

Editor for the metadata of movie files (Qt 6 Widgets, C++17, CMake): reads title, cast,
directors, description, genre, age rating and cover from MP4 files, finds movie data on TMDB.
Developer: Michael Speckenbach (SpeckenbachNet). Repository: github.com/SpeckenbachNet/MovieMetaM.

Developer documentation (German, after the model of PhotoBoothM): `docs/DEVELOPMENT.md` – keep it
up to date with larger changes.

## Working with the user

- The user writes **German**; answer in German. Code, comments and `tr()` source texts are
  **English** (German in `i18n/MovieMetaM_de.ts`).
- **Change nothing without the word "GO"** (files, commits, pushes). Discuss topics first, then
  propose the concrete change (files, what, why) and wait. Agreement ("ja", "passt") is not GO.
  Reading, searching and building for analysis are fine. Go step by step, one topic at a time.
- The **user sets the version** in `CMakeLists.txt` (`PROJECT_VERSION`); check it before
  committing. Commit message: `0.x.y Short German title` + bullet list, then the
  Co-Authored-By line. Commit only the files of the topic.
- Report faithfully what was tested (build only / app not looked at).

## Building

- Qt 6 only (6.12 on Linux), modules Widgets, Network, Svg, LinguistTools.
- **`tmdb_token.txt`** (project root, not in git, one line: TMDB API read access token) is read
  by CMake into the define `TMDB_TOKEN`. Without it the app runs but the search does not.
  After creating the file the first time: re-run CMake.
- Defines: `APP_NAME`, `APP_VERSION`, `COMPANY_NAME`. Settings: `QSettings()` with organization
  "SpeckenbachNet", INI format on every system.
- Windows: `app.rc.in` (icon `ressources/icons/app_icon.ico` + version info); macOS: bundle icon
  `app_icon.icns`.
- Only MP4/M4V are supported for now (`MediaInfoM::supportedSuffixes()`); other files are
  skipped with a message in the status bar.

## Translations

English source texts in `tr()` and `mainwin.ui` (like PhotoBoothM). `MovieMetaM_de.ts` must stay
complete (0 unfinished); `MovieMetaM_en.ts` only fills `<numerusform>` for `%n` texts.
Non-translatable ui strings get `notr="true"`. After changing texts: lupdate with
`-no-obsolete` (or target `update_translations`), fill every unfinished German entry. The .qm
files are built and embedded by `qt_add_translations` (`:/i18n`) and are not in git.
Age rating labels like "ab 12 Jahren" are official texts written into the files: not translated.

## Shared classes (`../libraries`)

`~/Development/QT/libraries/` holds versioned master copies (not in git): `toolbarm/` (ToolbarM
1.5.0) and `searchfield/` (SearchField 1.2.0), each with CHANGELOG.md and README.md. Projects
keep a copy: change the master copy first, raise the version (CHANGELOG, `...Version` constant,
header comment), then copy. Other projects (churchWorshipM, PhotoBoothM, template,
PDFDocumentScannerM) are updated later by the user – don't touch them unasked.
`~/Development/QT/libraries` is a link to `~/kDrive/Development/QT/libraries` (synced, Mac and
Linux).

## Style of the UI

- Colors from the palette (light and dark themes), no fixed colors where they meet theme colors.
- Accent color of the system (`palette(highlight)` + `highlighted-text`) for selected entries
  (movie list, search results, open combo list) and for tags; headings in `accentGrey()`.
- Everything rounded, radius 6 (`FieldLook::radius`): helpers in `themecolors.h` –
  `applyInputStyle`, `applySpinStyle`, `applyBoxStyle`, `applyComboStyle`, `applyTabStyle`.
  Combo boxes get a plain `QStyledItemDelegate` (the default menu delegate ignores the style
  sheet item rules); the combo arrow is the grey `chevron_down.svg` (not tinted).
- Icons: black SVG drawings tinted in the text color (ToolbarM, SearchField); origin and
  licenses in `ressources/icons/LICENSES.md` (save icons from Lucide, ISC).
- Painted pictures (status symbols, posters, age rating badges, cover) in the resolution of
  the screen (`devicePixelRatioF()`, e.g. 1.25 on Linux, 2 on Retina), else they blur.
  Icons shown in selected entries get the same pixmap for `QIcon::Selected` (Qt tints them).
- Item views as `QListView` with one column (a `QTableView` cannot round a whole row; Breeze
  draws its own square selection behind it). Entries drawn by `TwoLineDelegate`: icon, title,
  smaller dimmed second line (model text "title\nsecond line"), padding 7/6 px, 2 px gap, thin
  separator line from the text start (none next to the selection – the list repaints on every
  selection change).
- Desktop reference: `../churchWorshipM` (ToolbarM per page, SearchField); style reference:
  `../PhotoBoothM` (tablet-first, so not 1:1).
- Every page of `stackedWidget` has its own ToolbarM on top; `showPage()` switches pages.
- Donation: coffee button in the main toolbar, icon only, orange icon on a normal button
  (`setButtonColor(btn, QColor(), "#F5A623")`), tooltip "Buy me a coffee – …", opens
  https://paypal.me/michaelspeckenbach; the same link is in the help ("About MovieMetaM" →
  Support).

## Notes (Linux: niri, Wayland, scaling 1.25)

- Drag and drop on Wayland: read the mime data in `dragEnterEvent`. Read first in `dropEvent`,
  data from a browser (Brave/Chromium) is empty (see `ImageLabelM::readDragData`).
- Qt debug output of an app started outside a terminal goes to the journal:
  `journalctl _PID=<pid>`.
- Close test instances like the window's close button (`niri msg --json windows` for the id,
  then `niri msg action close-window --id <id>`), not with `kill`; never kill by name.

## Structure

- `main.cpp` – settings, translator, setup dialog on first start, main window.
- `mainwin.*`, `initialize.cpp` – main window: movie list (left), movie info (middle),
  TMDB search (right, tabs Results / Data); pages settings and help. Settings page:
  cards in a centered column, so far one card "General" (`settingsGeneralCard`, styled in
  `initializeForm()`).
  No status bar: messages are toasts (`showColumnStatus(StatusColumn, text, level, msec)` →
  `ToastM::show`), centered over the column they belong to (`StatusColumn::Window`: over the
  whole window); Info 3 s, Error 5 s.
- `helppage.*` – help page (own ToolbarM, topics list, Markdown text): texts in
  `help/<de|en>/<topic>.md` (`Help.qrc`), placeholders `{{version}}`, `{{qt}}`; language like
  the translator. For now one topic "About MovieMetaM" (website
  software.speckenbach.net/moviemetam, TMDB notice with logo).
- `mediainfom.*` – reads MP4 boxes (metadata, iTunMOVI XML, cover); `moviedata.*` – data of a movie.
- `tagwidget.*` – tag input (cast, directors, ...); `imagelabelm.*` – cover: paints itself
  (rounded, sharp), drag and drop in (`coverDropped` / `coverUrlDropped`) and out;
  `tableviewm.*` – movie list (`QListView`) with file drop; `setupdialog.*` – first start.
- `languages.*` – two languages (decided with the user): the app (German / English, setting
  `language`, takes effect after a restart) and the movie data for TMDB (genre list and search,
  e.g. `ja-JP`, setting `General/DataLanguage`, takes effect at once). Both follow the system
  until chosen in the setup dialog or the settings (a Japanese user gets the English app with
  Japanese data). The data languages are TMDB's primary translations, names from `QLocale`.
- `twolinedelegate.*` – entries of movie list and search results; `posterpreview.*` – enlarged
  poster (200×300) when the mouse rests on a search result (full poster in `kPosterRole`).
- TMDB data (double click on a result, tab "Data"): `onSearchTableViewDoubleClicked` loads
  `/movie/{id}` (`append_to_response=credits,release_dates`) and `/movie/{id}/images`;
  `fillDetailsTab`, `fillPosters`; "Apply" (`applyTmdbMovie`) takes everything found into the
  selected movie (empty values change nothing). `posterpicker.*` – the large poster with a badge
  "n / m"; a click opens a list (`TwoLineDelegate`) of up to 30 posters (language, size,
  rating). Age rating: `findTmdbRating` takes the entry of the country of the movie's system
  (home video before cinema) and `AgeRatings::findEntry` matches it to a step (key, then code);
  TMDB's texts may not fit every country – extend when a country does not match.
- Saving (`Mp4Writer`, TagLib 2.3.2 via FetchContent, static, LGPL 2.1; details in
  `docs/DEVELOPMENT.md`): "Save" / "Save all" → `startSaving()` runs `QtConcurrent` (the window is
  locked meanwhile; `SaveJob`s carry covers as `QImage`, no `QPixmap` in the thread) →
  `savingFinished()`. Default: TagLib writes directly into the file (fast, also on network
  drives); setting `General/SafeSave` = copy first (`name.tmp.mp4`, the suffix matters for the
  reader), check, replace (original to `.bak` first) – slow for big files (4.5 GB over the network:
  5 min). Keys with (c) are UTF-8 strings in TagLib. `iTunMOVI` is built in Apple's layout (the
  reader needs it); `codirectors` are kept; `(c)ART` / `aART` are not touched; an unchanged cover
  is not written again. Not done: a warning when closing with unsaved changes.
- `bubble.*` – `Bubble`: rounded note window (colors of the fields, mouse events go through);
  `TooltipFilter` (installed in `main.cpp`) shows ALL tooltips of the app as a `Bubble` below the
  pointer instead of the system's square one (item views: `ToolTipRole` of the entry under the mouse).
- `infoicon.*` – small info icon (tinted in the text color), a rounded bubble (window of its own) above it on hover
  and on click, not the system tooltip; used for
  the saving note (`InfoIcon::savingNote()`) beside the check box "Safe saving" in the setup dialog
  and the settings page.
- Content advisory (`rtng`: 0 none, 2 clean, 1/4 explicit) is an editable combo box; the video
  size (`MediaInfoM::readVideoSize`, `tkhd` of the video track) is shown centered under the cover (chip "1080p" in
  the tag look, size beside it; `formatRow` keeps its room when hidden) and sets
  the HD flag `hdvd` when saving (`MovieData::hdFlag`, by the width).
- `toastm.*` – short message over the content, fades in and out, mouse clicks go through; one
  per window, a new message replaces the old one; position Center / Top / Bottom inside a target
  widget, level Info / Error. May move to `../libraries` once it has proved itself.
- `themecolors.h` – shared colors and shapes (`FieldLook`, `apply…Style`).
- `agerating.*` – age rating systems as data: one JSON file per country
  (`ressources/ratings/<code>.json`, `Ratings.qrc`; country name en/de, system, id, steps with
  key = badge text, code = written into the file, score, label = combo text, description =
  tooltip, color, shape, style). `AgeRatings::systems()` feeds the settings (with a preview of
  the badges), the setup dialog and the rating combo box. Badges: `createAgeRatingIcon()` –
  shapes square, circle, diamond, triangle, octagon, hexagon, star; styles filled, outline
  (white, border and code in the color: JP, SK, TR), light (white, black border, code in the
  color: US, IS), dark (#313030, code in the color: FI); the font shrinks until the
  code fits. **The JSON files and `Ratings.qrc` are generated, not edited by hand:** change
  the table, then `python3 tools/ratings_from_xlsx.py ~/kDrive/Development/QT/age_ratings.xlsx`
  (needs openpyxl). 46 countries, 266 steps (as of 0.4.0).
  Principle (decided with the user): all players read the one entry `iTunEXTC`
  (`prefix|code|score|reason|`). Code = the form media servers know (Jellyfin lists
  `Emby.Server.Implementations/Localization/Ratings/*.json` as reference, e.g. DE "FSK 12",
  not Apple's "Ab 12 Jahren"); prefix and score as Apple/Subler (`Resources/Ratings.json`);
  home video before cinema where a country has its own system (Canada: CHVRS). Working table:
  `~/kDrive/Development/QT/age_ratings.xlsx` (not in git) – the JSON files follow it.
  Showing a movie (`showMovieRating()`, decided with the user): the system in the file
  (`iTunEXTC` prefix, `AgeRatings::countryOfSystem()`) decides which steps the combo shows; the
  system of the settings only applies to movies without an age rating. Selected by code, else by
  score (older Apple texts). An unknown system or step stays as it is: an extra entry
  "prefix: code" below the empty one, selected, no "changed". "Changed" = system or score differ.
  Saving the settings applies the new system at once (current movie shown again).
- `toolbarm.*`, `searchfield.*` – copies from `../libraries`.

## Open points (as of 0.6.3; numbers kept, 1, 2, 3, 4 and 5 are done)

6. Tags: one click removes a tag without asking – tooltip "Click to remove" or an × on hover.
7. The old TMDB token is in the git history: revoke it at TMDB and use a new one.
8. Origin of some icons still to be clarified (`LICENSES.md`); unused
   `ressources/images/statusgreen.svg` / `statusorange.svg` can go.
9. Mac: check the age rating icon in the closed combo box with the new combo style (before:
   about 2 px too high with the native style; the transparent-stripe fix was reverted).
10. Help topics (using the app, TMDB search, saving): write them last, when saving (4) and
    applying a search result (5) work.
