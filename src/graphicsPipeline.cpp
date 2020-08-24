#include <fstream>
#include "../include/graphicsPipeline.hpp"

graphicsPipeline::graphicsPipeline(vulkanInstance* instance, const swapChain& swapChain):
instance(instance){
    createRenderPass(swapChain);
    createGraphicsPipeline(swapChain);
}

graphicsPipeline::~graphicsPipeline() noexcept{
    instance->logicalDevice.destroyPipeline(pipelineVK, nullptr);
    instance->logicalDevice.destroyPipelineLayout(m_pipelineLayout, nullptr);
    instance->logicalDevice.destroyRenderPass(renderPass, nullptr);
}

void graphicsPipeline::createGraphicsPipeline(const swapChain& swapChain){
    auto vertShaderCode = readFile("../shaders/vert.spv");
    auto fragShaderCode = readFile("../shaders/frag.spv");

    vk::UniqueShaderModule vertShaderModule = createShaderModule(vertShaderCode); // using unique version gives automatic cleanup
    vk::UniqueShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        vk::PipelineShaderStageCreateFlagBits::eAllowVaryingSubgroupSizeEXT,
        vk::ShaderStageFlagBits::eVertex,
        vertShaderModule.get(),
        "main",
        nullptr
    };
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        vk::PipelineShaderStageCreateFlagBits::eAllowVaryingSubgroupSizeEXT,
        vk::ShaderStageFlagBits::eFragment,
        fragShaderModule.get(),
        "main",
        nullptr
    };
    auto shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
        vk::PipelineVertexInputStateCreateFlagBits{},
        0,
        nullptr,
        0,
        nullptr
    };
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ // defines the kind of geometry to be drawn from vertices
        vk::PipelineInputAssemblyStateCreateFlagBits{},
        vk::PrimitiveTopology::eTriangleList,
        false
    };
    vk::Viewport viewport{ // defines the region of the framebuffer we will render to
        0.0f,
        0.0f,
        static_cast<float>(swapChain.swapChainExtent.width),
        static_cast<float>(swapChain.swapChainExtent.height),
        0.0f,
        1.0f
    };
    vk::Rect2D scissor{{0, 0}, swapChain.swapChainExtent}; // declares a scissor rectangle that covers the entire framebuffer

    vk::PipelineViewportStateCreateInfo viewportState{
        vk::PipelineViewportStateCreateFlagBits{},
        1,
        &viewport,
        1,
        &scissor
    };
    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = false;
    rasterizer.rasterizerDiscardEnable = false; // allows the geometry to skip the rasterizer
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f; // large than 1.0f requires wideLines GPU feature
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = false;

    vk::PipelineMultisampleStateCreateInfo multisampling{}; // can be used for Anti-aliasing
    multisampling.sampleShadingEnable = false;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = false;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = false;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    auto dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eLineWidth};

    vk::PipelineDynamicStateCreateInfo dynamicState{
        vk::PipelineDynamicStateCreateFlagBits{},
        dynamicState.dynamicStateCount = 2,
        dynamicState.pDynamicStates = dynamicStates.begin()
    };
    
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};

    if (instance->logicalDevice.createPipelineLayout(&pipelineLayoutInfo, nullptr, &m_pipelineLayout) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages.begin();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    //pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;

    if (instance->logicalDevice.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &pipelineVK) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

void graphicsPipeline::createRenderPass(const swapChain& swapChain){
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = swapChain.swapChainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlags{0};
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (instance->logicalDevice.createRenderPass(&renderPassInfo, nullptr, &renderPass) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create render pass!");
    }
}

vk::UniqueShaderModule graphicsPipeline::createShaderModule(const std::vector<char>& code) const{
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    vk::UniqueShaderModule shaderModule = instance->logicalDevice.createShaderModuleUnique(createInfo);
    
    return shaderModule;
}

std::vector<char> graphicsPipeline::readFile(const std::string& filename){
    std::ifstream file{filename, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    std::size_t fileSize = (std::size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}
