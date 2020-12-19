#include "Renderer.h"

Renderer::Renderer() {
    setupDebug();
    initInstance();
    initDebug();
    initDevice();
}

Renderer::~Renderer() {
    deInitDevice();
    deInitDebug();
    deInitInstance();
}

void Renderer::initInstance() {

    VkApplicationInfo application_info {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "Epic Program";
    application_info.apiVersion = VK_MAKE_VERSION(1, 2, 162);
    application_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);

    VkInstanceCreateInfo instance_create_info {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledLayerCount = m_instanceLayers.size();
    instance_create_info.ppEnabledLayerNames = m_instanceLayers.data();
    instance_create_info.enabledExtensionCount = m_instanceExtensions.size();
    instance_create_info.ppEnabledExtensionNames = m_instanceExtensions.data();
    instance_create_info.pNext = &m_debug_report_callback_create_info;

    checkError(vkCreateInstance(&instance_create_info, nullptr, &m_instance));

}

void Renderer::deInitInstance() {

    vkDestroyInstance(m_instance, nullptr);
    m_instance = nullptr;

}

void Renderer::initDevice() {
    // Finding the current gpu
    uint32_t gpuCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &gpuCount, nullptr);
    std::vector<VkPhysicalDevice> gpuList(gpuCount);
    vkEnumeratePhysicalDevices(m_instance, &gpuCount, gpuList.data());

    m_gpu = gpuList[0];
    vkGetPhysicalDeviceProperties(m_gpu, &m_gpuProperties);

    // Finding the Queue Family Properties and the Index!
    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> familyProperties(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &familyCount, familyProperties.data());
    
    bool graphicsFamilyIndexFound = false;
    for(uint32_t i = 0; i < familyCount; i++) {
        if(familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamilyIndexFound = true;
            m_graphicsFamilyIndex = i;
        }
    }
    if(!graphicsFamilyIndexFound) {
        std::cout << "Vulkan Error: Can't find graphics family queue thingy" << std::endl;
        std::exit(-1);
    }

    // Instance Layers
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
    std::cout << "Instance Layers: " << std::endl;
    for( auto &i : layerProperties) {
        std::cout << "\t" << i.layerName << "\t\t\t " << i.description  << std::endl;
    }

    // Device Layers
    layerCount = 0;
    vkEnumerateDeviceLayerProperties(m_gpu, &layerCount, nullptr);
    std::vector<VkLayerProperties> deviceLayerProperties(layerCount);
    vkEnumerateDeviceLayerProperties(m_gpu, &layerCount, deviceLayerProperties.data());
    std::cout << "Device Layers: " << std::endl;
    for( auto &i : deviceLayerProperties) {
        std::cout << "\t" << i.layerName << "\t\t\t " << i.description  << std::endl;
    }


    float queue_priorities[] {1.0f};
    VkDeviceQueueCreateInfo device_queue_create_info {};
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.queueFamilyIndex = m_graphicsFamilyIndex;
    device_queue_create_info.queueCount = 1;
    device_queue_create_info.pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_create_info {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_create_info;
    device_create_info.enabledLayerCount = m_deviceLayers.size();
    device_create_info.ppEnabledLayerNames = m_deviceLayers.data();
    device_create_info.enabledExtensionCount = m_deviceExtensions.size();
    device_create_info.ppEnabledExtensionNames = m_deviceExtensions.data();

    checkError(vkCreateDevice(m_gpu, &device_create_info, nullptr, &m_device));
}

void Renderer::deInitDevice() {
    vkDestroyDevice(m_device, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObj, size_t location, int32_t msgCode, const char* layerPrefix, const char * msg, void * userData)  {
    std::ostringstream stream;

    stream << "VK_DEBUGGER: ";

    if(flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        stream << "INFO: ";
    } else if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        stream << "WARNING: ";
    } else if(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        stream << "PERFORMANCE: ";
    } else if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        stream << "ERROR: ";
    } else if(flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        stream << "DEBUG: ";
    }
    stream << msg << std::endl;

    std::cout << stream.str();

#ifdef _WIN32
    if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        MessageBox(NULL, stream.str().c_str(), "Vulkan Error!", 0);
    }
#endif // _WIN32

    return false;
}

void Renderer::setupDebug() {
    m_debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    m_debug_report_callback_create_info.pfnCallback = &VulkanDebugCallback;
    m_debug_report_callback_create_info.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT | 0;


    // Instance
    m_instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
    m_instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    // Device
    m_deviceLayers.push_back("VK_LAYER_KHRONOS_validation");
}

PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;


void Renderer::initDebug() {
    fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
    fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");

    if(fvkCreateDebugReportCallbackEXT == nullptr || fvkCreateDebugReportCallbackEXT == nullptr) {
        std::cout << "Vulkan Error: Can't get debug function pointers" << std::endl;
        std::exit(-1);
    }

    fvkCreateDebugReportCallbackEXT(m_instance, &m_debug_report_callback_create_info, nullptr, &m_debugReport);
}

void Renderer::deInitDebug() {
    fvkDestroyDebugReportCallbackEXT(m_instance, m_debugReport, nullptr);
    m_debugReport = nullptr;
}