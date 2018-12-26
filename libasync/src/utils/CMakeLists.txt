aux_source_directory(. UTILSSSDIRSRCS)
add_library(utils ${UTILSSSDIRSRCS})
set_property(TARGET utils PROPERTY CXX_STANDARD 17)
set_property(TARGET utils PROPERTY CXX_STANDARD_REQUIRED ON)