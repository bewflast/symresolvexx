#pragma once

#include "LoadedModule.h"
#include "ScopedModuleHandle.h"

#include <memory>
#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace symresolvexx::utils::platform {
auto getExportedSymbolAddress(ModuleHandle handle, char* symbol) -> std::uintptr_t;
auto getLoadedModuleInfo(std::string_view moduleName) -> LoadedModule;
auto getLoadedModuleSymbolsAddresses(const LoadedModule& loadedModuleInfo) -> std::unordered_map<std::string, std::uintptr_t>;
auto raiseSystemErrorException(const std::string& message) -> void;
auto makeLoadedModuleInfo(std::string_view moduleName) -> LoadedModule::ModuleInfo;
} // namespace symresolvexx::utils::platform