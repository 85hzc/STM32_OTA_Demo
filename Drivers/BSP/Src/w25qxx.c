#include "w25qxx.h"
#include "spi.h"


/**
  * ��������: �Ӵ���Flash��ȡ����
  * �������: pBuffer����Ŷ�ȡ�����ݵ�ָ��
  *           ReadAddr����ȡ����Ŀ���ַ
  *           NumByteToRead����ȡ���ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú����������������ȡ���ݳ���
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  uint8_t cmd[4] = {W25X_ReadData,0x00,0x00,0x00};	
  uint8_t byte1;
  uint8_t byte2;
  uint8_t byte3;
	
	byte1= ((ReadAddr & 0xFF0000) >> 16);
	byte2= ((ReadAddr & 0xFF00) >> 8);
	byte3= ReadAddr & 0xFF;
      
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ON();

  /* ���� �� ָ�� */
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);	

  /* ���� �� ��ַ��λ */
  HAL_SPI_Transmit(&hspi2, &byte1, 1, W25X_TIMEOUT_VALUE);
  /* ���� �� ��ַ��λ */
  HAL_SPI_Transmit(&hspi2, &byte2, 1, W25X_TIMEOUT_VALUE);
  /* ���� �� ��ַ��λ */
  HAL_SPI_Transmit(&hspi2, &byte3, 1, W25X_TIMEOUT_VALUE);

  while (NumByteToRead--) /* ��ȡ���� */
  {
    /* ��ȡһ���ֽ�*/
		HAL_SPI_Receive(&hspi2, pBuffer, 1, W25X_TIMEOUT_VALUE);
    /* ָ����һ���ֽڻ����� */
    pBuffer++;
  }

  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_OFF();
}


/**
  * ��������: ������Ƭ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������������Flash��Ƭ�ռ�
  */
void SPI_FLASH_BulkErase(void)
{
  uint8_t cmd[4] = {W25X_ChipErase,0x00,0x00,0x00};	
  
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* ��Ƭ���� Erase */
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ON();

  /* ������Ƭ����ָ��*/
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);	

  /* ���ô���FLASH: CS�ߵ�ƽ */
  FLASH_SPI_CS_OFF();

  /* �ȴ��������*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
  * ��������: ������FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
  * �� �� ֵ: ��
  * ˵    ��������Flashÿҳ��СΪ256���ֽ�
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t cmd[4] = {W25X_PageProgram,0x00,0x00,0x00};	
	uint8_t byte1;
	uint8_t byte2;
	uint8_t byte3;
	
	byte1 = ((WriteAddr & 0xFF0000) >> 16);
	byte2 = ((WriteAddr & 0xFF00) >> 8);
	byte3 = WriteAddr & 0xFF;
	
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* Ѱ�Ҵ���FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ON();
  /* д��дָ��*/
   HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);
  /*����д��ַ�ĸ�λ*/
   HAL_SPI_Transmit(&hspi2, &byte1, 1, W25X_TIMEOUT_VALUE);
  /*����д��ַ����λ*/
  HAL_SPI_Transmit(&hspi2, &byte2, 1, W25X_TIMEOUT_VALUE);
  /*����д��ַ�ĵ�λ*/
  HAL_SPI_Transmit(&hspi2, &byte3, 1, W25X_TIMEOUT_VALUE);

  if(NumByteToWrite > W25Q64_PAGE_SIZE)
  {
     NumByteToWrite = W25Q64_PAGE_SIZE;
  }

  /* д������*/
  while (NumByteToWrite--)
  {
    /* ���͵�ǰҪд����ֽ����� */
    HAL_SPI_Transmit(&hspi2, pBuffer, 1, W25X_TIMEOUT_VALUE);
    /* ָ����һ�ֽ����� */
    pBuffer++;
  }

  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_OFF();

  /* �ȴ�д�����*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
  * ��������: ������FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú���������������д�����ݳ���
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % W25Q64_PAGE_SIZE;
  count = W25Q64_PAGE_SIZE - Addr;
  NumOfPage =  NumByteToWrite / W25Q64_PAGE_SIZE;
  NumOfSingle = NumByteToWrite % W25Q64_PAGE_SIZE;

  if (Addr == 0) /* ����ַ�� SPI_FLASH_PageSize ����  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, W25Q64_PAGE_SIZE);
        WriteAddr +=  W25Q64_PAGE_SIZE;
        pBuffer += W25Q64_PAGE_SIZE;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* ����ַ�� SPI_FLASH_PageSize ������ */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / W25Q64_PAGE_SIZE;
      NumOfSingle = NumByteToWrite % W25Q64_PAGE_SIZE;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, W25Q64_PAGE_SIZE);
        WriteAddr +=  W25Q64_PAGE_SIZE;
        pBuffer += W25Q64_PAGE_SIZE;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}


/**
  * ��������: ʹ�ܴ���Flashд����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void SPI_FLASH_WriteEnable(void)
{
  uint8_t cmd[4] = {W25X_WriteEnable,0x00,0x00,0x00};
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ON();

  /* �������дʹ�� */
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);	

  /* ���ô���Flash��CS�ߵ�ƽ */
  FLASH_SPI_CS_OFF();
}



/**
  * ��������: �ȴ�����д�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����Polls the status of the Write In Progress (WIP) flag in the
  *           FLASH's status  register  and  loop  until write  opertaion
  *           has completed.
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;
  uint8_t cmd[4] = {W25X_ReadStatusReg,0x00,0x00,0x00};

  /* Select the FLASH: Chip Select low */
	
   FLASH_SPI_CS_ON();

  /* Send "Read Status Register" instruction */
 
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);	
  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
      HAL_SPI_Receive(&hspi2, &FLASH_Status, 1, W25X_TIMEOUT_VALUE);;	 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_OFF();
}


