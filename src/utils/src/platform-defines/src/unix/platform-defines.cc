#include <platform-defines/platform-defines.h>

#include <cstdint>
#include <sstream>
#include <unordered_map>

#include <link.h>
#include <dlfcn.h>
#include <cxxabi.h>

namespace
{
inline static constexpr const auto* SO_SHARED_LIB_FILE_EXTENSION {".so"};

struct dl_phdr_iterator_callback_passed_data
{
    symresolvexx::utils::platform::LoadedModule::ModuleInfo moduleInfo {};
    std::string targetModule;
};

auto iterateCallback (struct dl_phdr_info* lib, std::size_t /*size*/, void* data) -> int 
{
    auto iteratorCallbackDataPtr {static_cast<dl_phdr_iterator_callback_passed_data*>(data)};

    const auto moduleFullPathView{std::string_view{lib->dlpi_name}};
	const auto subFrom{moduleFullPathView.find_last_of("/\\")};
    const std::string moduleNameStripped {std::string_view::npos == subFrom ? moduleFullPathView : moduleFullPathView.substr(subFrom + 1)};

    if (moduleNameStripped != iteratorCallbackDataPtr->targetModule) {
        return 0;
    }

    auto& moduleInfo {iteratorCallbackDataPtr->moduleInfo};
    auto moduleFullPath{std::string{lib->dlpi_name}};
    moduleInfo.modulePath = std::string{lib->dlpi_name};
    moduleInfo.moduleBeginAddress = static_cast<std::uintptr_t>(lib->dlpi_addr);
    moduleInfo.moduleEndAddress = 0;

    for (int i = 0; i < lib->dlpi_phnum; i++) {
        if (PT_LOAD != lib->dlpi_phdr[i].p_type) {
            continue;
        }

        auto segmentAddressEnd = moduleInfo.moduleBeginAddress + lib->dlpi_phdr[i].p_vaddr + lib->dlpi_phdr[i].p_memsz;
        if (segmentAddressEnd > moduleInfo.moduleEndAddress) {
            moduleInfo.moduleEndAddress = static_cast<std::uintptr_t>(segmentAddressEnd);
        }
    }

    return 1;
};

auto demangleSymbol(std::string_view mangledSymbolName)  -> std::string
{
    auto status {0};
    const auto demangledSymbolNameBuffer {
        std::unique_ptr<char, decltype(&free)>{
            abi::__cxa_demangle(mangledSymbolName.data(), nullptr, nullptr, &status), 
            free
        }
    };

    if (-2 == status) {
        return std::string{mangledSymbolName};
    }
    
    if (0 != status) {
        auto formattedErrorMessage {std::stringstream{}};
        formattedErrorMessage   << "Failed to demangle symbol " 
                                << mangledSymbolName
                                << " error code: "
                                <<  status;

        throw std::runtime_error(formattedErrorMessage.str());
    };

    return std::string{demangledSymbolNameBuffer.get()};
}
} //namespace

namespace symresolvexx::utils::platform {

auto getExportedSymbolAddress(ModuleHandle handle, char* symbol) -> std::uintptr_t
{
    return reinterpret_cast<std::uintptr_t>(dlsym(handle, symbol));
}

auto getLoadedModuleInfo(std::string_view moduleName) -> LoadedModule
{
    return LoadedModule{moduleName};
}

auto openLoadedModuleHandle(std::string_view moduleName) -> ModuleHandle
{
    const std::string moduleFullName{std::string_view::npos == moduleName.find(SO_SHARED_LIB_FILE_EXTENSION) ? std::string{moduleName} + SO_SHARED_LIB_FILE_EXTENSION : moduleName};

    return dlopen(moduleFullName.data(), RTLD_NOW | RTLD_NOLOAD);
}

auto getLoadedModuleSymbolsAddresses(const LoadedModule& loadedModuleInfo)
    -> std::unordered_map<std::string, std::uintptr_t>
{
    auto symbolsAndAddresses{std::unordered_map<std::string, std::uintptr_t>{}};
    
    auto scopedModule {ScopedModuleHandle{loadedModuleInfo.getName()}};

    struct link_map* map = nullptr;
    if (dlinfo(scopedModule.get(), RTLD_DI_LINKMAP, &map) != 0 || !map) {
        return symbolsAndAddresses;
    }

    ElfW(Dyn)* dynamic = map->l_ld;
    if (!dynamic) {
        return symbolsAndAddresses;
    }

    const ElfW(Sym)* symbolsTable = nullptr;
    const char* stringTable = nullptr;
    size_t stringTableSize = 0;

    for (ElfW(Dyn)* d = dynamic; d->d_tag != DT_NULL; ++d) {
        switch (d->d_tag) {
            case DT_SYMTAB:
                symbolsTable = reinterpret_cast<const ElfW(Sym)*>(d->d_un.d_ptr);
                break;
            case DT_STRTAB:
                stringTable = reinterpret_cast<const char*>(d->d_un.d_ptr);
                break;
            case DT_STRSZ:
                stringTableSize = d->d_un.d_val;
                break;
        }
    }

    if (!symbolsTable || !stringTable || !stringTableSize) {
        return symbolsAndAddresses;
    }

    const ElfW(Sym)* sym = symbolsTable;
    while (reinterpret_cast<const char*>(sym) < stringTable + stringTableSize) {
        if (sym->st_name && sym->st_value) {
            const char* name = stringTable + sym->st_name;
            if (name >= stringTable && name < stringTable + stringTableSize) {
                void* abs_addr = reinterpret_cast<void*>(map->l_addr + sym->st_value);
                symbolsAndAddresses.emplace(name, reinterpret_cast<std::uintptr_t>(abs_addr));
                symbolsAndAddresses.emplace(demangleSymbol(name), reinterpret_cast<std::uintptr_t>(abs_addr));
            }
        }
        sym++;
    }

    return symbolsAndAddresses;
}

[[maybe_unused]] auto closeLoadedModuleHandle(ModuleHandle moduleHandle) -> void
{
    dlclose(moduleHandle);
}

auto raiseSystemErrorException(const std::string& message) -> void
{
    throw std::system_error{static_cast<int>(errno), std::system_category(), message};
}

auto makeLoadedModuleInfo(std::string_view moduleName) -> LoadedModule::ModuleInfo
{
    auto iteratorData {dl_phdr_iterator_callback_passed_data{}};
    const std::string moduleFullName{std::string_view::npos == moduleName.find(SO_SHARED_LIB_FILE_EXTENSION) ? std::string{moduleName} + SO_SHARED_LIB_FILE_EXTENSION : moduleName};

    iteratorData.targetModule = moduleFullName;

    dl_iterate_phdr(iterateCallback, static_cast<void*>(&iteratorData));
    iteratorData.moduleInfo.moduleName = moduleName;

    if (iteratorData.moduleInfo.modulePath.empty()) {
        throw std::runtime_error("Failed to get module information for  " + iteratorData.targetModule);
    }

    return iteratorData.moduleInfo;
}

} // namespace symresolvexx::utils::platform