aux_source_directory(. SRC_DIR)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(SRC frame/window.cc)
set(SRC ${SRC} log/log.cc)

add_library(wltk SHARED ${SRC})
