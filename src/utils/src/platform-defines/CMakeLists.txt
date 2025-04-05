add_library(symresolvexx-utils-platform-defines STATIC)

set_target_properties(symresolvexx-utils-platform-defines PROPERTIES PUBLIC_HEADER 
    include/platform-defines/platform-defines.h
)

target_include_directories(symresolvexx-utils-platform-defines PUBLIC include)

add_subdirectory(src)