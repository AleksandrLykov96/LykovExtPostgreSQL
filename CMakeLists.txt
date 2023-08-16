# CMakeList.txt: проект CMake для lykovAddInNative; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.1)
cmake_policy(SET CMP0091 NEW)

# Общие параметры
project (LykovExtPostgreSQL LANGUAGES C DESCRIPTION "Extension for PostgreSQL (Lykov's edition, only *nix)")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")

set(OUTPUT_DIRECTORY "/pgdisk/for_compile/main/lib")
set(POSTGRESQL_LIB_DIRECTORY "/pgdisk/for_compile/main/lib/")
set(POSTGRESQL_INCLUDE_DIRECTORY "/pgdisk/for_compile/main/include/")

set(LYKOV_ADD_FLAGS "-lpthread -fno-exceptions -shared -Wall -fPIC -lpq -O3 ")

# Базовые параметры
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CRT_LINKAGE static)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(LYKOV_PRECOMPILE_FILES "${CMAKE_SOURCE_DIR}/include/stdafx.h")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LYKOV_ADD_FLAGS}")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

# Определение библиотек
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# PG подтягиваем свою
set(PostgreSQL_LIBRARIES

        "${POSTGRESQL_LIB_DIRECTORY}libpq.a"
        "${POSTGRESQL_LIB_DIRECTORY}libpgport.a"
        "${POSTGRESQL_LIB_DIRECTORY}libpgcommon.a"
        OpenSSL::SSL
)

set(PostgreSQL_INCLUDE_DIRS
        
        "${POSTGRESQL_INCLUDE_DIRECTORY}"
        "${POSTGRESQL_INCLUDE_DIRECTORY}server")

find_package(Threads REQUIRED)

# Подготовка параметров для компиляции
set(Main_IncludeDir "${CMAKE_SOURCE_DIR}/include")
set(Main_SRC
        ${LYKOV_PRECOMPILE_FILES}

        "include/globalFunctions.h"
        "include/lykovExtPostgreSQL.h"

        "src/globalFunctions.c"
        "src/lykovExtPostgreSQL.c"
)

add_library(${PROJECT_NAME} SHARED ${Main_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${LYKOV_ADD_FLAGS})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${Main_IncludeDir}
        ${OPENSSL_INCLUDE_DIR}
        ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL ${PostgreSQL_LIBRARIES})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${LYKOV_PRECOMPILE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        POSITION_INDEPENDENT_CODE ON)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${OUTPUT_DIRECTORY}/$<TARGET_FILE_NAME:${PROJECT_NAME}>
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
message("    OUTPUT_DIRECTORY: " ${OUTPUT_DIRECTORY})
message(-----------------------------------------------------------------------------)
