aux_source_directory(. DIRSRCS)
add_library(async ${DIRSRCS})
set_property(TARGET async PROPERTY CXX_STANDARD 17)
set_property(TARGET async PROPERTY CXX_STANDARD_REQUIRED ON)