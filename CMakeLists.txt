cmake_minimum_required(VERSION 3.14)

project(TinyRender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES "Release")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -GL -fp:fast")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -ffast-math")
endif()

set(HEADERS
        include/api.h
        include/camera.h
        include/macros.h
        include/math.h
        include/model.h
        include/shader.h
        include/tgaimage.h
        include/win32.h
        )

set(SOURCES
        src/line.cpp
        src/camera.cpp
        src/math.cpp
        src/model.cpp
        src/shader.cpp
        src/tgaimage.cpp
        src/win32.cpp
        main.cpp
        )

add_executable(TinyRender  ${HEADERS} ${SOURCES})

if(MSVC)
    target_compile_options(TinyRender PRIVATE /fp:fast)
else()
    target_compile_options(TinyRender PRIVATE -ffast-math)
    target_link_libraries(TinyRender  m)
endif()

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT TinyRender)
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})