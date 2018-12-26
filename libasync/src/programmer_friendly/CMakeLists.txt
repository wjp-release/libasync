aux_source_directory(. PFSSDIRSRCS)
add_library(pf ${PFSSDIRSRCS})
set_property(TARGET pf PROPERTY CXX_STANDARD 17)
set_property(TARGET pf PROPERTY CXX_STANDARD_REQUIRED ON)