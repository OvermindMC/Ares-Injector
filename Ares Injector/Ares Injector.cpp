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

    while (!done) {

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

        ImGuiIO& io = ImGui::GetIO();
        auto windowSize = io.DisplaySize;

        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
        ImGui::SetNextWindowSize(windowSize);

        container->setAresStyles();

        if (ImGui::Begin("Home", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
            
            if (ImGui::BeginMenuBar()) {
                
                if (ImGui::BeginMenu("Main")) {
                    
                    ImGui::MenuItem("Exit", NULL, &done);
                    ImGui::EndMenu();

                };

                ImGui::EndMenuBar();

            };
            
            //
            
            ImGui::End();

        };

        ImGui::EndFrame();
        ImGui::Render();
        
        container->g_pd3dDeviceContext->OMSetRenderTargets(1, &container->g_mainRenderTargetView, nullptr);
        
        const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 1.f };
        container->g_pd3dDeviceContext->ClearRenderTargetView(container->g_mainRenderTargetView, clear_color_with_alpha);
        
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        container->g_pSwapChain->Present(1, 0);

    };

    container->uninitialize();
    delete container;
    return 1;

};