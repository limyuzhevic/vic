#include "src/core/Logger/Logger.hpp"
#include <iostream>

int main() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.info("Test message");
    logger.warning("Warning message");
    logger.error("Error message");
    std::cout << "Basic test passed!" << std::endl;
    return 0;
}
