#pragma once

/**
 * Automated test data generation
 */
#include <string>
#include <vector>

namespace nlm {
namespace testing {

class TestDataGenerator {
public:
    void generateTestData();
    std::vector<std::string> getGeneratedData() const;
    void saveData(const std::string& filename) const;
};

} // namespace testing
} // namespace nlm