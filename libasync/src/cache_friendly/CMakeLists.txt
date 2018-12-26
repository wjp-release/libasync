aux_source_directory(. CFSSDIRSRCS)
add_library(cf ${CFSSDIRSRCS})
set_property(TARGET cf PROPERTY CXX_STANDARD 17)
set_property(TARGET cf PROPERTY CXX_STANDARD_REQUIRED ON)