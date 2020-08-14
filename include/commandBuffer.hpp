#ifndef VULKANTRIANGLE_COMMANDBUFFER_HPP
#define VULKANTRIANGLE_COMMANDBUFFER_HPP

#include <vulkan/vulkan.hpp>
#include <vector>

class vulkanInstance;
class graphicsPipeline;
class swapChain;

class commandBuffer{
public:
    commandBuffer(vulkanInstance*, graphicsPipeline*, swapChain*);
    ~commandBuffer() noexcept;
    commandBuffer(commandBuffer&) = default;
    commandBuffer& operator=(commandBuffer&) = default;
    
    std::vector<vk::Framebuffer> swapChainFramebuffers;
    vk::CommandPool commandPool{};
    std::vector<vk::CommandBuffer> commandBuffers;
private:
    vulkanInstance* m_instance;
    graphicsPipeline* m_pipeline;
    swapChain* m_swapChain;
    
    void
    createCommandBuffers();
    void
    createCommandPool();
    void
    createFramebuffers();
    
};


#endif //VULKANTRIANGLE_COMMANDBUFFER_HPP
