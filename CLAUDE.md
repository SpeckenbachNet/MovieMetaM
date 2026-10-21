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
  `app_icon.icns`; the bundle's `Info.plist` is `ressources/macos/Info.plist.in` (Qt's template plus
  `CFBundleLocalizations` de/en: without it the native Open dialog stays English).
- Only MP4/M4V are supported for now (`MediaInfoM::supportedSuffixes()`); other files are
  skipped with a message in the status bar.

## Translations

English source texts in `tr()` and `mainwin.ui` (like PhotoBoothM). `MovieMetaM_de.ts` must stay
complete (0 unfinished); `MovieMetaM_en.ts` only fills `<numerusform>` for `%n` texts.
Non-translatable ui strings get `notr="true"`. After changing texts: lupdate with
`-no-obsolete` (or target `update_translations`), fill every unfinished German entry. The .qm
files are built and embedded by `qt_add_translations` (`:/i18n`) and are not in git.
Qt's own texts (standard buttons, file dialog) come from `qtbase_<lang>.qm`, loaded in `main.cpp`
from `QLibraryInfo::TranslationsPath` in the app language (to ship with a bundle/installer: open).
Age rating labels like "ab 12 Jahren" are official texts written into the files: not translated.
Other languages (fr, es, it, pt_BR, zh_CN, ja, ko): drafts by Claude, **not checked by a native speaker** – their entries keep
`type="unfinished"` (lrelease still puts them into the .qm), the help texts address the reader
with "vous" etc. The drafts are marked in the language list with the word for "draft" in their own language
(`draft` field in `kAppLanguages`), and an `InfoIcon` (`InfoIcon::draftLanguagesNote()`) beside the
language combo box (setup dialog, settings) says what that means (later: link to the website). When a
draft has been checked: remove the `unfinished` marks, set its `draft` to `nullptr` and note it here.
Checked so far: none.
After changing texts, new entries of the draft languages stay empty (English) until translated.
The texts in `mainwin.ui` and the help name UI items in bold ("**Save**"): the French and Spanish help use
their UI terms, keep them in step. Country names of the age ratings: translated in the .ts files (context "Country", the English
name is the key; list in the generated `countrynames.cpp`, looked up in `agerating.cpp`).

## Shared classes (`../libraries`)

