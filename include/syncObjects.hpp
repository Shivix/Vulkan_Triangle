#ifndef VULKANTRIANGLE_SYNCOBJECTS_HPP
#define VULKANTRIANGLE_SYNCOBJECTS_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

class vulkanInstance;
class swapChain;

class syncObjects{
public:
    syncObjects(vulkanInstance*, swapChain*);
    ~syncObjects();

    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> imagesInFlight;
    
private:
    vulkanInstance* instancePtr;
    swapChain* swapChainPtr;

    void
    createSyncObjects();
};


#endif //VULKANTRIANGLE_SYNCOBJECTS_HPP
