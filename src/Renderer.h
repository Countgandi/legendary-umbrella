#ifndef RENDERER
#define RENDERER

#include <vulkan/vulkan.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include "ErrorHandler.h"

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

class Renderer {
public:
    Renderer();
    ~Renderer();

    void initInstance();
    void deInitInstance();

    void initDevice();
    void deInitDevice();

    void setupDebug();
    void initDebug();
    void deInitDebug();

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_gpu = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties m_gpuProperties = {};

    uint32_t m_graphicsFamilyIndex = 0;

    std::vector<const char*> m_instanceLayers;
    std::vector<const char*> m_instanceExtensions;
    std::vector<const char*> m_deviceLayers;
    std::vector<const char*> m_deviceExtensions;

    VkDebugReportCallbackEXT m_debugReport = VK_NULL_HANDLE;

    VkDebugReportCallbackCreateInfoEXT m_debug_report_callback_create_info = {};

};

#endif // RENDERER