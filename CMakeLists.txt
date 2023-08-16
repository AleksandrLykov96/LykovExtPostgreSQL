# CMakeList.txt: проект CMake для lykovAddInNative; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.1)
cmake_policy(SET CMP0091 NEW)

# Общие параметры
project (LykovExtPostgreSQL LANGUAGES C DESCRIPTION "Extension for PostgreSQL (Lykov's edition, only *nix)")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")

set(OUTPUT_DIRECTORY "/pgdisk/for_compile/libs")

# Если нужно pro версия, то папки указываем руками
set(ITS_PRO_VERSION true)
if (ITS_PRO_VERSION)
    set(POSTGRESQL_LIB_DIRECTORY "/opt/pgpro/1c-15/lib/")
    set(POSTGRESQL_INCLUDE_DIRECTORY "/opt/pgpro/1c-15/include/")
endif()

set(POSTGRESQL_VERSION "pgpro_15")

# Базовые параметры
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CRT_LINKAGE static)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(LYKOV_PRECOMPILE_FILES "${CMAKE_SOURCE_DIR}/include/stdafx.h")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${AddFlags}")
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} ${AddFlags}")
set(CMAKE_C_FLAGS_DEBUG_INIT"${CMAKE_C_FLAGS_DEBUG_INIT} ${AddFlags}")
set(CMAKE_C_FLAGS_RELEASE_INIT "${CMAKE_C_FLAGS_RELEASE_INIT} ${AddFlags}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} ${AddFlags}")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

# Определение библиотек
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# PG подтягиваем свою
set(PostgreSQL_LIBRARIES
        OpenSSL::SSL
)

if (ITS_PRO_VERSION)
    list(APPEND PostgreSQL_LIBRARIES
        "${POSTGRESQL_LIB_DIRECTORY}libpq.a"
        "${POSTGRESQL_LIB_DIRECTORY}libpgport.a"
        "${POSTGRESQL_LIB_DIRECTORY}libpgcommon.a"
    )
else()
    list(APPEND PostgreSQL_LIBRARIES PostgreSQL:PostgreSQL)
endif()

set(PostgreSQL_INCLUDE_DIRS
        
        "${POSTGRESQL_INCLUDE_DIRECTORY}"
        "${POSTGRESQL_INCLUDE_DIRECTORY}server")

find_package(Threads REQUIRED)

if (NOT ITS_PRO_VERSION)
    find_package(PostgreSQL REQUIRED)
endif()

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

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${AddFlags})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${Main_IncludeDir}
        ${OPENSSL_INCLUDE_DIR}
        ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL ${PostgreSQL_LIBRARIES})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${LYKOV_PRECOMPILE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${POSTGRESQL_VERSION}_${PROJECT_NAME}
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
