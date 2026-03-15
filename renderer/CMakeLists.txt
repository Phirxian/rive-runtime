# PLS renderer and shader code generation
set(RIVE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(PLS_SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders)
set(PLS_GENERATED_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders)

# Build shaders (minify .glsl -> generated headers)
file(MAKE_DIRECTORY "${PLS_GENERATED_DIR}")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/pls_shaders.stamp"
  COMMAND ${CMAKE_MAKE_PROGRAM} -C "${PLS_SHADERS_DIR}" -j${CMAKE_BUILD_PARALLEL_LEVEL}
    "OUT=${PLS_GENERATED_DIR}"
  COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/pls_shaders.stamp"
  COMMENT "Building PLS shaders"
  VERBATIM
)
add_custom_target(pls_shaders DEPENDS "${CMAKE_BINARY_DIR}/pls_shaders.stamp")

# Core renderer sources (platform-agnostic)
set(PLS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rive_renderer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/render_context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rive_render_path.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rive_render_image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rive_render_paint.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/render_context_helper_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rive_render_factory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gr_triangulator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/intersection_board.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gradient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu_resource.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sk_rectanizer_skyline.cpp
)

# GL backend (desktop: Windows, Linux, macOS)
list(APPEND PLS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/gl_state.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/gl_utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/load_store_actions_ext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/render_buffer_gl_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/render_context_gl_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/render_target_gl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/pls_impl_webgl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/pls_impl_rw_texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/egl.c
  ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/gles2.c
  ${CMAKE_CURRENT_SOURCE_DIR}/glad/glad_custom.c
)

if(WIN32)
  list(APPEND PLS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/d3d/pipeline_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/d3d/d3d_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/d3d11/render_context_d3d_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/d3d12/d3d12_pipeline_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/d3d12/render_context_d3d12_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/d3d12/d3d12_utils.cpp
  )
endif()
if(APPLE AND NOT IOS)
  file(GLOB PLS_METAL_MM ${CMAKE_CURRENT_SOURCE_DIR}/src/metal/*.mm)
  list(APPEND PLS_SOURCES ${PLS_METAL_MM})
endif()

add_library(rive_pls_renderer STATIC ${PLS_SOURCES})
add_dependencies(rive_pls_renderer pls_shaders)
target_include_directories(rive_pls_renderer PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/glad
  ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/include
  ${RIVE_ROOT}/include
  ${RIVE_ROOT}/decoders/include
)
target_compile_definitions(rive_pls_renderer PRIVATE
  RIVE_DESKTOP_GL
  RIVE_DECODERS
)
target_link_libraries(rive_pls_renderer PUBLIC rive rive_decoders)
# Clang-only; GCC does not support -Wshorten-64-to-32
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT MSVC)
  target_compile_options(rive_pls_renderer PRIVATE -Wshorten-64-to-32)
endif()
if(UNIX AND NOT APPLE)
  target_compile_options(rive_pls_renderer PRIVATE -ffp-contract=on -fassociative-math -Wno-psabi)
endif()
if(WIN32)
  target_compile_definitions(rive_pls_renderer PRIVATE RIVE_WINDOWS NOMINMAX)
  target_link_libraries(rive_pls_renderer PRIVATE opengl32 d3d11 d3d12 dxguid dxgi d3dcompiler)
endif()
if(APPLE)
  target_link_libraries(rive_pls_renderer PRIVATE
    "-framework Cocoa" "-framework Metal" "-framework QuartzCore" "-framework IOKit"
  )
  target_compile_options(rive_pls_renderer PRIVATE -fobjc-arc)
endif()
if(UNIX AND NOT APPLE)
  target_link_libraries(rive_pls_renderer PRIVATE X11)
endif()
