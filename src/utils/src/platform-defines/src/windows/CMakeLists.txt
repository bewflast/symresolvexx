set(SOURCES 
            platform-defines.cc
)
target_sources(symresolvexx-utils-platform-defines PRIVATE ${SOURCES})
target_link_libraries(symresolvexx-utils-platform-defines PRIVATE DbgHelp.lib)
