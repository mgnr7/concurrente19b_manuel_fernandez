
# ¿Permite este mecanismo de control de concurrencia resolver este problema de manera predecible?

Usando este mecanismo de control se puede conocer el orden en el que cada thread va a imprimir en consola el mensaje,
ya que cada thread se encarga de incrementar el semáforo del thread siguiente en el arreglo.
De esta manera se sabe que en la consola van a aparecer los mensajes según el orden de los threads en el arreglo.


# Diferencias entre exclusión mutua y semáforos
La principal diferencia entre usar exclusión mutua y semáforos para este problema es que en el caso de la exclusión mutua cada thread tiene un mutex el cual está bloqueado al menos que sea el primer thread en el arreglo,
de esta manera para que un thread pueda ejecutar la función que se le pide, requiere que el thread anterior desbloquee su mutex.
En el caso de los semáforos, similar al ejemplo con mutex, existe un semáforo para cada thread, el cual posee el valor 0 para cada uno de los threads excepto para el primer thread en el arreglo, el cual tiene su semáforo inicializado en 1.
Esto implica que cada thread espera que el thread anterior incremente su semáforo para poder ejecutar la función.
