# **Ejercicio 5 [transmission_time_2]**

>En un instituto sismo gráfico se tienen 40 sensores que recolectan y acumulan datos constantemente. Cada minuto envían un archivo de 1.5MB hacia los servidores centrales, donde $1MB=10^6B$. Los sensores se turnan para enviar los datos con el fin de no saturar los servidores, y puede asumir que no ocurren colisiones de datos en la red entre los 40 sensores. Se tiene contratado un servicio de 16Mbps con el proveedor de servicio de Internet, por tanto, los servidores pueden recibir datos a esa velocidad. Se ha medido una latencia promedio de 2ms porque hay sensores muy remotos. Se quieren instalar 30 sensores más ¿Se puede mantener el mismo ancho de banda con el proveedor de servicio o se debe aumentar? En caso de incrementarse ¿cuánto sería el ancho de banda mínimo que asegure que los datos se pueden recibir por los servidores centrales?

Es necesario aumentar el ancho de banda, ya que para poder tener 70 sensores funcionando al mismo tiempo los servidores van a recibir al 2 archivos por segundo cada uno de diferente sensor.

Es por ello que se requiere un ancho de banda de al menos 24 Mbps, ya que equivalen a 3 MB o 2 archivos enviados por los sensores.

