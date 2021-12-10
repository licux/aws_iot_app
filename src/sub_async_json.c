#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <jansson.h>

#include "mqtt_var.h"

int message_receive(void *context, char *topicName, int topiclen, MQTTClient_message *message){
    printf("Message received! Topic[%s]\n", topicName);

    json_t *json = json_loads(message->payload, 0, NULL);
    printf("key=message -> %s\n", json_string_value(json_object_get(json, "message")));

 
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    json_decref(json);

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

    conn_opts.ssl = &ssl_opts;

    if((rc = MQTTClient_setCallbacks(client, NULL, NULL, message_receive, NULL)) != MQTTCLIENT_SUCCESS){
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
        int ch;
        do{
            ch = getchar();
        }while(ch != 'Q' && ch != 'q');

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