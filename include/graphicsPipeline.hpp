#ifndef VULKANTEST_GRAPHICSPIPELINE_HPP
#define VULKANTEST_GRAPHICSPIPELINE_HPP

#include <string>
#include <vector>
#include "../include/swapChain.hpp"


//extern VkDevice m_device;

class graphicsPipeline{
public:
    graphicsPipeline(const swapChain&, vk::Device*);
    ~graphicsPipeline() noexcept; // r of 3
    
    vk::RenderPass renderPass{};
    vk::Pipeline pipeline{};
    vk::Device* device;
private:
    vk::PipelineLayout pipelineLayout{};

    void
    createGraphicsPipeline(const swapChain&);
    void
    createRenderPass(const swapChain&);
    vk::ShaderModule
    createShaderModule(const std::vector<char>&) const;
    static std::vector<char>
    readFile(const std::string&);
    
    
    
    
};


#endif //VULKANTEST_GRAPHICSPIPELINE_HPP
