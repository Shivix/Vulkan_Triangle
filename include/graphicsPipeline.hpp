#ifndef VULKANTEST_GRAPHICSPIPELINE_HPP
#define VULKANTEST_GRAPHICSPIPELINE_HPP

#include <string>
#include <vector>
#include "../include/swapChain.hpp"


class vulkanInstance;

class graphicsPipeline{
public:
    graphicsPipeline(vulkanInstance*, const swapChain&);
    ~graphicsPipeline() noexcept; // r of 3
    
    void 
    refresh(const swapChain&);
    
    vk::RenderPass renderPass{};
    vk::Pipeline pipeline{};
    vulkanInstance* instance;
private:
    vk::PipelineLayout pipelineLayout{};

    vk::ShaderModule
    createShaderModule(const std::vector<char>&) const;
    static std::vector<char>
    readFile(const std::string&);
    
    void
    createGraphicsPipeline(const swapChain&);
    void
    createRenderPass(const swapChain&);
    
    
    
};


#endif //VULKANTEST_GRAPHICSPIPELINE_HPP
