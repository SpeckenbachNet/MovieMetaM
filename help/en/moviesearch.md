# Movie Search

On the right you find movie data on TMDB (The Movie Database) and take it into the selected movie. MovieMetaM needs an internet connection for this.

## Searching

Type a title into the search field and press Enter. The hits appear in the tab **Results**, each with poster, title and year. A short message tells you how many movies were found. The first 20 hits are shown; if TMDB knows more, it says for example "Showing 20 of 134 movies", and you narrow the search down.

The language TMDB searches and answers in is the **movie data language** in the settings.

## Looking at posters

Rest the mouse on a hit for a moment and the poster appears larger.

## Looking at the data

A double click on a hit loads its data into the tab **Data**: title, release date, studio, genre, age rating, description, actors, directors, screenplay and producers. The poster is shown above.

A click on the poster opens a list of up to 30 posters from TMDB. Each shows the language of the printing (or "No text"), the size in pixels and the rating. Click one to choose it. At the top right of the poster you see which one it is, for example "2 / 14".

![Tab Data with the poster and the list of posters](:/images/help/search.png)

MovieMetaM takes the age rating by these rules:

- If the saved file already has an age rating, for example "FSK 12", MovieMetaM takes the rating of the same country from TMDB (here Germany). That way a movie stays with its system.
- If the file has none yet, or you have changed the rating in the program (for example by applying data), the country from the settings is used, also after you change it there.
- If TMDB has nothing for that country, the movie's age rating stays as it is.

## Applying

**Apply** writes the data into the selected movie and takes the chosen poster as its cover. What TMDB doesn't know stays as it is – empty values change nothing.

> Afterwards the movie has a yellow dot in the list. Nothing is saved yet, how to save is explained under **Saving**.

If no movie is selected, a short message tells you so.
