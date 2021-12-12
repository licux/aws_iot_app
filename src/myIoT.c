#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <jansson.h>

#include "mqtt_var.h"

int message_receive(void *context, char *topicName, int topiclen, MQTTClient_message *message){
    json_t *json = json_loads(message->payload, 0, NULL);
    printf("Message received! Topic[%s], Command[%s, %d]\n", topicName, json_string_value(json_object_get(json, "command")), (int)json_integer_value(json_object_get(json, "value")));
 
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    json_decref(json);

    return 1;
}

void deliverMessage(void *context, MQTTClient_deliveryToken dt){
    // printf("Message published! Token value[%s]\n", context);
}

void publishMessage(MQTTClient *client, const char* topic, int value){
    MQTTClient_message pubmsg = {
         /* strcture detail is described on "MQTTClient.h" */
        {'M', 'Q', 'T', 'M'}, // the eyecather(must be MQTM)
        1,      // version number of the structure
        0,      // payload lenghth in bytes
        NULL,   // payload of MQTT message (void*)
    };
    MQTTClient_deliveryToken token;

    json_t *json = json_object();

    if(strcmp(topic, PUB_TOPIC_SENSOR) == 0){
        json_object_set(json, "analog_value",  json_integer(value));
    }else if(strcmp(topic, PUB_TOPIC_TRIGGER) == 0){
        json_object_set(json, "trigger", json_integer(value));
    }else{
        printf("Error! Not match topic:%s\n", topic);
    }
    char* json_string = json_dumps(json, json_object_size(json));

    pubmsg.payload = (void*)json_string;
    pubmsg.payloadlen = (int)strlen(json_string);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    int rc;
    if((rc = MQTTClient_publishMessage(client, topic, &pubmsg, &token)) != MQTTCLIENT_SUCCESS){
        printf("Failed to publsh message, return code %d\n", rc);
    }else{
        printf("Published message! topic:%s\n", topic);
        // printf("Payload...:\n");
        // printf("%s\n", json_string);
    }
    free(json_string);
    json_decref(json);
}

int main(int argc, char* argv[]){
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = conn_opts_initial;
    MQTTClient_SSLOptions ssl_opts = ssl_opts_initial;

    int rc;
    /* MQTTCLIENT_PERSISTENCE_NONE -> memory-based persistence mechanism */
    if((rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    conn_opts.ssl = &ssl_opts;

    if((rc = MQTTClient_setCallbacks(client, NULL, NULL, message_receive, deliverMessage)) != MQTTCLIENT_SUCCESS){
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        return rc;
    }

    if((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", SUB_TOPIC, CLIENTID, QOS);
    if((rc = MQTTClient_subscribe(client, SUB_TOPIC, QOS) != MQTTCLIENT_SUCCESS)){
        printf("Failed to subscribe, return code %d\n", rc);
    }else{
        // Sccess to subscribe
        char cmd[31];
        int value;
        while(1){
            printf(">");
            scanf("%s%d", cmd, &value);
            if(strcmp(cmd, "Q") == 0 || strcmp(cmd, "q") == 0){
                printf("Quit..\n");
                break;
            }else if(strcmp(cmd, "sensor") == 0){
                publishMessage(client, PUB_TOPIC_SENSOR, value);
            }else if(strcmp(cmd, "trigger") == 0){
                publishMessage(client, PUB_TOPIC_TRIGGER, value);
            }else{
                printf("Invalid Command Error!\n");
            }
        }

        if((rc = MQTTClient_unsubscribe(client, SUB_TOPIC)) != MQTTCLIENT_SUCCESS){
            printf("Failed ato unsubscribe, return code %d\n", rc);
            rc = EXIT_FAILURE;
        }
    }

    if((rc = MQTTClient_disconnect(client, TIMEOUT)) != MQTTCLIENT_SUCCESS){
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

    MQTTClient_destroy(&client);
    return rc;
}