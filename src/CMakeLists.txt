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
            "runtime/platform/Qt/QtWindow.cpp"
            "runtime/vk/core/VulkanInstance.cpp" 
            "runtime/vk/core/VulkanCore.cpp"
            "runtime/vk/VulkanBackend.cpp"
            "runtime/vk/present/VulkanSurface.cpp"
            "core/Log.cpp"
            "core/Result.cpp"
            "core/Time.cpp"
            "runtime/vk/core/VulkanQueue.cpp" 
            "runtime/vk/core/VulkanAdapter.cpp"
            "runtime/vk/core/VulkanDevice.cpp"
            "runtime/vk/present/VulkanSwapchain.cpp"
     "runtime/vk/present/PresentTarget.cpp" "runtime/vk/present/PresentationSystem.cpp")

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

if(WIN32)
    set(GAMEENGINE_THIRDPARTY_BIN_DIR "${PROJECT_SOURCE_DIR}/thirdparty/bin")
    file(GLOB GAMEENGINE_THIRDPARTY_DLLS CONFIGURE_DEPENDS
        "${GAMEENGINE_THIRDPARTY_BIN_DIR}/*.dll"
    )

    if(GAMEENGINE_THIRDPARTY_DLLS)
        add_custom_command(TARGET gameengine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${GAMEENGINE_THIRDPARTY_DLLS}
                $<TARGET_FILE_DIR:gameengine>
            COMMAND_EXPAND_LISTS
            COMMENT "Copying third-party DLLs"
        )
    endif()
endif()

if(WIN32 AND VK_ENABLE_QT)
    add_custom_command(TARGET gameengine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:gameengine>
            $<TARGET_FILE_DIR:gameengine>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying target runtime DLLs"
    )

    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt
        HINTS "${QT_ROOT}/bin"
        REQUIRED
    )

    add_custom_command(TARGET gameengine POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --no-compiler-runtime
            --dir "$<TARGET_FILE_DIR:gameengine>"
            "$<TARGET_FILE:gameengine>"
        COMMENT "Deploying Qt runtime plugins"
        VERBATIM
    )
endif()

if(MSVC)
    target_compile_options(gameengine
        PRIVATE
            /W4
            /permissive-
    )
endif()
