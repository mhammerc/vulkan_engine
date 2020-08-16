#include "vulkan_engine.h"
#include "vulkan/instance.h"

using namespace Engine;

int main()
{
    spdlog::set_level(spdlog::level::debug);

    Vulkan::Instance i {};
    return 0;
}
