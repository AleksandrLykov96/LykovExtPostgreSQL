# CMakeList.txt: проект CMake для lykovAddInNative; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.20)
cmake_policy(SET CMP0091 NEW)

# Общие параметры
project (LykovExtPostgreSQL LANGUAGES C DESCRIPTION "Extension for PostgreSQL (Lykov's edition)")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")

set(output_directory ${CMAKE_SOURCE_DIR}/bin)

# Базовые параметры
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CRT_LINKAGE static)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_EXE_LINKER_FLAGS "-static")

if (WIN32)  
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--no-as-needed -MT -Wall -pedantic -O3 -static-libgcc -fPIC -lpthread ")
endif()

if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif()

# Определение библиотек
if(WIN32)
    set(PostgreSQL_ADDITIONAL_VERSIONS "14")
endif()

find_package(PostgreSQL REQUIRED)

if(WIN32)
    find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
endif()

# Подготовка параметров для компиляции
set(Main_IncludeDir "${CMAKE_SOURCE_DIR}/include")
set(Main_SRC
        "include/stdafx.h"
        "src/dllmain.c"
        
        "include/lykovExtPostgreSQL.h"
        "src/lykovExtPostgreSQL.c"
)

if(WIN32)
    list(APPEND Main_SRC lykovExtPostgreSQL.rc lykovExtPostgreSQL.def)
    add_definitions(-DUNICODE -DWIN32 -D_USRDLL -D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -DADDINCPP_EXPORTS)
    if(TARGET_PLATFORM_32)
        add_definitions(-D_USE_32BIT_TIME_T)
    endif()
endif()

add_library(${PROJECT_NAME} SHARED ${Main_SRC})

if (NOT WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-fPIC -Wall -Wunknown-pragmas -pedantic -O3 -ldl -lc -lm -Wl,--no-as-needed -lpthread -Xlinker --version-script ${CMAKE_CURRENT_SOURCE_DIR}/version.script ")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-lws2_32 ")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
        ${Main_IncludeDir}
        ${PostgreSQL_INCLUDE_DIRS}
        "D:/cpp/vcpkg/installed/x64-windows-static/include/server/port/win32"
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PostgreSQL_LIBRARIES})
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
endif()

target_precompile_headers(${PROJECT_NAME} PUBLIC "include/stdafx.h")
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        POSITION_INDEPENDENT_CODE ON)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${output_directory}/$<TARGET_FILE_NAME:${PROJECT_NAME}>
        MAIN_DEPENDENCY ${PROJECT_NAME}
)

#------------------- Окончание (вывод параметров) -------------------
message(-------------------------------LYKOV_Параметры-------------------------------)
message("    PROJECT_NAME: " ${PROJECT_NAME})
message("    CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
message("    CMAKE_TOOLCHAIN_FILE: " ${CMAKE_TOOLCHAIN_FILE})
message("    CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS})
message("    PostgreSQL_LIBRARIES: " ${PostgreSQL_LIBRARIES})
message("    PostgreSQL_INCLUDE_DIRS: " ${PostgreSQL_INCLUDE_DIRS})
message("    output_directory: " ${output_directory})
message(-----------------------------------------------------------------------------)
