/**
 *!
 * \file        b_device.h
 * \version     v0.0.1
 * \date        2019/06/05
 * \author      Bean(notrynohigh@outlook.com)
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2019 Bean
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************
 */
#ifndef __B_DEVICE_H__
#define __B_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*Includes ----------------------------------------------*/
#include <stdint.h>
#include <string.h>
/**
 * \addtogroup BABYOS
 * \{
 */

/**
 * \addtogroup CORE
 * \{
 */

/**
 * \addtogroup DEVICE
 * \{
 */

/**
 * \defgroup DEVICE_Exported_TypesDefinitions
 * \{
 */
#define B_DEVICE_REG(dev, driver, desc)
#include "b_device_list.h"

typedef enum
{
#define B_DEVICE_REG(dev, driver, desc) dev,
#include "b_device_list.h"
    bDEV_NULL,
    bDEV_MAX_NUM
} bDeviceName_t;

typedef union
{
    uint32_t v;
    void    *_p;
} bDeviceMsg_t;

/**
 * \}
 */

/**
 * \defgroup DEVICE_Exported_Functions
 * \{
 */
int bDeviceInit(void);
int bDeviceReinit(uint8_t no);
int bDeviceOpen(uint8_t no);
int bDeviceClose(uint8_t no);
int bDeviceRead(uint8_t no, uint32_t offset, uint8_t *pdata, uint16_t len);
int bDeviceWrite(uint8_t no, uint32_t offset, uint8_t *pdata, uint16_t len);
int bDeviceCtl(uint8_t no, uint8_t cmd, void *param);
int bDeviceISNormal(uint8_t no);
int bDeviceModifyHalIf(uint8_t no, uint32_t offset, const uint8_t *pVal, uint8_t size);
int bDeviceReadMessage(uint8_t no, bDeviceMsg_t *pmsg);
int bDeviceWriteMessage(uint8_t no, bDeviceMsg_t *pmsg);
/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif

/************************ Copyright (c) 2019 Bean *****END OF FILE****/
