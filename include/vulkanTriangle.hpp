#ifndef VULKANTEST_VULKANTRIANGLE_HPP
#define VULKANTEST_VULKANTRIANGLE_HPP

#include "graphicsPipeline.hpp"
#include "commandBuffer.hpp"
#include "syncObjects.hpp"

class vulkanTriangle{
    
public:
    vulkanTriangle();
    ~vulkanTriangle() noexcept;
    vulkanTriangle(vulkanTriangle&) = delete;
    vulkanTriangle& operator=(vulkanTriangle&) = delete;

    void run();

private:
    
    std::size_t m_currentFrame = 0;
    
    vulkanInstance m_instance;
    swapChain m_swapChain = swapChain(&m_instance);
    graphicsPipeline m_pipeline = graphicsPipeline(&m_instance, m_swapChain);
    commandBuffer m_commandBuffer = commandBuffer(&m_instance, &m_pipeline, &m_swapChain);
    syncObjects m_syncobjects = syncObjects(&m_instance, &m_swapChain);
    
    void 
    drawFrame();
    void 
    mainLoop();
    void
    refreshSwapChain() const;
};

#endif //VULKANTEST_VULKANTRIANGLE_HPP
