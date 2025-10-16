cmake_minimum_required(VERSION 3.16...3.30)
project(LearnWebGPU VERSION 0.1.0 LANGUAGES CXX C)

include(FetchContent)

FetchContent_Declare(
    webgpu
    URL https://github.com/eliemichel/WebGPU-distribution/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(webgpu)

add_executable(App main.cpp)
target_link_libraries(App PRIVATE webgpu)
target_copy_webgpu_binaries(App)

set_target_properties(App PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)

if (MSVC)
    target_compile_options(App PRIVATE /W4)
else()
    target_compile_options(App PRIVATE -Wall -Wextra -pedantic)
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
