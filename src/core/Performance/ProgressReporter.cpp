// Progress reporting implementation
#include "ProgressReporter.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {

struct ProgressReporter::Impl {
    size_t maxWidth;
};

ProgressReporter::ProgressReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->maxWidth = 50;
}

void ProgressReporter::update(const std::string& message, int current, int total) {
    // Simple progress display
    if (total > 0) {
        int progress = static_cast<int>(total * pImpl->maxWidth / 100);
        std::cout << "[";
        for (int i = 0; i < progress; ++i) std::cout << "=";
        for (int i = progress; i < static_cast<int>(pImpl->maxWidth); ++i) std::cout << " ";
        std::cout << "] " << current << " of " << total;
        if (!message.empty()) std::cout << " " << message;
        std::cout << "\r";
        std::cout.flush();
    }
}

void ProgressReporter::logTestResults(const std::vector<TestResult>& results) {
    std::cout << std::endl;
    std::cout << "Test Results Summary:" << std::endl;
    std::cout << std::setw(20) << std::left << "Test" << "Time (s)" << "Success" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (const auto& result : results) {
        std::cout << std::setw(20) << std::left << result.name;
        std::cout << std::setw(8) << result.executionTime << " " << (result.success ? "PASS" : "FAIL");
        std::cout << std::endl;
    }
}

} // namespace nlm