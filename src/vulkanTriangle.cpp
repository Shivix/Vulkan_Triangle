#include <fstream>
#include "../include/vulkanTriangle.hpp"

vulkanTriangle::vulkanTriangle(){
    glfwSetFramebufferSizeCallback(m_instance.window, framebufferResizeCallback);
}

vulkanTriangle::~vulkanTriangle() noexcept{
    
    for (auto imageView : m_swapChain.swapChainImageViews) {
        m_instance.logicalDevice.destroyImageView(imageView, nullptr);
    }
}

void vulkanTriangle::drawFrame(){
    m_instance.logicalDevice.waitForFences(1, &m_syncobjects.inFlightFences[m_currentFrame], true, UINT64_MAX);
    
    uint32_t imageIndex;
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

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_framebufferResized) {
        m_framebufferResized = false;
        refreshSwapChain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % m_syncobjects.MAX_FRAMES_IN_FLIGHT;
}

void vulkanTriangle::framebufferResizeCallback(GLFWwindow* window, int width, int height){
    //auto app = reinterpret_cast<vulkanTriangle*>(glfwGetWindowUserPointer(window));
    //app->framebufferResized = true;
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

void vulkanTriangle::refreshSwapChain(){ // TODO: clean up is temporary
    //instance.m_device.waitIdle();
    //swapChain newSwapChain;
    //p_swapChain.refresh();
    //createImageViews();
    //pipeline.refresh(p_swapChain);
    //createFramebuffers();
    //createCommandBuffers();
    //
    //for (auto framebuffer : m_commandBuffer.swapChainFramebuffers) {
    //    instance.m_device.destroyFramebuffer(framebuffer, nullptr);
    //}
    //instance.m_device.freeCommandBuffers(m_commandBuffer.commandPool, static_cast<uint32_t>(m_commandBuffer.commandBuffers.size()),
    //                                     m_commandBuffer.commandBuffers.data());
    //for (auto imageView : p_swapChain.swapChainImageViews) {
    //    instance.m_device.destroyImageView(imageView, nullptr);
    //}
}
