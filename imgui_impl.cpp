#define IMGUI_IMPLEMENTATION

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "3rd_party/imgui/misc/single_file/imgui_single_file.h"

#include "3rd_party/imgui/backends/imgui_impl_win32.cpp"
#include "3rd_party/imgui/backends/imgui_impl_dx11.cpp"