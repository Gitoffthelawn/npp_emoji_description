cmake_minimum_required(VERSION 3.15)
project(EmojiDescription VERSION 0.2.14 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исходники плагина
set(PLUGIN_SOURCES
    src/NppPluginDemo.cpp
    src/PluginDefinition.cpp
)

set(PLUGIN_HEADERS
    src/PluginDefinition.h
    src/PluginInterface.h
    src/Notepad_plus_msgs.h
    src/Scintilla.h
    src/Sci_Position.h
    src/menuCmdID.h
)

# Resource file (если используем)
set(PLUGIN_RESOURCES
    src/NppPluginDemo.rc
)

# Создаём DLL
add_library(${PROJECT_NAME} SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
    ${PLUGIN_RESOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/DockingFeature
)

# Устанавливаем имя выходного файла
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "EmojiDescription"
    SUFFIX ".dll"
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Линкуем необходимые библиотеки Windows
target_link_libraries(${PROJECT_NAME} PRIVATE
    shlwapi
    comctl32
)

# UNICODE определения
target_compile_definitions(${PROJECT_NAME} PRIVATE
    UNICODE
    _UNICODE
)

# Для Windows - указываем что это GUI приложение
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()