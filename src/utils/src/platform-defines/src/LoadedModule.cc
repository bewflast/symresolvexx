#include <platform-defines/platform-defines.h>

namespace symresolvexx::utils::platform {

LoadedModule::LoadedModule(std::string_view moduleName) : moduleInfo_{platform::makeLoadedModuleInfo(moduleName)} {}

std::uintptr_t LoadedModule::getBeginAddress() const noexcept
{
    return moduleInfo_.moduleBeginAddress;
}

std::uintptr_t LoadedModule::getEndAddress() const noexcept
{
    return moduleInfo_.moduleEndAddress;
}

std::filesystem::path LoadedModule::getPath() const noexcept
{
    return moduleInfo_.modulePath;
}

std::string LoadedModule::getName() const noexcept
{
    return moduleInfo_.moduleName;
}

} // namespace symresolvexx::utils::platform