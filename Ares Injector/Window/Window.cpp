#include "Window.h"

auto ImGuiContainer::init(void) -> void {

    this->windowClass.cbSize = sizeof(WNDCLASSEX);
    this->windowClass.style = CS_HREDRAW | CS_VREDRAW;
    this->windowClass.lpfnWndProc = ImGuiContainer::CustomWndProc;
    this->windowClass.cbClsExtra = 0;
    this->windowClass.cbWndExtra = 0;
    this->windowClass.hInstance = GetModuleHandle(NULL);
    this->windowClass.hIcon = NULL;
    this->windowClass.hCursor = NULL;
    this->windowClass.hbrBackground = NULL;
    this->windowClass.lpszMenuName = NULL;
    this->windowClass.lpszClassName = L"SwapChain Window";
    this->windowClass.hIconSm = NULL;

    ::RegisterClassExW(&this->windowClass);
    this->window = ::CreateWindowW(this->windowClass.lpszClassName, L"Ares Injector", WS_BORDER, 100, 100, 1280, 800, nullptr, nullptr, this->windowClass.hInstance, nullptr);

    if (!CreateDeviceD3D(this->window)) {
        this->CleanupDeviceD3D();
        ::UnregisterClassW(this->windowClass.lpszClassName, this->windowClass.hInstance);
        return;
    };

    ::ShowWindow(this->window, SW_SHOWDEFAULT);
    ::UpdateWindow(this->window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, 24.f);

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(this->window);
    ImGui_ImplDX11_Init(this->g_pd3dDevice, this->g_pd3dDeviceContext);

    ::SetWindowLongPtr(this->window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    ::SetWindowLongPtr(this->window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&ImGuiContainer::CustomWndProc));

};

auto ImGuiContainer::uninitialize(void) -> void {

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    this->CleanupDeviceD3D();
    ::DestroyWindow(this->window);
    ::UnregisterClassW(this->windowClass.lpszClassName, this->windowClass.hInstance);

};

auto ImGuiContainer::CreateDeviceD3D(HWND hwnd) -> bool {

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &this->g_pSwapChain, &this->g_pd3dDevice, &featureLevel, &this->g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &this->g_pSwapChain, &this->g_pd3dDevice, &featureLevel, &this->g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    this->CreateRenderTarget();
    return true;

};

auto ImGuiContainer::CleanupDeviceD3D(void) -> void {

    this->CleanupRenderTarget();
    if (this->g_pSwapChain) { this->g_pSwapChain->Release(); this->g_pSwapChain = nullptr; }
    if (this->g_pd3dDeviceContext) { this->g_pd3dDeviceContext->Release(); this->g_pd3dDeviceContext = nullptr; }
    if (this->g_pd3dDevice) { this->g_pd3dDevice->Release(); this->g_pd3dDevice = nullptr; }

};

auto ImGuiContainer::CreateRenderTarget(void) -> void {

    ID3D11Texture2D* pBackBuffer;
    this->g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    this->g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &this->g_mainRenderTargetView);
    pBackBuffer->Release();

};

auto ImGuiContainer::CleanupRenderTarget(void) -> void {

    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }

};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ImGuiContainer::CustomWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiContainer* container = reinterpret_cast<ImGuiContainer*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (container != nullptr && container->g_pd3dDevice != nullptr)
    {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            container->g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
            container->g_ResizeHeight = (UINT)HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
};

auto ImGuiContainer::setAresStyles(void) -> void {

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
    colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Border
    colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
    colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

    // Text
    colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.f };

    // Popups
    colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
    colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

    // Seperator
    colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
    colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
    colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
    colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

    auto& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(5, 8);
    style.WindowRounding = 5.0f;
    style.FramePadding = ImVec2(5, 5);
    style.FrameRounding = 4.0f;
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 5.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 4;
    style.PopupRounding = 4;
    style.ChildRounding = 4;

};