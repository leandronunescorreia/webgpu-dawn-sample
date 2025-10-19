cmake_minimum_required(VERSION 3.16...3.30)
project(LearnWebGPU VERSION 0.1.0 LANGUAGES CXX C)

include(FetchContent)

FetchContent_Declare(
    webgpu
    URL https://github.com/eliemichel/WebGPU-distribution/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(webgpu)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add compute_engine source files
set(COMPUTE_ENGINE_SRC
    src/computeengine/compute_engine.cpp
)

add_executable(App main.cpp ${COMPUTE_ENGINE_SRC})
target_link_libraries(App PRIVATE webgpu)
target_copy_webgpu_binaries(App)

set_target_properties(App PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)

# Set default C++ standard for the project to C++20
set(CMAKE_CXX_STANDARD 20 CACHE STRING "Default C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    target_compile_options(App PRIVATE /W4)
else()
    # Common warning flags
    target_compile_options(App PRIVATE -Wall -Wextra -pedantic)
    # Treat warnings as errors generally, but allow unused-variable warnings
    # to remain non-fatal so small unused locals (often in examples) don't
    # break builds. This disables only the error behavior for unused-variable.
    target_compile_options(App PRIVATE -Wno-error=unused-variable)
endif()

if (XCODE)
    set_target_properties(App PROPERTIES
        XCODE_GENERATE_SCHEME ON
        XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
    )
endif()

set_directory_properties(PROPERTIES
    VS_STARTUP_PROJECT App
)

if (EMSCRIPTEN)
    set_target_properties(App PROPERTIES SUFFIX ".html")
endif()
