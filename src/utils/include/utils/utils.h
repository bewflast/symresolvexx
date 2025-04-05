#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>


namespace symresolvexx::utils {
auto getExportedSymbolAddress(std::string_view moduleName, std::string_view symbolName) -> std::uintptr_t;
auto getLoadedModuleSymbols(std::string_view loadedModuleName) -> std::unordered_map<std::string, std::uintptr_t>;
auto getLoadedModuleBeginAndEndAddresses(std::string_view loadedModuleName)
    -> std::pair<std::uintptr_t, std::uintptr_t>;
} // namespace symresolvexx::utils