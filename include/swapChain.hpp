#ifndef VULKANTEST_SWAPCHAIN_HPP
#define VULKANTEST_SWAPCHAIN_HPP

#include "../include/vulkanInstance.hpp"

class swapChain{
public:
    swapChain(vulkanInstance*);
    ~swapChain();

    vulkanInstance* instance;
    vk::SwapchainKHR m_swapChain{};
    std::vector<vk::Image> swapChainImages{};
    vk::Extent2D swapChainExtent{};
    vk::Format swapChainImageFormat{vk::Format::eUndefined};
    
    vk::Extent2D
    chooseSwapExtent(const vk::SurfaceCapabilitiesKHR&) const;
    static vk::PresentModeKHR
    chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>&);
    static vk::SurfaceFormatKHR
    chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
    void
    createSwapChain();
    
    
    
    
};


#endif //VULKANTEST_SWAPCHAIN_HPP
