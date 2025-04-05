#include "platform-defines/ScopedModuleHandle.h"
#include <platform-defines/platform-defines.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <DbgHelp.h>
#include <processthreadsapi.h>
#include <psapi.h>

#include <array>
#include <string>
#include <memory>
#include <cstdint>
#include <type_traits>
#include <string_view>
#include <unordered_map>

namespace {
inline static constexpr const auto* DLL_SHARED_LIB_FILE_EXTENSION {".dll"};

inline static constexpr const auto UNDECORATED_SYMBOL_BUFFER_SIZE {1024UL};
inline static constexpr const auto LOADED_MODULE_FILE_PATH_BUFFER_SIZE {1024UL};

inline static constexpr const auto UNDECORATE_SYMBOL_RULES_FLAGS {
    UNDNAME_COMPLETE                |
    UNDNAME_NO_THISTYPE             |
    UNDNAME_NO_MS_THISTYPE          |
    UNDNAME_NO_MS_KEYWORDS          |
    UNDNAME_NO_MEMBER_TYPE          |
    UNDNAME_NO_CV_THISTYPE          |
    UNDNAME_NO_SPECIAL_SYMS         |
    UNDNAME_NO_ALLOCATION_MODEL     |
    UNDNAME_NO_RETURN_UDT_MODEL     |
    UNDNAME_NO_THROW_SIGNATURES     |
    UNDNAME_NO_FUNCTION_RETURNS     |
    UNDNAME_NO_ACCESS_SPECIFIERS    |
    UNDNAME_NO_LEADING_UNDERSCORES  |
    UNDNAME_NO_ALLOCATION_LANGUAGE  

};

auto formatUnDecoratedSymbol(std::string_view symbol) -> std::string 
{
    auto formattedSymbol  {std::string{symbol}};

    const std::array<std::string_view, 3> substringsToRemove {" *", "class ", " &"};
    
    for (const auto& toRemove : substringsToRemove) {
        for (auto spacePtr {formattedSymbol.find(toRemove)}; spacePtr != std::string::npos; spacePtr = formattedSymbol.find(toRemove)) {
            formattedSymbol.erase(spacePtr, toRemove.length() - (toRemove[1] == '*' || toRemove[1] == '&' ? 1 : 0));
        }
    }

    return formattedSymbol;
}

auto unDecorateSymbol(const std::string& symbolName) -> std::string
{
    auto undecoratedSymbolNameBuffer{std::array<char, LOADED_MODULE_FILE_PATH_BUFFER_SIZE>{'\0'}};
    std::ranges::fill(undecoratedSymbolNameBuffer, '\0');

    if (0 == UnDecorateSymbolName(symbolName.c_str(), undecoratedSymbolNameBuffer.data(), undecoratedSymbolNameBuffer.size(), UNDECORATE_SYMBOL_RULES_FLAGS)) {
        return symbolName;
    }

    return formatUnDecoratedSymbol(std::string{undecoratedSymbolNameBuffer.data()});
}

BOOL CALLBACK enumSymbolsCallback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
    auto* symbolsMap{reinterpret_cast<std::unordered_map<std::string, std::uintptr_t>*>(UserContext)};
    symbolsMap->emplace(pSymInfo->Name, pSymInfo->Address);
    symbolsMap->emplace(unDecorateSymbol(pSymInfo->Name), pSymInfo->Address);
    return TRUE;
}

auto getLoadedModuleFilePath(HMODULE moduleHandle) -> std::filesystem::path
{
    auto loadedModuleFilePathBuffer(std::string(LOADED_MODULE_FILE_PATH_BUFFER_SIZE, '\0'));

    GetModuleFileNameA(moduleHandle, loadedModuleFilePathBuffer.data(), loadedModuleFilePathBuffer.size());

    return loadedModuleFilePathBuffer;
}

auto getLoadedModuleEndAddress(HMODULE moduleHandle) -> std::uintptr_t
{
    auto currentProcessHandle{GetCurrentProcess()};
    auto moduleInfo{MODULEINFO{0}};

    GetModuleInformation(currentProcessHandle, moduleHandle, &moduleInfo, sizeof(moduleInfo));

    const auto loadedModuleSize{static_cast<std::uintptr_t>(moduleInfo.SizeOfImage)};
    const auto loadedModuleBeginAddress{reinterpret_cast<std::uintptr_t>(moduleHandle)};

    return loadedModuleBeginAddress + loadedModuleSize;
}

} //namespace

namespace symresolvexx::utils::platform {
auto getLoadedModuleInfo(std::string_view moduleName) -> LoadedModule
{
    return LoadedModule{moduleName};
}

auto openLoadedModuleHandle(std::string_view moduleName) -> ModuleHandle
{
    const std::string moduleFullName{std::string_view::npos == moduleName.find(DLL_SHARED_LIB_FILE_EXTENSION) ? std::string{moduleName} + DLL_SHARED_LIB_FILE_EXTENSION : moduleName};
    auto* moduleHandle{GetModuleHandleA(moduleFullName.c_str())};

    return static_cast<void*>(moduleHandle);
}

auto closeLoadedModuleHandle([[maybe_unused]] ModuleHandle moduleHandle) -> void 
{}

auto getExportedSymbolAddress(ModuleHandle handle, char* symbol) -> std::uintptr_t
{
    return reinterpret_cast<std::uintptr_t>(GetProcAddress(static_cast<HMODULE>(handle), symbol));
}

auto getLoadedModuleSymbolsAddresses(const LoadedModule& loadedModuleInfo)
    -> std::unordered_map<std::string, std::uintptr_t>
{
    auto currentProcessHandle{GetCurrentProcess()};
    auto options = SymGetOptions();
    SymSetOptions(options & ~SYMOPT_UNDNAME); 
    SymInitialize(currentProcessHandle, nullptr, FALSE);
    std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(&SymCleanup)> symCleanupCallGuard{
        currentProcessHandle, SymCleanup};

    auto loadedSymModule{
        SymLoadModuleEx(
            currentProcessHandle, 
            nullptr, 
            loadedModuleInfo.getPath().string().c_str(), 
            nullptr, 
            loadedModuleInfo.getBeginAddress(), 
            0, 
            nullptr, 
            0
        )
    };

    auto symbolsAndAddresses{std::unordered_map<std::string, std::uintptr_t>{}};
    SymEnumSymbols(currentProcessHandle, loadedSymModule, "*", enumSymbolsCallback, &symbolsAndAddresses);

    return symbolsAndAddresses;
}

auto makeLoadedModuleInfo(std::string_view moduleName) -> LoadedModule::ModuleInfo
{
    auto moduleInfo {symresolvexx::utils::platform::LoadedModule::ModuleInfo{}};
    auto moduleHandle {ScopedModuleHandle{moduleName}};

    moduleInfo.modulePath = getLoadedModuleFilePath(static_cast<HMODULE>(moduleHandle.get()));
    moduleInfo.moduleBeginAddress = reinterpret_cast<std::uintptr_t>(moduleHandle.get());
    moduleInfo.moduleEndAddress = getLoadedModuleEndAddress(static_cast<HMODULE>(moduleHandle.get()));

    return moduleInfo;
}


auto raiseSystemErrorException(const std::string& message) -> void
{
    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), message};
}

} // namespace symresolvexx::utils::platform
