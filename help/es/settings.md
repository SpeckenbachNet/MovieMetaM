# Ajustes

Abre los ajustes con el engranaje de la barra de herramientas. **Guardar ajustes** aplica tus cambios; el botón solo está activo si has cambiado algo. **Atrás** vuelve a la ventana principal.

## Idioma

El idioma de MovieMetaM. Un cambio solo tiene efecto después de reiniciar.

## Idioma de los datos de la película

MovieMetaM consulta a TMDB en este idioma: la lista de géneros, la búsqueda, los títulos y descripciones de los resultados y los pósteres (pósteres en este idioma, en inglés y sin texto). Esto tiene efecto de inmediato, sin reiniciar. El idioma de los datos de la película puede ser distinto del de la aplicación: quien usa MovieMetaM en inglés puede obtener igualmente datos de películas en japonés.

Si no eliges nada, ambos idiomas siguen el del sistema.

## Clasificación por edades

El país cuyo sistema de clasificación por edades usa MovieMetaM. La vista previa muestra los niveles como insignias.

Esto solo se aplica a las películas cuyo archivo guardado aún no tiene clasificación. Si un archivo ya tiene una, la película conserva su sistema (consulta **Información de la película** y **Búsqueda de películas**). Un cambio tiene efecto de inmediato y se aplica enseguida a la película que se muestra.

## Aplicar datos

**Rellenar solo los campos vacíos** hace que **Aplicar** (en la búsqueda de películas) deje como está todo lo que ya has escrito y rellene solo los huecos, también la carátula. Desactivado por defecto: **Aplicar** toma entonces todo lo que tiene TMDB.

## Géneros propios

Géneros que quieres tener en la lista de géneros de la información de la película, por ejemplo «Serie» o «Concierto». Escribe un género y pulsa Intro, igual que con los nombres en la información de la película (quitar, orden, las mismas teclas). Tus géneros propios aparecen primero, después los de TMDB. **Mostrar solo los géneros propios** deja fuera de la lista los géneros de TMDB. La lista cambia cuando guardas los ajustes.

> Al aplicar datos de TMDB se sigue tomando el género que TMDB conoce para la película. Un género que escribes tú mismo en la información de la película no necesita ningún ajuste.

## Guardado

**Guardado seguro** escribe los datos en una copia, la comprueba y solo después sustituye al archivo. Es más seguro, pero necesita espacio y más tiempo. Qué ocurre exactamente se explica en **Guardado**.

## TMDB

MovieMetaM busca en TMDB con un token integrado en la aplicación. Si quieres usar tu propia cuenta de TMDB, introduce tu propio token (el «API Read Access Token» de los ajustes de API de tu cuenta de TMDB, gratuito). El campo oculta el token; el ojo al final lo muestra. Debajo del campo ves qué token se está usando. Si dejas el campo vacío, se vuelve a usar el token integrado.

> El token se guarda en el archivo de ajustes de MovieMetaM, sin cifrar.
