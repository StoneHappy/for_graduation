add_library(stblib stbImplement.cpp ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image.h)

target_include_directories(stblib 
PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)