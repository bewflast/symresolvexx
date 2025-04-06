#pragma once

#include <cstddef>
#include <span>
#include <optional>
#include <string>
#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace symresolvexx::utils {
auto getExportedSymbolAddress(std::string_view moduleName, std::string_view symbolName) -> std::uintptr_t;
auto getLoadedModuleSymbols(std::string_view loadedModuleName) -> std::unordered_map<std::string, std::uintptr_t>;
auto getLoadedModuleBeginAndEndAddresses(std::string_view loadedModuleName)
    -> std::pair<std::uintptr_t, std::uintptr_t>;
auto findSignaturePatternInMemory(
    std::span<std::optional<std::byte>> symbolSignaturePattern, std::span<std::byte> memoryView) -> std::uintptr_t;
} // namespace symresolvexx::utils