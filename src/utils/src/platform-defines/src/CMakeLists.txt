if(WIN32)
    add_subdirectory(windows)
else()
    add_subdirectory(unix)
endif()

set(SOURCES 
    ScopedModuleHandle.cc
    LoadedModule.cc
)
target_sources(symresolvexx-utils-platform-defines PRIVATE ${SOURCES})