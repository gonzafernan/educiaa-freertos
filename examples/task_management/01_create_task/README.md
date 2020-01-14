# Ejemplos de RTOS aplicados en la EDU-CIAA-NXP
Variación de los ejemplos del libro "Masteringthe FreeRTOS™ Real Time Kernel: A Hands-On Tutorial Guide" de Richard Barry, pero con una variación para interactuar con los componentes de la placa de desarrollo EDU-CIAA-NXP con la librería sAPI del firmware_v3.

# Manejo de Tareas: Creación de tareas
Este ejemplo es similar al ejemplo 1 "Creating Tasks" del libro, en el capítulo 3 "Task Management", sección 3.4 "Creating Tasks", página 52.

En este caso la primer tarea simplemente realiza un toggle del LEDR (rojo del led RGB), y la segunda tarea realiza un toggle simultaneo de los LEDs 1 y 2 (LED1 y LED2).

## Hardware necesario para implementar el ejemplo
Para llevar a cabo este ejemplo solo es necesaria la placa EDU-CIAA-NXP, ya que el programa solo consiste en el toggle de los LEDs provistos en la misma.

## Resultado del ejemplo