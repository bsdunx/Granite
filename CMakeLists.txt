cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
project(Granite LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
    set(GRANITE_CXX_FLAGS -Wshadow -Wall -Wextra -Wno-comment -Wno-missing-field-initializers -Wno-empty-body -ffast-math)
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        set(GRANITE_CXX_FLAGS ${GRANITE_CXX_FLAGS} -Wno-backslash-newline-escape)
    endif()
    if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Release"))
        message("Disabling frame pointer for profiling/debug.")
        set(GRANITE_CXX_FLAGS ${GRANITE_CXX_FLAGS} -fno-omit-frame-pointer)
    endif()
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
        message("Enabling SSE3 support.")
        set(GRANITE_CXX_FLAGS ${GRANITE_CXX_FLAGS} -msse3)
    endif()
elseif (MSVC)
    set(GRANITE_CXX_FLAGS /D_CRT_SECURE_NO_WARNINGS /wd4267 /wd4244 /wd4309 /wd4005 /MP /DNOMINMAX)
endif()

# We are overriding settings in subdirectories.
# Avoid warnings.
if (${CMAKE_VERSION} VERSION_GREATER "3.13.0")
   message("Setting CMake policy CMP0077.")
   cmake_policy(SET CMP0077 NEW)
endif()

option(GRANITE_SHARED "Build Granite as a shared library." ON)
option(GRANITE_ISPC_TEXTURE_COMPRESSION "Enable ISPC texture compression" ON)
option(GRANITE_ASTC_ENCODER_COMPRESSION "Enable astc-encoder texture compression" ON)
option(GRANITE_TOOLS "Build Granite tools." ON)
option(GRANITE_KHR_DISPLAY_ACQUIRE_XLIB "Try to acquire Xlib display when using VK_KHR_display." OFF)
option(GRANITE_SHADER_COMPILER_OPTIMIZE "Optimize SPIR-V." ON)
option(GRANITE_VULKAN_ONLY "Only enable Vulkan backend in build." OFF)
option(GRANITE_VULKAN_SHADER_MANAGER_RUNTIME_COMPILER "Enable Vulkan GLSL runtime compiler." ON)
option(GRANITE_SPIRV_DUMP "Dump compiled SPIR-V modules to cache." OFF)
option(GRANITE_VULKAN_MT "Make Vulkan backend thread-safe." ON)
option(GRANITE_VULKAN_FOSSILIZE "Enable support for Fossilize." OFF)
option(GRANITE_AUDIO "Enable Audio support." ON)
option(GRANITE_PLATFORM "Granite Platform" "GLFW")
option(GRANITE_HIDDEN "Declare symbols as hidden by default. Useful if you build Granite as a static library and you link to it in your shared library." OFF)
option(GRANITE_SANITIZE_ADDRESS "Sanitize address" OFF)
option(GRANITE_SANITIZE_THREADS "Sanitize threads" OFF)
option(GRANITE_SANITIZE_MEMORY "Sanitize memory" OFF)
option(GRANITE_TARGET_NATIVE "Target native arch (-march=native)" ON)
option(GRANITE_BULLET "Enable Bullet support." OFF)
option(GRANITE_RENDERDOC_CAPTURE "Enable support for RenderDoc capture from API." ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (GRANITE_HIDDEN)
    if (CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    endif()
    endif()

if (GRANITE_TARGET_NATIVE)
    if (CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    endif()
endif()

set(GRANITE_LINK_FLAGS)
if (GRANITE_SANITIZE_ADDRESS)
    set(GRANITE_CXX_FLAGS ${GRANITE_CXX_FLAGS} -fsanitize=address)
    set(GRANITE_LINK_FLAGS "${GRANITE_LINK_FLAGS} -fsanitize=address")
endif()

if (GRANITE_SANITIZE_THREADS)
    set(GRANITE_CXX_FLAGS ${GRANITE_CXX_FLAGS} -fsanitize=thread)
    set(GRANITE_LINK_FLAGS "${GRANITE_LINK_FLAGS} -fsanitize=thread")
endif()

if (GRANITE_SANITIZE_MEMORY)
    set(GRANITE_CXX_FLAGS ${GRANITE_CXX_FLAGS} -fsanitize=memory)
    set(GRANITE_LINK_FLAGS "${GRANITE_LINK_FLAGS} -fsanitize=memory")
endif()

if (GRANITE_SHARED)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    if (MSVC)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
endif()

set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_INSTALL ON CACHE BOOL "" FORCE)
set(ENABLE_HLSL ON CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SHADERC_THIRD_PARTY_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party" CACHE STRING "Third party path." FORCE)
set(SPIRV_CHECK_CONTEXT OFF CACHE BOOL "Disable SPIR-V IR context checking." FORCE)
set(SPIRV-Headers_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/spirv-headers" CACHE STRING "SPIRV-Headers path")
set(SHADERC_SKIP_TESTS ON)
set(FOSSILIZE_RAPIDJSON_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include" CACHE STRING "Fossilize rapidjson path." FORCE)
set(FOSSILIZE_VULKAN_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/khronos/vulkan-headers/include/vulkan" CACHE STRING "Fossilize Vulkan path." FORCE)
set(MUFFT_ENABLE_FFTW OFF CACHE BOOL "Disable FFTW tests." FORCE)

add_subdirectory(third_party/spirv-cross EXCLUDE_FROM_ALL)
add_library(volk STATIC third_party/volk/volk.c third_party/volk/volk.h)
if (NOT WIN32)
    target_link_libraries(volk PRIVATE dl)
endif()
target_include_directories(volk PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/volk
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/khronos/vulkan-headers/include
)

if (NOT GRANITE_VULKAN_ONLY)
    add_subdirectory(third_party/shaderc EXCLUDE_FROM_ALL)
    add_subdirectory(third_party/stb EXCLUDE_FROM_ALL)
    add_subdirectory(third_party/astc-encoder/Source EXCLUDE_FROM_ALL)
    add_subdirectory(third_party/meshoptimizer EXCLUDE_FROM_ALL)
    add_subdirectory(third_party/mikktspace EXCLUDE_FROM_ALL)
    add_subdirectory(third_party/muFFT EXCLUDE_FROM_ALL)
    add_subdirectory(third_party/libco EXCLUDE_FROM_ALL)
    add_subdirectory(third_party EXCLUDE_FROM_ALL)
endif()

set(FOSSILIZE_CLI OFF CACHE BOOL "Fossilize CLI." FORCE)
set(FOSSILIZE_TESTS OFF CACHE BOOL "Fossilize tests." FORCE)
if (GRANITE_VULKAN_FOSSILIZE)
    add_subdirectory(third_party/fossilize EXCLUDE_FROM_ALL)
endif()

if (GRANITE_SHARED)
    add_library(granite SHARED)
    if (WIN32)
        install(TARGETS granite RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
    else()
        set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib:${CMAKE_INSTALL_PREFIX}/lib")
        install(TARGETS granite DESTINATION lib)
    endif()
else()
    add_library(granite STATIC)
    if (GRANITE_POSITION_INDEPENDENT OR GRANITE_SHARED)
        set_target_properties(${NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    else()
        set_target_properties(${NAME} PROPERTIES POSITION_INDEPENDENT_CODE OFF)
    endif()
endif()
target_compile_options(granite PRIVATE ${GRANITE_CXX_FLAGS})

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    target_link_libraries(granite PUBLIC atomic)
endif()

function(add_granite_executable NAME)
        add_executable(${NAME} ${ARGN})
    # Need this for some reason on OSX.
    if (APPLE)
        set_target_properties(${NAME} PROPERTIES LINK_FLAGS "-Wl,-all_load")
    endif()
    target_link_libraries(${NAME} PRIVATE granite)
    target_compile_options(${NAME} PRIVATE ${GRANITE_CXX_FLAGS})
    if (GRANITE_SANITIZE_ADDRESS OR GRANITE_SANITIZE_THREADS OR GRANITE_SANITIZE_MEMORY)
        set_target_properties(${NAME} PROPERTIES LINK_FLAGS "${GRANITE_LINK_FLAGS}")
    endif()
endfunction()

function(add_granite_application TARGET_NAME)
    add_granite_executable(${TARGET_NAME} ${ARGN})
        target_link_libraries(${TARGET_NAME} PRIVATE granite-application-entry)
        if (WIN32)
            if (CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
                set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS -mwindows)
            else()
                set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:Windows)
            endif()
        endif()
endfunction()

function(add_granite_headless_application TARGET_NAME)
    add_executable(${TARGET_NAME} ${ARGN})
    target_compile_options(${TARGET_NAME} PRIVATE ${GRANITE_CXX_FLAGS})
    if (GRANITE_SANITIZE_ADDRESS OR GRANITE_SANITIZE_THREADS OR GRANITE_SANITIZE_MEMORY)
        set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "${GRANITE_LINK_FLAGS}")
    endif()
    target_link_libraries(${TARGET_NAME} granite-application-entry-headless)
    if (WIN32)
        if (CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
            set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS -mwindows)
        else()
            set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:Windows)
        endif()
    endif()
endfunction()

function(add_granite_offline_tool NAME)
    add_executable(${NAME} ${ARGN})

    # Need this for some reason on OSX.
    if (APPLE)
        set_target_properties(${NAME} PROPERTIES LINK_FLAGS "-Wl,-all_load")
    endif()
    target_link_libraries(${NAME} PRIVATE granite)
    target_compile_options(${NAME} PRIVATE ${GRANITE_CXX_FLAGS})
    if (GRANITE_SANITIZE_ADDRESS OR GRANITE_SANITIZE_THREADS OR GRANITE_SANITIZE_MEMORY)
        set_target_properties(${NAME} PROPERTIES LINK_FLAGS "${GRANITE_LINK_FLAGS}")
    endif()

        if (WIN32)
            if (CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
                set_target_properties(${NAME} PROPERTIES LINK_FLAGS -mconsole)
            else()
                set_target_properties(${NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:Console)
            endif()
        endif()
endfunction()

set(GRANITE_ISPC_LIBRARY_DIR "" CACHE STRING "ISPC library directory." FORCE)
set(GRANITE_ISPC_INCLUDE_DIR "" CACHE STRING "ISPC include directory." FORCE)

target_include_directories(granite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/granite)

target_sources(granite PRIVATE
        granite/application/application.hpp
        granite/application/application_wsi.hpp

        granite/util/logging.hpp granite/util/logging.cpp
        granite/util/message_queue.hpp granite/util/message_queue.cpp
        granite/util/aligned_alloc.cpp granite/util/aligned_alloc.hpp
        granite/util/bitops.hpp
        granite/util/array_view.hpp
        granite/util/variant.hpp
        granite/util/enum_cast.hpp
        granite/util/hash.hpp
        granite/util/intrusive.hpp
        granite/util/intrusive_list.hpp
        granite/util/object_pool.hpp
        granite/util/stack_allocator.hpp
        granite/util/temporary_hashmap.hpp
        granite/util/volatile_source.hpp
        granite/util/read_write_lock.hpp
        granite/util/async_object_sink.hpp
        granite/util/unstable_remove_if.hpp
        granite/util/intrusive_hash_map.hpp
        granite/util/timer.hpp granite/util/timer.cpp
        granite/util/small_vector.hpp
        granite/util/timeline_trace_file.hpp granite/util/timeline_trace_file.cpp
        granite/util/thread_name.hpp granite/util/thread_name.cpp

        granite/vulkan/texture_format.cpp granite/vulkan/texture_format.hpp
        granite/vulkan/context.cpp granite/vulkan/context.hpp
        granite/vulkan/vulkan_headers.hpp granite/vulkan/vulkan_prerotate.hpp
        granite/vulkan/device.cpp granite/vulkan/device.hpp
        granite/vulkan/wsi.cpp granite/vulkan/wsi.hpp
        granite/vulkan/wsi_timing.cpp granite/vulkan/wsi_timing.hpp
        granite/vulkan/buffer_pool.cpp granite/vulkan/buffer_pool.hpp
        granite/vulkan/image.cpp granite/vulkan/image.hpp
        granite/vulkan/cookie.cpp granite/vulkan/cookie.hpp
        granite/vulkan/sampler.cpp granite/vulkan/sampler.hpp
        granite/vulkan/command_pool.cpp granite/vulkan/command_pool.hpp
        granite/vulkan/fence_manager.cpp granite/vulkan/fence_manager.hpp
        granite/vulkan/descriptor_set.cpp granite/vulkan/descriptor_set.hpp
        granite/vulkan/semaphore_manager.cpp granite/vulkan/semaphore_manager.hpp
        granite/vulkan/command_buffer.cpp granite/vulkan/command_buffer.hpp
        granite/vulkan/shader.cpp granite/vulkan/shader.hpp
        granite/vulkan/render_pass.cpp granite/vulkan/render_pass.hpp
        granite/vulkan/buffer.cpp granite/vulkan/buffer.hpp
        granite/vulkan/semaphore.cpp granite/vulkan/semaphore.hpp
        granite/vulkan/memory_allocator.cpp granite/vulkan/memory_allocator.hpp
        granite/vulkan/fence.hpp granite/vulkan/fence.cpp
        granite/vulkan/format.hpp
        granite/vulkan/limits.hpp
        granite/vulkan/type_to_string.hpp
        granite/vulkan/quirks.hpp
        granite/vulkan/vulkan_common.hpp
        granite/vulkan/event_manager.cpp granite/vulkan/event_manager.hpp
        granite/vulkan/pipeline_event.cpp granite/vulkan/pipeline_event.hpp
        granite/vulkan/query_pool.cpp granite/vulkan/query_pool.hpp)

if (GRANITE_RENDERDOC_CAPTURE)
    target_link_libraries(granite PRIVATE renderdoc-app)
    target_compile_definitions(granite PRIVATE GRANITE_RENDERDOC_CAPTURE)
    target_sources(granite PRIVATE granite/vulkan/renderdoc_capture.cpp)
    if (NOT WIN32)
        target_link_libraries(granite PRIVATE dl)
    endif()
endif()

if (GRANITE_VULKAN_MT)
    target_compile_definitions(granite PUBLIC GRANITE_VULKAN_MT)
    target_sources(granite PRIVATE granite/vulkan/thread_id.hpp granite/vulkan/thread_id.cpp)
endif()

if (WIN32)
    target_compile_definitions(volk PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()

if (NOT GRANITE_VULKAN_ONLY)
    add_library(granite-application-entry STATIC granite/application/application_entry.cpp)
    target_link_libraries(granite-application-entry granite)
    target_compile_options(granite-application-entry PRIVATE ${GRANITE_CXX_FLAGS})

    add_library(granite-application-entry-headless STATIC granite/application/application_entry.cpp)
    target_link_libraries(granite-application-entry-headless granite)
    target_compile_definitions(granite-application-entry-headless PRIVATE APPLICATION_ENTRY_HEADLESS=1)

    target_compile_definitions(granite PRIVATE GRANITE_DEFAULT_BUILTIN_DIRECTORY=\"${CMAKE_CURRENT_SOURCE_DIR}/assets\")
    target_compile_definitions(granite PRIVATE GRANITE_DEFAULT_CACHE_DIRECTORY=\"${CMAKE_BINARY_DIR}/cache\")
    target_compile_definitions(granite PUBLIC NOMINMAX GRANITE_LOGGING_QUEUE)

    target_sources(granite PRIVATE
            granite/application/global_managers.hpp granite/application/global_managers.cpp
            granite/application/application.cpp
            granite/application/application_wsi.cpp granite/application/application_wsi_events.hpp
            granite/application/platforms/application_headless.cpp granite/application/platforms/hw_counters/hw_counter_interface.h
            granite/application/input/input.hpp granite/application/input/input.cpp
            granite/application/events/application_events.hpp
            granite/application/scene_viewer_application.cpp granite/application/scene_viewer_application.hpp
            granite/application/application_cli_wrapper.cpp granite/application/application_cli_wrapper.hpp

            granite/event/event.cpp granite/event/event.hpp
            granite/ecs/ecs.cpp granite/ecs/ecs.hpp

            granite/filesystem/filesystem.cpp granite/filesystem/filesystem.hpp
            granite/filesystem/path.cpp granite/filesystem/path.hpp
            granite/filesystem/netfs/fs-netfs.cpp granite/filesystem/netfs/fs-netfs.hpp

            granite/network/looper.cpp granite/network/netfs.hpp granite/network/network.hpp
            granite/network/socket.cpp granite/network/tcp_listener.cpp

            granite/math/math.hpp granite/math/math.cpp
            granite/math/frustum.hpp granite/math/frustum.cpp
            granite/math/aabb.cpp granite/math/aabb.hpp
            granite/math/interpolation.cpp granite/math/interpolation.hpp
            granite/math/muglm/muglm.cpp granite/math/muglm/muglm.hpp
            granite/math/muglm/muglm_impl.hpp granite/math/muglm/matrix_helper.hpp
            granite/math/transforms.cpp granite/math/transforms.hpp
            granite/math/simd.hpp granite/math/simd_headers.hpp

            granite/renderer/render_parameters.hpp
            granite/renderer/render_queue.hpp granite/renderer/render_queue.cpp
            granite/renderer/simple_renderer.hpp granite/renderer/simple_renderer.cpp
            granite/renderer/mesh.hpp granite/renderer/mesh.cpp
            granite/renderer/scene.hpp granite/renderer/scene.cpp
            granite/renderer/scene_renderer.hpp granite/renderer/scene_renderer.cpp
            granite/renderer/shader_suite.hpp granite/renderer/shader_suite.cpp
            granite/renderer/render_context.hpp granite/renderer/render_context.cpp
            granite/renderer/camera.hpp granite/renderer/camera.cpp
            granite/renderer/material.hpp granite/renderer/material.cpp
            granite/renderer/abstract_renderable.hpp
            granite/renderer/render_components.hpp
            granite/renderer/mesh_util.hpp granite/renderer/mesh_util.cpp
            granite/renderer/material_util.hpp granite/renderer/material_util.cpp
            granite/renderer/renderer.hpp granite/renderer/renderer.cpp
            granite/renderer/renderer_enums.hpp
            granite/renderer/material_manager.hpp granite/renderer/material_manager.cpp
            granite/renderer/animation_system.hpp granite/renderer/animation_system.cpp
            granite/renderer/render_graph.cpp granite/renderer/render_graph.hpp
            granite/renderer/ground.hpp granite/renderer/ground.cpp
            granite/renderer/post/hdr.hpp granite/renderer/post/hdr.cpp
            granite/renderer/post/fxaa.hpp granite/renderer/post/fxaa.cpp
            granite/renderer/post/smaa.hpp granite/renderer/post/smaa.cpp
            granite/renderer/post/temporal.hpp granite/renderer/post/temporal.cpp
            granite/renderer/post/aa.hpp granite/renderer/post/aa.cpp
            granite/renderer/post/ssao.hpp granite/renderer/post/ssao.cpp
            granite/renderer/utils/image_utils.hpp granite/renderer/utils/image_utils.cpp
            granite/renderer/lights/lights.cpp granite/renderer/lights/lights.hpp
            granite/renderer/lights/clusterer.cpp granite/renderer/lights/clusterer.hpp
            granite/renderer/lights/volumetric_fog.cpp granite/renderer/lights/volumetric_fog.hpp
            granite/renderer/lights/light_info.hpp
            granite/renderer/lights/deferred_lights.hpp granite/renderer/lights/deferred_lights.cpp
            granite/renderer/scene_loader.cpp granite/renderer/scene_loader.hpp
            granite/renderer/mesh_manager.cpp granite/renderer/mesh_manager.hpp
            granite/renderer/common_renderer_data.cpp granite/renderer/common_renderer_data.hpp
            granite/renderer/cpu_rasterizer.cpp granite/renderer/cpu_rasterizer.hpp
            granite/renderer/threaded_scene.cpp granite/renderer/threaded_scene.hpp

            granite/scene_formats/texture_compression.hpp granite/scene_formats/texture_compression.cpp
            granite/scene_formats/gltf.cpp granite/scene_formats/gltf.hpp
            granite/scene_formats/obj.cpp granite/scene_formats/obj.hpp
            granite/scene_formats/scene_formats.hpp granite/scene_formats/scene_formats.cpp
            granite/scene_formats/light_export.cpp granite/scene_formats/light_export.hpp
            granite/scene_formats/camera_export.cpp granite/scene_formats/camera_export.hpp
            granite/scene_formats/memory_mapped_texture.cpp granite/scene_formats/memory_mapped_texture.hpp
            granite/scene_formats/texture_utils.cpp granite/scene_formats/texture_utils.hpp
            granite/scene_formats/texture_files.cpp granite/scene_formats/texture_files.hpp
            granite/scene_formats/gltf_export.cpp granite/scene_formats/gltf_export.hpp
            granite/scene_formats/rgtc_compressor.cpp granite/scene_formats/rgtc_compressor.hpp
            granite/scene_formats/tmx_parser.cpp granite/scene_formats/tmx_parser.hpp
            granite/scene_formats/texture_decoder.cpp granite/scene_formats/texture_decoder.hpp

            granite/threading/thread_group.cpp granite/threading/thread_group.hpp
            granite/threading/task_composer.cpp granite/threading/task_composer.hpp

            granite/ui/font.hpp granite/ui/font.cpp
            granite/ui/flat_renderer.hpp granite/ui/flat_renderer.cpp
            granite/ui/sprite.cpp granite/ui/sprite.hpp
            granite/ui/widget.hpp granite/ui/widget.cpp
            granite/ui/window.hpp granite/ui/window.cpp
            granite/ui/vertical_packing.cpp granite/ui/vertical_packing.hpp
            granite/ui/horizontal_packing.cpp granite/ui/horizontal_packing.hpp
            granite/ui/image_widget.cpp granite/ui/image_widget.hpp
            granite/ui/label.cpp granite/ui/label.hpp
            granite/ui/slider.cpp granite/ui/slider.hpp
            granite/ui/click_button.cpp granite/ui/click_button.hpp
            granite/ui/toggle_button.cpp granite/ui/toggle_button.hpp
            granite/ui/ui_manager.hpp granite/ui/ui_manager.cpp

            granite/util/cli_parser.cpp granite/util/cli_parser.hpp
            granite/util/dynamic_library.cpp granite/util/dynamic_library.hpp
            granite/util/string_helpers.cpp granite/util/string_helpers.hpp

            granite/util/bitmap_to_mesh.cpp granite/util/bitmap_to_mesh.hpp
            granite/util/cooperative_task.hpp granite/util/cooperative_task.cpp
            granite/util/generational_handle.hpp
            granite/util/lru_cache.hpp
            granite/util/no_init_pod.hpp
    )

    if (${CMAKE_SYSTEM} MATCHES "Linux")
        include(FindPkgConfig)
        pkg_check_modules(HOTPLUG_UDEV libudev)
        if (HOTPLUG_UDEV_FOUND)
            target_sources(granite PRIVATE granite/application/input/input_linux.cpp granite/application/input/input_linux.hpp)
            target_include_directories(granite PUBLIC ${HOTPLUG_UDEV_INCLUDE_DIRS})
            target_link_libraries(granite PRIVATE ${HOTPLUG_UDEV_LIBRARIES})
            target_compile_options(granite PRIVATE ${HOTPLUG_UDEV_CFLAGS_OTHER})
        endif()
    elseif (WIN32)
        target_sources(granite PRIVATE granite/application/input/xinput_windows.cpp granite/application/input/xinput_windows.hpp)
    endif()

    target_sources(granite PRIVATE
            granite/vulkan/managers/shader_manager.cpp
            granite/vulkan/managers/shader_manager.hpp
            granite/vulkan/managers/texture_manager.cpp
            granite/vulkan/managers/texture_manager.hpp)

    target_compile_definitions(granite PUBLIC GRANITE_VULKAN_FILESYSTEM)
    if (GRANITE_VULKAN_SHADER_MANAGER_RUNTIME_COMPILER)
        target_compile_definitions(granite PUBLIC GRANITE_VULKAN_SHADER_MANAGER_RUNTIME_COMPILER=1)
        target_sources(granite PRIVATE
            granite/compiler/compiler.cpp granite/compiler/compiler.hpp

            granite/renderer/ocean.hpp granite/renderer/ocean.cpp
            granite/renderer/fft/glfft.cpp
            granite/renderer/fft/glfft.hpp
            granite/renderer/fft/glfft_common.hpp
            granite/renderer/fft/glfft_interface.hpp
            granite/renderer/fft/glfft_granite_interface.hpp
            granite/renderer/fft/glfft_granite_interface.cpp
            granite/renderer/fft/glfft_wisdom.cpp
            granite/renderer/fft/glfft_wisdom.hpp)
        target_link_libraries(granite PRIVATE shaderc SPIRV-Tools)
    endif()

    if (HOTPLUG_UDEV_FOUND)
		target_compile_definitions(granite PRIVATE HAVE_LINUX_INPUT)
	elseif (WIN32)
		target_compile_definitions(granite PRIVATE HAVE_XINPUT_WINDOWS)
	endif()

    if (WIN32)
        target_sources(granite PRIVATE granite/filesystem/windows/os_filesystem.cpp granite/filesystem/windows/os_filesystem.hpp)
        target_link_libraries(granite PRIVATE ws2_32)
        target_include_directories(granite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/granite/filesystem/windows)
    else()
        target_sources(granite PRIVATE granite/filesystem/linux/os_filesystem.cpp granite/filesystem/linux/os_filesystem.hpp)
        target_include_directories(granite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/granite/filesystem/linux)
    endif()

    if (GRANITE_AUDIO)
        include(FindPkgConfig)

        target_sources(granite PRIVATE
                granite/audio/audio_interface.cpp granite/audio/audio_interface.hpp
                granite/audio/audio_mixer.cpp granite/audio/audio_mixer.hpp
                granite/audio/audio_resampler.cpp granite/audio/audio_resampler.hpp
                granite/audio/dsp/sinc_resampler.cpp granite/audio/dsp/sinc_resampler.hpp
                granite/audio/dsp/dsp.hpp granite/audio/dsp/dsp.cpp
                granite/audio/dsp/tone_filter.hpp granite/audio/dsp/tone_filter.cpp
                granite/audio/dsp/tone_filter_stream.hpp granite/audio/dsp/tone_filter_stream.cpp
                granite/audio/audio_events.hpp
                granite/audio/dsp/audio_fft_eq.cpp granite/audio/dsp/audio_fft_eq.hpp
                granite/audio/dsp/pole_zero_filter_design.cpp granite/audio/dsp/pole_zero_filter_design.hpp
                granite/audio/vorbis_stream.hpp granite/audio/vorbis_stream.cpp)

        target_include_directories(granite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/granite)
        target_link_libraries(granite PRIVATE stb-vorbis muFFT)
        target_compile_definitions(granite PUBLIC HAVE_GRANITE_AUDIO=1)

        if (NOT WIN32)
            pkg_check_modules(PULSEAUDIO libpulse)
            if (PULSEAUDIO_FOUND)
                target_sources(granite PRIVATE granite/audio/audio_pulse.cpp granite/audio/audio_pulse.hpp)
                target_link_libraries(granite PRIVATE ${PULSEAUDIO_LIBRARIES})
                target_include_directories(granite PRIVATE ${PULSEAUDIO_INCLUDEDIR})
                target_compile_definitions(granite PRIVATE AUDIO_HAVE_PULSE=1)
            endif()
        elseif (WIN32)
            target_sources(granite PRIVATE audio/audio_wasapi.cpp audio/audio_wasapi.hpp)
            target_compile_definitions(granite PRIVATE AUDIO_HAVE_WASAPI=1)
        endif()
    endif()

    if (GRANITE_BULLET)
        set(USE_GLUT OFF CACHE BOOL "" FORCE)
        set(BUILD_EGL OFF CACHE BOOL "" FORCE)
        set(BUILD_BULLET3 OFF CACHE BOOL "" FORCE)
        set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
        set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "" FORCE)
        set(USE_DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
        set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
        set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
        set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
        set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
        set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
        set(BUILD_ENET OFF CACHE BOOL "" FORCE)
        set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
        set(INSTALL_LIBS ON CACHE BOOL "" FORCE)
        option(GRANITE_BULLET_ROOT "" "Path to a Bullet library checkout.")
        if (NOT GRANITE_BULLET_ROOT)
            set(GRANITE_BULLET_ROOT $ENV{BULLET_ROOT})
            message("GRANITE_BULLET_ROOT not set, overriding with environment variable BULLET_ROOT.")
        endif()
        message("Including bullet3 from GRANITE_BULLET_ROOT: ${GRANITE_BULLET_ROOT}")
        add_subdirectory(${GRANITE_BULLET_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/third_party/bullet3 EXCLUDE_FROM_ALL)
        target_sources(granite PRIVATE granite/physics/physics_system.cpp granite/physics/physics_system.hpp)
        target_include_directories(granite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/granite/physics PRIVATE ${GRANITE_BULLET_ROOT}/src)
        target_compile_definitions(granite PUBLIC HAVE_GRANITE_PHYSICS=1)
        target_link_libraries(granite PRIVATE BulletDynamics BulletCollision LinearMath)
    endif()

    if (GRANITE_ISPC_TEXTURE_COMPRESSION)
        find_library(ISPC_LIBRARY NAMES ispc_texcomp HINTS ${GRANITE_ISPC_LIBRARY_DIR})
        find_path(ISPC_INCLUDE_DIR NAMES ispc_texcomp.h HINTS ${GRANITE_ISPC_INCLUDE_DIR})
        if (ISPC_LIBRARY AND ISPC_INCLUDE_DIR)
            message("Enabling ISPC texture compression.")
            target_link_libraries(granite PRIVATE ${ISPC_LIBRARY})
            target_compile_definitions(granite PRIVATE HAVE_ISPC)
            target_include_directories(granite PUBLIC ${ISPC_INCLUDE_DIR})
        else()
            message("Could not find ISPC texture compression.")
        endif()
    endif()

    if (GRANITE_ASTC_ENCODER_COMPRESSION)
        target_link_libraries(granite PRIVATE astc-encoder)
        target_compile_definitions(granite PRIVATE HAVE_ASTC_ENCODER)
    endif()

    if (GRANITE_VULKAN_FOSSILIZE)
        target_compile_definitions(granite PUBLIC GRANITE_VULKAN_FOSSILIZE)
        target_sources(granite PRIVATE vulkan/device_fossilize.cpp)
        target_link_libraries(granite PUBLIC fossilize)
    endif()

    if (GRANITE_SHADER_COMPILER_OPTIMIZE)
        target_compile_definitions(granite PRIVATE GRANITE_COMPILER_OPTIMIZE=1)
    else()
        target_compile_definitions(granite PRIVATE GRANITE_COMPILER_OPTIMIZE=0)
    endif()

    if (NOT GRANITE_PLATFORM)
       set(GRANITE_PLATFORM "GLFW")
    endif()

    if (${GRANITE_PLATFORM} MATCHES "GLFW")
        set(GLFW_BUILD_TESTS OFF)
        add_subdirectory(third_party/glfw EXCLUDE_FROM_ALL)
        target_sources(granite PRIVATE
                granite/application/platforms/application_glfw.cpp)
        target_link_libraries(granite PRIVATE glfw)
    elseif (${GRANITE_PLATFORM} MATCHES "KHR_DISPLAY")
        target_sources(granite PRIVATE
                granite/application/platforms/application_khr_display.cpp)
        if (GRANITE_KHR_DISPLAY_ACQUIRE_XLIB)
            target_link_libraries(granite PRIVATE X11)
            target_compile_definitions(granite PRIVATE "KHR_DISPLAY_ACQUIRE_XLIB")
            target_compile_definitions(volk PUBLIC VK_USE_PLATFORM_XLIB_XRANDR_EXT=1)
        endif()
    else()
        message(FATAL "GRANITE_PLATFORM is not set.")
    endif()
endif()

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_definitions(granite PUBLIC VULKAN_DEBUG)
endif()

if (CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
    set_source_files_properties(granite/vulkan/managers/texture_manager.cpp PROPERTIES COMPILE_FLAGS "-Wno-type-limits -Wno-ignored-qualifiers -Wno-unused-parameter")
endif()

if (GRANITE_SPIRV_DUMP)
    target_compile_definitions(granite PRIVATE GRANITE_SPIRV_DUMP)
endif()

target_link_libraries(granite PUBLIC volk PRIVATE spirv-cross-core)

if (NOT GRANITE_VULKAN_ONLY)
    target_link_libraries(granite
            PUBLIC
                rapidjson
            PRIVATE
                stb
                mikktspace
                meshoptimizer
                libco)
    target_include_directories(granite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb/stb)
endif()

if (NOT WIN32)
    target_link_libraries(granite PRIVATE -pthread dl m)
else()
    target_link_libraries(granite PUBLIC ws2_32)
endif()

if (NOT GRANITE_VULKAN_ONLY)
    if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
        set(GRANITE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        set(GRANITE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
    endif()
    function(add_blob_archive_target TARGET_NAME BLOB_NAME)
        find_package(Python3 REQUIRED)
        set_source_files_properties(
                ${CMAKE_BINARY_DIR}/${BLOB_NAME}.h
                ${CMAKE_BINARY_DIR}/${BLOB_NAME}.c
                PROPERTIES GENERATED ON)
        add_library(${TARGET_NAME} STATIC
                ${CMAKE_BINARY_DIR}/${BLOB_NAME}.h
                ${CMAKE_BINARY_DIR}/${BLOB_NAME}.c)
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_BINARY_DIR})
        # ... CMake, y u gotta be like this.
        set(_arg_list)
        set(_arg_counter 0)
        foreach(_arg IN LISTS ARGN)
            math(EXPR _arg_counter_mod "${_arg_counter} % 2")
            if (NOT ${_arg_counter_mod})
                list(APPEND _arg_list --input)
            endif()
            list(APPEND _arg_list ${_arg})
            math(EXPR _arg_counter "${_arg_counter} + 1")
        endforeach()
        add_custom_command(OUTPUT
            ${CMAKE_BINARY_DIR}/${BLOB_NAME}.h
            ${CMAKE_BINARY_DIR}/${BLOB_NAME}.c
            COMMAND ${Python3_EXECUTABLE}
                ${GRANITE_SOURCE_DIR}/tools/blobify.py
                --output ${CMAKE_BINARY_DIR}/${BLOB_NAME}.blob
                ${_arg_list}
            COMMAND ${Python3_EXECUTABLE}
                ${GRANITE_SOURCE_DIR}/tools/bin_to_text.py
                --output ${CMAKE_BINARY_DIR}/${BLOB_NAME}
                --blob-name ${BLOB_NAME}
                ${CMAKE_BINARY_DIR}/${BLOB_NAME}.blob
            BYPRODUCTS
                ${CMAKE_BINARY_DIR}/${BLOB_NAME}.blob)
    endfunction()

    add_subdirectory(tests)
    add_subdirectory(viewer)

    if (GRANITE_TOOLS)
        add_subdirectory(tools)
        add_granite_executable(netfs-server granite/network/netfs_server.cpp)
        add_subdirectory(granite/renderer/fft/test)
    endif()
endif()
