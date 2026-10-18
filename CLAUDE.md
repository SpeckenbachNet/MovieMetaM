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
The master copies live on the Linux machine only (not on the Mac).

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
  TMDB search (right, tabs Results / Data / Cover); pages settings and info.
- `mediainfom.*` – reads MP4 boxes (metadata, iTunMOVI XML, cover); `moviedata.*` – data of a movie.
- `tagwidget.*` – tag input (cast, directors, ...); `imagelabelm.*` – cover: paints itself
  (rounded, sharp), drag and drop in (`coverDropped` / `coverUrlDropped`) and out;
  `tableviewm.*` – movie list (`QListView`) with file drop; `setupdialog.*` – first start.
- `twolinedelegate.*` – entries of movie list and search results; `posterpreview.*` – enlarged
  poster (200×300) when the mouse rests on a search result (full poster in `kPosterRole`).
- `themecolors.h` – shared colors and shapes (`FieldLook`, `apply…Style`).
- `toolbarm.*`, `searchfield.*` – copies from `../libraries`.

## Open points (as of 0.3.13)

1. ToolbarM 1.3.0 was made on the Mac in MovieMetaM's copy (no master copy there). On Linux:
   copy `toolbarm.h/.cpp` from MovieMetaM to `../libraries/toolbarm` (master is 1.2.0 –
   check it has not changed meanwhile), raise the version in README.md if it is named there and
   add to CHANGELOG.md:
   `## 1.3.0` – `- New: setButtonRadius() / setToolbarRadius() set the corner radius of the
   buttons / the toolbar frame (default 15 / 18, still limited to half the height).`
   Same for SearchField 1.2.0 (master is 1.1.0) to `../libraries/searchfield`, CHANGELOG:
   `## 1.2.0` – `- New: setCornerRadius() (default stays the pill shape, limited to half the
   height).` and `- Clear button drawn as a filled circle with a cut-out x in the color of the
   magnifier (instead of the style's x).`
2. UI overhaul, further steps: settings page as cards (no sidebar yet); help page (like
   PhotoBoothM HelpPage) with "About MovieMetaM" for now (the help button opens the info page
   meanwhile); the setup dialog's combo boxes with `applyComboStyle`.
3. Bugs: setup dialog `tr("…","HK")` bracket bug (also KR, UK) and language choice not saved;
   12 countries offered but only DE/UK/US in `ratingMap`; new rating system only after restart;
   after removing a movie the form still shows it; false "changed" for movies without date
   (1972 default) or with a genre not in the TMDB list (genres in English, search in German);
   arrow keys in the movie list don't switch the movie; clicking the empty area of the movie
   list loses the selection; a search without results leaves the old results; search term not
   URL-encoded (use QUrlQuery).
4. Saving to the MP4 file is not implemented (saveMovieDetails / save all are empty) – needs a
   design decision first (own MP4 writer or a library like TagLib).
5. Applying a TMDB search result (double click only logs the id); the tabs "Data" and "Cover"
   are empty placeholders for it.
6. Tags: one click removes a tag without asking – tooltip "Click to remove" or an × on hover.
7. The old TMDB token is in the git history: revoke it at TMDB and use a new one.
8. Origin of some icons still to be clarified (`LICENSES.md`); unused
   `ressources/images/statusgreen.svg` / `statusorange.svg` can go.
9. Mac: check the age rating icon in the closed combo box with the new combo style (before:
   about 2 px too high with the native style; the transparent-stripe fix was reverted).
