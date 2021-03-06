#include "csro_device.h"
#include "cJSON.h"

#ifdef NLIGHT

static csro_switch      channel[3];
static TimerHandle_t	key_timer = NULL;
static TimerHandle_t	led_timer = NULL;


static void nlight_channel_set(csro_switch *ch, uint8_t value, uint8_t source)
{
    if((ch->index + 1 > NLIGHT) || (value > 2)) { return; }
    if(value == 2) { value = !ch->state; }
    ch->state = value;
    if(ch->state == 1)  { ch->on_source = source; ch->on_tim = datetime.time_now; }
    else                { ch->off_source = source; ch->off_tim = datetime.time_now; }
    csro_mqtt_msg_trigger_status(NULL);
}

static void nlight_add_channel_json(cJSON *target, cJSON *channel_json, csro_switch *channel)
{
	char key[20];
	sprintf(key, "channel%d", channel->index + 1);
	cJSON_AddItemToObject(target, key, channel_json = cJSON_CreateObject());
    cJSON_AddStringToObject(channel_json, "state", channel->state == 1 ? "on" : "off");
    cJSON_AddNumberToObject(channel_json, "onsource", channel->on_source);
    cJSON_AddNumberToObject(channel_json, "ontim", channel->on_tim);
	cJSON_AddNumberToObject(channel_json, "offsource", channel->off_source);
    cJSON_AddNumberToObject(channel_json, "offtim", channel->off_tim);
}

void csro_nlight_prepare_status_message(void)
{
    cJSON *basic_json=cJSON_CreateObject();
    cJSON *channel_json[3];
    if ( NLIGHT >= 1 ) {  nlight_add_channel_json(basic_json, channel_json[0], &channel[0]); }
    if ( NLIGHT >= 2 ) {  nlight_add_channel_json(basic_json, channel_json[1], &channel[1]); }
    if ( NLIGHT >= 3 ) {  nlight_add_channel_json(basic_json, channel_json[2], &channel[2]); }
    char *out = cJSON_PrintUnformatted(basic_json);
	strcpy(mqtt.content, out);
	free(out);
	cJSON_Delete(basic_json);
}


void csro_nlight_handle_self_message(MessageData *data)
{
    uint32_t value = 0;
    bool valid = false;
    if (NLIGHT >= 1) { if(csro_system_parse_level1_json_number(data->message->payload, &value, "channel1")) { nlight_channel_set(&channel[0], value, 1); valid = true; } }
    if (NLIGHT >= 2) { if(csro_system_parse_level1_json_number(data->message->payload, &value, "channel2")) { nlight_channel_set(&channel[1], value, 1); valid = true; } }
    if (NLIGHT >= 3) { if(csro_system_parse_level1_json_number(data->message->payload, &value, "channel3")) { nlight_channel_set(&channel[2], value, 1); valid = true; } }
    if(valid == true) { csro_mqtt_msg_trigger_status(NULL); }
}

void csro_nlight_handle_hass_message(MessageData *data)
{
    char sub_topic[50];
    bool valid = false;
    char payload[200];

    strncpy(payload, data->message->payload, data->message->payloadlen);
    csro_systen_get_hass_message_sub_topic(data, sub_topic);
    
    if (strcmp(sub_topic, "channel1") == 0) 
    {   
        debug("payload:%s\n", payload);
        if (strcmp(payload, "on") == 0)          { nlight_channel_set(&channel[0], 1, 2); valid = true; }
        else if (strcmp(payload, "off") == 0)    { nlight_channel_set(&channel[0], 0, 2); valid = true; }
    }
    else if (strcmp(sub_topic, "channel2") == 0) 
    {
        debug("payload:%s\n", payload);
        if (strcmp(payload, "on") == 0)          { nlight_channel_set(&channel[1], 1, 2); valid = true; }
        else if (strcmp(payload, "off") == 0)    { nlight_channel_set(&channel[1], 0, 2); valid = true; }
    }
    else if (strcmp(sub_topic, "channel3") == 0) 
    {
        debug("payload:%s\n", payload);
        if (strcmp(payload, "on") == 0)          { nlight_channel_set(&channel[2], 1, 2); valid = true; }
        else if (strcmp(payload, "off") == 0)    { nlight_channel_set(&channel[2], 0, 2); valid = true; }
    }
    if(valid == true) { csro_mqtt_msg_trigger_status(NULL); }
}

void csro_nlight_alarm_action(uint16_t action)
{

}


static void key_timer_callback( TimerHandle_t xTimer )
{

}


static void led_timer_callback( TimerHandle_t xTimer )
{

}


void csro_nlight_init(void)
{
    for(size_t i = 0; i < NLIGHT; i++) { channel[i].index = i; }

    key_timer = xTimerCreate("key_timer", 25/portTICK_RATE_MS, pdTRUE, (void *)0, key_timer_callback);
    led_timer = xTimerCreate("led_timer", 25/portTICK_RATE_MS, pdTRUE, (void *)0, led_timer_callback);
    if (key_timer != NULL) { xTimerStart(key_timer, 0); }
    if (led_timer != NULL) { xTimerStart(led_timer, 0); }
}

#endif