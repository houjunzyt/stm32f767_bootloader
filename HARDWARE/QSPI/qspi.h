#ifndef __QSPI_H_
#define __QSPI_H_

#include "sys.h"

extern QSPI_HandleTypeDef QSPI_Handler;    //QSPI���

uint8_t QSPI_Init(void);												//��ʼ��QSPI
void QSPI_Send_CMD(uint32_t instruction,uint32_t address,uint32_t dummyCycles,uint32_t instructionMode,uint32_t addressMode,uint32_t addressSize,uint32_t dataMode);			//QSPI��������
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen);							//QSPI��������
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen);							//QSPI��������

#endif
