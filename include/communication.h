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

#define NO_PUBLISH

enum MLAlgo
{
    None,
    rTPNN,
    LinReg,
    LogReg
};

inline auto ml_algo = MLAlgo::LinReg;

class Communication
{
protected:
    String m_wifi_ssid = "";
    String m_wifi_password = "";
    String m_client_id = "";
    String m_mqtt_host = "";
    int m_mqtt_port = 1883;
    bool m_setup = true;
    MLAlgo m_ml_algo = ml_algo;
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

    static Communication *m_communication;

    const char *m_ntp_server = "pool.ntp.org";
    const long m_gmt_offset_sec = 7200;  // Replace with your GMT offset (seconds)
    const int m_daylight_offset_sec = 0; // Replace with your daylight offset (seconds)

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

    void send_data(JsonDocument sensor_data, JsonDocument ml_data);

    String get_datetime_string();
    String get_todays_date_string();
    String get_yesterdays_date_string();
    String get_client_id();
    time_t get_rawtime();
    tm *get_localtime();

    MLAlgo get_ml_algo();
};

#endif