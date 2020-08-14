#include "../include/syncObjects.hpp"
#include "../include/vulkanInstance.hpp"
#include "../include/swapChain.hpp"


syncObjects::syncObjects(vulkanInstance* instance, swapChain* m_swapChain):
        instancePtr(instance),
        swapChainPtr(m_swapChain){
    createSyncObjects();
}

syncObjects::~syncObjects(){
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        instancePtr->logicalDevice.destroySemaphore(renderFinishedSemaphores[i], nullptr); // TODO: poss remove with c++
        instancePtr->logicalDevice.destroySemaphore(imageAvailableSemaphores[i], nullptr);
        instancePtr->logicalDevice.destroyFence(inFlightFences[i], nullptr);
    }
}

void syncObjects::createSyncObjects(){
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChainPtr->swapChainImages.size(), nullptr);

    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::FenceCreateInfo fenceInfo{vk::FenceCreateFlagBits::eSignaled};

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (instancePtr->logicalDevice.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != vk::Result::eSuccess ||
            instancePtr->logicalDevice.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != vk::Result::eSuccess ||
            instancePtr->logicalDevice.createFence(&fenceInfo, nullptr, &inFlightFences[i]) != vk::Result::eSuccess) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}