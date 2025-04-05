#include <symresolvexx/symresolvexx.h>
#include <utils/utils.h>

namespace symresolvexx {
auto resolveSymbol(std::string_view symbolName, std::string_view moduleName) -> void*
{
    const auto tryGetExported {utils::getExportedSymbolAddress(moduleName, symbolName)};
    if (0 != tryGetExported) {
        return reinterpret_cast<void*>(tryGetExported);
    }

    const auto symbolsAndAddresses{utils::getLoadedModuleSymbols(moduleName)};
    if (symbolsAndAddresses.contains({symbolName.data(), symbolName.size()})) {
        return reinterpret_cast<void*>(symbolsAndAddresses.at({symbolName.data(), symbolName.size()}));
    }

    return nullptr;
}

auto resolveSymbolByPatternSignature(std::span<std::byte> symbolSignature, std::string_view moduleName) -> void*
{
    return nullptr;
}
} // namespace symresolvexx
