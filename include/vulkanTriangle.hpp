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
#include "commandBuffer.hpp"

class vulkanTriangle{ // TODO: fix style TODO: go through tutorial again and add more comments TODO: rename and add another class
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    
public:
    vulkanTriangle();
    ~vulkanTriangle() noexcept; // TODO: rule of 3

    void run();

private:
    
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
    std::size_t currentFrame = 0;
    bool framebufferResized = false;
    
    vulkanInstance instance;
    swapChain p_swapChain = swapChain(&instance); // TODO: rename stuff
    graphicsPipeline pipeline = graphicsPipeline(&instance, p_swapChain);
    commandBuffer m_commandBuffer = commandBuffer(&instance, &pipeline, &p_swapChain);
    
    
    void 
    createSyncObjects();
    void 
    drawFrame();
    void 
    initVulkan();
    static void
    framebufferResizeCallback(GLFWwindow*, int, int);
    
    void 
    mainLoop();
    void
    refreshSwapChain();
};

#endif //VULKANTEST_VULKANTRIANGLE_HPP
