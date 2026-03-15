# Build dependency libraries from dependencies/ (populated by scripts/download_deps.sh)
# Paths are set in cmake/RiveConfig.cmake

if(NOT EXISTS "${HARFBUZZ_DIR}/src")
  message(FATAL_ERROR "Dependencies not found. Run: ./scripts/download_deps.sh")
endif()

# --- HarfBuzz ---
# When Rive uses harfbuzz renames, rive_harfbuzz must be built with the same renames
# so that librive.a (calling rive_hb_*) links against librive_harfbuzz.a (defining rive_hb_*).
file(GLOB HARFBUZZ_SOURCES "${HARFBUZZ_DIR}/src/*.cc")
list(FILTER HARFBUZZ_SOURCES EXCLUDE REGEX ".*hb-buffer-serialize\\.cc$")
list(FILTER HARFBUZZ_SOURCES EXCLUDE REGEX ".*hb-buffer-verify\\.cc$")
add_library(rive_harfbuzz STATIC ${HARFBUZZ_SOURCES})
target_include_directories(rive_harfbuzz PUBLIC "${HARFBUZZ_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(rive_harfbuzz PRIVATE HB_NO_BUFFER_SERIALIZE HB_NO_BUFFER_VERIFY)
if(RIVE_WITH_TEXT AND NOT RIVE_NO_HARFBUZZ_RENAMES AND EXISTS "${RIVE_DEPS_DIR}/rive_harfbuzz_renames.h")
  if(MSVC)
    target_compile_options(rive_harfbuzz PRIVATE "/FI${RIVE_DEPS_DIR}/rive_harfbuzz_renames.h")
  else()
    target_compile_options(rive_harfbuzz PRIVATE "-include ${RIVE_DEPS_DIR}/rive_harfbuzz_renames.h")
  endif()
endif()

# --- SheenBidi ---
set(SHEENBIDI_SOURCES
  "${SHEENBIDI_DIR}/Source/BidiChain.c"
  "${SHEENBIDI_DIR}/Source/BidiTypeLookup.c"
  "${SHEENBIDI_DIR}/Source/BracketQueue.c"
  "${SHEENBIDI_DIR}/Source/GeneralCategoryLookup.c"
  "${SHEENBIDI_DIR}/Source/IsolatingRun.c"
  "${SHEENBIDI_DIR}/Source/LevelRun.c"
  "${SHEENBIDI_DIR}/Source/PairingLookup.c"
  "${SHEENBIDI_DIR}/Source/RunQueue.c"
  "${SHEENBIDI_DIR}/Source/SBAlgorithm.c"
  "${SHEENBIDI_DIR}/Source/SBBase.c"
  "${SHEENBIDI_DIR}/Source/SBCodepointSequence.c"
  "${SHEENBIDI_DIR}/Source/SBLine.c"
  "${SHEENBIDI_DIR}/Source/SBLog.c"
  "${SHEENBIDI_DIR}/Source/SBMirrorLocator.c"
  "${SHEENBIDI_DIR}/Source/SBParagraph.c"
  "${SHEENBIDI_DIR}/Source/SBScriptLocator.c"
  "${SHEENBIDI_DIR}/Source/ScriptLookup.c"
  "${SHEENBIDI_DIR}/Source/ScriptStack.c"
  "${SHEENBIDI_DIR}/Source/StatusStack.c"
)
add_library(rive_sheenbidi STATIC ${SHEENBIDI_SOURCES})
target_include_directories(rive_sheenbidi PUBLIC "${SHEENBIDI_DIR}/Headers")

# --- Yoga ---
set(YOGA_SOURCES
  "${YOGA_DIR}/yoga/Utils.cpp"
  "${YOGA_DIR}/yoga/YGConfig.cpp"
  "${YOGA_DIR}/yoga/YGLayout.cpp"
  "${YOGA_DIR}/yoga/YGEnums.cpp"
  "${YOGA_DIR}/yoga/YGNodePrint.cpp"
  "${YOGA_DIR}/yoga/YGNode.cpp"
  "${YOGA_DIR}/yoga/YGValue.cpp"
  "${YOGA_DIR}/yoga/YGStyle.cpp"
  "${YOGA_DIR}/yoga/Yoga.cpp"
  "${YOGA_DIR}/yoga/event/event.cpp"
  "${YOGA_DIR}/yoga/log.cpp"
)
add_library(rive_yoga STATIC ${YOGA_SOURCES})
target_include_directories(rive_yoga PUBLIC "${YOGA_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(rive_yoga PRIVATE YOGA_EXPORT=)

# --- Miniaudio ---
if(APPLE AND (IOS OR CMAKE_OSX_SYSROOT MATCHES "iphone"))
  set(MINIAUDIO_SOURCES "${MINIAUDIO_DIR}/miniaudio.c")
else()
  set(MINIAUDIO_SOURCES "${MINIAUDIO_DIR}/miniaudio.c")
endif()
add_library(miniaudio STATIC ${MINIAUDIO_SOURCES})
target_include_directories(miniaudio PUBLIC "${MINIAUDIO_DIR}")

# --- Zlib ---
set(ZLIB_SOURCES
  "${ZLIB_DIR}/adler32.c"
  "${ZLIB_DIR}/compress.c"
  "${ZLIB_DIR}/crc32.c"
  "${ZLIB_DIR}/deflate.c"
  "${ZLIB_DIR}/gzclose.c"
  "${ZLIB_DIR}/gzlib.c"
  "${ZLIB_DIR}/gzread.c"
  "${ZLIB_DIR}/gzwrite.c"
  "${ZLIB_DIR}/inflate.c"
  "${ZLIB_DIR}/infback.c"
  "${ZLIB_DIR}/inftrees.c"
  "${ZLIB_DIR}/inffast.c"
  "${ZLIB_DIR}/trees.c"
  "${ZLIB_DIR}/uncompr.c"
  "${ZLIB_DIR}/zutil.c"
)
add_library(zlib STATIC ${ZLIB_SOURCES})
target_include_directories(zlib PUBLIC "${ZLIB_DIR}")

# --- LibPNG ---
if(NOT EXISTS "${CMAKE_BINARY_DIR}/include/libpng")
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include/libpng")
  file(COPY "${LIBPNG_DIR}/scripts/pnglibconf.h.prebuilt" DESTINATION "${CMAKE_BINARY_DIR}/include/libpng")
  file(RENAME "${CMAKE_BINARY_DIR}/include/libpng/pnglibconf.h.prebuilt" "${CMAKE_BINARY_DIR}/include/libpng/pnglibconf.h")
endif()
file(GLOB LIBPNG_SOURCES "${LIBPNG_DIR}/png*.c")
add_library(libpng STATIC ${LIBPNG_SOURCES})
target_include_directories(libpng PUBLIC "${LIBPNG_DIR}" "${ZLIB_DIR}" "${CMAKE_BINARY_DIR}/include/libpng")
target_link_libraries(libpng PRIVATE zlib)

# --- LibJPEG (explicit list; exclude jmemdos.c and other DOS-specific files) ---
set(LIBJPEG_SOURCES
  "${LIBJPEG_DIR}/jaricom.c"
  "${LIBJPEG_DIR}/jcapimin.c"
  "${LIBJPEG_DIR}/jcapistd.c"
  "${LIBJPEG_DIR}/jcarith.c"
  "${LIBJPEG_DIR}/jccoefct.c"
  "${LIBJPEG_DIR}/jccolor.c"
  "${LIBJPEG_DIR}/jcdctmgr.c"
  "${LIBJPEG_DIR}/jchuff.c"
  "${LIBJPEG_DIR}/jcinit.c"
  "${LIBJPEG_DIR}/jcmainct.c"
  "${LIBJPEG_DIR}/jcmarker.c"
  "${LIBJPEG_DIR}/jcmaster.c"
  "${LIBJPEG_DIR}/jcomapi.c"
  "${LIBJPEG_DIR}/jcparam.c"
  "${LIBJPEG_DIR}/jcprepct.c"
  "${LIBJPEG_DIR}/jcsample.c"
  "${LIBJPEG_DIR}/jctrans.c"
  "${LIBJPEG_DIR}/jdapimin.c"
  "${LIBJPEG_DIR}/jdapistd.c"
  "${LIBJPEG_DIR}/jdarith.c"
  "${LIBJPEG_DIR}/jdatadst.c"
  "${LIBJPEG_DIR}/jdatasrc.c"
  "${LIBJPEG_DIR}/jdcoefct.c"
  "${LIBJPEG_DIR}/jdcolor.c"
  "${LIBJPEG_DIR}/jddctmgr.c"
  "${LIBJPEG_DIR}/jdhuff.c"
  "${LIBJPEG_DIR}/jdinput.c"
  "${LIBJPEG_DIR}/jdmainct.c"
  "${LIBJPEG_DIR}/jdmarker.c"
  "${LIBJPEG_DIR}/jdmaster.c"
  "${LIBJPEG_DIR}/jdmerge.c"
  "${LIBJPEG_DIR}/jdpostct.c"
  "${LIBJPEG_DIR}/jdsample.c"
  "${LIBJPEG_DIR}/jdtrans.c"
  "${LIBJPEG_DIR}/jerror.c"
  "${LIBJPEG_DIR}/jfdctflt.c"
  "${LIBJPEG_DIR}/jfdctfst.c"
  "${LIBJPEG_DIR}/jfdctint.c"
  "${LIBJPEG_DIR}/jidctflt.c"
  "${LIBJPEG_DIR}/jidctfst.c"
  "${LIBJPEG_DIR}/jidctint.c"
  "${LIBJPEG_DIR}/jquant1.c"
  "${LIBJPEG_DIR}/jquant2.c"
  "${LIBJPEG_DIR}/jutils.c"
  "${LIBJPEG_DIR}/jmemmgr.c"
  "${LIBJPEG_DIR}/jmemansi.c"
)
add_library(libjpeg STATIC ${LIBJPEG_SOURCES})
target_include_directories(libjpeg PUBLIC "${LIBJPEG_DIR}")

# --- LibWebP (include demux for WebPDemux* used by decode_webp.cpp) ---
file(GLOB LIBWEBP_DSP "${LIBWEBP_DIR}/src/dsp/*.c")
file(GLOB LIBWEBP_UTILS "${LIBWEBP_DIR}/src/utils/*.c")
file(GLOB LIBWEBP_DEC "${LIBWEBP_DIR}/src/dec/*.c")
set(LIBWEBP_DEMUX
  "${LIBWEBP_DIR}/src/demux/anim_decode.c"
  "${LIBWEBP_DIR}/src/demux/demux.c"
)
set(LIBWEBP_SOURCES ${LIBWEBP_DSP} ${LIBWEBP_UTILS} ${LIBWEBP_DEC} ${LIBWEBP_DEMUX})
add_library(libwebp STATIC ${LIBWEBP_SOURCES})
target_include_directories(libwebp PUBLIC "${LIBWEBP_DIR}" "${LIBWEBP_DIR}/src")

# --- GLFW (for examples) ---
if(EXISTS "${GLFW_DIR}/CMakeLists.txt")
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  add_subdirectory("${GLFW_DIR}" "${CMAKE_BINARY_DIR}/glfw" EXCLUDE_FROM_ALL)
endif()
