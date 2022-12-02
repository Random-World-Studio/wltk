project(wltk)

set(CMAKE_CXX_STANDARD 20)

set(WL_LIBS wayland-server wayland-client wayland-cursor wayland-egl)
set(GL_LIBS EGL GLESv2)

set(LIBS ${WL_LIBS} spdlog)

aux_source_directory(. PROJ_DIR)
add_subdirectory(src)
add_subdirectory(demo)
