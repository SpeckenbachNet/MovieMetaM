# Búsqueda de películas

A la derecha encuentras datos de películas en TMDB (The Movie Database) y los pasas a la película seleccionada. MovieMetaM necesita una conexión a Internet para ello.

## Buscar

Escribe un título en el campo de búsqueda y pulsa Intro. Los resultados aparecen en la pestaña **Resultados**, cada uno con póster, título y año. Un breve mensaje indica cuántas películas se han encontrado. Se muestran los primeros 20 resultados; si TMDB conoce más, dice por ejemplo «Se muestran 20 de 134 películas», y tú acotas la búsqueda.

El idioma en el que TMDB busca y responde es el **Idioma de los datos de la película** de los ajustes.

## Ver los pósteres

Deja el ratón un momento sobre un resultado y el póster aparece más grande.

## Ver los datos

Un doble clic en un resultado carga sus datos en la pestaña **Datos**: título, fecha de estreno, estudio, género, clasificación por edades, descripción, actores, directores, guion y productores. Arriba se muestra el póster.

Un clic en el póster abre una lista de hasta 30 pósteres de TMDB. Cada uno muestra el idioma del texto impreso (o «Sin texto»), el tamaño en píxeles y la valoración. Haz clic en uno para elegirlo. Arriba a la derecha del póster ves cuál es, por ejemplo «2 / 14».

![Pestaña Datos con el póster y la lista de pósteres](:/images/help/search.png)

MovieMetaM toma la clasificación por edades con estas reglas:

- Si el archivo guardado ya tiene una clasificación, por ejemplo «FSK 12», MovieMetaM toma de TMDB la clasificación del mismo país (aquí Alemania). Así una película se queda con su sistema.
- Si el archivo aún no tiene ninguna, o has cambiado la clasificación en el programa (por ejemplo al aplicar datos), se usa el país de los ajustes, también después de cambiarlo allí.
- Si TMDB no tiene nada para ese país, la clasificación de la película se queda como está.

## Aplicar

**Aplicar** escribe los datos en la película seleccionada y toma el póster elegido como carátula. Lo que TMDB no conoce se queda como está: los valores vacíos no cambian nada.

> Después la película tiene un punto amarillo en la lista. Todavía no se ha guardado nada, cómo guardar se explica en **Guardado**.

Si no hay ninguna película seleccionada, un breve mensaje te lo dice.
