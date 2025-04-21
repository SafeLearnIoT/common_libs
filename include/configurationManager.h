#include <ArduinoJson.h>
class ConfigurationManager
{
    bool m_periodicConnection;
    bool m_runMachineLearning;
    bool m_machineLearningTrainingMode;
    bool m_sendMachineLearningData;
    bool m_getMachineLearningParamsFromGateway;
    bool m_sendSensorData;
    int m_actionInterval;
    String m_testName;

public:
    explicit ConfigurationManager(JsonDocument configuration)
    {
        if (configuration.containsKey("periodicConnection") &&
            configuration.containsKey("runMachineLearning") &&
            configuration.containsKey("machineLearningTrainingMode") &&
            configuration.containsKey("getMachineLearningParamsFromGateway") &&
            configuration.containsKey("sendMachineLearningData") &&
            configuration.containsKey("sendSensorData") &&
            configuration.containsKey("actionInterval") &&
            configuration.containsKey("testName"))
        {

            m_periodicConnection = configuration["periodicConnection"].as<bool>();
            m_runMachineLearning = configuration["runMachineLearning"].as<bool>();
            m_sendMachineLearningData = configuration["sendMachineLearningData"].as<bool>();
            m_sendSensorData = configuration["sendSensorData"].as<bool>();
            m_machineLearningTrainingMode = configuration["machineLearningTrainingMode"].as<bool>();
            m_getMachineLearningParamsFromGateway = configuration["getMachineLearningParamsFromGateway"].as<bool>();
            m_actionInterval = configuration["actionInterval"].as<int>();
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

    bool getMLParamsFromGateway()
    {
        return m_getMachineLearningParamsFromGateway;
    }

    bool getSendMLData()
    {
        return m_sendMachineLearningData;
    }

    bool getSendSensorData()
    {
        return m_sendSensorData;
    }

    bool getMachineLearningTrainingMode(){
        return m_machineLearningTrainingMode;
    }

    int getActionIntervalMin()
    {
        return m_actionInterval;
    }

    unsigned long getActionIntervalMillis()
    {
        return m_actionInterval * 60000;
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