cmake_minimum_required(VERSION 3.10)

add_definitions(-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS)

add_library(ImGUI-static STATIC imgui.cpp
                                imgui_demo.cpp
                                imgui_draw.cpp
                                imgui_tables.cpp
                                imgui_widgets.cpp
                                backends/imgui_impl_sdl.cpp
                                backends/imgui_impl_opengl3.cpp)
