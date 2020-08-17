#ifndef VULKANTEST_VULKANINSTANCE_HPP
#define VULKANTEST_VULKANINSTANCE_HPP

#include <vulkan/vulkan.hpp>
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
    ~vulkanInstance() noexcept;
    vulkanInstance(vulkanInstance&) = delete;
    vulkanInstance& operator=(vulkanInstance&) = delete;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow* window;
    vk::DebugUtilsMessengerEXT debugMessenger{};
    vk::SurfaceKHR surface{};
    vk::PhysicalDevice physicalDevice = nullptr;
    vk::Device logicalDevice{};
    vk::Queue graphicsQueue{};
    vk::Queue presentQueue{};
    const std::vector<const char*> validationLayers{"VK_LAYER_KHRONOS_validation"};
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
    
    vk::Instance m_instance{};
private:
    
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
    pickPhysicalDevice();
    static void
    populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT&);
    void
    setupDebugMessenger();
};


#endif //VULKANTEST_VULKANINSTANCE_HPP
