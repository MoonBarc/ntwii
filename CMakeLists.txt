cmake_minimum_required(VERSION 3.20)

project(ntwii CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(BUILD_SHARED_LIBS off)
set(WITH_JAVA off)
set(WITH_CSCORE off)
set(WITH_WPICAL off)
set(WITH_WPIMATH off)
set(WITH_WPILIB off)
set(WITH_TESTS off)
set(WITH_GUI off)
set(WITH_SIMULATION_MODULES off)
set(WITH_PROTOBUF off)

set(WITH_NTCORE on)
add_subdirectory(external/wpilib)
include_directories(
    external/wpilib/ntcore/src/main/native/include
    external/wpilib/ntcore/src/generated/main/native/include
)

# wiiuse
set(BUILD_EXAMPLE off)
set(BUILD_EXAMPLE_SDL off)
set(INSTALL_EXAMPLES off)
add_definitions(-DWIIUSE_STATIC)
add_subdirectory(external/wiiuse)
include_directories(external/wiiuse/src)

add_executable(ntwii)
add_subdirectory(src)

target_link_libraries(ntwii PRIVATE ntcore wiiuse)
