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
    csro_mqtt_msg_trigger_state(NULL);
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

void csro_nlight_prepare_basic_message(void)
{
    cJSON *basic_json=cJSON_CreateObject();
    cJSON *channel0 = NULL;
    cJSON *channel1 = NULL;
    cJSON *channel2 = NULL;
    
    if ( NLIGHT >= 1 ) {  nlight_add_channel_json(basic_json, channel0, &channel[0]); }
    if ( NLIGHT >= 2 ) {  nlight_add_channel_json(basic_json, channel1, &channel[1]); }
    if ( NLIGHT >= 3 ) {  nlight_add_channel_json(basic_json, channel2, &channel[2]); }

    char *out = cJSON_PrintUnformatted(basic_json);
	strcpy(mqtt.content, out);
	free(out);
	cJSON_Delete(basic_json);
}


void csro_nlight_handle_self_message(MessageData *data)
{
    char        sub_topic[50];
    csro_systen_get_self_message_sub_topic(data, sub_topic);

    if (strcmp(sub_topic, "light") == 0) 
    {
        char command[50];
        if (NLIGHT >= 1){
            if(csro_system_parse_level1_json_string(data->message->payload, command, "channel1")) {
                    if (strcmp(command, "on") == 0)     { nlight_channel_set(&channel[0], 1, 1); }
                else if (strcmp(command, "off") == 0)   { nlight_channel_set(&channel[0], 0, 1); }
            }
        }
        if (NLIGHT >= 2) {
            if(csro_system_parse_level1_json_string(data->message->payload, command, "channel2")) {
                    if (strcmp(command, "on") == 0)     { nlight_channel_set(&channel[1], 1, 1); }
                else if (strcmp(command, "off") == 0)   { nlight_channel_set(&channel[1], 0, 1); }
            }
        }
        if (NLIGHT >= 3) {
            if(csro_system_parse_level1_json_string(data->message->payload, command, "channel3")) {
                    if (strcmp(command, "on") == 0)     { nlight_channel_set(&channel[2], 1, 1); }
                else if (strcmp(command, "off") == 0)   { nlight_channel_set(&channel[2], 0, 1); }
            }
        }
    }
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