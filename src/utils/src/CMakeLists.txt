add_subdirectory(platform-defines)

set(SOURCES 
            utils.cc
)
target_sources(symresolvexx-utils PRIVATE ${SOURCES})

target_link_libraries(symresolvexx-utils PRIVATE symresolvexx-utils-platform-defines)