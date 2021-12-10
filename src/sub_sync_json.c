#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <jansson.h>
#include "mqtt_var.h"

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

    int rc;
    /* MQTTCLIENT_PERSISTENCE_NONE -> memory-based persistence mechanism */
    if((rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS){
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    
    conn_opts.ssl = &ssl_opts;

    if((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }

    if((rc = MQTTClient_subscribe(client, SUB_TOPIC, QOS) != MQTTCLIENT_SUCCESS)){
        printf("Failed to subscribe, return code %d\n", rc);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }

    char* topicName;
    int topicLen;
    MQTTClient_message *message;
    printf("Waiting to receive message...\n");
    if((rc = MQTTClient_receive(client, &topicName, &topicLen, &message, TIMEOUT)) != MQTTCLIENT_SUCCESS){
        printf("Failed to receive, return code %d\n", rc);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }
    printf("Message received! Topic[%s]\n", topicName);
    printf("Received message payload:\n");
    printf("%.*s\n", message->payloadlen, (char*)message->payload);
    json_t *json = json_loads(message->payload, 0, NULL);
    printf("key=message -> %s\n", json_string_value(json_object_get(json, "message")));
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    

    json_decref(json);

    if((rc = MQTTClient_disconnect(client, TIMEOUT)) != MQTTCLIENT_SUCCESS){
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

    MQTTClient_destroy(&client);
    return rc;
}