#include <cstddef>
#include <span>
#include <string_view>
#include <optional>

namespace symresolvexx {
auto resolveSymbol(std::string_view symbolName, std::string_view moduleName) -> void*;
auto resolveSymbolBySignaturePattern(
    std::span<std::optional<std::byte>> symbolSignaturePattern, std::string_view moduleName) -> void*;
} // namespace symresolvexx