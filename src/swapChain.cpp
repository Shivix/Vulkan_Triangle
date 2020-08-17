#include "../include/swapChain.hpp"

swapChain::swapChain(vulkanInstance* instance):
instance(instance){
    createSwapChain();
    createImageViews();
}

swapChain::~swapChain() noexcept{
    instance->logicalDevice.destroySwapchainKHR(swapChainVK, nullptr);
}

vk::Extent2D swapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(instance->window, &width, &height);
        
        vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

vk::PresentModeKHR swapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::SurfaceFormatKHR swapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats){
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

void swapChain::createImageViews(){
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        vk::ImageViewCreateInfo createInfo{
                vk::ImageViewCreateFlags{},
                swapChainImages[i],
                vk::ImageViewType::e2D, // how the image should be interpreted (1D, 2D, 3D and cube maps)
                swapChainImageFormat,
                vk::ComponentMapping{vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity},
                vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}

        };

        if (instance->logicalDevice.createImageView(&createInfo, nullptr, &swapChainImageViews[i]) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}


void swapChain::createSwapChain() {
    vulkanInstance::SwapChainSupportDetails swapChainSupport = instance->querySwapChainSupport(instance->physicalDevice);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{}; // TODO: make own func
    createInfo.surface = instance->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;// can change for postprocessing

    vulkanInstance::QueueFamilyIndices indices = instance->findQueueFamilies(instance->physicalDevice);
    auto queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices.begin();
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque; // states that we do not want any transformations
    createInfo.presentMode = presentMode;
    createInfo.clipped = true;
    createInfo.oldSwapchain = nullptr; // implement old swap chain to render while resizing window https://vulkan-tutorial.com/en/Drawing_a_triangle/Swap_chain_recreation

    if (instance->logicalDevice.createSwapchainKHR(&createInfo, nullptr, &swapChainVK) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create swap chain!");
    }
    
    swapChainImages = instance->logicalDevice.getSwapchainImagesKHR(swapChainVK);
    
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void swapChain::refresh(){

    createSwapChain();

    
    
    instance->logicalDevice.destroySwapchainKHR(swapChainVK, nullptr);
}
