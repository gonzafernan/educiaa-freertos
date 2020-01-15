# Ejemplos de RTOS aplicados en la EDU-CIAA-NXP
Variación de los ejemplos del libro "Masteringthe FreeRTOS™ Real Time Kernel: A Hands-On Tutorial Guide" de Richard Barry, pero con una variación para interactuar con los componentes de la placa de desarrollo EDU-CIAA-NXP con la librería sAPI del firmware_v3.

# Manejo de Tareas: Creación de tareas
## Ejemplo 2: Pasar parámetros a una tarea
Este ejemplo es similar al ejemplo 2 "Task Parameter" del libro, en el capítulo 3 "Task Management", sección 3.4 "Creating Tasks", página 56.

En este caso, el ejemplo consiste en una simplificación del [ejemplo 1 "Creating Tasks"](https://github.com/FernandezGFG/CIAA-RTOS/tree/master/examples/task_management/01_create_task). Dado que las dos tareas implementadas son muy similares, se puede reemplazar por dos instancias de la misma, pasandoles como parámetro que LED hacer titilar.

## Hardware necesario para implementar el ejemplo
Para llevar a cabo este ejemplo solo es necesaria la placa EDU-CIAA-NXP, ya que el programa solo consiste en el toggle de los LEDs provistos en la misma.

## Resultado del ejemplo
Se puede observar que los resultados obtenidos son los mismos que los del ejemplo 1.