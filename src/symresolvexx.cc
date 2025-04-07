#include <symresolvexx/symresolvexx.h>
#include <utils/utils.h>

namespace symresolvexx::resolveSymbol {
auto byName(std::string_view symbolName, std::string_view moduleName) -> void*
{
    const auto tryGetExported{utils::getExportedSymbolAddress(moduleName, symbolName)};
    if (0 != tryGetExported) {
        return reinterpret_cast<void*>(tryGetExported);
    }

    const auto symbolsAndAddresses{utils::getLoadedModuleSymbols(moduleName)};
    if (symbolsAndAddresses.contains({symbolName.data(), symbolName.size()})) {
        return reinterpret_cast<void*>(symbolsAndAddresses.at({symbolName.data(), symbolName.size()}));
    }

    return nullptr;
}

auto bySignaturePattern(std::span<const std::optional<std::byte>> symbolSignaturePattern, std::string_view moduleName)
    -> void*
{
    const auto [moduleAddressBegin, moduleAddressEnd]{utils::getLoadedModuleBeginAndEndAddresses(moduleName)};
    const auto memoryView{std::span<std::byte>{
        reinterpret_cast<std::byte*>(moduleAddressBegin), reinterpret_cast<std::byte*>(moduleAddressEnd)}};

    return reinterpret_cast<void*>(utils::findSignaturePatternInMemory(symbolSignaturePattern, memoryView));
}
} // namespace symresolvexx::resolveSymbol
