#include "communication.h"

Communication *Communication::m_communication = nullptr;

Communication *Communication::get_instance(
    const String &wifi_ssid,
    const String &wifi_password,
    const String &client_id,
    const String &mqtt_host,
    const int mqtt_port,
    MQTTClientCallbackSimple callback)
{
    if (m_communication == nullptr)
    {
        m_communication = new Communication(wifi_ssid, wifi_password, client_id, mqtt_host, mqtt_port, callback);
    }
    return m_communication;
}

Communication *Communication::get_instance()
{
    return m_communication;
}

void Communication::setup()
{
    // Check params
    if (m_wifi_ssid.isEmpty())
    {
        Serial.println("[Error] m_wifi_ssid is empty.");
    }
    if (m_wifi_ssid.isEmpty())
    {
        Serial.println("[Error] m_wifi_ssid is empty.");
    }
    if (m_wifi_password.isEmpty())
    {
        Serial.println("[Error] m_wifi_password is empty.");
    }
    if (m_client_id.isEmpty())
    {
        Serial.println("[Error] m_client_id is empty.");
    }
    if (m_mqtt_host.isEmpty())
    {
        Serial.println("[Error] m_mqtt_host is empty.");
    }
    if (m_callback == nullptr)
    {
        Serial.println("[Error] m_callback is nullptr.");
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(m_wifi_ssid.c_str(), m_wifi_password.c_str());
#ifndef NO_MQTT
    m_mqtt_client.begin(m_mqtt_host.c_str(), m_mqtt_port, m_wifi_client);
    m_mqtt_client.onMessage(m_callback);
#endif
    configTime(m_gmt_offset_sec, m_daylight_offset_sec, m_ntp_server);

    connect();
}

void Communication::pause_communication()
{
    // Optional: publish a going-offline message
    JsonDocument status_info;
    status_info["time"] = millis();
    status_info["device"] = m_client_id;
    status_info["status"] = "going offline";

    String msg;
    serializeJson(status_info, msg);
    publish("status", msg);

    delay(2000);
    // Disconnect the MQTT client
    if (m_mqtt_client.connected())
    {
        m_mqtt_client.disconnect();
    }

    delay(2000);
    // Turn off the WiFi
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    Serial.println("Communication paused.");
}

void Communication::resume_communication()
{
#ifdef OFFLINE
    return;
#endif
    // Turn on WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(m_wifi_ssid.c_str(), m_wifi_password.c_str());

    // Wait for WiFi connection
    Serial.print("Reconnecting WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    delay(5000);

#ifndef NO_MQTT
    // Reconnect MQTT client
    if (!m_mqtt_client.connected())
    {
        connect();
    }
#endif

    // Optional: publish a back-online message
    JsonDocument status_info;
    status_info["time"] = millis();
    status_info["device"] = m_client_id;
    status_info["status"] = "back online";

    String msg;
    serializeJson(status_info, msg);
    publish("status", msg);

    Serial.println("Communication resumed.");
}

void Communication::connect()
{
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

#ifndef NO_MQTT
    Serial.print("\nconnecting...");
    while (!m_mqtt_client.connect(m_client_id.c_str()))
    {
        Serial.print(".");
        delay(1000);
    }
#endif

    Serial.println("\nconnected!");
    if (m_setup)
    {
        JsonDocument status_info;
        status_info["time"] = millis();
        status_info["device"] = m_client_id;
        status_info["status"] = "setup done";

        String msg;
        serializeJson(status_info, msg);
        publish("status", msg);
        m_setup = false;
    }
}

void Communication::publish(String topic, String payload)
{
#ifndef NO_MQTT
    m_mqtt_client.publish(topic + '/' + m_client_id, payload.c_str(), false, 2);
#endif
    Serial.print("Pub attempt on topic: ");
    Serial.println(topic);
}

void Communication::handle_mqtt_loop()
{
#ifdef NO_MQTT
    return;
#endif
    if (!WiFi.getSleep())
    {
        m_mqtt_client.loop();
        delay(10);

        if (!m_mqtt_client.connected())
        {
            connect();
        }
    }
}

time_t Communication::get_rawtime()
{
    time_t rawtime;
    return time(&rawtime);
}

tm *Communication::get_localtime()
{
    auto raw_time = get_rawtime();
    return localtime(&raw_time);
}

String Communication::get_datetime_string()
{
    auto raw_time = get_rawtime();
    auto local_time = localtime(&raw_time);
    char buff[200];
    strftime(buff, 200, "%Y-%m-%d_%H_%M_%S", local_time);
    return buff;
}

String Communication::get_todays_date_string()
{
    auto raw_time = get_rawtime();
    auto local_time = localtime(&raw_time);
    char buff[200];
    strftime(buff, 200, "%Y_%m_%d", local_time);
    return buff;
}

String Communication::get_yesterdays_date_string()
{
    auto raw_time = get_rawtime();
    auto local_time = localtime(&raw_time);
    local_time->tm_mday -= 1;
    mktime(local_time);
    char buff[200];
    strftime(buff, 200, "%Y_%m_%d", local_time);
    return buff;
}

String Communication::get_client_id()
{
    return m_client_id;
}

void Communication::send_data(JsonDocument sensor_data, JsonDocument ml_data)
{
#ifndef NO_MQTT
    resume_communication();
#endif

    if (!sensor_data.isNull())
    {
        String sensor_data_string;
        serializeJson(sensor_data, sensor_data_string);
        publish("data", sensor_data_string);
    }

    if (!ml_data.isNull())
    {
        String ml_data_string;
        serializeJson(ml_data, ml_data_string);
        publish("ml", ml_data_string);
    }
#ifndef NO_MQTT
    pause_communication();
#endif
}