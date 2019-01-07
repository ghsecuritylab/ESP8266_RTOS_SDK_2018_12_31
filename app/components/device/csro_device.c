#include "csro_device.h"
#include "cJSON.h"


void csro_device_prepare_basic_message(void)
{
    #ifdef NLIGHT
        csro_nlight_prepare_basic_message();
    #elif defined DLIGHT
        csro_dlight_prepare_basic_message();
    #elif defined MOTOR
        csro_motor_prepare_basic_message();
    #elif defined AQI_MONITOR
        csro_air_monitor_prepare_basic_message();
    #elif defined AIR_SYSTEM
        csro_air_system_prepare_basic_message();
    #endif
}

void csro_device_prepare_timer_message(void)
{
    uint8_t timer_count = 0;
    char    key[20];
    cJSON   *alarm_json=cJSON_CreateObject();

    for(size_t i = 0; i < ALARM_NUMBER; i++)
    {
        if (alarms[i].valid == true) 
        { 
            timer_count++; 
            sprintf(key, "alarm%d", timer_count);
            uint32_t alarm_value = (((uint32_t)alarms[i].weekday) << 25) + (((uint32_t)alarms[i].minutes) << 14) + alarms[i].action;
            cJSON_AddNumberToObject(alarm_json, key, alarm_value);
        }
        else 
        {
            cJSON_AddNumberToObject(alarm_json, "count", timer_count);
            break;
        }
    }
    char *out = cJSON_PrintUnformatted(alarm_json);
	strcpy(mqtt.content, out);
	free(out);
	cJSON_Delete(alarm_json);
}

void csro_device_handle_self_message(MessageData* data)
{
    char        sub_topic[50];
    csro_systen_get_self_message_sub_topic(data, sub_topic);

    if (strcmp(sub_topic, "interval") == 0) 
    {
        uint32_t value = 0;
        if(csro_system_parse_level1_json_number(data->message->payload, &value, "value")) 
        { csro_system_set_interval(value); }
    }

    else if (strcmp(sub_topic, "system_info") == 0) 
    {
        uint32_t value = 0;
        if(csro_system_parse_level1_json_number(data->message->payload, &value, "value")) 
        { if (value == 1) { csro_mqtt_msg_trigger_system(); } }
    }

    else if (strcmp(sub_topic, "restart") == 0) 
    {
        uint32_t value = 0;
        if(csro_system_parse_level1_json_number(data->message->payload, &value, "value")) 
        { if (value == 1) { esp_restart(); } }
    }

    else if (strcmp(sub_topic, "alarm_info") == 0) 
    {
        uint32_t value = 0;
        if(csro_system_parse_level1_json_number(data->message->payload, &value, "value")) 
        { if (value == 1) {csro_mqtt_msg_trigger_alarm(); } }
    }

    else if (strcmp(sub_topic, "alarm_func") == 0) 
    {
        uint32_t index, weekday, minutes, action;
        char command[50];
        if(csro_system_parse_level1_json_string(data->message->payload, command, "command"))
        {
            if (strcmp(command, "add") == 0)
            {
                if (csro_system_parse_level1_json_number(data->message->payload, &weekday, "weekday") != true) { return; }
                if (csro_system_parse_level1_json_number(data->message->payload, &minutes, "minutes") != true) { return; }
                if (csro_system_parse_level1_json_number(data->message->payload, &action, "action") != true) { return; }
                csro_alarm_add(weekday, minutes, action);
            }
            else if (strcmp(command, "modify") == 0)
            {
                if (csro_system_parse_level1_json_number(data->message->payload, &index, "index") != true) { return; }
                if (csro_system_parse_level1_json_number(data->message->payload, &weekday, "weekday") != true) { return; }
                if (csro_system_parse_level1_json_number(data->message->payload, &minutes, "minutes") != true) { return; }
                if (csro_system_parse_level1_json_number(data->message->payload, &action, "action") != true) { return; }
                csro_alarm_modify(index, weekday, minutes, action);
            }
            else if (strcmp(command, "delete") == 0)
            {
                if (csro_system_parse_level1_json_number(data->message->payload, &index, "index") != true) { return; }
                csro_alarm_delete(index);
            }
            else if (strcmp(command, "clear") == 0)
            {
                csro_alarm_clear();
            }
            csro_mqtt_msg_trigger_alarm();
        }
    }

    else
    {
        #ifdef NLIGHT
            csro_nlight_handle_self_message(data);
        #elif defined DLIGHT
            csro_dlight_handle_self_message(data);
        #elif defined MOTOR
            csro_motor_handle_self_message(data);
        #elif defined AQI_MONITOR
            csro_air_monitor_handle_self_message(data);
        #elif defined AIR_SYSTEM
            csro_air_system_handle_self_message(data);
        #endif
    }
}

void csro_device_handle_group_message(MessageData* data)
{
    uint32_t value = 0;
    if(csro_system_parse_level1_json_number(data->message->payload, &value, "update")) 
    {
        if(value == 1) { csro_mqtt_msg_trigger_state(NULL); }
    }
}

void csro_device_alarm_action(uint16_t action)
{
    #ifdef NLIGHT
        csro_nlight_alarm_action(action);
    #elif defined DLIGHT
        csro_dlight_alarm_action(action);
    #elif defined MOTOR
        csro_motor_alarm_action(action);
    #elif defined AQI_MONITOR
        csro_air_monitor_alarm_action(action);
    #elif defined AIR_SYSTEM
        csro_air_system_alarm_action(action);
    #endif
}

void csro_device_init(void)
{
    #ifdef NLIGHT
        csro_nlight_init();
    #elif defined DLIGHT
        csro_dlight_init();
    #elif defined MOTOR
        csro_motor_init();
    #elif defined AQI_MONITOR
        csro_air_monitor_init();
    #elif defined AIR_SYSTEM
        csro_air_system_init();
    #endif
}