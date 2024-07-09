#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#ifdef ESP8266
#include "ESP8266WiFi.h"
#else
#include "WiFi.h"
#endif

#include <Arduino.h>
#include <ArduinoJson.h>
#include "time.h"
#include "MQTT.h"
#include <utility>
#include <vector>

class Communication
{
protected:
    String m_wifi_ssid = "";
    String m_wifi_password = "";
    String m_client_id = "";
    String m_mqtt_host = "";
    int m_mqtt_port = 1883;
    bool m_setup = true;
    MQTTClientCallbackSimple m_callback = nullptr;

    Communication(
        const String &wifi_ssid,
        const String &wifi_password,
        const String &client_id,
        const String &mqtt_host,
        const int mqtt_port,
        MQTTClientCallbackSimple callback) : m_wifi_ssid(wifi_ssid),
                                             m_wifi_password(wifi_password),
                                             m_client_id(client_id),
                                             m_mqtt_host(mqtt_host),
                                             m_mqtt_port(mqtt_port),
                                             m_callback(callback)
    {
    }

    static Communication *communication_;

    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 7200;  // Replace with your GMT offset (seconds)
    const int daylightOffset_sec = 0; // Replace with your daylight offset (seconds)

    MQTTClient m_mqtt_client;
    WiFiClient m_wifi_client;

    tm *get_time();
    void connect();

public:
    Communication(Communication &other) = delete;
    void operator=(const Communication &) = delete;
    static Communication *get_instance(const String &wifi_ssid,
                                       const String &wifi_password,
                                       const String &client_id,
                                       const String &mqtt_host,
                                       const int mqtt_port,
                                       MQTTClientCallbackSimple callback);
    static Communication *get_instance();

    void setup();
    void pause_communication();
    void resume_communication();
    void publish(String topic, String payload);
    void handle_mqtt_loop();

    void send_data(JsonDocument sensor_data, JsonDocument rtpnn_data, JsonDocument reglin_data);

    String get_datetime_string();
    String get_todays_date_string();
    String get_yesterdays_date_string();
    String get_client_id();
    time_t get_rawtime();
    tm *get_localtime();
};

#endif