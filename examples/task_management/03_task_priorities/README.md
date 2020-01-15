# Ejemplos de RTOS aplicados en la EDU-CIAA-NXP
Variación de los ejemplos del libro "Masteringthe FreeRTOS™ Real Time Kernel: A Hands-On Tutorial Guide" de Richard Barry, pero con una variación para interactuar con los componentes de la placa de desarrollo EDU-CIAA-NXP con la librería sAPI del firmware_v3.

# Manejo de Tareas: Medidas de tiempo e interrupción por Tick
## Ejemplo 3: Experimentación con prioridades
Este ejemplo es similar al ejemplo 3 "Experimenting with priorities" del libro, en el capítulo 3 "Task Management", sección 3.6 "Time Measurement and the Tick Interrupt", página 63.

En este caso, el ejemplo es exactamente igual al [ejemplo 2 "Task Parameter"](https://github.com/FernandezGFG/CIAA-RTOS/tree/master/examples/task_management/02_task_parameter), la unica diferencia es que las tareas tienen distinta prioridad.

## Hardware necesario para implementar el ejemplo
Para llevar a cabo este ejemplo solo es necesaria la placa EDU-CIAA-NXP, ya que el programa solo consiste en el toggle de los LEDs provistos en la misma.

## Resultado del ejemplo
Se puede observar que solo titila uno de los LEDs (el de mayor prioridad).

La tarea que titila el LED1 nunca pasa a estado de *Running* a pesar de estar disponible. Esto porque a diferencia del ejemplo 2, donde las tareas tenían igual prioridad y el scheduler utilizaba un mecanismo de *time slice*, ahora el scheduler ejecuta solo la tarea de mayor prioridad que nunca deja de estar disponible para ejecutarse.