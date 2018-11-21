aux_source_directory(. SSDIRSRCS)
add_library(spawnsync ${SSDIRSRCS})
set_property(TARGET spawnsync PROPERTY CXX_STANDARD 17)
set_property(TARGET spawnsync PROPERTY CXX_STANDARD_REQUIRED ON)