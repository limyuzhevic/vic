// Test statistics implementation
// Provides timing, memory, and iteration statistics collection for tests

#include "TestStatistics.hpp"

namespace nlm {

// Initialize test statistics
TestStatistics& getTestStatistics() {
    return TestStatistics::getInstance();
}

// Print formatted test statistics
void printTestStatistics(const TestStatistics& stats) {
    stats.printStatistics();
}

// Reset all test statistics
void resetTestStatistics() {
    TestStatistics::getInstance().clear();
}

} // namespace nlm
