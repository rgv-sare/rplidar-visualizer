#include "LIDARFrameGrabber.hpp"

#if defined(__linux__) || defined(__APPLE__)
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include <string>
#include <vector>
#include "Logger.hpp"
#include <cstring>
#include <GLFW/glfw3.h>
#include <algorithm>


em::Logger logger("LIDARFrameGrabber");

std::vector<std::string> LIDARFrameGrabber::getAvailableDevices()
{
    std::vector<std::string> devices;

#ifdef __linux__
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("/dev")) != nullptr)
    {
        while ((ent = readdir(dir)) != nullptr)
        {
            std::string deviceName = ent->d_name;
            if (deviceName.substr(0, 3) == "tty")
            {
                std::string fullPath = "/dev/" + deviceName;
                int fd = open(fullPath.c_str(), O_RDWR | O_NOCTTY);
                if (fd != -1) // if open is successful
                {
                    devices.push_back(fullPath);
                    close(fd); // don't forget to close the file descriptor
                }
            }
        }
        closedir(dir);
    }
#elif __APPLE__
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("/dev")) != nullptr)
    {
        while ((ent = readdir(dir)) != nullptr)
        {
            std::string deviceName = ent->d_name;
            if (deviceName.substr(0, 3) == "cu." || deviceName.substr(0, 4) == "tty.")
            {
                devices.push_back("/dev/" + deviceName);
            }
        }

        closedir(dir);
    }
#elif defined(_WIN32)
    // Get the available COM ports
    for (int i = 1; i <= 256; i++)
    {
        std::string portName = "COM" + std::to_string(i);
        HANDLE hComm = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hComm != INVALID_HANDLE_VALUE)
        {
            devices.push_back(portName);
            CloseHandle(hComm);
        }
    }
#endif

    return devices;
}

LIDARFrameGrabber::LIDARFrameGrabber(std::string port) :
    m_port(port),
    m_message("Idle"),
    m_status(IDLE),
    m_shouldStop(false)
{
}

void LIDARFrameGrabber::start()
{
    m_status = PENDING;
    m_message = "Starting";
    m_nodes.reserve(8192);
    m_thread = std::thread(workerThread, this);
}

void LIDARFrameGrabber::stop()
{
    m_shouldStop = true;

    if (m_thread.joinable())
        m_thread.join();

    m_status = IDLE;
}

const std::string& LIDARFrameGrabber::getPort() const
{
    return m_port;
}

LIDARFrameGrabber::Status LIDARFrameGrabber::getStatus() const
{
    return m_status;
}

const std::string& LIDARFrameGrabber::getStatusMessage() const
{
    return m_message;
}

const std::string& LIDARFrameGrabber::getSerialNumber() const
{
    return m_serialNumber;
}

const std::string& LIDARFrameGrabber::getFirmwareVersion() const
{
    return m_firmwareVersion;
}

const std::string& LIDARFrameGrabber::getHardwareVersion() const
{
    return m_hardwareVersion;
}

bool LIDARFrameGrabber::isConnected() const
{
    return m_status == OK;
}

float LIDARFrameGrabber::getFPS() const
{
    return m_fps;
}

const std::vector<LIDARFrameGrabber::Node>& LIDARFrameGrabber::getNodes() const
{
    return m_nodes;
}

const LIDARFrameGrabber::Node& LIDARFrameGrabber::longestNode() const
{
    return m_longestNode;
}

LIDARFrameGrabber::LIDARHealth LIDARFrameGrabber::getHealth()
{
    return m_health;
}

LIDARFrameGrabber::LIDARInfo LIDARFrameGrabber::getInfo()
{
    return m_info;
}

