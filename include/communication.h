#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#ifdef ESP8266
#include "ESP8266WiFi.h"
#else
#include "WiFi.h"
#endif

#include "MQTT.h"

class Communication
{
    String m_wifi_ssid = "";
    String m_wifi_password = "";
    String m_client_id = "";
    String m_mqtt_host = "";

    MQTTClientCallbackSimple m_callback = nullptr;
    int m_mqtt_port = 1883;

    MQTTClient m_mqtt_client;
    WiFiClient m_wifi_client;

    void connect();

public:
    Communication(const String &wifi_ssid, const String &wifi_password, const String &client_id, const String &mqtt_host, const int mqtt_port, MQTTClientCallbackSimple callback) : m_wifi_ssid(wifi_ssid), m_wifi_password(wifi_password), m_client_id(client_id), m_mqtt_host(mqtt_host), m_mqtt_port(mqtt_port), m_callback(callback) {}
    void setup();
    void publish(String topic, String payload);
    void handle_mqtt_loop();
};

#endif