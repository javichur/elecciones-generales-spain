# elecciones-generales-spain
Implementación en C para asignar los escaños del Congreso de los Diputados, tras una elecciones generales en España, a partir de los votos de cada circunscripción. Código fuente escrito 2004, como reto durante el primer año de ingeniería informática.

#Ficheros
- elecciones-generales.c. Fichero único de código fuente, ANSI C.
- elecciones.txt. Fichero de entrada. Contiene el nº de votos recibido por cada partido político, en cada circunscripción, y los parámetros de configuración: total de ciudadanos con derecho a voto, total diputados de la cámara, nº de circunscripciones, total de partidos presentados, etc.

#Compilación
gcc elecciones-generales.c -o elecciones-generales

#Ejecución
./elecciones-generales > fichero-salida.txt

#Puntos de mejora y errores pendientes de corrección
Este ejemplo es uno de mis primeros proyectos escritos en C. Tras encontrarlo casi 15 años después, hay muchas cosas mejorables, como:
- Refactorizar el código, hay funciones demasiado largas.
- Mejorar el formato del fichero de entrada "elecciones.txt".
- Documentar el código con un formato unificado.
- Hay una errata en el fichero de entrada "elecciones.txt". La suma de todos los valores de entrada de "Votos nulos" no coincide con la suma oficial facilitada por el Ministerio de Interior. Hay que revisarlo.
