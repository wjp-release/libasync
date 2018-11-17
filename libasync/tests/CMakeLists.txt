aux_source_directory(. DIRSRCS)
add_executable(tests ${DIRSRCS})
target_compile_options(tests PRIVATE --coverage)
target_link_libraries(tests gtest_main async --coverage)