/**
 * @brief  ����һ������
 *
 * ����һ��ɽ��������ʱ��:150ms
 *
 * @param[in] Dst_Addr ������ַ ����ʵ����������
 *
 * @return   ��
 */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{ 
  uint8_t cmd[4] = {W25X_SectorErase,0x00,0x00,0x00};
  uint8_t byte1=0;
  uint8_t byte2=0;
  uint8_t byte3=0;

  byte1 = ((SectorAddr & 0xFF0000) >> 16);
  byte2 = ((SectorAddr & 0xFF00) >> 8);
  byte3 = SectorAddr & 0xFF;
  
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();
  
  /* �ȴ�д�����*/  
  SPI_FLASH_WaitForWriteEnd();

  /* Ѱ�Ҵ���FLASH: CS�͵�ƽ */  
  FLASH_SPI_CS_ON();
  
  /* д��дָ��*/
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);	
  /*����д��ַ�ĸ�λ*/	
  HAL_SPI_Transmit(&hspi2, &byte1, 1, W25X_TIMEOUT_VALUE);	
  /*����д��ַ����λ*/  
  HAL_SPI_Transmit(&hspi2, &byte2, 1, W25X_TIMEOUT_VALUE);
  /*����д��ַ�ĵ�λ*/	
  HAL_SPI_Transmit(&hspi2, &byte3, 1, W25X_TIMEOUT_VALUE);	

  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_OFF();

  /* �ȴ�д�����*/
  SPI_FLASH_WaitForWriteEnd();
}

//#define FLASH_HEADTAIL
//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00


/**
  * ��������: ��ȡ����Flash�ͺŵ�ID
  * �������: ��
  * �� �� ֵ: uint32_t������Flash���ͺ�ID
  * ˵    ����  FLASH_ID      IC�ͺ�      �洢�ռ��С         
                0xEF3015      W25X16        2M byte
                0xEF4015	    W25Q16        4M byte
                0XEF4017      W25Q64        8M byte
                0XEF4018      W25Q128       16M byte  (YS-F1Pro������Ĭ������)
  */
uint32_t SPI_FLASH_ReadID(void)
{
  uint8_t cmd[4] = {W25X_JedecDeviceID,0x00,0x00,0x00};
  uint8_t byte1=0;
	uint8_t byte2=0;
	uint8_t byte3=0;
	uint32_t temp=0;

  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ON();

  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);	
  
  HAL_SPI_Receive(&hspi2, &byte1, 1, W25X_TIMEOUT_VALUE);
  HAL_SPI_Receive(&hspi2, &byte2, 1, W25X_TIMEOUT_VALUE);
  HAL_SPI_Receive(&hspi2, &byte3, 1, W25X_TIMEOUT_VALUE);
 
  /* ���ô���Flash��CS�ߵ�ƽ */
  FLASH_SPI_CS_OFF();
  
  temp = (byte1 << 16) | (byte2 << 8) | byte3;
  return temp;
}

/**
 * @brief  ��ȡ�豸ID
 *
 * @return   �豸ID
 */
uint8_t SPI_Flash_ReadDeviceID(void)
{  
  uint8_t cmd[4] = {W25X_DeviceID,0x00,0x00,0x00};
  uint8_t byte = 0;

  FLASH_SPI_CS_ON();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi2, cmd, 4, W25X_TIMEOUT_VALUE);	
  /* Reception of the data */
  HAL_SPI_Receive(&hspi2, &byte, 1, W25X_TIMEOUT_VALUE);
  FLASH_SPI_CS_OFF();
  
  return byte;
}


/**
 * @brief  �������ģʽ
 *
 * @return   ��
 */
void SPI_Flash_PowerDown(void)
{
  uint8_t cmd[] = {W25X_PowerDown};
  FLASH_SPI_CS_ON();
  /* Send the read status command */
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);
  FLASH_SPI_CS_OFF();
}


/**
 * @brief  Flash����
 *
 * @return   ��
 */
void SPI_Flash_WAKEUP(void)
{
  uint8_t cmd[] = {W25X_ReleasePowerDown};
  FLASH_SPI_CS_ON();
  /* Send the read status command */
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25X_TIMEOUT_VALUE);
  FLASH_SPI_CS_OFF();
}


/**
 * @brief  Flash ��ʼ��
 *
 * @return   Flash ID
 */
uint8_t SPI_Flash_Init(void)
{
  FLASH_WP_DISABLE();

  return SPI_Flash_ReadDeviceID();    
}



/**
  * @brief  �����ļ����ⲿFlash
  *
  * @param[in]    addr    Ext Flash address
  * @param[in]    data    data buffer address
  * @param[in]    data_size  data buffer size
  *
  * @return       none
  */
void WriteImage2ExtFlash(uint32_t addr, uint8_t * data, uint16_t data_size)
{
  if(((addr) & (W25Q64_SECTOR_SIZE-1))==0)
  {
    SPI_FLASH_SectorErase(addr);
  }
  else if(((addr & (W25Q64_SECTOR_SIZE-1)) + data_size) > W25Q64_SECTOR_SIZE)
  {
    SPI_FLASH_SectorErase((addr+data_size)&(~(W25Q64_SECTOR_SIZE-1)));
  }
  
  SPI_FLASH_BufferWrite(data, addr, data_size);
}


