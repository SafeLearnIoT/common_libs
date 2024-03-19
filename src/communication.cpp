#include "communication.h"

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
}

void Communication::publish(String topic, String payload)
{
    m_mqtt_client.publish(m_client_id + topic, payload.c_str());
}

void Communication::handle_mqtt_loop()
{
    m_mqtt_client.loop();
    delay(10);

    if (!m_mqtt_client.connected())
    {
        connect();
    }
}
