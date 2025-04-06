#include <cstddef>
#include <span>
#include <string_view>
#include <optional>

namespace symresolvexx::resolveSymbol {
auto byName(std::string_view symbolName, std::string_view moduleName) -> void*;
auto bySignaturePattern(std::span<std::optional<std::byte>> symbolSignaturePattern, std::string_view moduleName)
    -> void*;
} // namespace symresolvexx::resolveSymbol