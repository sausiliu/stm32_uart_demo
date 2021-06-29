
/* Includes ------------------------------------------------------------------*/
#include "cmd_list.h"
#include "stdlib.h"
#include "string.h"
#include "parson.h"
#include "usart.h"

struct command_list_node *cmd_list_header;

static int do_water(void *arg)
{
}

static const struct command_opertation operation_ids[] = 
{
    {.cmd = "water", .action = do_water},
};


int CMD_LIST_NODE_INIT(struct command_list_node *node)
{
    if(NULL == node)
        node = (struct command_list_node *)malloc(sizeof(struct command_list_node));
    
    if (NULL == node)
        return CMD_MEM_ERR;
    node->next = NULL;
}

/*Add command*/
int CMD_ADD_TAIL(uint8_t *data, uint16_t Size)
{
    struct command_list_node *cmd_node;
    JSON_Value *root_value;
    JSON_Object *root_object;
    root_value = json_parse_string((const char *)data);
    if(NULL == root_value)
        return CMD_ADD_FAILED;

    CMD_LIST_NODE_INIT(cmd_node);
    if(NULL == cmd_node)// malloc failed
        goto MEM_ERR;
    root_object = json_value_get_object(root_value);
    const char * cmd_content = json_object_get_string(root_object, "cmd");
    strncpy(cmd_node->cmd, cmd_content, CMD_LEN);
    
    struct command_list_node *p = cmd_list_header;
    while (NULL != p->next)
    {
        p = p->next;
    }
    p->next = cmd_node;
    
    json_value_free(root_value);
    return 0;
MEM_ERR:
    json_value_free(root_value);
    return CMD_MEM_ERR;
}

/*Delete command*/
