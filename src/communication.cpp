#include "communication.h"

Communication *Communication::communication_ = nullptr;

Communication *Communication::get_instance(const String &wifi_ssid, const String &wifi_password, const String &client_id, const String &mqtt_host, const int mqtt_port, MQTTClientCallbackSimple callback)
{
    if (communication_ == nullptr)
    {
        communication_ = new Communication(wifi_ssid, wifi_password, client_id, mqtt_host, mqtt_port, callback);
    }
    return communication_;
}

Communication *Communication::get_instance()
{
    return communication_;
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
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    connect();
}

void Communication::pause_communication()
{
    // Optional: publish a going-offline message
    auto msg = String("going offline;");
    msg += String(millis());
    publish("status", msg);

    delay(5000);

    // Disconnect the MQTT client
    if (m_mqtt_client.connected())
    {
        m_mqtt_client.disconnect();
    }

    delay(5000);

    // Turn off the WiFi
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    Serial.println("Communication paused.");
}

void Communication::resume_communication()
{
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

    // Reconnect MQTT client
    if (!m_mqtt_client.connected())
    {
        connect();
    }

    // Optional: publish a back-online message
    auto msg = String("back online;");
    msg += String(millis());
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

    Serial.print("\nconnecting...");
    while (!m_mqtt_client.connect(m_client_id.c_str()))
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");
    if (m_setup)
    {
        auto msg = String("setup done;");
        msg += String(millis());
        publish("status", msg);
        m_setup = false;
    }
}

void Communication::publish(String topic, String payload)
{
    m_mqtt_client.publish(m_client_id + topic, payload.c_str(), false, 2);
}

void Communication::handle_mqtt_loop()
{
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

bool Communication::is_older_than_five_days(String file_name)
{
    int file_year = file_name.substring(0, 4).toInt();
    int file_month = file_name.substring(5, 7).toInt();
    int file_day = file_name.substring(8, 10).toInt();

    struct tm fileTime = {0};
    fileTime.tm_year = file_year - 1900; // tm_year is years since 1900
    fileTime.tm_mon = file_month - 1;    // tm_mon is 0-based
    fileTime.tm_mday = file_day;

    time_t fileEpoch = mktime(&fileTime); // Convert to time_t (seconds since Epoch)

    double diff = difftime(get_rawtime(), fileEpoch) / (60 * 60 * 24);

    return diff > 5;
}

// This is my communication class methods
// How to implement pause_communication and resume_communication?
// I want to have system that turns off communication via mqtt and wifi and enables it when needed