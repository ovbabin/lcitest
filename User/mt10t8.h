#ifndef INCLUDE_GUARD_MT10T8_H
#define INCLUDE_GUARD_MT10T8_H

#include <stdint.h>
#include <stm32f4xx_hal.h>

typedef struct MT10T8_Pin {
  GPIO_TypeDef *port;
  uint16_t pin;
} MT10T8_Pin;

typedef struct MT10T8_Init {
  MT10T8_Pin A0;
  MT10T8_Pin DB0;
  MT10T8_Pin DB1;
  MT10T8_Pin DB2;
  MT10T8_Pin DB3;
  MT10T8_Pin WR1;
  MT10T8_Pin WR2;
} MT10T8_Init;


typedef struct MT10T8_Dev {
  MT10T8_Pin A0;
  MT10T8_Pin DB0;
  MT10T8_Pin DB1;
  MT10T8_Pin DB2;
  MT10T8_Pin DB3;
  MT10T8_Pin WR1;
  MT10T8_Pin WR2;
} MT10T8_Dev;

void MT10T8_Init(MT10T8_Dev *dev);
void MT10T8_Reset(MT10T8_Dev *dev);
void MT10T8_Enable(MT10T8_Dev *dev);
void MT10T8_Disable(MT10T8_Dev *dev);
void MT10T8_Clear(MT10T8_Dev *dev);
void MT10T8_Print(MT10T8_Dev *dev, const char *str);

#endif /* INCLUDE_GUARD_MT10T8_H */
