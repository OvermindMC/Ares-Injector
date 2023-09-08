#include "Utils/SocketClient.h"
#include "Window/Window.h"

int main(int argc, char* argv[]) {
    
    Utils::initCustomScheme();
    
    if (argc > 1) {
        Utils::handleCustomUri(argv[1]);
    };

    auto socket = new SocketClient("139.162.240.124", 8000);
    auto user = socket->getUser();

    if (user.size() <= 0) {

        std::cout << "Unable to login, check the website!";
        Sleep(5000);
        return 1;

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

            if (!Utils::isAresInjected()) {
                
                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Inject").x) * 0.5f);

                if (ImGui::Button("Inject")) {
                    Utils::injectLatest();
                };

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