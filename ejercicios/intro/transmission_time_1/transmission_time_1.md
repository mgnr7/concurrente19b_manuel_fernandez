# **Ejercicio 4 [transmission_time_1]**
>Un equipo de investigadores necesita hacer un análisis de 7.2TB de datos, donde 1TB = 10<sup>12</sup>B. 
Dado que el análisis tomará mucho tiempo, se ha decidido usar un laboratorio de 28 computadoras que están conectadas por un _switch_ de Ethernet con un ancho de banda de 100Mbps y una latencia promedio de 20ms. 
Restando el ancho de banda extra para "enrutar" y corregir errores (_overhead_), suponga que la red puede transmitir datos a 85Mbps. Los datos están alojados en una de las 28 computadoras de la red y usted debe tratar de distribuir cantidades iguales de datos a cada una de las otras computadoras del laboratorio. Los datos están en archivos de 1MB cada uno. Los investigadores quieren saber ¿cuánto tiempo tardará en estar listos los datos para iniciar el análisis en el laboratorio?

Sabemos que :  $T = L + ^s/_b$
Donde _T_ es tiempo, _L_ es latencia, _s_ son los bytes transmitidos y _b_ es el ancho de banda.
Además $20ms$ (20 mili segundos) = $0,02$ segundos.

Como hay que distribuir 7.2TB de datos entre 28 computadoras, cada computadora debería de tener al rededor de 0.26TB de datos para que la distribución sea equitativa.

$7.2TB = 7200000 MB$

Como hay 28 computadoras se tiene: $\frac{7200000}{28}$ o $257142MB$ para cada computadora

Al sustituir los valores en la formula se tiene: 
$T = 0,02 +$$\frac{257142MB}{10.65MB}$ 
Lo cual nos indica que para transmitir $257142MB$ se requieren $24144$  segundos o $6.7$ horas.




