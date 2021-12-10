#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"
#include <jansson.h>
#include "mqtt_var.h"

void deliverMessage(void *context, MQTTClient_deliveryToken dt){
    printf("Message published! Token value[%d]\n", dt);
}

int message_receive(void *context, char *topicName, int topiclen, MQTTClient_message *message){
    printf("Message received! Topic[%s]\n", topicName);
    printf("Received message payload:\n");
    printf("%.*s\n", message->payloadlen, (char*)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char* argv[]){
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = conn_opts_initial;
    MQTTClient_SSLOptions ssl_opts = ssl_opts_initial;

    MQTTClient_message pubmsg = {
         /* strcture detail is described on "MQTTClient.h" */
        {'M', 'Q', 'T', 'M'}, // the eyecather(must be MQTM)
        1,      // version number of the structure
        0,      // payload lenghth in bytes
        NULL,   // payload of MQTT message (void*)
    };
    MQTTClient_deliveryToken token;
    int rc;

    /* MQTTCLIENT_PERSISTENCE_NONE -> memory-based persistence mechanism */
    if((rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    
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

    char* payload = "This is MQTT message";
    json_t *json = json_object();
    json_object_set(json, "lat", json_real((double)34.79));
    json_object_set(json, "log", json_real((double)134.67));
    char* json_string = json_dumps(json, json_object_size(json));
    // printf("%s\n", str);
    // free(str);
    
    pubmsg.payload = (void*)json_string;
    pubmsg.payloadlen = (int)strlen(json_string);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if((rc = MQTTClient_publishMessage(client, PUB_TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS){
        printf("Failed to publsh message, return code %d\n", rc);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }
    
    printf("Wait to confirm message publish...\n");
    usleep(1000000);
    printf("Wakeup!\n");


    MQTTClient_destroy(&client);
    return rc;
}


