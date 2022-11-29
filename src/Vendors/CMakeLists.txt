if(NOT TARGET gtest_main AND ENABLE_TEST)
    option(gtest_disable_pthreads "Disable uses of pthreads in gtest." ON)
    add_subdirectory(googletest)
    set_target_properties(gtest_main PROPERTIES FOLDER Vendors/googletest)
    set_target_properties(gtest PROPERTIES FOLDER Vendors/googletest)
    set_target_properties(gmock PROPERTIES FOLDER Vendors/googletest)
    set_target_properties(gmock_main PROPERTIES FOLDER Vendors/googletest)
endif()

if(NOT TARGET glm)
    option(BUILD_STATIC_LIBS "" ON)
    option(BUILD_TESTING "" OFF)
    option(GLM_TEST_ENABLE "" OFF)
    add_subdirectory(glm)
    set_target_properties(glm_static PROPERTIES FOLDER Vendors/glm)
    if(TARGET glm_shared)
        set_target_properties(glm_shared PROPERTIES FOLDER Vendors/glm)
    endif()
    
    set_target_properties(uninstall PROPERTIES FOLDER Vendors/glm)
endif()

if(NOT TARGET stblib)
    add_subdirectory(stblib)
    set_target_properties(stblib PROPERTIES FOLDER Vendors/stb)
endif()

if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
    set_target_properties(tinyobjloader PROPERTIES FOLDER Vendors/tinyobjloaderp)
endif()

if(NOT TARGET assimp)
    OPTION( BUILD_SHARED_LIBS
    "Build package with shared libraries."
    OFF
    )
    OPTION( ASSIMP_BUILD_FRAMEWORK
    "Build package as Mac OS X Framework bundle."
    OFF
    )
    OPTION( ASSIMP_DOUBLE_PRECISION
    "Set to ON to enable double precision processing"
    OFF
    )
    OPTION( ASSIMP_OPT_BUILD_PACKAGES
    "Set to ON to generate CPack configuration files and packaging targets"
    OFF
    )
    OPTION( ASSIMP_ANDROID_JNIIOSYSTEM
    "Android JNI IOSystem support is active"
    OFF
    )
    OPTION( ASSIMP_NO_EXPORT
    "Disable Assimp's export functionality."
    OFF
    )
    OPTION( ASSIMP_BUILD_ZLIB
    "Build your own zlib"
    OFF
    )
    OPTION( ASSIMP_BUILD_ASSIMP_TOOLS
    "If the supplementary tools for Assimp are built in addition to the library."
    OFF
    )
    OPTION ( ASSIMP_BUILD_SAMPLES
    "If the official samples are built as well (needs Glut)."
    OFF
    )
    OPTION ( ASSIMP_BUILD_TESTS
    "If the test suite for Assimp is built in addition to the library."
    OFF
    )
    OPTION ( ASSIMP_COVERALLS
    "Enable this to measure test coverage."
    OFF
    )
    OPTION( ASSIMP_INSTALL
    "Disable this if you want to use assimp as a submodule."
    OFF
    )
    OPTION ( ASSIMP_WARNINGS_AS_ERRORS
    "Treat all warnings as errors."
    OFF
    )
    OPTION ( ASSIMP_ASAN
    "Enable AddressSanitizer."
    OFF
    )
    OPTION ( ASSIMP_UBSAN
    "Enable Undefined Behavior sanitizer."
    OFF
    )
    OPTION ( ASSIMP_BUILD_DOCS
    "Build documentation using Doxygen."
    OFF
    )
    OPTION( ASSIMP_INJECT_DEBUG_POSTFIX
    "Inject debug postfix in .a/.so/.dll lib names"
    OFF
    )

    OPTION ( ASSIMP_IGNORE_GIT_HASH
    "Don't call git to get the hash."
    OFF
    )
    option(USE_STATIC_CRT "Link against the static runtime libraries." OFF)
    add_subdirectory(Assimp)
    set_target_properties(assimp PROPERTIES FOLDER Vendors/Assimp)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER Vendors/Assimp)
    set_target_properties(zlibstatic PROPERTIES FOLDER Vendors/Assimp)
    target_compile_options(zlibstatic PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

endif()

if(NOT TARGET EnTT)
    add_subdirectory(entt)
endif()

if(NOT TARGET yaml-cpp)
    option(YAML_CPP_BUILD_CONTRIB "Enable yaml-cpp contrib in library" OFF)
    option(YAML_CPP_BUILD_TOOLS "Enable parse tools" OFF)
    option(YAML_BUILD_SHARED_LIBS "Build yaml-cpp shared library" ${BUILD_SHARED_LIBS})
    option(YAML_CPP_INSTALL "Enable generation of yaml-cpp install targets" OFF)
    option(YAML_CPP_FORMAT_SOURCE "Format source" ON)
    add_subdirectory(yaml-cpp)
    set_target_properties(yaml-cpp PROPERTIES FOLDER Vendors/yaml-cpp)
endif()
