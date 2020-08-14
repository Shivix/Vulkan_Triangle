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
#include "syncObjects.hpp"

class vulkanTriangle{ // TODO: fix style TODO: go through tutorial again and add more comments TODO: rename and add another class
    
public:
    vulkanTriangle();
    ~vulkanTriangle() noexcept; // TODO: rule of 3

    void run();

private:
    
    std::size_t m_currentFrame = 0;
    bool m_framebufferResized = false;
    
    vulkanInstance m_instance;
    swapChain m_swapChain = swapChain(&m_instance); // TODO: rename stuff
    graphicsPipeline m_pipeline = graphicsPipeline(&m_instance, m_swapChain);
    commandBuffer m_commandBuffer = commandBuffer(&m_instance, &m_pipeline, &m_swapChain);
    syncObjects m_syncobjects = syncObjects(&m_instance, &m_swapChain);
    
    void 
    drawFrame();
    static void
    framebufferResizeCallback(GLFWwindow*, int, int);
    
    void 
    mainLoop();
    void
    refreshSwapChain();
};

#endif //VULKANTEST_VULKANTRIANGLE_HPP
