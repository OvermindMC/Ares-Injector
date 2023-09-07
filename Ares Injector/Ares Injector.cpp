#include "Utils/Utils.h"
#include "Window/Window.h"

int main(int argc, char* argv[]) {
    
    Utils::initCustomScheme();
    
    if (argc > 1) {
        Utils::handleCustomUri(argv[1]);
    };

    auto container = new ImGuiContainer();
    container->init();
    
    bool done = false;
    int count = 0;

    while (!done) {

        count++;

        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        };

        if (done)
            break;

        if (container->g_ResizeWidth != 0 && container->g_ResizeHeight != 0) {
            container->CleanupRenderTarget();
            container->g_pSwapChain->ResizeBuffers(0, container->g_ResizeWidth, container->g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            container->g_ResizeWidth = container->g_ResizeHeight = 0;
            container->CreateRenderTarget();
        };

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(600.f, 600.f));
        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
        if (ImGui::Begin("Hello, World!")) {

            ImGui::Text("Count: %d", count);

            ImGui::End();

        };

        ImGui::Render();
        container->g_pd3dDeviceContext->OMSetRenderTargets(1, &container->g_mainRenderTargetView, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        container->g_pSwapChain->Present(1, 0);

    };

    container->uninitialize();
    return 1;

};