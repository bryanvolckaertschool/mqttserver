#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <mysql.h>
#define ADDRESS "tcp://broker.hivemq.com:1883"
#define CLIENTID "BVMD-SERVER"
#define TOPIC "BVMD-DATA"
// #define PAYLOAD     "Hello World!"
#define QOS 0
#define TIMEOUT 10000L
volatile MQTTClient_deliveryToken deliveredtoken;
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char *payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = message->payload;
    
    for (i = 0; i < message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }

    mysql_send(atoi(message->payload));
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}
int main(int argc, char *argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;
    MQTTClient_create(&client, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
           TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);
    do
    {
        ch = getchar();
    } while (ch != 'Q' && ch != 'q');
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
void mysql_send(int value)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "62.171.130.95";
    char *user = "root";
    char *password = "LjAU528fL)Gu^P]&"; /* set me first */
    char *database = "BVMD";
    conn = mysql_init(NULL);
    char query[80];

    /* Connect to database */
    if (!mysql_real_connect(conn, server,
                            user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    /* send SQL query */

    sprintf(query, "INSERT INTO `sensor_data`(`value`) VALUES (%d)", value);
    printf("\n%s\n", query);
    // Insert change into IO table
    if (mysql_query(conn, query))
        finish_with_error(conn);
    /* close connection */
    mysql_close(conn);
}

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}