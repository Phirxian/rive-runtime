add_executable(glfw_example main.cpp)
target_compile_definitions(glfw_example PRIVATE RIVE_DESKTOP_GL)
target_include_directories(glfw_example PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/renderer/include
  ${CMAKE_SOURCE_DIR}/renderer
  ${CMAKE_SOURCE_DIR}/renderer/glad
  ${CMAKE_SOURCE_DIR}/renderer/glad/include
)
target_link_libraries(glfw_example PRIVATE
  rive_pls_renderer
  glfw
)
if(UNIX AND NOT APPLE)
  target_link_libraries(glfw_example PRIVATE X11)
endif()
if(APPLE)
  target_link_libraries(glfw_example PRIVATE
    "-framework Cocoa" "-framework OpenGL" "-framework IOKit"
  )
endif()
if(WIN32)
  target_link_libraries(glfw_example PRIVATE opengl32)
endif()
