#include <cstddef>
#include <span>
#include <string_view>

namespace symresolvexx {
auto resolveSymbol(std::string_view symbolName, std::string_view moduleName) -> void*;
auto resolveSymbolByPatternSignature(std::span<std::byte> symbolSignature, std::string_view moduleName) -> void*;
} // namespace symresolvexx