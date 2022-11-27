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
    OPTION ( ASSIMP_WARNINGS_AS_ERRORS
        "Treat all warnings as errors."
        OFF
    )
    OPTION( ASSIMP_INSTALL
        "Disable this if you want to use assimp as a submodule."
        OFF
    )
    OPTION( ASSIMP_BUILD_ASSIMP_TOOLS
        "If the supplementary tools for Assimp are built in addition to the library."
        OFF
    )
    OPTION ( ASSIMP_BUILD_TESTS
        "If the test suite for Assimp is built in addition to the library."
        OFF
    )
    OPTION( ASSIMP_INJECT_DEBUG_POSTFIX
        "Inject debug postfix in .a/.so/.dll lib names"
        OFF
    )
    option(USE_STATIC_CRT "Link against the static runtime libraries." ON)
    add_subdirectory(Assimp)
    set_target_properties(assimp PROPERTIES FOLDER Vendors/Assimp)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER Vendors/Assimp)
    set_target_properties(zlibstatic PROPERTIES FOLDER Vendors/Assimp)
    target_compile_options(zlibstatic PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

endif()

if(NOT TARGET EnTT)
    add_subdirectory(entt)
endif()
