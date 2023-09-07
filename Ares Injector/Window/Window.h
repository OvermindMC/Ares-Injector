#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d2d1.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#include "../ImGui/imgui.h"
#include "../ImGui/DroidSans.hpp"
#include "../ImGui/backends/imgui_impl_dx11.h"
#include "../ImGui/backends/imgui_impl_win32.h"

class ImGuiContainer {
public:
	ID3D11Device* g_pd3dDevice = nullptr;
	IDXGISwapChain* g_pSwapChain = nullptr;
	UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
	ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
	ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
public:
	HWND window;
	WNDCLASSEX windowClass;
public:
	auto init(void) -> void;
	auto uninitialize(void) -> void;
public:
	auto CreateDeviceD3D(HWND) -> bool;
	auto CleanupDeviceD3D(void) -> void;
	auto CreateRenderTarget(void) -> void;
	auto CleanupRenderTarget(void) -> void;
public:
	auto handleResizeBuffers(void) -> void;
	auto finalizeFrame(void) -> void;
public:
	auto setAresStyles(void) -> void;
public:
	static LRESULT CALLBACK CustomWndProc(HWND, UINT, WPARAM, LPARAM);
};