void LIDARFrameGrabber::workerThread(LIDARFrameGrabber* grabber)
{
    sl_result result;
    sl::ILidarDriver* driver = *sl::createLidarDriver();

    grabber->m_channel = *sl::createSerialPortChannel(grabber->m_port.c_str(), 115200);

    if(SL_IS_FAIL(driver->connect(grabber->m_channel)))
    {
        grabber->m_status = ERROR;
        grabber->m_message = "Failed to connect to serial port";
        logger.errorf("Failed to connect to serail port: %s", grabber->m_port.c_str());
        return;
    }
    grabber->m_message = "Connected to serial port";

    result = driver->getDeviceInfo(grabber->m_info);

    if(SL_IS_FAIL(result))
    {
        grabber->m_status = ERROR;
        if(result == SL_RESULT_OPERATION_TIMEOUT)
            grabber->m_message = "Operation timed out";
        else
            grabber->m_message = "Failed to get device info";

        logger.errorf("Failed to get device info");

        return;
    }
    grabber->m_message = "Device info received";
    printLidarInfo(*grabber);

    result = driver->getHealth(grabber->m_health);

    if(SL_IS_OK(result))
    {
        switch(grabber->m_health.status) {
        case SL_LIDAR_STATUS_OK:
            grabber->m_message = "Device health status is OK";
            logger.infof("Health status is OK");
            break;
        case SL_LIDAR_STATUS_WARNING:
            grabber->m_status = ERROR;
            grabber->m_message = "Device health status is WARNING";
            logger.warnf("Health status is WARNING");
            break;
        case SL_LIDAR_STATUS_ERROR:
            grabber->m_status = ERROR;
            grabber->m_message = "Device health status is ERROR";
            logger.errorf("Health status is ERROR");
            break;
        }
    }
    else
    {
        grabber->m_status = ERROR;
        grabber->m_message = "Failed to get device health";
        logger.errorf("Failed to get device health");
    }

    grabber->m_status = OK;
    logger.infof("LIDAR frame grabber started");

    // driver->startScan(false, true);
    while(!grabber->m_shouldStop)
    {
        driver->startScan(false, true);
        double time = glfwGetTime();
        captureFrame(*grabber, driver);
        grabber->m_fps = 1.0f / (glfwGetTime() - time);
        grabber->m_lastFrameTime = time;
    }

    driver->stop();
    driver->disconnect();
}

void LIDARFrameGrabber::printLidarInfo(LIDARFrameGrabber& grabber)
{
    char serialnum[64] = {0};
    char buffer[16] = {0};
    for (int i = 0; i < 16; i++)
    {
        snprintf(buffer, sizeof(buffer) - 1, "%02X", grabber.m_info.serialnum[i]);
        strncat(serialnum, buffer, sizeof(serialnum) - 1);
    }

    grabber.m_serialNumber = serialnum;

    snprintf(buffer, sizeof(buffer) - 1, "%d.%d", grabber.m_info.firmware_version >> 8, grabber.m_info.firmware_version & 0xFF);
    grabber.m_firmwareVersion = buffer;

    snprintf(buffer, sizeof(buffer) - 1, "%d", grabber.m_info.hardware_version);
    grabber.m_hardwareVersion = buffer;

    logger.infof("Device Info:");
    logger.infof("  Serial Number:");
    logger.infof("    %s", serialnum);
    logger.infof("  firmware_version: %d.%d", grabber.m_info.firmware_version >> 8, grabber.m_info.firmware_version & 0xFF);
    logger.infof("  hardware_version: %d", grabber.m_info.hardware_version);
}

sl_result LIDARFrameGrabber::captureFrame(LIDARFrameGrabber& grabber, sl::ILidarDriver* driver)
{
    sl_result result;
    
    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t count = 8192;

    result = driver->grabScanDataHq(nodes, count);

    if(SL_IS_OK(result) || result == SL_RESULT_OPERATION_TIMEOUT)
    {
        driver->ascendScanData(nodes, count);

        grabber.m_nodes.clear();
        grabber.m_longestNode.angle = 0.0f;
        grabber.m_longestNode.distance = 0.0f;

        for (size_t i = 0; i < count; i++)
        {
            Node node;
            node.angle = nodes[i].angle_z_q14 * 90.0f / (1 << 14);
            node.distance = nodes[i].dist_mm_q2 / 4.0f;
            grabber.m_nodes.push_back(node);

            if(node.distance > grabber.m_longestNode.distance)
                grabber.m_longestNode = node;
        }
    }
    else
    {
        logger.errorf("Failed to capture frame");
    }

    return result;
}