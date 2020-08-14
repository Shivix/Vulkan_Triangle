#include "../include/commandBuffer.hpp"
#include "../include/vulkanInstance.hpp"
#include "../include/graphicsPipeline.hpp"

commandBuffer::commandBuffer(vulkanInstance* instance, graphicsPipeline* pipeline, swapChain* m_swapChain):
        m_instance(instance),
        m_pipeline(pipeline),
        m_swapChain(m_swapChain){
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
}

commandBuffer::~commandBuffer() noexcept{
    m_instance->logicalDevice.destroyCommandPool(commandPool, nullptr);

    for (auto framebuffer : swapChainFramebuffers) {
        m_instance->logicalDevice.destroyFramebuffer(framebuffer, nullptr);
    }
}

void commandBuffer::createCommandBuffers(){
    commandBuffers.resize(swapChainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo{
            commandPool,
            vk::CommandBufferLevel::ePrimary,
            static_cast<uint32_t>(commandBuffers.size())
    };


    if (m_instance->logicalDevice.allocateCommandBuffers(&allocInfo, commandBuffers.data()) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for(size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo{};
        if (commandBuffers[i].begin(&beginInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        vk::RenderPassBeginInfo renderPassInfo{
                m_pipeline->renderPass,
                swapChainFramebuffers[i],
                {0, 0},
                1,
                reinterpret_cast<const vk::ClearValue*>(&clearColor)
        };

        commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline->pipelineVK);
        commandBuffers[i].draw(3, 1, 0, 0);
        commandBuffers[i].endRenderPass();
        commandBuffers[i].end();
        //if (commandBuffers[i].end() != vk::Result::eSuccess) {
        //    throw std::runtime_error("failed to record command buffer!");
        //}
    }
}

void commandBuffer::createCommandPool(){
    vulkanInstance::QueueFamilyIndices queueFamilyIndices = m_instance->findQueueFamilies(m_instance->physicalDevice);

    vk::CommandPoolCreateInfo poolInfo{
            vk::CommandPoolCreateFlags{0},
            queueFamilyIndices.graphicsFamily.value()
    };
    if (m_instance->logicalDevice.createCommandPool(&poolInfo, nullptr, &commandPool) != vk::Result::eSuccess) { // currently only sets the command buffers at the beginning
        throw std::runtime_error("failed to create command pool!");
    }
}

void commandBuffer::createFramebuffers(){
    swapChainFramebuffers.resize(m_swapChain->swapChainImageViews.size());

    for (size_t i = 0; i < m_swapChain->swapChainImageViews.size(); i++) {
        vk::ImageView attachments = { // was an array but should be okay now
                m_swapChain->swapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo{
                vk::FramebufferCreateFlags{},
                m_pipeline->renderPass,
                1,      // attachment count
                &attachments,
                m_swapChain->swapChainExtent.width,
                m_swapChain->swapChainExtent.height,
                1               // layers
        };
        if (m_instance->logicalDevice.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}