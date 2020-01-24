#include "oneWire.h"

static void __oneWire_Delay(uint32_t delayTime)
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	while(__HAL_TIM_GET_COUNTER(&htim2) < delayTime);
}

void OneWire_Init(OneWire_InitTypeDef* onewire, GPIO_TypeDef* port, uint32_t pin) {

	onewire->Pin = pin;
	onewire->Port = port;
	onewire->RecoveryTime = 2;
	onewire->SlotDuration = 65;
	__oneWire_PinOutputMode(onewire);
}

static OneWire_OK __oneWire_InitPulse(OneWire_InitTypeDef* onewire) {
	__oneWire_Delay(50);
	__oneWire_PullLow(onewire);
	__oneWire_Delay(480);
	__oneWire_PullHigh(onewire);
	for(int i = 20; i>0; i--) {
		if(__oneWire_ReadPin(onewire) == GPIO_PIN_SET) {
			__oneWire_Delay(15);
		}
		else {
			__oneWire_Delay(i*15);
			return ONEWIRE_OK;
		}
	}
	return ONEWIRE_NOT_OK;
}

static void __oneWire_WriteHigh(OneWire_InitTypeDef* onewire) {
	__oneWire_Delay(50);
	__oneWire_PullLow(onewire);
	__oneWire_Delay(10);
	__oneWire_PullHigh(onewire);
	__oneWire_Delay(onewire->SlotDuration - 10);
}

static void __oneWire_WriteLow(OneWire_InitTypeDef* onewire) {
	__oneWire_Delay(50);
	__oneWire_PullLow(onewire);
	__oneWire_Delay(onewire->SlotDuration);
}

static GPIO_PinState __oneWire_Read(OneWire_InitTypeDef* onewire) {
	GPIO_PinState test1, test2, test3;
	__oneWire_PullLow(onewire);
	__oneWire_Delay(2);
	__oneWire_PullHigh(onewire);
	__oneWire_Delay(4);
	test1 = __oneWire_ReadPin(onewire);
	__oneWire_Delay(4);
	test2 = __oneWire_ReadPin(onewire);
	__oneWire_Delay(4);
	test3 = __oneWire_ReadPin(onewire);
	if(test1 + test2 + test3 > 1) { return GPIO_PIN_SET; }
	else { return GPIO_PIN_RESET; }
}

static void __oneWire_PullLow(OneWire_InitTypeDef* onewire) {
	HAL_GPIO_WritePin(onewire->Port, onewire->Pin, GPIO_PIN_RESET);
}

static void __oneWire_PullHigh(OneWire_InitTypeDef* onewire) {
	HAL_GPIO_WritePin(onewire->Port, onewire->Pin, GPIO_PIN_SET);
}

static GPIO_PinState __oneWire_ReadPin(OneWire_InitTypeDef* onewire) {
	return HAL_GPIO_ReadPin(onewire->Port, onewire->Pin);
}

static void __oneWire_PinOutputMode(OneWire_InitTypeDef* onewire) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = onewire->Pin;

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(onewire->Port, &GPIO_InitStruct);
}

OneWire_OK OneWire_SendRomCommand(OneWire_InitTypeDef* onewire, uint8_t command, uint64_t romCode) {
	if(__oneWire_InitPulse(onewire) == ONEWIRE_NOT_OK) { return ONEWIRE_NOT_OK; }
	__oneWire_Delay(onewire->RecoveryTime);
	for(int i = 0; i < 8; i++) {
		if(command & 0x1) { __oneWire_WriteHigh(onewire); }
		else { __oneWire_WriteLow(onewire); }
		command >>= 1;
		__oneWire_Delay(onewire->RecoveryTime);
	}
	if(command == SKIP_ROM) { return ONEWIRE_OK; }
	else {
		for(int i = 0; i < 64; i++) {
			if(romCode & 0x1) { __oneWire_WriteHigh(onewire); }
			else { __oneWire_WriteLow(onewire); }
			romCode >>= 1;
			__oneWire_Delay(onewire->RecoveryTime);
		}
	}
	return ONEWIRE_OK;
}

OneWire_OK OneWire_SendSlaveCommand(OneWire_InitTypeDef* onewire, uint8_t command) {
	__oneWire_Delay(onewire->RecoveryTime);
	for(int i = 0; i < 8; i++) {
		if(command & 0x1) { __oneWire_WriteHigh(onewire); }
		else { __oneWire_WriteLow(onewire); }
		command >>= 1;
		__oneWire_Delay(onewire->RecoveryTime);
	}
	return ONEWIRE_OK;
}

uint8_t OneWire_ReadByte(OneWire_InitTypeDef* onewire) {
	uint8_t holder = 0;
	for(int i = 0; i < 8; i++) {
		holder |= __oneWire_Read(onewire);
		if (i != 7) { holder <<= 1; }
	}
	return holder;
}
