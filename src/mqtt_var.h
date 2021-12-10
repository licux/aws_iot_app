#ifndef __MQTT_VAR__
#define __MQTT_VAR__

#include "MQTTClient.h"
#include "aws_cert.h"

const char* CLIENTID = "myDevice";
const char* SUB_TOPIC = "myIoT/cmd";
const char* PUB_TOPIC = "myIoT/sensor";

const int QOS = 1;
const long TIMEOUT = 10000L;

MQTTClient_connectOptions conn_opts_initial = {
    /* strcture detail is described on "MQTTClient.h" */
    {'M', 'Q', 'T', 'C'}, // the eyecather(must be MQTC)
    8,      // version of structure
    60,     // keep alive interval
    1,      // clean session
    1,      // reliable
    NULL,   // MQTTClient_willOptions
    NULL,   // username
    NULL,   // password
    30,     // connection timeout
    0,      // retry Interval
    NULL,   // MQTTClient_SSLOptions
    0,      // the number of serverURI 
    NULL,   // serverURIs
    MQTTVERSION_DEFAULT, // MQTT version
    {NULL, 0, 0}, // Returned from the connect when the MQTT version used to connect is 3.1.1
    {0, NULL}, // binary password
    -1, // maximum number of messages in flight
    0,      // MQTT c5 clean start flag
    NULL,   // HTTP headers for websockets
    NULL,   // HTTP proxy for websockets
    NULL,   // HTTPs proxy for websockets
};

MQTTClient_SSLOptions ssl_opts_initial = {
    {'M', 'Q', 'T', 'S'}, // the eyecatcher(must be MQTS)
    3,      // struct version
    root_ca,   // trustStore (PEM format containig public digital certificates)
    certificate,   // keyStore (PEM format containing public certificate chain)
    private_key,   // private key
    NULL,   // password of private key
    NULL,   // the list of ciper suites
    1,      // true/false option to enbale verifaication of server certificate
    MQTT_SSL_VERSION_TLS_1_2, // SSL/TLS version to use
    1,      // whether to carry out post-connect checks
    ca_path,   // CA path
    NULL,   // callback function for OpenSSL error handler
    NULL,   // application-specific contex for OpenSSL error handler
    NULL,   // callback function for setting TLS-PSK options
    NULL,   //Application-specific contex for ssl_psk_cb
    0,      // disable default trust store
    NULL,   // protocol-list
    0,      // protocal length of protocol-list vector
};

#endif