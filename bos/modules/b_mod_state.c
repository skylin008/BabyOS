/**
 *!
 * \file        b_mod_state.c
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

/*Includes ----------------------------------------------*/
#include "modules/inc/b_mod_state.h"

#include <string.h>

#include "b_section.h"

#if (defined(_STATE_ENABLE) && (_STATE_ENABLE == 1))
/**
 * \addtogroup BABYOS
 * \{
 */

/**
 * \addtogroup MODULES
 * \{
 */

/**
 * \addtogroup STATE
 * \{
 */

/**
 * \defgroup STATE_Private_Variables
 * \{
 */
static bStateInfo_t *pbStateInfo = NULL;
static LIST_HEAD(bStateHead);
bSECTION_DEF_FLASH(b_mod_state, bStateInfo_t *);
/**
 * \}
 */

/**
 * \defgroup STATE_Private_Functions
 * \{
 */
static void _bStatePolling()
{
    bStateAttr_t     *attr = NULL;
    struct list_head *pos  = NULL;
    if (pbStateInfo != NULL)
    {
        B_SAFE_INVOKE(pbStateInfo->handler);
    }
    list_for_each(pos, &bStateHead)
    {
        attr = list_entry(pos, bStateAttr_t, list);
        if (attr->info != NULL)
        {
            B_SAFE_INVOKE(attr->info->handler);
        }
    }
}

BOS_REG_POLLING_FUNC(_bStatePolling);

static bStateAttr_t *_bStateFindAttr(const char *name)
{
    bStateAttr_t     *attr = NULL;
    struct list_head *pos  = NULL;
    list_for_each(pos, &bStateHead)
    {
        attr = list_entry(pos, bStateAttr_t, list);
        if (strcmp(attr->name, name) == 0)
        {
            return attr;
        }
    }
    return NULL;
}

static bStateInfo_t *_bStateFindInfo(bStateAttr_t *attr, uint32_t state)
{
    bStateInfo_t     *info = NULL;
    struct list_head *pos  = NULL;
    list_for_each(pos, &attr->state)
    {
        info = list_entry(pos, bStateInfo_t, list);
        if (info->state == state)
        {
            return info;
        }
    }
    return NULL;
}

/**
 * \}
 */

/**
 * \addtogroup STATE_Exported_Functions
 * \{
 */

int bStateTransfer(uint32_t state)
{
    if (pbStateInfo != NULL)
    {
        if (pbStateInfo->state == state)
        {
            return 0;
        }
    }
    bSECTION_FOR_EACH(b_mod_state, bStateInfo_t *, pptmp)
    {
        if (pptmp == NULL)
        {
            continue;
        }
        bStateInfo_t *ptmp = *pptmp;
        if (ptmp == NULL)
        {
            continue;
        }
        if (ptmp->state == state)
        {
            if (pbStateInfo != NULL)
            {
                B_SAFE_INVOKE(pbStateInfo->exit);
                B_SAFE_INVOKE(ptmp->enter, pbStateInfo->state);
            }
            else
            {
                B_SAFE_INVOKE(ptmp->enter, ptmp->state);
            }
            pbStateInfo = ptmp;
            return 0;
        }
    }
    return -1;
}

int bStateInvokeEvent(uint32_t event, void *arg)
{
    int i = 0;
    if (pbStateInfo == NULL)
    {
        return -1;
    }
    if (pbStateInfo->event_table.p_event_table == NULL || pbStateInfo->event_table.number == 0)
    {
        return -2;
    }
    for (i = 0; i < pbStateInfo->event_table.number; i++)
    {
        if (pbStateInfo->event_table.p_event_table[i].event == event)
        {
            B_SAFE_INVOKE(pbStateInfo->event_table.p_event_table[i].handler, event, arg);
            break;
        }
    }
    return 0;
}

int bGetCurrentState()
{
    if (pbStateInfo == NULL)
    {
        return -1;
    }
    return pbStateInfo->state;
}

///-----------------------------------------------------

int bStateCreate(const char *name, bStateAttr_t *attr)
{
    bStateAttr_t *attr_tmp = NULL;
    if (name == NULL || attr == NULL)
    {
        return -1;
    }
    attr_tmp = _bStateFindAttr(name);
    if (attr_tmp == NULL)
    {
        attr->name = name;
        attr->info = NULL;
        list_add(&attr->list, &bStateHead);
        INIT_LIST_HEAD(&attr->state);
        return 0;
    }
    return -1;
}

int bStateAdd(const char *name, bStateInfo_t *pinfo)
{
    bStateAttr_t *attr = NULL;
    if (name == NULL || pinfo == NULL)
    {
        return -1;
    }
    attr = _bStateFindAttr(name);
    if (attr == NULL)
    {
        return -1;
    }
    if (_bStateFindInfo(attr, pinfo->state) == NULL)
    {
        list_add(&pinfo->list, &attr->state);
        return 0;
    }
    return -1;
}

int bStateTransferExt(const char *name, uint32_t state)
{
    bStateAttr_t *attr = NULL;
    bStateInfo_t *info = NULL;
    if (name == NULL)
    {
        return -1;
    }
    attr = _bStateFindAttr(name);
    if (attr == NULL)
    {
        return -1;
    }

    info = _bStateFindInfo(attr, state);
    if (info == NULL)
    {
        return -1;
    }

    if (attr->info == NULL)
    {
        B_SAFE_INVOKE(info->enter, info->state);
    }
    else if (attr->info->state == state)
    {
        ;
    }
    else
    {
        B_SAFE_INVOKE(attr->info->exit);
        B_SAFE_INVOKE(info->enter, attr->info->state);
    }
    attr->info = info;
    return 0;
}

int bStateInvokeEventExt(const char *name, uint32_t event, void *arg)
{
    bStateAttr_t *attr = NULL;
    int           i    = 0;
    if (name == NULL)
    {
        return -1;
    }
    attr = _bStateFindAttr(name);
    if (attr == NULL)
    {
        return -1;
    }
    if (attr->info == NULL)
    {
        return -1;
    }

    if (attr->info->event_table.p_event_table == NULL || attr->info->event_table.number == 0)
    {
        return -2;
    }

    for (i = 0; i < attr->info->event_table.number; i++)
    {
        if (attr->info->event_table.p_event_table[i].event == event)
        {
            B_SAFE_INVOKE(attr->info->event_table.p_event_table[i].handler, event, arg);
            break;
        }
    }
    return 0;
}

int bGetCurrentStateExt(const char *name)
{
    bStateAttr_t *attr = NULL;
    if (name == NULL)
    {
        return -1;
    }
    attr = _bStateFindAttr(name);
    if (attr == NULL)
    {
        return -1;
    }
    if (attr->info == NULL)
    {
        return -1;
    }
    return attr->info->state;
}

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
#endif
/************************ Copyright (c) 2019 Bean *****END OF FILE****/
