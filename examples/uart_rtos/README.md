# Ejemplos de RTOS aplicados en la EDU-CIAA-NXP
Variación de los ejemplos del libro "Masteringthe FreeRTOS™ Real Time Kernel: A Hands-On Tutorial Guide" de Richard Barry, pero con una variación para interactuar con los componentes de la placa de desarrollo EDU-CIAA-NXP con la librería sAPI del firmware_v3.

# Manejo de Tareas: Medidas de tiempo e interrupción por Tick
## Ejemplo 4: Utilización del estado bloqueado para crear un delay
Este ejemplo es similar al ejemplo 4 "Using the Blocked state to create a delay" del libro, en el capítulo 3 "Task Management", sección 3.7 "Expanding the *Not Running* State", página 67.

En este caso, se corrige el [ejemplo 3 "Experimenting with priorities"](https://github.com/FernandezGFG/CIAA-RTOS/tree/master/examples/task_management/03_task_priorities), utilizando la API **vTaskDelay()** para llevar a las tareas a estado *Blocked* con lo que la tarea de mayor prioridad ya puede dar paso a la ejecución de la tarea de menor prioridad.

## Hardware necesario para implementar el ejemplo
Para llevar a cabo este ejemplo solo es necesaria la placa EDU-CIAA-NXP, ya que el programa solo consiste en el toggle de los LEDs provistos en la misma.

## Resultado del ejemplo
Se puede observar que ya no titila uno de los LEDs sino ambos como en el ejemplo 2.