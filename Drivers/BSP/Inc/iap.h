#ifndef __IAP_H__
#define __IAP_H__

#include <stdint.h>
#include "board.h"

// �汾��
#define __BOOT_VERSION__        0, 0, 0, 2
#define __APP_VERSION__         0, 0, 1, 2
#define __HW_VERSION__          0, 0, 2, 0

// �̼���ʼ��ַ��ƫ����
#define APP_CODE_MAX_SIZE       0x20000 // Max 128K
#define APP_VECT_OFFSET         0x04000
#define APP_IMAGE_START         (FLASH_BASE | APP_VECT_OFFSET)

// ���ú�ʹ���ж�
#define JUMP_ENTER_CRITICAL()   __disable_irq()
#define JUMP_EXIT_CRITICAL()    __enable_irq()

#endif

