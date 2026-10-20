# Saving

Changes are first only in MovieMetaM. The movie file changes only when you save. A movie with unsaved changes has a yellow dot in the movie list.

## Saving

- **Save** (toolbar) writes the selected movie.
- **Save all** writes all changed movies.

Both are only active when there is something to save. While MovieMetaM writes, the window is locked; with big files this can take a while. A message tells you how many movies were saved, or what went wrong, for example if the file is not writable.

After saving, MovieMetaM reads the file again and checks title, description, age rating and cover. If something doesn't match, you get an error message.

## Directly into the file

MovieMetaM writes the data **directly into the movie file**, as most tag editors do. This is fast, also for big files and on network drives. If the writing is interrupted (crash, power failure, lost network connection), the file can be damaged. Keep a copy of files that are important to you.

## Safe saving

For more safety switch on **Safe saving** in the settings: the data is then written into a copy, the copy is checked, and only then it replaces the file. This needs room for a second copy of the file and takes much longer for big files and on network drives (a 4.5 GB file over the network takes about five minutes).

## Closing and removing

If a movie has unsaved changes, MovieMetaM asks:

- **Closing the window:** **Save all** saves and then closes by itself (if there was an error, the window stays open). **Discard** closes without saving. **Cancel** leaves everything as it is.
- **Removing from the list:** see **Movie List**.
