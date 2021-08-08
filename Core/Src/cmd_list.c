
/* Includes ------------------------------------------------------------------*/
#include "cmd_list.h"
#include "stdlib.h"
#include "string.h"
#include "parson.h"
#include "usart.h"
#include "ads1115.h"

struct command_list_node *cmd_list_header;
char *CMD_ENUM[] =
{
    "OK", //0
    "EA", //CMD_ADD_FAILED
    "EM", //CMD_MEM_ERR
    "EP", //
};

static int do_reset(void *arg)
{
		printf("I will reset after 5 seconds !!!\r\n");
		for (int i = 5; i > 0; i--)
		{
				printf("%d\r\n", i);
				HAL_Delay(1000);
		}
		NVIC_SystemReset();
    return CMD_OK;
}

static int do_water(void *arg)
{
    //printf("do comd do_water \n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_Delay(6000);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    return CMD_OK;
}

static int get_adc(void *arg)
{
    double voltage[4];
    char * adc_name[4] = {"adc0", "adc1", "adc2", "adc3"};

    voltage[0] = ads1115_get_voltage_val(hi2c1, 0x01, ADC0_SINGLE_MODE, CONFIG_REG_L);
    voltage[1] = ads1115_get_voltage_val(hi2c1, 0x01, ADC1_SINGLE_MODE, CONFIG_REG_L);
    voltage[2] = ads1115_get_voltage_val(hi2c1, 0x01, ADC2_SINGLE_MODE, CONFIG_REG_L);
    voltage[3] = ads1115_get_voltage_val(hi2c1, 0x01, ADC3_SINGLE_MODE, CONFIG_REG_L);

    /*Create json object*/
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    char *serialized_string = NULL;

    json_object_set_string(root_object, "device", "humi");

    for(int i = 0; i < 4; i++)
    {
        json_object_set_number(root_object, adc_name[i], voltage[i]);
    }

    serialized_string = json_serialize_to_string(root_value);
    //printf("%04d%s", strlen(serialized_string), serialized_string);
		printf("%s", serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    return CMD_OK;
}

static const struct command_opertation operation_ids[] =
{
    {.cmd = "water", .action = do_water},
    {.cmd = "adc", .action = get_adc},
		{.cmd = "reset", .action = do_reset}
};


struct
command_list_node * CMD_LIST_NODE_INIT(void)
{
    struct command_list_node *node = NULL;
    node = (struct command_list_node *)malloc(sizeof(struct command_list_node));

    if (NULL == node)
        return NULL;

    node->next = NULL;
    return node;
}

/*Add command*/
int CMD_ADD_TAIL(uint8_t *data, uint16_t Size)
{
    struct command_list_node *cmd_node = NULL;
    JSON_Value *root_value = NULL;
    JSON_Object *root_object = NULL;
    const char * cmd_content = NULL;
    struct command_list_node *p = cmd_list_header;

    root_value = json_parse_string((const char *)data);

    if(NULL == root_value)
        return CMD_PARSON_ERR;

    cmd_node = CMD_LIST_NODE_INIT();

    if(NULL == cmd_node)// malloc failed
        goto MEM_ERR;

    root_object = json_value_get_object(root_value);
    cmd_content = json_object_get_string(root_object, "cmd");
    strncpy(cmd_node->cmd, cmd_content, CMD_LEN);

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

/*command consumer*/
void CMD_JUST_DO_ONE()
{
    struct command_list_node *p = cmd_list_header;

    if (p->next)//there's commands
    {
        p = p->next;

        for(int i = 0; i < ARRAY_SIZE(operation_ids); i++)
        {
            if (!strcmp(p->cmd, operation_ids[i].cmd))
                operation_ids[i].action(NULL);
        }

        cmd_list_header->next = p->next;
        free(p);
    }
}

void CMD_Show_List(void)
{
    struct command_list_node *p = cmd_list_header;

    while(p->next)
    {
        p = p->next;
        printf("cmd: %s \n", p->cmd);
    }
}

char *CMD_Get_Error(int err)
{
    return CMD_ENUM[err];
}

/*Delete command*/
