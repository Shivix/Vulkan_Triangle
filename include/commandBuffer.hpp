#ifndef VULKANTRIANGLE_COMMANDBUFFER_HPP
#define VULKANTRIANGLE_COMMANDBUFFER_HPP

#include <vulkan/vulkan.hpp>
#include <vector>

class vulkanInstance;
class graphicsPipeline;

class commandBuffer{
public:
    commandBuffer(vulkanInstance*, graphicsPipeline*);
    ~commandBuffer() noexcept;
    
    std::vector<vk::Framebuffer> swapChainFramebuffers;
    vk::CommandPool commandPool{};
    std::vector<vk::CommandBuffer> commandBuffers;
private:
    vulkanInstance* instance;
    graphicsPipeline* pipeline;
    
    void
    createCommandBuffers();
    void
    createCommandPool();
    
    
};


#endif //VULKANTRIANGLE_COMMANDBUFFER_HPP
