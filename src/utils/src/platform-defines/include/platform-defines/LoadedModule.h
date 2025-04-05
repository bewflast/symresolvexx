#pragma once

#include <cstdint>
#include <filesystem>

namespace symresolvexx::utils::platform {
class LoadedModule final
{
public:
    explicit LoadedModule(std::string_view moduleName);

    LoadedModule(const LoadedModule& other)             = delete;
    LoadedModule& operator=(const LoadedModule& other)  = delete;
    
    LoadedModule(const LoadedModule&& other)            = delete;
    LoadedModule& operator=(const LoadedModule&& other) = delete;
    
    ~LoadedModule() = default;
    
    [[nodiscard]] std::string           getName() const noexcept;
    [[nodiscard]] std::filesystem::path getPath() const noexcept;
    [[nodiscard]] std::uintptr_t        getEndAddress() const noexcept;
    [[nodiscard]] std::uintptr_t        getBeginAddress() const noexcept;
    
    struct ModuleInfo
    {
        std::string           moduleName;
        std::filesystem::path modulePath;
        std::uintptr_t        moduleBeginAddress;
        std::uintptr_t        moduleEndAddress;
    };
    
private:    
    const ModuleInfo moduleInfo_;
};
} // namespace symresolvexx::utils