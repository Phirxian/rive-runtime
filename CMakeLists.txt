cmake_minimum_required(VERSION 3.16)
project(rive_runtime C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Default install prefix (override with: cmake -DCMAKE_INSTALL_PREFIX=/path)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix" FORCE)
endif()

# Config
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/RiveConfig.cmake")

# Dependency libraries (run ./scripts/download_deps.sh first)
add_subdirectory(dependencies)

# Rive core library
add_subdirectory(rive)

# Decoders
add_subdirectory(decoders)

# Renderer (PLS)
add_subdirectory(renderer)

# GLFW example
add_subdirectory(examples/glfw_example)

# --- Install (make install) ---
include(GNUInstallDirs)

# Executable
install(TARGETS glfw_example
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Static libraries (for linking from other projects).
# Dependencies (rive_yoga, rive_harfbuzz, rive_sheenbidi, miniaudio) are required
# when linking against librive.a; link order: -lrive -lrive_pls_renderer -lrive_decoders
# -lrive_yoga -lrive_harfbuzz -lrive_sheenbidi -lminiaudio
install(TARGETS rive rive_decoders rive_pls_renderer rive_yoga rive_harfbuzz rive_sheenbidi miniaudio
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Public headers (layout: include/rive/..., include/utils/...)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/renderer/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/decoders/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
# Link instructions for external projects
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/INSTALL_LINKING.txt
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/rive-runtime
  RENAME INSTALL_LINKING.txt
)
