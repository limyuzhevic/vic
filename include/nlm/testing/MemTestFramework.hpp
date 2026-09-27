#pragma once

/**
 * Memory leak detection framework
 */
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {
namespace testing {

struct MemoryLeakResult {
    void* address;
    size_t size;
    std::string type;
    std::string site;
};

class MemTestFramework {
public:
    void registerTest(std::function<void()> test);
    void runTests();
    std::vector<MemoryLeakResult> getLeaks() const;
};

} // namespace testing
} // namespace nlm