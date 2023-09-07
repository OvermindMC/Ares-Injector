#include "Utils/Utils.h"
#include "Window/Window.h"

int main(int argc, char* argv[]) {
    
    Utils::initCustomScheme();
    
    if (argc > 1) {
        Utils::handleCustomUri(argv[1]);
    };

    auto container = new ImGuiContainer();
    container->init();

    while (true) {

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello, World!");
        ImGui::End();

        ImGui::Render();
        container->g_pd3dDeviceContext->OMSetRenderTargets(1, &container->g_mainRenderTargetView, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        container->g_pSwapChain->Present(1, 0);

    };

    container->uninitialize();
    return 1;

};