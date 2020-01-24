#ifndef __oneWire_H
#define __oneWire_H
#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "tim.h"
#include "gpio.h"
#include "stm32f4xx.h"

#define SEARCH_ROM 0xf0
#define READ_ROM 0x33
#define MATCH_ROM 0x55
#define SKIP_ROM 0xcc
#define ALARM_SEARCH 0xec

 typedef struct
 {
   uint32_t Pin;
   GPIO_TypeDef * Port;
   uint32_t RecoveryTime;
   uint32_t SlotDuration;

 }OneWire_InitTypeDef;

typedef enum
{
  ONEWIRE_NOT_OK = 0,
  ONEWIRE_OK
}OneWire_OK;

static void __oneWire_Delay(uint32_t delayTime);
void OneWire_Init(OneWire_InitTypeDef* onewire, GPIO_TypeDef* port, uint32_t pin);
static OneWire_OK __oneWire_InitPulse(OneWire_InitTypeDef* onewire);
static void __oneWire_WriteHigh(OneWire_InitTypeDef* onewire);
static void __oneWire_WriteLow(OneWire_InitTypeDef* onewire);
static GPIO_PinState __oneWire_Read(OneWire_InitTypeDef* onewire);
static void __oneWire_PullLow(OneWire_InitTypeDef* onewire);
static GPIO_PinState __oneWire_ReadPin(OneWire_InitTypeDef* onewire);
static void __oneWire_PinOutputMode(OneWire_InitTypeDef* onewire);
static void __oneWire_PinInputMode(OneWire_InitTypeDef* onewire);
OneWire_OK OneWire_SendRomCommand(OneWire_InitTypeDef* onewire, uint8_t command, uint64_t romCode);
void OneWire_SendSlaveCommand(OneWire_InitTypeDef* onewire, uint8_t command);
uint8_t OneWire_ReadByte(OneWire_InitTypeDef* onewire);

#ifdef __cplusplus
}
#endif
#endif
