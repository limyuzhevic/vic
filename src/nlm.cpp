#include "CLI/NLMCLI.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        nlm::NLMApplication app("nlm");
        return app.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception caught\n";
        return 1;
    }
}
