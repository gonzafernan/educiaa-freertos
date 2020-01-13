# RTOS en placa de desarrollo EDU-CIAA para el control de brazo robótico básico

## Sobre el proyecto CIAA
La **Computadora Industrial Abierta Argentina** (CIAA) es un proyecto que nace en el año 2013 como una iniciativa conjunta entre el sector académico y el industrial de Argentina.

La primera versión de la CIAA es denominada CIAA-NXP, por estar basada en un procesador de la empresa *NXP Semiconductors*. Luego se desarrollaron otras versiones basadas en procesadores de otras marcas, como la CIAA-FSL, la CIAA-INTEL, la CIAA-PIC, etc. Esto convierte a la CIAA no solo en la primer y única computadora **industrial** y **abierta**, sino también en la primera realmente **libre**, ya que su diseño no está atado a los procesadores de una determinada compañía.

Además, se diseñó una versión educativa de la plataforma, denominada **EDU-CIAA**, más simple y de menor costo, para lograr un impacto en la enseñanza primaria, secundaria y universitaria.

Para más información sobre el proyecto CIAA, el [link a la página web del proyecto](http://www.proyecto-ciaa.com.ar/devwiki/doku.php?id=start).

## Placa de desarrollo EDU-CIAA-NXP
La EDU-CIAA-NXP es entonces, una versión de bajo cosro de la CIAA-NXP y es la utilizada para este proyecto. 

![EDU-CIAA-NXP](docs/edu_ciaa.jpg)

Su microcontrolador es el LPC4337 (dual core ARM Cortex-M4F y Cortex-M0). La placa también cuenta con los siguientes módulos:

- 2 puertos micro-USB (uno para aplicaciones y debugging, otro para alimentación).
- 4 salidas digitales implementadas con leds RGB.
- 4 entradas digitales con pulsadores.
- 1 puerto de comunicaciones RS 485 con bornera.
- 2 conectores de expansión:
  - P1:
    - 3 entradas analógicas (ADC0_1,2y3),
    - 1 salida analógica (DAC0),
    - 1 puerto I2C,
    - 1 puerto asincrónico full duplex (para RS-232).
    - 1 puerto CAN,
    - 1 conexión para un teclado de 3×4,

  - P2:
    - 1 puerto Ethernet,
    - 1 puerto SPI,
    - 1 puerto para Display LCD con 4 bits de datos, Enable y RS.
    - 9 pines genéricos de I/O.

A continuación se presenta un diagrama de bloques con los módulos que posee la placa:

![módulos EDU-CIAA-NXP](docs/modulos_edu_ciaa.jpg)

Para más información sobre la placa de desarrollo EDU-CIAA-NXP, el [link a la página web destinada a la placa](http://www.proyecto-ciaa.com.ar/devwiki/doku.php?id=desarrollo:edu-ciaa:edu-ciaa-nxp).

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
