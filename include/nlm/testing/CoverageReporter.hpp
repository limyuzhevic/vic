#pragma once

/**
 * Coverage analysis and reporting
 */
#include <string>
#include <vector>

namespace nlm {
namespace testing {

class CoverageReporter {
public:
    void collectCoverage();
    void generateReport();
    double getLineCoverage() const;
    double getBranchCoverage() const;
};

} // namespace testing
} // namespace nlm