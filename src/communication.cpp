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
    m_mqtt_client.begin(m_mqtt_host.c_str(), m_mqtt_port, m_wifi_client);
    m_mqtt_client.onMessage(m_callback);

    configTime(m_gmt_offset_sec, m_daylight_offset_sec, m_ntp_server);

    connect();
}

void Communication::pause_communication()
{
    JsonDocument status_info;

    auto raw_time = get_rawtime();
    auto time_struct = localtime(&raw_time);
    status_info["time_sent"] = raw_time;

    status_info["active_time"] = millis();
    status_info["device"] = m_client_id;
    status_info["status"] = "going offline";

    String msg;
    serializeJson(status_info, msg);
    publish("status", msg);

    delay(1000);
    if (m_mqtt_client.connected())
    {
        m_mqtt_client.disconnect();
    }

    delay(1000);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    Serial.println("Communication paused.");
}

void Communication::resume_communication()
{
    if(WiFi.status() == WL_CONNECTED){
        return;
    }

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

    if (!m_mqtt_client.connected())
    {
        connect();
    }

    m_connection_time = millis();

    JsonDocument status_info;

    auto raw_time = get_rawtime();
    auto time_struct = localtime(&raw_time);
    status_info["time_sent"] = raw_time;

    status_info["active_time"] = millis();
    status_info["device"] = m_client_id;
    status_info["status"] = "back online";

    String msg;
    serializeJson(status_info, msg);
    publish("status", msg);

    Serial.println("Communication resumed.");
}

void Communication::hold_connection()
{
    m_hold_connection = true;
    Serial.println("Connection holded.");
}

void Communication::release_connection()
{
    m_hold_connection = false;
    Serial.println("Connection released.");
}

void Communication::connect()
{
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("\nconnecting...");
    while (!m_mqtt_client.connect(m_client_id.c_str()))
    {
        Serial.print(".");
        delay(1000);
    }

    m_mqtt_client.subscribe("cmd/" + m_client_id);

    Serial.println("\nconnected!");

    if (m_setup)
    {
        JsonDocument status_info;

        auto raw_time = get_rawtime();
        auto time_struct = localtime(&raw_time);
        status_info["time_sent"] = raw_time;

        status_info["active_time"] = millis();
        status_info["device"] = m_client_id;
        status_info["status"] = "setup done";

        String msg;
        serializeJson(status_info, msg);
        publish("status", msg);
        m_setup = false;
    }
}

void Communication::publish(String topic, String payload, bool retain)
{
    m_mqtt_client.publish(topic + '/' + m_client_id, payload.c_str(), retain, 2);
    Serial.println("[PUB][" + topic + '/' + m_client_id + "] " + payload);
}

void Communication::handle_mqtt_loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    m_mqtt_client.loop();
    delay(10);

    if (!m_mqtt_client.connected())
    {
        connect();
    }

    if (millis() - m_connection_time > 10000 && !m_hold_connection)
    {
        pause_communication();
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

void Communication::send_data(JsonDocument sensor_data)
{
    if (!sensor_data.isNull())
    {
        String sensor_data_string;
        serializeJson(sensor_data, sensor_data_string);
        publish("data", sensor_data_string);
    }
}