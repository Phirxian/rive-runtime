# Rive decoders (PNG, JPEG, WebP)
set(RIVE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(RIVE_DECODERS_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/bitmap_decoder.cpp"
)
if(NOT RIVE_NO_PNG OR NOT RIVE_NO_JPEG OR NOT RIVE_NO_WEBP)
  list(APPEND RIVE_DECODERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/bitmap_decoder_thirdparty.cpp")
endif()
if(NOT RIVE_NO_PNG)
  list(APPEND RIVE_DECODERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/decode_png.cpp")
endif()
if(NOT RIVE_NO_JPEG)
  list(APPEND RIVE_DECODERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/decode_jpeg.cpp")
endif()
if(NOT RIVE_NO_WEBP)
  list(APPEND RIVE_DECODERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/decode_webp.cpp")
endif()

add_library(rive_decoders STATIC ${RIVE_DECODERS_SOURCES})
target_include_directories(rive_decoders PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${RIVE_ROOT}/include"
)
target_link_libraries(rive_decoders PUBLIC rive)
if(NOT RIVE_NO_PNG)
  target_include_directories(rive_decoders PUBLIC "${LIBPNG_DIR}" "${CMAKE_BINARY_DIR}/include/libpng")
  target_compile_definitions(rive_decoders PRIVATE RIVE_PNG)
  target_link_libraries(rive_decoders PUBLIC zlib libpng)
endif()
if(NOT RIVE_NO_JPEG)
  target_compile_definitions(rive_decoders PRIVATE RIVE_JPEG)
  target_link_libraries(rive_decoders PUBLIC libjpeg)
endif()
if(NOT RIVE_NO_WEBP)
  target_compile_definitions(rive_decoders PRIVATE RIVE_WEBP)
  target_include_directories(rive_decoders PRIVATE "${LIBWEBP_DIR}/src")
  target_link_libraries(rive_decoders PUBLIC libwebp)
endif()
