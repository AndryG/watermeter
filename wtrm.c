//#include "wtrm-conf.h"

/** Флаги прерываний */
volatile u8 intf;

/** Флаги состояний */
u8 f;

void init();

__C_task main(void){

  init();
  while(1){

    qtDispatch();

    qtDecrementDelay2(1);


  }

}

/**
 Начальная настройка. Запускается единожды при старте программы
 */
void init(){

  qtInit();

}

/**************************************************************
 **
 ** How to implement a low-level initialization function in C
 ** =========================================================
 **
 ** 1) Only use local auto variables.
 ** 2) Don't use global or static variables.
 ** 3) Don't use global or static objects (EC++ only).
 ** 4) Don't use agregate initializers, e.g. struct a b = {1};
 ** 5) Don't call any library functions (function calls that
 **    the compiler generates, e.g. to do integer math, are OK).
 ** 6) Setup the RSTACK as is appropriate!
 **    See code below or use the command line --enable_external_bus
 **
 **************************************************************/
int __low_level_init(void){
  return 1;
}
