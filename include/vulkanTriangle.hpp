#ifndef VULKANTEST_VULKANTRIANGLE_HPP
#define VULKANTEST_VULKANTRIANGLE_HPP

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <set>
#include <cstdint>
#include <algorithm>
#include "graphicsPipeline.hpp"

class vulkanTriangle{ // TODO: fix style TODO: go through tutorial again and add more comments TODO: rename and add another class
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    
public:
    vulkanTriangle();
    ~vulkanTriangle() noexcept; // TODO: rule of 3

    void run();

private:
    
    std::vector<vk::ImageView> swapChainImageViews;
    std::vector<vk::Framebuffer> swapChainFramebuffers;
    vk::CommandPool commandPool{};
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
    std::size_t currentFrame = 0;
    
    vulkanInstance instance;
    swapChain p_swapChain = swapChain(&instance);
    graphicsPipeline pipeline = graphicsPipeline(p_swapChain, &instance.m_device);
    
    void 
    createCommandBuffers();
    void
    createCommandPool();
    void 
    createFramebuffers();
    void 
    createImageViews();
    void 
    createSyncObjects();
    void 
    drawFrame();
    void 
    initVulkan();
    
    void 
    mainLoop();
};

#endif //VULKANTEST_VULKANTRIANGLE_HPP
