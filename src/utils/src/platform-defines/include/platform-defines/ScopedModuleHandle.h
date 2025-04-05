#pragma once

#include <memory>
#include <string_view>

namespace symresolvexx::utils::platform
{
    using ModuleHandle = void*;

    auto openLoadedModuleHandle(std::string_view moduleName) -> ModuleHandle;
    [[maybe_unused]] auto closeLoadedModuleHandle(ModuleHandle moduleHandle) -> void;
}

namespace symresolvexx::utils::platform
{
class ScopedModuleHandle
{
public:
    explicit ScopedModuleHandle(std::string_view moduleName);

    ScopedModuleHandle(ScopedModuleHandle&& other)  noexcept = default;
    ScopedModuleHandle& operator=(ScopedModuleHandle&& other) noexcept = default;

    ~ScopedModuleHandle() = default;

    ScopedModuleHandle() = delete;

    ScopedModuleHandle(const ScopedModuleHandle& other)             = delete;
    ScopedModuleHandle& operator=(const ScopedModuleHandle& other)  = delete;

    [[nodiscard]] ModuleHandle get() const;
private:
    std::unique_ptr<std::remove_pointer_t<ModuleHandle>, decltype(&closeLoadedModuleHandle)> moduleHandle_;
};
}