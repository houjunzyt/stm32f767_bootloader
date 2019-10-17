#include "qspi.h"
#include "delay.h"

QSPI_HandleTypeDef QSPI_Handler;    //QSPI���
    
/*****************************************************************
*�������ܣ�QSPI��ʼ��
*�����������
*����ֵ  ����
*****************************************************************/
uint8_t QSPI_Init(void)
{
    QSPI_Handler.Instance=QUADSPI;                          //QSPI
    QSPI_Handler.Init.ClockPrescaler=2;                     //QPSI��Ƶ�ȣ�W25Q256���Ƶ��Ϊ104M��
                                                            //���Դ˴�Ӧ��Ϊ2��QSPIƵ�ʾ�Ϊ216/(2+1)=72MHZ
    QSPI_Handler.Init.FifoThreshold=4;                      //FIFO��ֵΪ4���ֽ�
    QSPI_Handler.Init.SampleShifting=QSPI_SAMPLE_SHIFTING_HALFCYCLE;//������λ�������(DDRģʽ��,��������Ϊ0)
    QSPI_Handler.Init.FlashSize=POSITION_VAL(0X2000000)-1;  //SPI FLASH��С��W25Q256��СΪ32M�ֽ�
    QSPI_Handler.Init.ChipSelectHighTime=QSPI_CS_HIGH_TIME_4_CYCLE;//Ƭѡ�ߵ�ƽʱ��Ϊ4��ʱ��(13.8*4=55.2ns),���ֲ������tSHSL����
    QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_0;          //ģʽ0
    QSPI_Handler.Init.FlashID=QSPI_FLASH_ID_1;              //��һƬflash
    QSPI_Handler.Init.DualFlash=QSPI_DUALFLASH_DISABLE;     //��ֹ˫����ģʽ
    if(HAL_QSPI_Init(&QSPI_Handler)==HAL_OK) return 0;      //QSPI��ʼ���ɹ�
    else return 1;
}

/*****************************************************************
*�������ܣ�QSPI�ײ�����,�������ã�ʱ��ʹ��,�˺����ᱻHAL_QSPI_Init()����
*���������hqspi:QSPI���
*����ֵ  ����
*****************************************************************/
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_QSPI_CLK_ENABLE();        //ʹ��QSPIʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();       //ʹ��GPIOBʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();       //ʹ��GPIOFʱ��
    
    //��ʼ��PB6 Ƭѡ�ź�
    GPIO_Initure.Pin=GPIO_PIN_6;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //����
    GPIO_Initure.Pull=GPIO_PULLUP;              
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
    GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;   //����ΪQSPI
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //PF8,9
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;
    GPIO_Initure.Pull=GPIO_NOPULL;              
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    
    //PB2
    GPIO_Initure.Pin=GPIO_PIN_2;
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;   //����ΪQSPI
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //PF6,7
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
}

/*****************************************************************
*�������ܣ�QSPI��������
*���������
        instruction:Ҫ���͵�ָ��
        address:���͵���Ŀ�ĵ�ַ
        dummyCycles:��ָ��������
        instructionMode:ָ��ģʽ;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE  
        addressMode:��ַģʽ; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
        dataMode:����ģʽ; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE
*����ֵ  ����
*****************************************************************/
void QSPI_Send_CMD(uint32_t instruction,uint32_t address,uint32_t dummyCycles,uint32_t instructionMode,uint32_t addressMode,uint32_t addressSize,uint32_t dataMode)
{
    QSPI_CommandTypeDef Cmdhandler;
    
    Cmdhandler.Instruction=instruction;                 	//ָ��
    Cmdhandler.Address=address;                            	//��ַ
    Cmdhandler.DummyCycles=dummyCycles;                     //���ÿ�ָ��������
    Cmdhandler.InstructionMode=instructionMode;				//ָ��ģʽ
    Cmdhandler.AddressMode=addressMode;   					//��ַģʽ
    Cmdhandler.AddressSize=addressSize;   					//��ַ����
    Cmdhandler.DataMode=dataMode;             				//����ģʽ
    Cmdhandler.SIOOMode=QSPI_SIOO_INST_EVERY_CMD;       	//ÿ�ζ�����ָ��
    Cmdhandler.AlternateByteMode=QSPI_ALTERNATE_BYTES_NONE; //�޽����ֽ�
    Cmdhandler.DdrMode=QSPI_DDR_MODE_DISABLE;           	//�ر�DDRģʽ
    Cmdhandler.DdrHoldHalfCycle=QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command(&QSPI_Handler,&Cmdhandler,5000);
}

/*****************************************************************
*�������ܣ�QSPI����ָ�����ȵ�����
*���������
    buf:�������ݻ������׵�ַ
    datalen:Ҫ��������ݳ���
*����ֵ  ��0,���� ����,�������
*****************************************************************/
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                           //�������ݳ���
    if(HAL_QSPI_Receive(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //��������
    else return 1;
}

/*****************************************************************
*�������ܣ�QSPI����ָ�����ȵ�����
*���������
        buf:�������ݻ������׵�ַ
        datalen:Ҫ��������ݳ���
*����ֵ  ��0,���� ����,�������
*****************************************************************/
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                            //�������ݳ���
    if(HAL_QSPI_Transmit(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //��������
    else return 1;
}
