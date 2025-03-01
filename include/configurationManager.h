#include <ArduinoJson.h>
class ConfigurationManager
{
    bool m_periodicConnection;
    bool m_runMachineLearning;
    bool m_sendMachineLearningData;
    bool m_sendSensorData;
    String m_testName;

public:
    explicit ConfigurationManager(JsonDocument configuration)
    {
        if (configuration.containsKey("periodicConnection") &&
            configuration.containsKey("runMachineLearning") &&
            configuration.containsKey("sendMachineLearningData") &&
            configuration.containsKey("sendSensorData") &&
            configuration.containsKey("testName"))
        {

            m_periodicConnection = configuration["periodicConnection"].as<bool>();
            m_runMachineLearning = configuration["runMachineLearning"].as<bool>();
            m_sendMachineLearningData = configuration["sendMachineLearningData"].as<bool>();
            m_sendSensorData = configuration["sendSensorData"].as<bool>();
            m_testName = configuration["testName"].as<String>();
        }
        else
        {
            Serial.println("[Error] Configuration is not valid.");
        }
    }

    bool getPeriodicConnection()
    {
        return m_periodicConnection;
    }

    bool getRunMachineLearning()
    {
        return m_runMachineLearning;
    }

    bool getSendMLData()
    {
        return m_sendMachineLearningData;
    }

    bool getSendSensorData()
    {
        return m_sendSensorData;
    }

    String getTestName()
    {
        return m_testName;
    }

    void printStatus()
    {
        Serial.println("=====================================");
        Serial.println("Configuration Manager Status for test");
        Serial.print("[Test Name]                  ");
        Serial.println(m_testName);
        Serial.print("[Periodic Connection]        ");
        Serial.println(m_periodicConnection);
        Serial.print("[Run Machine Learning]       ");
        Serial.println(m_runMachineLearning);
        Serial.print("[Send Machine Learning Data] ");
        Serial.println(m_sendMachineLearningData);
        Serial.print("[Send Sensor Data]           ");
        Serial.println(m_sendSensorData);
        Serial.println("======================================");
    }
};