aux_source_directory(. DEMO_DIR)

add_executable(demo1_wltest wltest.cc)
target_link_libraries(demo1_wltest ${WL_LIBS})
