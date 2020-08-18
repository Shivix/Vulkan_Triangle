#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
#pragma ide diagnostic ignored "Simplify"
#include <stdexcept>
#include <set>
#include <iostream>
#include "../include/vulkanInstance.hpp"

vulkanInstance::vulkanInstance(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // prevents from creating a OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // prevents resizing of window

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
}

vulkanInstance::~vulkanInstance() noexcept{
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(m_instance, debugMessenger, nullptr);
    }
    logicalDevice.destroy(nullptr);
    m_instance.destroySurfaceKHR(surface, nullptr);
    m_instance.destroy(nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool vulkanInstance::checkDeviceExtensionSupport(vk::PhysicalDevice device){

    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void vulkanInstance::checkExtensions(){

    std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

    std::cout << "available extensions:\n";

    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    std::cout << std::flush; // flushing manually else it doesnt flush till after the program has been closed in debug mode
}

bool vulkanInstance::checkValidationLayerSupport() {

    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (const std::string_view layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (layerName == layerProperties.layerName) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }
    return true;
}

vk::Result vulkanInstance::CreateDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                      const vk::AllocationCallbacks* pAllocator,
                                                        vk::DebugUtilsMessengerEXT* pDebugMessenger){
    
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return static_cast<vk::Result>(func(instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(pCreateInfo),
                                            reinterpret_cast<const VkAllocationCallbacks*>(pAllocator),
                                            reinterpret_cast<VkDebugUtilsMessengerEXT_T**>(pDebugMessenger))); // vulkan.hpp debugMessenger is funky
    } else {
        return vk::Result::eErrorExtensionNotPresent;
    }
}

void vulkanInstance::createInstance(){
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    vk::ApplicationInfo appInfo{};
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // NOLINT(hicpp-signed-bitwise)
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // NOLINT(hicpp-signed-bitwise)
    appInfo.apiVersion = VK_API_VERSION_1_0; // NOLINT(hicpp-signed-bitwise)

    vk::InstanceCreateInfo createInfo{};
    createInfo.pApplicationInfo = &appInfo;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = static_cast<vk::DebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    checkExtensions();

    if (vk::createInstance(&createInfo, nullptr, &m_instance) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create instance!");
    }
}

void vulkanInstance::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo createInfo{};
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    if (enableValidationLayers) { // for compatibility purposes
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    }

    if (physicalDevice.createDevice(&createInfo, nullptr, &logicalDevice) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create logical device!");
    }

    logicalDevice.getQueue(indices.graphicsFamily.value(), 0, &graphicsQueue);
    logicalDevice.getQueue(indices.presentFamily.value(), 0, &presentQueue);
}

void vulkanInstance::createSurface(){
    if (glfwCreateWindowSurface(m_instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

vk::Bool32 vulkanInstance::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, // can name and use these variables for different debugging
                                       VkDebugUtilsMessageTypeFlagsEXT,
                                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*){

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return false;
}

void vulkanInstance::DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT p_debugMessenger, const vk::AllocationCallbacks* pAllocator) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        func(instance, p_debugMessenger, reinterpret_cast<const VkAllocationCallbacks*>(pAllocator));
    }
}

vulkanInstance::QueueFamilyIndices vulkanInstance::findQueueFamilies(vk::PhysicalDevice device) const {
    vulkanInstance::QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }
        if (indices.isComplete()) {
            break;
        }
        vk::Bool32 presentSupport = false;
        device.getSurfaceSupportKHR(i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }
        i++;
    }

    return indices;
}

std::vector<const char*> vulkanInstance::getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool vulkanInstance::isDeviceSuitable(vk::PhysicalDevice device) {
    vk::PhysicalDeviceProperties deviceProperties;
    device.getProperties(&deviceProperties);
    vk::PhysicalDeviceFeatures deviceFeatures;
    device.getFeatures(&deviceFeatures);
    // Define suitable GPU here

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    QueueFamilyIndices indices = findQueueFamilies(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void vulkanInstance::pickPhysicalDevice() {
    
    std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
    
    if (devices.empty()) {
        throw std::runtime_error("failed to find any GPUs that support Vulkan!");
    }

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }
    if (physicalDevice == static_cast<vk::PhysicalDevice>(nullptr)) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void vulkanInstance::populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance; 
    createInfo.pfnUserCallback = debugCallback;
}

vulkanInstance::SwapChainSupportDetails vulkanInstance::querySwapChainSupport(vk::PhysicalDevice device) const{
    SwapChainSupportDetails details;

    device.getSurfaceCapabilitiesKHR(surface, &details.capabilities);
    
    details.formats = device.getSurfaceFormatsKHR(surface);
    
    details.presentModes = device.getSurfacePresentModesKHR(surface);
    return details;
}

void vulkanInstance::setupDebugMessenger(){
    if (!enableValidationLayers) {return;}

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &debugMessenger) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}
