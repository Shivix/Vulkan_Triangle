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
    
    for (auto imageView : p_swapChain.swapChainImageViews) {
        instance.m_device.destroyImageView(imageView, nullptr);
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
    
    vk::Semaphore waitSemaphores = {imageAvailableSemaphores[currentFrame]};
    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::Semaphore signalSemaphores = renderFinishedSemaphores[currentFrame];
    
    vk::SubmitInfo submitInfo{
    1,
    &waitSemaphores,
    &waitStages,
    1,
    &m_commandBuffer.commandBuffers[imageIndex],
    1,
    &signalSemaphores
};

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
    createSyncObjects();
}

void vulkanTriangle::framebufferResizeCallback(GLFWwindow* window, int width, int height){
    //auto app = reinterpret_cast<vulkanTriangle*>(glfwGetWindowUserPointer(window));
    //app->framebufferResized = true;
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
