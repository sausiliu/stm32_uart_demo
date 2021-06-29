/**
  ******************************************************************************
  * @file    cmd_list.h
  * @brief   This file contains all the function prototypes for
  *          the cmd_list.c file
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CMD_LIST_H__
#define __CMD_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define CMD_ERROR (uint8_t *)"E"
#define CMD_OK          0
#define CMD_ADD_FAILED  1
#define CMD_MEM_ERR     2

#define CMD_LEN         10

extern struct command_list_node *cmd_list_header;

struct command_list_node
{
    struct command_list_node *next;
    char cmd[CMD_LEN];
};

struct command_opertation
{
    char cmd[CMD_LEN];
    int (*action)(void *arg);
};

int CMD_LIST_NODE_INIT(struct command_list_node *header);
int CMD_ADD_TAIL(uint8_t *data, uint16_t Size);

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
