#ifndef VULKANTEST_SWAPCHAIN_HPP
#define VULKANTEST_SWAPCHAIN_HPP

#include "../include/vulkanInstance.hpp"

class swapChain{
public:
    explicit swapChain(vulkanInstance*);
    ~swapChain() noexcept;
    swapChain(swapChain&) = default;
    swapChain& operator=(swapChain&) = default;

    vulkanInstance* instance;
    vk::SwapchainKHR swapChainVK{};
    std::vector<vk::Image> swapChainImages{};
    vk::Extent2D swapChainExtent{};
    vk::Format swapChainImageFormat{vk::Format::eUndefined};
    std::vector<vk::ImageView> swapChainImageViews;
    
    vk::Extent2D
    chooseSwapExtent(const vk::SurfaceCapabilitiesKHR&) const;
    static vk::PresentModeKHR
    chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>&);
    static vk::SurfaceFormatKHR
    chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
    void
    createImageViews();
    void
    createSwapChain();
};


#endif //VULKANTEST_SWAPCHAIN_HPP
