#include "Utils/Utils.h"
#include "Window/Window.h"

int main(int argc, char* argv[]) {
    
    Utils::initCustomScheme();
    
    if (argc > 1) {
        Utils::handleCustomUri(argv[1]);
    };

    auto token = Utils::getToken();
    
    if (token.length() <= 0)
        return 1;

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

        container->handleResizeBuffers();

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
            
            ImGui::End();

        };

        ImGui::EndFrame();
        ImGui::Render();
        
        container->finalizeFrame();

    };

    container->uninitialize();
    delete container;
    return 1;

};