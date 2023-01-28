#include "mt10t8.h"
#include "main.h"

#define MT10T8_BUFFER_SIZE 10

typedef enum MT10T8_Mode {
  MT10T8_ModeAddr = 0,
  MT10T8_ModeData = 1,
} MT10T8_Mode;

static inline MT10T8_Pin MakePin(GPIO_TypeDef *port, uint16_t pin) {
  MT10T8_Pin result = { port, pin };
  return result;
}

static inline void MT10T8_SetPin(MT10T8_Pin pin, uint8_t val) {
  HAL_GPIO_WritePin(pin.port, pin.pin, ((val) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void MT10T8_Enable(MT10T8_Dev *dev) {
  MT10T8_SetPin(dev->WR2, 1);
}

void MT10T8_Disable(MT10T8_Dev *dev) {
  MT10T8_SetPin(dev->WR2, 0);
}

static void MT10T8_SendNibble(MT10T8_Dev *dev, MT10T8_Mode mode, uint8_t val) {
  MT10T8_SetPin(dev->DB0, ((val >> 0) & 1));
  MT10T8_SetPin(dev->DB1, ((val >> 1) & 1));
  MT10T8_SetPin(dev->DB2, ((val >> 2) & 1));
  MT10T8_SetPin(dev->DB3, ((val >> 3) & 1));
  MT10T8_SetPin(dev->A0, mode);
  MT10T8_SetPin(dev->WR1, 1);
  HAL_Delay(1);
  MT10T8_SetPin(dev->WR1, 0);
  HAL_Delay(1);
}

static void MT10T8_SendAddr(MT10T8_Dev *dev, uint8_t addr) {
  MT10T8_SendNibble(dev, MT10T8_ModeAddr, addr);
}

static void MT10T8_SendData(MT10T8_Dev *dev, uint8_t data) {
  MT10T8_SendNibble(dev, MT10T8_ModeData, (data >> 0) & 0x0f);
  MT10T8_SendNibble(dev, MT10T8_ModeData, (data >> 4) & 0x0f);
}

void MT10T8_Init(MT10T8_Dev *dev) {
  dev->A0  = MakePin(MT10T8_A0_GPIO_Port , MT10T8_A0_Pin );
  dev->DB0 = MakePin(MT10T8_DB0_GPIO_Port, MT10T8_DB0_Pin);
  dev->DB1 = MakePin(MT10T8_DB1_GPIO_Port, MT10T8_DB1_Pin);
  dev->DB2 = MakePin(MT10T8_DB2_GPIO_Port, MT10T8_DB2_Pin);
  dev->DB3 = MakePin(MT10T8_DB3_GPIO_Port, MT10T8_DB3_Pin);
  dev->WR1 = MakePin(MT10T8_WR1_GPIO_Port, MT10T8_WR1_Pin);
  dev->WR2 = MakePin(MT10T8_WR2_GPIO_Port, MT10T8_WR2_Pin);
  MT10T8_Reset(dev);
}

void MT10T8_Reset(MT10T8_Dev *dev) {
  MT10T8_SetPin(dev->WR1, 0); // Don't send signals to LCI.
  MT10T8_Enable(dev);
  MT10T8_SendAddr(dev, 0xf); // Send register address 0xF to LCI.
  MT10T8_SendData(dev, 0x1); // Send data 0x1 to LCI register 0xF for LCI unlock.
  MT10T8_Disable(dev);
}

static uint8_t MT10T8_CharToSegmentCode(char chr)
{
  switch (chr) {
  default: return 0; // Space
  case '-': return 0x01;
  case '*': return 0xA9;
  case '=': return 0x05;
  case '_': return 0x04;
  case '!': return 0x70;
  case '^': return 0x41;
  case '0': return 0xEE;
  case '1': return 0x60;
  case '2': return 0x2F;
  case '3': return 0x6D;
  case '4': return 0xE1;
  case '5': return 0xCD;
  case '6': return 0xCF;
  case '7': return 0x68;
  case '8': return 0xEF;
  case '9': return 0xED;
  case 'A': return 0xEB;
  case 'b': return 0xC7;
  case 'c': return 0x07;
  case 'd': return 0x67;
  case 'E': return 0x8F;
  case 'F': return 0x8B;
  case 'J': return 0x6E;
  case 'G': return 0xCE;
  case 'h': return 0xC3;
  case 'L': return 0x86;
  case 'n': return 0x43;
  case 'o': return 0x47;
  case 'P': return 0xAB;
  case 'r': return 0x03;
  case 'U': return 0xE6;
  case 'X': return 0xE3;
  case 'Y': return 0xE5;
  }
}

void MT10T8_PrintChar(MT10T8_Dev *dev, char chr) {
  uint8_t val = MT10T8_CharToSegmentCode(chr);
  MT10T8_SendData(dev, val);
}

typedef struct MT10T8_Buffer {
  char data[MT10T8_BUFFER_SIZE];
} MT10T8_Buffer;

void MT10T8_SendBuffer(MT10T8_Dev *dev, const MT10T8_Buffer *buf) {
  size_t i;
  MT10T8_SendAddr(dev, 0);
  for (i = 0; i < sizeof buf->data; ++i) {
    MT10T8_PrintChar(dev, buf->data[i]);
  }
}

void MT10T8_PrintString(MT10T8_Dev *dev, const char *str) {
  size_t i;
  MT10T8_SendAddr(dev, 0);
  for (i = 0; i < MT10T8_BUFFER_SIZE && *str != '\0'; ++i, ++str) {
    MT10T8_PrintChar(dev, *str);
  }
}

void MT10T8_Print(MT10T8_Dev *dev, const char *str) {
  MT10T8_PrintString(dev, str);
}

void MT10T8_Clear(MT10T8_Dev *dev) {
  MT10T8_PrintString(dev, "        ");
}

void MT10T8_PrintDecInt(MT10T8_Dev *dev, int val) {
  char str[MT10T8_BUFFER_SIZE];
  MT10T8_PrintString(dev, str);
}

void MT10T8_PrintDecUInt(MT10T8_Dev *dev, unsigned int val) {
}

void MT10T8_PrintHex32(MT10T8_Dev *dev, uint32_t val) {
  char str[MT10T8_BUFFER_SIZE];
  size_t i;
  // static_assert(2 + 2 * sizeof val <= MT10T8_BUFFER_SIZE);
  str[0] = 'h';
  str[1] = ' ';
  for (i = 0; i < sizeof val; ++i) {
    uint8_t b = (uint8_t) (val >> ((sizeof val - 1 - i) * 8));
    str[2 + i*2 + 0] = b & 0x0f;
    str[2 + i*2 + 1] = b >> 4;
  }
  MT10T8_PrintString(dev, str);
}
