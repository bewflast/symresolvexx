<h1 align="center" id="title">SYMRESOLVEXX</h1>

<p id="description">Simple library for runtime resolving symbols by their's signature patterns or names</p>

  
  
<h2>🧐 Features</h2>

Here're some of the project's best features:

*   Another symbol resolving library
*   Ideologically splitted implementations of symbol resolving - there is no using of pattern scan algorithm when resolving symbol by it's name: library iterates over all available symbols(debug + exported) of target module (shared lib)
*   simple interface containing only 2 functions (resolve by symbol name and resolve by signature pattern)
*   project written on modern C++ (C++20)

<h2>🛠️ Installation Steps:</h2>

<p>1. How to add project with cmake (example)</p>

```
add_subdirectory(external/symresolvexx) target_link_libraries(example PRIVATE symresolvexx)
```

<h2>🍰 Contribution Guidelines:</h2>

Any contributions you make are greatly appreciated. If you have a suggestion that would make this better please fork the repo and create a pull request. Don't forget to give the project a star! Thanks again!

<h2>🛡️ License:</h2>

This project is licensed under the MIT license

<h2>EXAMPLE</h2>

*   Resolve by symbol's name
```C++
const auto* CThreadInitAddr{symresolvexx::resolveSymbol::byName("_ZN7CThread4InitEv", "server")};
const auto* CBaseClientExecuteStringCommand{symresolvexx::resolveSymbol::byName("?ExecuteStringCommand@CBaseClient@@UEAA_NPEBD@Z ", "server")};

//you can also use demangled name but only if you are sure what are you doing
//the format is [sub::]name([const ]param[*])
//anyway, mangled/decorated symbols' names are preferred to use

const auto* CThreadInitAddr{symresolvexx::resolveSymbol::byName("CThread::Init()", "server")};
const auto* CThreadInitAddr{symresolvexx::resolveSymbol::byName("CBaseClient::ExecuteStringCommand(char const*)", "server")};
```

*   Resolve by symbol's signature pattern

```C++
namespace  {
constexpr auto strPatternToSpan(std::string_view pattern) -> std::vector<std::optional<std::byte>>
{
  auto result {std::vector<std::optional<std::byte>>{}};
  result.reserve(pattern.size());
  for (const auto& byteChar : pattern) {
    result.emplace_back(byteChar == '*' ? std::optional<std::byte>{std::nullopt} : static_cast<std::byte>(byteChar));
  }
  return result;
}
}

constexpr const auto* PhysOnCleanupDeleteList {"\xB9****\xE8****\x8B\x0D****\x85\xC9\x74\x08\x8B\x01\xFF\xA0\xC4"};
const auto* PhysOnCleanupDeleteListAddr {symresolvexx::resolveSymbol::bySignaturePattern(strPatternToSpan(PhysOnCleanupDeleteList), "server")};
```
