#include "utils/utils.h"

#include <cstdint>
#include <platform-defines/platform-defines.h>

#include <memory>
#include <string_view>
#include <iterator>

namespace {
auto isSymbolExported(std::string_view moduleName) -> bool
{
    std::unique_ptr<std::remove_pointer_t<symresolvexx::utils::platform::ModuleHandle>,
        decltype(&symresolvexx::utils::platform::closeLoadedModuleHandle)>
        moduleHandle{symresolvexx::utils::platform::openLoadedModuleHandle(moduleName),
            symresolvexx::utils::platform::closeLoadedModuleHandle};

    return nullptr != moduleHandle;
}
} // namespace
namespace symresolvexx::utils {
auto getExportedSymbolAddress(std::string_view moduleName, std::string_view symbolName) -> std::uintptr_t
{
    platform::ScopedModuleHandle moduleHandle{moduleName};

    if (nullptr == moduleHandle.get()) {
        return 0;
    }

    auto symbolNameStr{std::string{symbolName}}; // dlsym takes char*

    return platform::getExportedSymbolAddress(moduleHandle.get(), symbolNameStr.data());
}

auto getLoadedModuleSymbols(std::string_view loadedModuleName) -> std::unordered_map<std::string, std::uintptr_t>
{
    const auto loadedModuleInfo{platform::getLoadedModuleInfo(loadedModuleName)};
    return platform::getLoadedModuleSymbolsAddresses(loadedModuleInfo);
}

auto getLoadedModuleBeginAndEndAddresses(std::string_view loadedModuleName) -> std::pair<std::uintptr_t, std::uintptr_t>
{
    const auto loadedModuleInfo{platform::getLoadedModuleInfo(loadedModuleName)};

    return {loadedModuleInfo.getBeginAddress(), loadedModuleInfo.getEndAddress()};
}

auto findSignaturePatternInMemory(std::span<const std::optional<std::byte>> symbolSignaturePattern,
    std::span<const std::byte>                                              memoryView) -> std::uintptr_t
{
    if (symbolSignaturePattern.empty() || memoryView.size() < symbolSignaturePattern.size()) {
        return 0;
    }

    const auto* scanStartAddr = memoryView.data();
    const auto* scanEndAddr   = std::next(
        scanStartAddr, static_cast<std::int64_t>(memoryView.size() - symbolSignaturePattern.size()));

    for (const auto* currentMemoryPtr = scanStartAddr; currentMemoryPtr <= scanEndAddr;
        currentMemoryPtr              = std::next(currentMemoryPtr)) {
        bool matched = true;

        for (auto i = size_t{0}; i < symbolSignaturePattern.size(); ++i) {
            if (symbolSignaturePattern[i].has_value()
                and *std::next(currentMemoryPtr, static_cast<std::int64_t>(i)) != symbolSignaturePattern[i].value()) {
                matched = false;
                break;
            }
        }

        if (matched) {
            return reinterpret_cast<std::uintptr_t>(currentMemoryPtr);
        }
    }

    return 0;
}
} // namespace symresolvexx::utils