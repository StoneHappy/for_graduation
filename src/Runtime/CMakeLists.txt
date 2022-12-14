set(TARGET_NAME ${PROJECT_NAME}Runtime)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE HPP_FILES "*.hpp")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE UI_FILES "*.ui")
qt5_wrap_ui(UI_HEADERS UI_FILES) 

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${UI_FILES} ${HPP_FILES})

add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES} ${UI_FILES} ${HPP_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${PROJECT_NAME})

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC
Qt5::Core
Qt5::Gui
Qt5::Widgets
Vulkan::Vulkan
glm::glm_static
stblib
tinyobjloader
assimp::assimp
EnTT::EnTT
yaml-cpp
Recast
Detour
DetourCrowd
          )

target_include_directories(
  ${TARGET_NAME} 
  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/Shader/generated/cpp/>
)

add_dependencies(${TARGET_NAME} "Shadercompile")