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