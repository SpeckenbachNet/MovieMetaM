# Guardado

Los cambios están primero solo en MovieMetaM. El archivo de la película solo cambia cuando guardas. Una película con cambios sin guardar tiene un punto amarillo en la lista de películas.

## Guardar

- **Guardar** (barra de herramientas) escribe la película seleccionada.
- **Guardar todo** escribe todas las películas modificadas.

Ambos solo están activos cuando hay algo que guardar. Mientras MovieMetaM escribe, la ventana está bloqueada; con archivos grandes puede tardar un rato. Un mensaje indica cuántas películas se han guardado, o qué ha fallado, por ejemplo si el archivo no se puede escribir.

Después de guardar, MovieMetaM vuelve a leer el archivo y comprueba el título, la descripción, la clasificación por edades y la carátula. Si algo no coincide, recibes un mensaje de error.

## Directamente en el archivo

MovieMetaM escribe los datos **directamente en el archivo de la película**, como la mayoría de los editores de etiquetas. Es rápido, también con archivos grandes y en unidades de red.

> Si se interrumpe la escritura (fallo, corte de luz, conexión de red perdida), el archivo puede dañarse. Guarda una copia de los archivos que sean importantes para ti.

## Guardado seguro

Para más seguridad, activa **Guardado seguro** en los ajustes: los datos se escriben entonces en una copia, se comprueba la copia y solo después sustituye al archivo. Esto necesita espacio para una segunda copia del archivo y tarda mucho más con archivos grandes y en unidades de red (un archivo de 4,5 GB por la red tarda unos cinco minutos).

## Cerrar y quitar

Si una película tiene cambios sin guardar, MovieMetaM pregunta:

- **Cerrar la ventana:** **Guardar todo** guarda y luego se cierra solo (si hubo un error, la ventana sigue abierta). **Descartar** cierra sin guardar. **Cancelar** lo deja todo como está.
- **Quitar de la lista:** véase **Lista de películas**.
