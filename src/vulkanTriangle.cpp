#include "../include/vulkanTriangle.hpp"

vulkanTriangle::vulkanTriangle() = default;

vulkanTriangle::~vulkanTriangle() noexcept{
    
    for (auto imageView : m_swapChain.swapChainImageViews) {
        m_instance.logicalDevice.destroyImageView(imageView, nullptr);
    }
}

void vulkanTriangle::drawFrame(){
    m_instance.logicalDevice.waitForFences(1, &m_syncobjects.inFlightFences[m_currentFrame], true, UINT64_MAX);
    
    uint32_t imageIndex = 0;
    vk::Result result = m_instance.logicalDevice.acquireNextImageKHR(m_swapChain.swapChainVK, UINT64_MAX, m_syncobjects.imageAvailableSemaphores[m_currentFrame], nullptr, &imageIndex);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        refreshSwapChain();
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    
    if (m_syncobjects.imagesInFlight[imageIndex] != static_cast<vk::Fence>(nullptr)) { // seems weird
        m_instance.logicalDevice.waitForFences(1, &m_syncobjects.imagesInFlight[imageIndex], true, UINT64_MAX);
    }
    m_syncobjects.imagesInFlight[imageIndex] = m_syncobjects.inFlightFences[m_currentFrame];
    
    vk::Semaphore waitSemaphores = {m_syncobjects.imageAvailableSemaphores[m_currentFrame]};
    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::Semaphore signalSemaphores = m_syncobjects.renderFinishedSemaphores[m_currentFrame];
    
    vk::SubmitInfo submitInfo{
    1,
    &waitSemaphores,
    &waitStages,
    1,
    &m_commandBuffer.commandBuffers[imageIndex],
    1,
    &signalSemaphores
};

    m_instance.logicalDevice.resetFences(1, &m_syncobjects.inFlightFences[m_currentFrame]);

    if (m_instance.graphicsQueue.submit(1, &submitInfo, m_syncobjects.inFlightFences[m_currentFrame]) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &signalSemaphores;

    vk::SwapchainKHR swapChains = m_swapChain.swapChainVK;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = m_instance.presentQueue.presentKHR(&presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        refreshSwapChain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % m_syncobjects.MAX_FRAMES_IN_FLIGHT;
}

void vulkanTriangle::mainLoop(){
    while (!glfwWindowShouldClose(m_instance.window)) {
        glfwPollEvents();
        drawFrame();
    }
    m_instance.logicalDevice.waitIdle();
}

void vulkanTriangle::run() {
    mainLoop();
}

void vulkanTriangle::refreshSwapChain() const{ // TODO: clean up is temporary
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_instance.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_instance.window, &width, &height);
        glfwWaitEvents();
    }
    
    m_instance.logicalDevice.waitIdle();
}
