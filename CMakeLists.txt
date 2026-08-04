cmake_minimum_required(VERSION 4.3)
project(WWII)
set(CMAKE_C_COMPILER "clang")
add_executable(wwii)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(wwii PRIVATE m)
	target_link_libraries(wwii PRIVATE GL)
	target_link_libraries(wwii PRIVATE GLEW)
	target_link_libraries(wwii PRIVATE enet)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	find_package(glfw3 CONFIG REQUIRED)
	find_package(GLEW CONFIG REQUIRED)
	find_package(unofficial-enet CONFIG REQUIRED)
	target_link_libraries(wwii PRIVATE winmm)
	target_link_libraries(wwii PRIVATE Ws2_32)
	target_link_libraries(wwii PRIVATE GLEW::GLEW)
	target_link_libraries(wwii PRIVATE unofficial::enet::enet)
endif()
target_link_libraries(wwii PRIVATE glfw)
target_sources(wwii PRIVATE
    src/flag.c
    src/grphcs.c
    src/main.c
    src/menu.c
    src/net.c
    src/structs.c
    src/unit.c
    src/win.c
)
