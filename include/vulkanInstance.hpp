#ifndef VULKANTEST_VULKANINSTANCE_HPP
#define VULKANTEST_VULKANINSTANCE_HPP

#include <vulkan/vulkan.hpp> // TODO: time to c++ it up yo
#include <vector>
#include <optional>
#include <GLFW/glfw3.h>

class vulkanInstance{
    
#ifdef NDEBUG
    static const bool enableValidationLayers = false;
#else
    static const bool enableValidationLayers = true;
#endif
    
public:
    vulkanInstance();
    ~vulkanInstance();

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    vk::DebugUtilsMessengerEXT debugMessenger{};// TODO: alphabetical too?
    GLFWwindow* window;
    vk::SurfaceKHR surface{};
    vk::PhysicalDevice physicalDevice = nullptr;
    vk::Device m_device{};
    vk::Queue graphicsQueue{};
    vk::Queue presentQueue{};
    const std::vector<const char*> validationLayers{"VK_LAYER_KHRONOS_validation"}; // FIXME:
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };
    
public:
    vulkanInstance::QueueFamilyIndices
    findQueueFamilies(vk::PhysicalDevice) const;
    SwapChainSupportDetails
    querySwapChainSupport(vk::PhysicalDevice) const;
    
private:
    vk::Instance m_instance{};
    
    bool
    checkDeviceExtensionSupport(vk::PhysicalDevice);
    static void
    checkExtensions();
    bool
    checkValidationLayerSupport();
    static vk::Result
    CreateDebugUtilsMessengerEXT(vk::Instance,
                                 const vk::DebugUtilsMessengerCreateInfoEXT*,
                                 const vk::AllocationCallbacks*,
                                 vk::DebugUtilsMessengerEXT*);
    void
    createInstance();
    void
    createLogicalDevice();
    void
    createSurface();
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
                  VkDebugUtilsMessageTypeFlagsEXT,
                  const VkDebugUtilsMessengerCallbackDataEXT*,
                  void*);
    static void
    DestroyDebugUtilsMessengerEXT(vk::Instance, vk::DebugUtilsMessengerEXT, const vk::AllocationCallbacks*);
    [[nodiscard]] static std::vector<const char*>
    getRequiredExtensions();
    bool
    isDeviceSuitable(vk::PhysicalDevice);
    void
    pickPhysicalDevice();// TODO: device class
    static void
    populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT&);
    void
    setupDebugMessenger();
};


#endif //VULKANTEST_VULKANINSTANCE_HPP
