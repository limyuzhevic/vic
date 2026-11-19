#include "MathUtils.hpp"
#include <iostream>
#include <chrono>
#include <sstream>

namespace nlm {

void DebugUtils::assert(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "ASSERTION FAILED: " << message << std::endl;
        std::abort();
    }
}

void DebugUtils::logPerformance(const std::string& operation, double timeMs) {
    std::cout << "PERFORMANCE: " << operation << " took " << timeMs << " ms" << std::endl;
}

void DebugUtils::checkMemoryLeak(size_t expectedAllocation, size_t actualAllocation) {
    if (actualAllocation > expectedAllocation * 2) {
        std::cout << "WARNING: Possible memory leak detected!" << std::endl;
    }
}

bool DebugUtils::validateVector(const std::vector<float>& vec, size_t expectedSize) {
    return vec.size() == expectedSize;
}

std::string DebugUtils::generateDebugInfo(const std::string& prefix) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << prefix << " " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
