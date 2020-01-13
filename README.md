# CIAA-RTOS

# Modo de uso
Para trabajar con la placa EDU-CIAA NXP agregué como submódulo el [firmware_v3](firmware_v3/), de modo que puedo mantener el proyecto con el firmware actualizado. El problema que surge es como trabajar sin modificar el submódulo. Para esto realicé un linkeo simbólico (SymLink en Linux) de la carpeta [libs](libs/) y el archivo [Makefile](Makefile) del submódulo, que son los archivos necesarios para la compilación de los distintos ejemplos, a el repositorio del proyecto. Esto mediante la ejecución de los siguientes comandos en consola:

```bash
$ ln -s firmware_v3/libs/ libs
$ ln -s firmware_v3/Makefile Makefila
```

De esta forma ya solo resta agregar los archivos [board.mk](board.mk) y [program-mk](program.mk) para poder compilar los ejemplos desde el repositorio del proyecto.

## Definición de la placa de desarrollo
La placa de desarrollo con la que se trabaja en el proyecto es la EDU-CIAA NXP, por lo que el archivo [board.mk](board.mk) es el siguiente:

```mk
# board.mk
BOARD = edu_ciaa_nxp
```

## Definición del programa a compilar
El programa a compilar queda definido en el archivo [program.mk](program.mk) de la siguiente forma:

```mk
# program.mk
PROGRAM_PATH = firmware_v3/examples/c/sapi/gpio
PROGRAM_NAME = blinky
```

# Ejemplos
Con el objetivo de testear el funcionamiento de cada elemento del brazo robótico por separado, se desarrollaron una serie de ejemplos que pueden compilarse y ejecutarlos individualmente.

## Movimiento de motores paso a paso