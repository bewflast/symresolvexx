add_library(symresolvexx-utils STATIC)

set_target_properties(symresolvexx-utils PROPERTIES PUBLIC_HEADER 
    include/utils/utils.h
)

target_include_directories(symresolvexx-utils PUBLIC include)

add_subdirectory(src)