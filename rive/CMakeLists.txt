# Rive core static library
get_filename_component(RIVE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# Only compile .cpp (and .mm on Apple); headers are included by sources
file(GLOB_RECURSE RIVE_SOURCES "${RIVE_ROOT}/src/*.cpp")
list(APPEND RIVE_SOURCES "${RIVE_ROOT}/utils/no_op_factory.cpp")
if(APPLE)
  file(GLOB_RECURSE RIVE_MM "${RIVE_ROOT}/src/*.mm")
  list(APPEND RIVE_SOURCES ${RIVE_MM})
endif()

add_library(rive STATIC ${RIVE_SOURCES})
target_include_directories(rive PUBLIC
  "${RIVE_ROOT}/include"
  "${HARFBUZZ_DIR}/src"
  "${SHEENBIDI_DIR}/Headers"
  "${MINIAUDIO_DIR}"
  "${YOGA_DIR}"
)
target_compile_definitions(rive PRIVATE YOGA_EXPORT= _RIVE_INTERNAL_)
target_link_libraries(rive PUBLIC rive_harfbuzz rive_sheenbidi rive_yoga miniaudio)

target_include_directories(rive PRIVATE "${RIVE_ROOT}/dependencies")
# Copy renames headers to build dir so -include path is always valid from compiler cwd
set(RIVE_DEPS_RENAMES "${RIVE_ROOT}/dependencies")
set(RIVE_RENAMES_BUILD_DIR "${CMAKE_BINARY_DIR}/rive_renames")
file(MAKE_DIRECTORY "${RIVE_RENAMES_BUILD_DIR}")
set(HARFBUZZ_RENAMES_H "${RIVE_DEPS_RENAMES}/rive_harfbuzz_renames.h")
set(YOGA_RENAMES_H "${RIVE_DEPS_RENAMES}/rive_yoga_renames.h")
if(RIVE_WITH_TEXT AND NOT RIVE_NO_HARFBUZZ_RENAMES)
  if(EXISTS "${HARFBUZZ_RENAMES_H}")
    configure_file("${HARFBUZZ_RENAMES_H}" "${RIVE_RENAMES_BUILD_DIR}/rive_harfbuzz_renames.h" COPYONLY)
    if(MSVC)
      target_compile_options(rive PRIVATE "/FI${RIVE_RENAMES_BUILD_DIR}/rive_harfbuzz_renames.h")
    else()
      target_compile_options(rive PRIVATE "-include ${RIVE_RENAMES_BUILD_DIR}/rive_harfbuzz_renames.h")
    endif()
  else()
    message(FATAL_ERROR
      "Required file not found:\n  ${HARFBUZZ_RENAMES_H}\n"
      "This file is part of the repo (source tree, not build/). From the build dir run:\n  ls ../dependencies/rive_harfbuzz_renames.h\n"
      "If missing, restore it from git or build with -DRIVE_NO_HARFBUZZ_RENAMES=ON to skip (may cause link errors).")
  endif()
endif()
if(RIVE_WITH_LAYOUT AND NOT RIVE_NO_YOGA_RENAMES)
  if(EXISTS "${YOGA_RENAMES_H}")
    configure_file("${YOGA_RENAMES_H}" "${RIVE_RENAMES_BUILD_DIR}/rive_yoga_renames.h" COPYONLY)
    if(MSVC)
      target_compile_options(rive PRIVATE "/FI${RIVE_RENAMES_BUILD_DIR}/rive_yoga_renames.h")
    else()
      target_compile_options(rive PRIVATE "-include ${RIVE_RENAMES_BUILD_DIR}/rive_yoga_renames.h")
    endif()
  else()
    message(FATAL_ERROR
      "Required file not found:\n  ${YOGA_RENAMES_H}\n"
      "From the build dir run: ls ../dependencies/rive_yoga_renames.h\n"
      "If missing, use -DRIVE_NO_YOGA_RENAMES=ON to skip (may cause link errors).")
  endif()
endif()
if(RIVE_WITH_AUDIO)
  target_compile_definitions(rive PRIVATE WITH_RIVE_AUDIO MA_NO_RESOURCE_MANAGER)
endif()
if(RIVE_WITH_LAYOUT)
  target_compile_definitions(rive PRIVATE WITH_RIVE_LAYOUT)
endif()
if(RIVE_WITH_TEXT)
  target_compile_definitions(rive PRIVATE WITH_RIVE_TEXT)
endif()

if(MSVC)
  target_compile_definitions(rive PRIVATE _USE_MATH_DEFINES)
endif()
