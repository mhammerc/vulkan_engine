#include "shadermodule.h"
#include <fstream>
#include <utility>

using namespace Engine::Vulkan;

ShaderModule::~ShaderModule()
{
    if (_shaderModule)
    {
        vkDestroyShaderModule(*_device, _shaderModule, nullptr);
    }
}

ShaderModule::operator VkShaderModule() const
{
    return _shaderModule;
}

ShaderModule::ShaderModule(not_null<VkShaderModule> shaderModule, not_null<LogicalDevice *> device, Stage stage, std::string entrypoint) :
_shaderModule(shaderModule),
_device(device),
_stage(stage),
_entrypoint(std::move(entrypoint))
{}

ShaderModule ShaderModule::createFromFile(not_null<LogicalDevice *> device, std::string const &path, Stage stage, std::string const &entrypoint)
{
    if (entrypoint.empty())
    {
        throw std::runtime_error("A shader entrypoint must not be empty.");
    }

    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Can not open shader: " + path);
    }

    usize fileSize = (usize)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), buffer.size());
    file.close();

    VkShaderModuleCreateInfo createInfo
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = buffer.size(),
        .pCode = reinterpret_cast<const uint32*>(buffer.data())
    };

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    ThrowError(vkCreateShaderModule(*device, &createInfo, nullptr, &shaderModule));

    return ShaderModule(shaderModule, device, stage, entrypoint);
}

VkPipelineShaderStageCreateInfo ShaderModule::toPipeline() const
{
    VkPipelineShaderStageCreateInfo s
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = static_cast<VkShaderStageFlagBits>(_stage),
        .module = _shaderModule,
        .pName = _entrypoint.c_str(),
        .pSpecializationInfo = nullptr, // For constant data, may be implemented later.
    };

    return s;
}


