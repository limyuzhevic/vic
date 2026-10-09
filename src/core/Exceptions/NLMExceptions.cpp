#include "NLMExceptions.hpp"
#include <chrono>
#include <stacktrace>
#include <sstream>
#include <iomanip>

namespace nlm {

// Helper function to get current timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm