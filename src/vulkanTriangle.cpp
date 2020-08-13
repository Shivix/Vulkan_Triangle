#include <fstream>
#include "../include/vulkanTriangle.hpp"

vulkanTriangle::vulkanTriangle(){
    glfwSetFramebufferSizeCallback(instance.window, framebufferResizeCallback);
}

vulkanTriangle::~vulkanTriangle() noexcept{
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        instance.m_device.destroySemaphore(renderFinishedSemaphores[i], nullptr); // TODO: poss remove with c++
        instance.m_device.destroySemaphore(imageAvailableSemaphores[i], nullptr);
        instance.m_device.destroyFence(inFlightFences[i], nullptr);
    }
    instance.m_device.destroyCommandPool(commandPool, nullptr);
    
    for (auto framebuffer : swapChainFramebuffers) {
        instance.m_device.destroyFramebuffer(framebuffer, nullptr);
    }
    for (auto imageView : swapChainImageViews) {
        instance.m_device.destroyImageView(imageView, nullptr);
    }
}

void vulkanTriangle::createCommandBuffers(){
    commandBuffers.resize(swapChainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo{
            commandPool,
            vk::CommandBufferLevel::ePrimary,
            static_cast<uint32_t>(commandBuffers.size())
    };
    

    if (instance.m_device.allocateCommandBuffers(&allocInfo, commandBuffers.data()) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for(size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo{};
        if (commandBuffers[i].begin(&beginInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        vk::RenderPassBeginInfo renderPassInfo{
            pipeline.renderPass,
            swapChainFramebuffers[i],
            {0, 0},
            1,
            reinterpret_cast<const vk::ClearValue*>(&clearColor)
        };

        commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline);
        commandBuffers[i].draw(3, 1, 0, 0);
        commandBuffers[i].endRenderPass();
        commandBuffers[i].end();
        //if (commandBuffers[i].end() != vk::Result::eSuccess) {
        //    throw std::runtime_error("failed to record command buffer!");
        //}
    }
}

void vulkanTriangle::createCommandPool(){
    vulkanInstance::QueueFamilyIndices queueFamilyIndices = instance.findQueueFamilies(instance.physicalDevice);

    vk::CommandPoolCreateInfo poolInfo{
    vk::CommandPoolCreateFlags{0},
    queueFamilyIndices.graphicsFamily.value()
    };
    if (instance.m_device.createCommandPool(&poolInfo, nullptr, &commandPool) != vk::Result::eSuccess) { // currently only sets the command buffers at the beginning
        throw std::runtime_error("failed to create command pool!");
    }
}

void vulkanTriangle::createFramebuffers(){
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        vk::ImageView attachments = { // was an array but should be okay now
                swapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo{
            vk::FramebufferCreateFlags{},
            pipeline.renderPass,
            1,      // attachment count
            &attachments,
            p_swapChain.swapChainExtent.width,
            p_swapChain.swapChainExtent.height,
            1               // layers
        };
        if (instance.m_device.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void vulkanTriangle::createImageViews(){
    swapChainImageViews.resize(p_swapChain.swapChainImages.size());
    for (size_t i = 0; i < p_swapChain.swapChainImages.size(); i++) {
        vk::ImageViewCreateInfo createInfo{ 
            vk::ImageViewCreateFlags{},
            p_swapChain.swapChainImages[i],
            vk::ImageViewType::e2D, // how the image should be interpreted (1D, 2D, 3D and cube maps)
            p_swapChain.swapChainImageFormat,
            vk::ComponentMapping{vk::ComponentSwizzle::eIdentity,
                                 vk::ComponentSwizzle::eIdentity,
                                 vk::ComponentSwizzle::eIdentity,
                                 vk::ComponentSwizzle::eIdentity},
            vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
            
        };
        
        if (instance.m_device.createImageView(&createInfo, nullptr, &swapChainImageViews[i]) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void vulkanTriangle::createSyncObjects(){
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(p_swapChain.swapChainImages.size(), nullptr);
    
    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::FenceCreateInfo fenceInfo{vk::FenceCreateFlagBits::eSignaled};

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (instance.m_device.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != vk::Result::eSuccess ||
            instance.m_device.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != vk::Result::eSuccess ||
            instance.m_device.createFence(&fenceInfo, nullptr, &inFlightFences[i]) != vk::Result::eSuccess) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void vulkanTriangle::drawFrame(){
    instance.m_device.waitForFences(1, &inFlightFences[currentFrame], true, UINT64_MAX);
    
    uint32_t imageIndex;
    vk::Result result = instance.m_device.acquireNextImageKHR(p_swapChain.m_swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr, &imageIndex);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        refreshSwapChain();
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    
    if (imagesInFlight[imageIndex] != static_cast<vk::Fence>(nullptr)) { // seems weird
        instance.m_device.waitForFences(1, &imagesInFlight[imageIndex], true, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];
    
    vk::SubmitInfo submitInfo{};
    
    vk::Semaphore waitSemaphores = {imageAvailableSemaphores[currentFrame]};
    
    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphores;
    submitInfo.pWaitDstStageMask = &waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    vk::Semaphore signalSemaphores = renderFinishedSemaphores[currentFrame];
    
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphores;

    instance.m_device.resetFences(1, &inFlightFences[currentFrame]);

    if (instance.graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &signalSemaphores;

    vk::SwapchainKHR swapChains = p_swapChain.m_swapChain;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = instance.presentQueue.presentKHR(&presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
        framebufferResized = false;
        refreshSwapChain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void vulkanTriangle::initVulkan(){ // vulkan class for raii? (vulkanInstance?)
    createImageViews();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
}

void vulkanTriangle::framebufferResizeCallback(GLFWwindow* window, int width, int height){
    auto app = reinterpret_cast<vulkanTriangle*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void vulkanTriangle::mainLoop(){
    while (!glfwWindowShouldClose(instance.window)) {
        glfwPollEvents();
        drawFrame();
    }
    instance.m_device.waitIdle();
}

void vulkanTriangle::run() {
    initVulkan();
    mainLoop();
}

void vulkanTriangle::refreshSwapChain(){ // TODO: clean up is temporary
    instance.m_device.waitIdle();
    // swapChain newSwapChain;
    p_swapChain.refresh();
    createImageViews();
    pipeline.refresh(p_swapChain);
    createFramebuffers();
    createCommandBuffers();
    
    for (auto framebuffer : swapChainFramebuffers) {
        instance.m_device.destroyFramebuffer(framebuffer, nullptr);
    }
    instance.m_device.freeCommandBuffers(commandPool, static_cast<uint32_t>(commandBuffers.size()), 
                                         commandBuffers.data());
    for (auto imageView : swapChainImageViews) {
        instance.m_device.destroyImageView(imageView, nullptr);
    }
}
