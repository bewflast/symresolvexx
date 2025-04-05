add_subdirectory(utils)

set(SOURCES 
            symresolvexx.cc
)
target_sources(symresolvexx PRIVATE ${SOURCES})
target_link_libraries(symresolvexx PRIVATE symresolvexx-utils)