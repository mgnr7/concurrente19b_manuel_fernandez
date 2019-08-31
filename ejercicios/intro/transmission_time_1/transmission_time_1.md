# **Ejercicio 4 [transmission_time_1]**
>Un equipo de investigadores necesita hacer un análisis de 7.2TB de datos, donde 1TB = 10<sup>12</sup>B. 
Dado que el análisis tomará mucho tiempo, se ha decidido usar un laboratorio de 28 computadoras que están conectadas por un _switch_ de Ethernet con un ancho de banda de 100Mbps y una latencia promedio de 20ms. 
Restando el ancho de banda extra para "enrutar" y corregir errores (_overhead_), suponga que la red puede transmitir datos a 85Mbps. Los datos están alojados en una de las 28 computadoras de la red y usted debe tratar de distribuir cantidades iguales de datos a cada una de las otras computadoras del laboratorio. Los datos están en archivos de 1MB cada uno. Los investigadores quieren saber ¿cuánto tiempo tardará en estar listos los datos para iniciar el análisis en el laboratorio?

Sabemos que :  $T = L + ^s/_b$
Donde _T_ es tiempo, _L_ es latencia, _s_ son los bytes transmitidos y _b_ es el ancho de banda.
Además $20ms$ (20 mili segundos) = $0,02$ segundos.

Como hay que distribuir 7.2TB de datos entre 28 computadoras, cada computadora debería de tener al rededor de 0.26TB de datos para que la distribución sea equitativa.
$0.26TB = 2600600 MB$

Al sustituir los valores en la formula se tiene:
$T = 0,02 +$$\frac{1MB}{10.65MB}$ 
Lo cual nos indica que para transmitir 1MB se requieren $\frac{1213}{10650}$  segundos.

Al multiplicar el tiempo de transmisión del mensaje por la cantidad de MB que hay que transmitir hacia cada computadora se obtiene:
$296199$ segundos, lo que equivale a 4936 minutos o 82 horas.



