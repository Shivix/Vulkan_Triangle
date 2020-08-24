#ifndef VULKANTEST_GRAPHICSPIPELINE_HPP
#define VULKANTEST_GRAPHICSPIPELINE_HPP

#include "../include/swapChain.hpp"

class vulkanInstance;

class graphicsPipeline{
public:
    graphicsPipeline(vulkanInstance*, const swapChain&);
    ~graphicsPipeline() noexcept;
    graphicsPipeline(graphicsPipeline&) = default;
    graphicsPipeline& operator=(graphicsPipeline&) = default;
    
    vk::RenderPass renderPass{};
    vk::Pipeline pipelineVK{};
    vulkanInstance* instance;
private:
    vk::PipelineLayout m_pipelineLayout{};

    vk::UniqueShaderModule
    createShaderModule(const std::vector<char>&) const;
    static std::vector<char>
    readFile(const std::string&);
    
    void
    createGraphicsPipeline(const swapChain&);
    void
    createRenderPass(const swapChain&);
};


#endif //VULKANTEST_GRAPHICSPIPELINE_HPP
