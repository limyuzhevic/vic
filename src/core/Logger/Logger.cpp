#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace nlm {

// Logger implementation
// All methods are defined inline in the header for now

// Global logger manager instance
LoggerManager& getGlobalLogger() {
    return LoggerManager::getInstance();
}

} // namespace nlm