`~/Development/QT/libraries/` holds versioned master copies (not in git): `toolbarm/` (ToolbarM
1.6.0) and `searchfield/` (SearchField 1.2.0), each with CHANGELOG.md and README.md. Projects
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
- Icons: own black line drawings (24 × 24, stroke 2, round caps and joins) tinted in the text
  color (ToolbarM, SearchField); origin and license (all own, GPL v3) in
  `ressources/icons/LICENSES.md`. Master copy and rules for new icons (Inkscape: unit px, scale 1;
  never open the project's files directly, work on a copy): `../libraries/icons/`.
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
  `initializeForm()`; rows: language, data language, age rating, preview, saving, own genres
  (`TagWidget`, `General/CustomGenres`) and the check box `General/OnlyCustomGenres`; the genre
  box of the movie info is built by `fillGenreList()`: own genres first, then TMDB's, or only the
  own ones; Apply still takes TMDB's genre).
  No status bar: messages are toasts (`showColumnStatus(StatusColumn, text, level, msec)` →
  `ToastM::show`), centered over the column they belong to (`StatusColumn::Window`: over the
  whole window); Info by the length of the text
  (1 s + 35 ms per character, 1.5 to 4 s), Error 5 s.
- `helppage.*` – help page (own ToolbarM, topics list, Markdown text): texts in
  `help/<de|en>/<topic>.md` (`Help.qrc`), placeholders `{{version}}`, `{{qt}}`; language like
  the translator. Topics follow the screen from left to right (`enum Topic`, the help opens
  with the first), "About MovieMetaM" last (website software.speckenbach.net/moviemetam, TMDB
  notice with logo). Texts address the reader with "du". So far: Movie List, Movie Info, Movie Search, Saving, Settings, About.
  Look (`HelpPage::spaceOut()`): headings in `accentGrey(pal, 0.6)`, a rule under the title (inserted in
  `loadText()`), a quote (`> ...`) becomes a note: a two-cell table (stripe in the accent color +
  box a shade away from the field color; `quotesToNotes()`, rounded corners are not possible in
  `QTextDocument`). The page is built again on a palette change (light / dark).
  Screenshots (PNG, `ressources/images/help/`, `Images.qrc`, English for both languages, Retina
  shots: `scaleImages()` shows them in half the pixels, `kScreenshotScale`): list state, names with ×,
  poster list, cover with × and video size. Own pictures only, no film posters (rights); the covers in them are drawn/free.
- `mediainfom.*` – reads MP4 boxes (metadata, iTunMOVI XML, cover); `moviedata.*` – data of a movie.
- `tagwidget.*` – tag input (cast, directors, ...; a name longer than the list is elided with "…",
  no horizontal scroll bar: `TagDelegate::sizeHint` has only a height, so the × stays in sight; the
  tooltip of an elided tag has the full name, `TagModel`): a × on hover (`TagDelegate` inside
  `tagwidget.cpp`) removes a tag, also Delete/Backspace on the selected one
  (afterwards – also after dragging – no tag is selected; Escape, a click outside the list – also on an
  empty area, app-wide event filter – or leaving it with Tab unselects); a click on the text
  selects or drags to reorder; `imagelabelm.*` – cover: paints itself
  (rounded, sharp), drag and drop in (`coverDropped` / `coverUrlDropped`) and out; removing
  (`coverRemoveRequested`): a round × in the corner on hover, or click (selected = focused, accent
  frame) and Delete / Backspace; Escape, Tab or a click outside unselects;
  `tableviewm.*` – movie list (`QListView`) with file drop (a dropped folder is expanded in
  `addFilesToModel`: MP4/M4V with subfolders, natural order, hidden files such as `._x.mp4` left out
  – `QDirIterator` lets them through, so the names are checked by hand; the remembered folder
  `General/LastFolder` is the dropped one); Delete / Backspace in the list remove the selected
  movies (`removeRequested` → `onRemoveFilesButtonClicked`, like the button −; Ctrl/Cmd+A selects all); `setupdialog.*` – first start.
- `languages.*` – two languages (decided with the user): the app (the languages with a compiled
  `:/i18n/MovieMetaM_<code>.qm`, English always; codes like `de`, `pt_BR`, `zh_CN`; setting
  `language`, takes effect after a restart; `resolveAppLanguage()`: the code itself, else the
  same language family (`pt_PT` → `pt_BR`, `zh_TW` → `zh_CN`), else English; the one function is
  used by the translator, the help and the age ratings). Languages: de, en; drafts (by Claude, not checked): fr and es (UI and help), it, pt_BR,
  zh_CN, ja, ko (UI only, the help falls back to English) (table `kAppLanguages`, `APP_LANGUAGES` in `CMakeLists.txt`, help texts in `help/<code>/`,
  a help topic without a text in the language falls back to English) and the movie data for
  TMDB (genre list and search, e.g. `ja-JP`, setting `General/DataLanguage`, takes effect at once). Both follow the system
  until chosen in the setup dialog or the settings (a Japanese user gets the English app with
  Japanese data). The data languages are TMDB's primary translations, names from `QLocale`.
- `twolinedelegate.*` – entries of movie list and search results; `posterpreview.*` – enlarged
  poster (200×300) when the mouse rests on a search result (full poster in `kPosterRole`).
- TMDB data (double click on a result, tab "Data"): `onSearchTableViewDoubleClicked` loads
  `/movie/{id}` (`append_to_response=credits,release_dates`) and `/movie/{id}/images`;
  `fillDetailsTab`, `fillPosters`; "Apply" (`applyTmdbMovie`) takes everything found into the
  selected movie (empty values change nothing; setting `General/FillEmptyOnly` "Only fill empty
  fields", off by default: then only fields that are still empty are filled, cover and age rating
  included – a never-set date is 1972-01-01 in the form, so it counts as empty). `posterpicker.*` – the large poster with a badge
  "n / m"; a click opens a list (`TwoLineDelegate`) of up to 30 posters (language, size,
  rating). Age rating: `findTmdbRating` takes the entry of the country from `ratingCountry()` – the
  system of the SAVED file as long as the movie still has it, else the country of the settings (also
  for a rating applied or chosen but not saved yet; saving the settings refreshes the tab "Data")
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
  is not written again. Closing with unsaved changes asks (`closeEvent`: Save all / Discard /
  Cancel; "Save all" closes by itself once `savingFinished` had no error). "Remove" under the movie list asks the
  same when a selected movie has unsaved changes (Save and remove / Remove / Cancel, Cancel is the
  default); "Save and remove" saves the changed ones, `savingFinished` removes then (`m_removeAfterSaving`).
- The first line of a list entry is `listTitle()`: the title of the movie, else the file name before
  the first dot with "_" as space; `updateListEntry()` (from `onDataChanged`) keeps it in step with
  the current title (typing, apply). Selecting a movie, typing in the title field and applying data fill the search field with the same
  text (`showTitleInSearch()`; only this direction; no year, no clean-up of names like "1080p" – decided with the user as
  the simple version; the search itself is started by Enter).
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
  (`ressources/ratings/<code>.json`, `Ratings.qrc`; country name en/de (the English one is the key for the translation of the name), system, id, steps with
  key = badge text, code = written into the file, score, label = combo text, description =
  tooltip, color, shape, style). `AgeRatings::systems()` feeds the settings (with a preview of
  the badges), the setup dialog and the rating combo box. Badges: `createAgeRatingIcon()` –
  shapes square, circle, diamond, triangle, octagon, hexagon, star; styles filled, outline
  (white, border and code in the color: JP, SK, TR), light (white, black border, code in the
  color: US, IS), dark (#313030, code in the color: FI); the font shrinks until the
  code fits. **The JSON files and `Ratings.qrc` are generated, not edited by hand:** change
  the table, then `python3 tools/ratings_from_xlsx.py ~/kDrive/Development/QT/age_ratings.xlsx` (also writes `countrynames.cpp`: run lupdate afterwards for new countries)
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

## Open points (as of 0.8.27; numbers kept, 1 to 10 are done)

9. Mac: check the age rating icon in the closed combo box with the new combo style (before:
   about 2 px too high with the native style; the transparent-stripe fix was reverted). **Done.**
10. Help topics, one at a time (text first in the chat, de + en, then the files). Plan agreed
    with the user (title / second line / icon / file):
    1. Movie List / Add, select, remove / `add` / `movielist` – **done (0.8.0)**
    2. Movie Info / Edit, cover, age rating / `app_icon` / `movieinfo` – fields, tags (×, drag,
       Delete), cover (drag in and out), video size / HD, age rating (system of the file,
       unknown entries "prefix: code"), content advisory – **done (0.8.1)**
    3. Movie Search / Find and apply TMDB data / `search` / `moviesearch` – search, poster
       preview, double click → tab "Data", poster picker, "Apply" (empty values change nothing) – **done (0.8.2)**
    4. Saving / Save, safe saving / `save` / `saving` – Save / Save all, the check afterwards,
       safe saving, questions on closing and removing; the section "Speichern" moves here out
       of `about.md` – **done (0.8.3)**
    5. Settings / Languages, age rating, saving / `settings` / `settings` – app language (after
       a restart) vs. data language (at once), rating system only for movies without one, safe
       saving → refers to Saving – **done (0.8.4)**
    6. About: license section to "du"; credits without Lucide, Material Icons, SVG Repo (all
       icons are own since 0.7.6), no more "symbols from churchWorshipM" – **done (0.8.5)**
    Own film strip icons for the topics Movie List / Info / Search / Saving (`*_movie.svg`, also in
    `../libraries/icons/` 1.1.0): done.
11. Movie list: a file that cannot be read is not added (decided with the user, no red state:
    nothing to edit or save in it). `MediaInfoM::isReadableMp4()` walks the top-level boxes and
    wants an intact `moov` (a readable file without tags is fine); `addFilesToModel` counts the
    others and shows "n file(s) could not be read." (Error toast) – **done (0.8.6)**
