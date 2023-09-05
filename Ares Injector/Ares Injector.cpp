#include "Utils/Utils.h"

int main(int argc, char* argv[]) {
    
    Utils::initCustomScheme();
    
    if (argc > 1) {
        Utils::handleCustomUri(argv[1]);
    };

    std::cout << Utils::getAresPath();

    Sleep(15000);
};