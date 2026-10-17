# MovieMetaM

Editor for the metadata of movie files (Qt 6 Widgets, C++17, CMake): reads title, cast,
directors, description, genre, age rating and cover from MP4 files, finds movie data on TMDB.
Developer: Michael Speckenbach (SpeckenbachNet). Repository: github.com/SpeckenbachNet/MovieMetaM.

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
1.2.0) and `searchfield/` (SearchField 1.1.0), each with CHANGELOG.md and README.md. Projects
keep a copy: change the master copy first, raise the version (CHANGELOG, `...Version` constant,
header comment), then copy. Other projects (churchWorshipM, PhotoBoothM, template,
PDFDocumentScannerM) are updated later by the user – don't touch them unasked.

## Style of the UI

- Colors from the palette (light and dark themes), no fixed colors where they meet theme colors.
- Icons: black SVG drawings tinted in the text color (ToolbarM, SearchField); origin and
  licenses in `ressources/icons/LICENSES.md` (save icons from Lucide, ISC).
- Desktop reference: `../churchWorshipM` (ToolbarM per page, SearchField); style reference:
  `../PhotoBoothM` (tablet-first, so not 1:1).
- Every page of `stackedWidget` has its own ToolbarM on top; `showPage()` switches pages.

## Structure

- `main.cpp` – settings, translator, setup dialog on first start, main window.
- `mainwin.*`, `initialize.cpp` – main window: movie list (left), movie info (middle),
  TMDB search (right); pages settings and info.
- `mediainfom.*` – reads MP4 boxes (metadata, iTunMOVI XML, cover); `moviedata.*` – data of a movie.
- `tagwidget.*` – tag input (cast, directors, ...); `imagelabelm.*` – cover with drag and drop;
  `tableviewm.*` – movie list with file drop; `setupdialog.*` – first start.
- `toolbarm.*`, `searchfield.*` – copies from `../libraries`.

## Open points (as of 0.3.4)

1. UI overhaul, next steps: settings page as cards (no sidebar yet); help page (like PhotoBoothM
   HelpPage) with "About MovieMetaM" for now (the help button opens the info page meanwhile);
   main page: cards instead of grey heading bars.
2. Bugs: setup dialog `tr("…","HK")` bracket bug (also KR, UK) and language choice not saved;
   12 countries offered but only DE/UK/US in `ratingMap`; new rating system only after restart;
   after removing a movie the form still shows it; false "changed" for movies without date
   (1972 default) or with a genre not in the TMDB list (genres in English, search in German);
   arrow keys in the movie list don't switch the movie; cover download can land in a new
   search's rows; search term not URL-encoded (use QUrlQuery).
3. Saving to the MP4 file is not implemented (saveMovieDetails / save all are empty) – needs a
   design decision first (own MP4 writer or a library like TagLib).
4. Applying a TMDB search result (double click only logs the id).
5. The old TMDB token is in the git history: revoke it at TMDB and use a new one.
6. Origin of some icons still to be clarified (`LICENSES.md`).
