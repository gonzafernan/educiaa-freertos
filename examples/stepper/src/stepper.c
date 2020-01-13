#include "sapi.h"

int main(void){

   /* ------------- INICIALIZACIONES ------------- */

   /* Inicializar la placa */
   boardConfig();

   gpioInit(GPIO0, GPIO_OUTPUT);
   gpioInit(GPIO1, GPIO_OUTPUT);
   gpioInit(GPIO2, GPIO_OUTPUT);
   gpioInit(GPIO3, GPIO_OUTPUT);

   gpioInit(GPIO4, GPIO_OUTPUT);
   gpioInit(GPIO5, GPIO_OUTPUT);
   gpioInit(GPIO6, GPIO_OUTPUT);
   gpioInit(GPIO7, GPIO_OUTPUT);

   // Count
   // unsigned char count = 0x00;
   int iter = 0;
   int stepper = 1;

   /* ------------- REPETIR POR SIEMPRE ------------- */
   while(1) {
        if (stepper == 0){
            switch (iter){
                case 0:
                    gpioWrite(GPIO0, ON);
                    gpioWrite(GPIO1, ON);
                    gpioWrite(GPIO2, OFF);
                    gpioWrite(GPIO3, OFF);
                    break;

                case 1:
                    gpioWrite(GPIO0, OFF);
                    gpioWrite(GPIO1, ON);
                    gpioWrite(GPIO2, OFF);
                    gpioWrite(GPIO3, OFF);
                    break;

                case 2:
                    gpioWrite(GPIO0, OFF);
                    gpioWrite(GPIO1, ON);
                    gpioWrite(GPIO2, ON);
                    gpioWrite(GPIO3, OFF);
                    break;

                case 3:
                    gpioWrite(GPIO0, OFF);
                    gpioWrite(GPIO1, OFF);
                    gpioWrite(GPIO2, ON);
                    gpioWrite(GPIO3, OFF);
                    break;

                case 4:
                    gpioWrite(GPIO0, OFF);
                    gpioWrite(GPIO1, OFF);
                    gpioWrite(GPIO2, ON);
                    gpioWrite(GPIO3, ON);
                    break;

                case 5:
                    gpioWrite(GPIO0, OFF);
                    gpioWrite(GPIO1, OFF);
                    gpioWrite(GPIO2, OFF);
                    gpioWrite(GPIO3, ON);
                    break;

                case 6:
                    gpioWrite(GPIO0, ON);
                    gpioWrite(GPIO1, OFF);
                    gpioWrite(GPIO2, OFF);
                    gpioWrite(GPIO3, ON);
                    break;

                case 7:
                    gpioWrite(GPIO0, ON);
                    gpioWrite(GPIO1, OFF);
                    gpioWrite(GPIO2, OFF);
                    gpioWrite(GPIO3, OFF);
                    break;
            }
        }

        if (stepper == 1){
            switch (iter){
                case 0:
                    gpioWrite(GPIO4, ON);
                    gpioWrite(GPIO5, ON);
                    gpioWrite(GPIO6, OFF);
                    gpioWrite(GPIO7, OFF);
                    break;

                case 1:
                    gpioWrite(GPIO4, OFF);
                    gpioWrite(GPIO5, ON);
                    gpioWrite(GPIO6, OFF);
                    gpioWrite(GPIO7, OFF);
                    break;

                case 2:
                    gpioWrite(GPIO4, OFF);
                    gpioWrite(GPIO5, ON);
                    gpioWrite(GPIO6, ON);
                    gpioWrite(GPIO7, OFF);
                    break;

                case 3:
                    gpioWrite(GPIO4, OFF);
                    gpioWrite(GPIO5, OFF);
                    gpioWrite(GPIO6, ON);
                    gpioWrite(GPIO7, OFF);
                    break;

                case 4:
                    gpioWrite(GPIO4, OFF);
                    gpioWrite(GPIO5, OFF);
                    gpioWrite(GPIO6, ON);
                    gpioWrite(GPIO7, ON);
                    break;

                case 5:
                    gpioWrite(GPIO4, OFF);
                    gpioWrite(GPIO5, OFF);
                    gpioWrite(GPIO6, OFF);
                    gpioWrite(GPIO7, ON);
                    break;

                case 6:
                    gpioWrite(GPIO4, ON);
                    gpioWrite(GPIO5, OFF);
                    gpioWrite(GPIO6, OFF);
                    gpioWrite(GPIO7, ON);
                    break;

                case 7:
                    gpioWrite(GPIO4, ON);
                    gpioWrite(GPIO5, OFF);
                    gpioWrite(GPIO6, OFF);
                    gpioWrite(GPIO7, OFF);
                    break;
            }
        }

        if (iter == 7){
            iter = 0;
        } else {
            iter++;
        }

        delay(1);

   }

   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}