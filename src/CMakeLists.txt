add_executable(gameengine
    main.cpp
)

target_compile_features(gameengine
    PRIVATE
        cxx_std_20
)

target_include_directories(gameengine
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/include"
)

target_link_libraries(gameengine
    PRIVATE
        Vulkan::Vulkan
)

if(WIN32)
    target_compile_definitions(gameengine
        PRIVATE
            VK_USE_PLATFORM_WIN32_KHR
            NOMINMAX
    )
endif()

if(VK_ENABLE_QT)
    target_sources(gameengine
        PRIVATE
            runtime/platform/Qt/SurfaceQt.cpp
    )

    target_compile_definitions(gameengine
        PRIVATE
            VK_ENABLE_QT
    )

    target_link_libraries(gameengine
        PRIVATE
            Qt6::Core
            Qt6::Gui
            Qt6::Widgets
    )
endif()

if(MSVC)
    target_compile_options(gameengine
        PRIVATE
            /W4
            /permissive-
    )
endif()
