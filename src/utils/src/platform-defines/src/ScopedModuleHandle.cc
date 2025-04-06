#include "platform-defines/ScopedModuleHandle.h"
#include "platform-defines/platform-defines.h"

namespace symresolvexx::utils::platform {
ScopedModuleHandle::ScopedModuleHandle(std::string_view moduleName)
    : moduleHandle_{openLoadedModuleHandle(moduleName), platform::closeLoadedModuleHandle} {};

ModuleHandle ScopedModuleHandle::get() const
{
    return moduleHandle_.get();
}
} // namespace symresolvexx::utils::platform