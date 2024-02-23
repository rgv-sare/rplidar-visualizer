#pragma once

#include <vector>
#include <string>
#include <thread>

#include "sl_lidar.h"
#include "sl_lidar_driver.h"

class LIDARFrameGrabber
{
public:
    typedef sl_lidar_response_device_health_t LIDARHealth;
    typedef sl_lidar_response_device_info_t LIDARInfo;

    struct Node
    {
        float angle = 0.0f;
        float distance = 0.0f;
    };

    enum Status
    {
        OK,
        PENDING,
        ERROR,
        IDLE
    };

    LIDARFrameGrabber(std::string port);

    void start();
    void stop();

    const std::string& getPort() const;
    const std::string& getStatusMessage() const;
    const std::string& getSerialNumber() const;
    const std::string& getFirmwareVersion() const;
    const std::string& getHardwareVersion() const;

    bool isConnected() const;
    float getFPS() const;

    const std::vector<Node>& getNodes() const;
    const Node& longestNode() const;

    Status getStatus() const;
    LIDARHealth getHealth();
    LIDARInfo getInfo();

    static std::vector<std::string> getAvailableDevices();
private:
    std::string m_port;
    Status m_status;
    std::string m_message;
    std::thread m_thread;
    float m_fps;
    double m_lastFrameTime;

    LIDARHealth m_health;
    LIDARInfo m_info;
    sl::IChannel* m_channel;
    std::vector<Node> m_nodes;
    Node m_longestNode;

    std::string m_serialNumber;
    std::string m_firmwareVersion;
    std::string m_hardwareVersion;

    bool m_shouldStop;

    static void workerThread(LIDARFrameGrabber* grabber);
    static void printLidarInfo(LIDARFrameGrabber& grabber);
    static sl_result captureFrame(LIDARFrameGrabber& grabber, sl::ILidarDriver* driver);
};