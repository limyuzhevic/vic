#include "main_cli.hpp"

int main(int argc, char** argv) {
    try {
        NLMCLI cli;
        return cli.run(argc, argv) ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
