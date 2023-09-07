#include "Window.h"

auto ImGuiContainer::init(void) -> void {

    this->windowClass.cbSize = sizeof(WNDCLASSEX);
    this->windowClass.style = CS_HREDRAW | CS_VREDRAW;
    this->windowClass.lpfnWndProc = DefWindowProc;
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
    this->window = ::CreateWindowW(this->windowClass.lpszClassName, L"ImGui Window Title", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, this->windowClass.hInstance, nullptr);

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

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(this->window);
    ImGui_ImplDX11_Init(this->g_pd3dDevice, this->g_pd3dDeviceContext);

};

auto ImGuiContainer::uninitialize(void) -> void {

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
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
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }

};

auto ImGuiContainer::CreateRenderTarget(void) -> void {

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &this->g_mainRenderTargetView);
    pBackBuffer->Release();

};

auto ImGuiContainer::CleanupRenderTarget(void) -> void {

    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }

};