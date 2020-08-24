#ifndef VULKAN_ENGINE_SHADERMODULE_H
#define VULKAN_ENGINE_SHADERMODULE_H

#include "vulkan.h"
#include "logicaldevice.h"

namespace Engine::Vulkan
{
class ShaderModule : public OnlyMovable
{
public:
    enum class Stage : uint32
    {
        Vertex = VK_SHADER_STAGE_VERTEX_BIT,
        Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    [[nodiscard]] static ShaderModule createFromSpirvFile(not_null<LogicalDevice*> device, std::string const &path,
                                                          Stage stage, std::string const &entrypoint = "main");

    ~ShaderModule();
    ShaderModule (ShaderModule &&) noexcept = default;
    ShaderModule &operator=(ShaderModule &&) noexcept = default;
    operator VkShaderModule() const;

    [[nodiscard]] VkPipelineShaderStageCreateInfo toPipeline() const;

private:
    // shaderModule arg: not_null or VkHandle? (apply it everywhere)
    ShaderModule(not_null<VkShaderModule> shaderModule, not_null<LogicalDevice*> device, Stage stage, std::string entrypoint);

    VkHandle<VkShaderModule> _shaderModule;
    not_null<LogicalDevice*> _device;
    Stage _stage;
    std::string _entrypoint;
};
}

#endif //VULKAN_ENGINE_SHADERMODULE_H
