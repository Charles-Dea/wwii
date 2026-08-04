cmake_minimum_required(VERSION 4.3)
project(WWII)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
add_executable(wwii)
target_link_libraries(wwii PRIVATE glfw)
target_link_libraries(wwii PRIVATE GLEW::GLEW)
target_link_libraries(wwii PRIVATE unofficial::enet::enet)
target_link_libraries(wwii PRIVATE winmm)
target_link_libraries(wwii PRIVATE Ws2_32)
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