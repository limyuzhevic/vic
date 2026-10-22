#include "TestFramework.hpp"

namespace nlm {
namespace test {

// Global test runner instance
class TestRunnerImpl {
private:
    TestRunner runner;
    static TestRunnerImpl* instance;
    
public:
    TestRunnerImpl() : runner() {}
    
    TestRunner& getRunner() { return runner; }
    
    static TestRunnerImpl& getInstance() {
        if (!instance) {
            instance = new TestRunnerImpl();
        }
        return *instance;
    }
};

TestRunnerImpl* TestRunnerImpl::instance = nullptr;

TestRunner& getTestRunner() {
    return TestRunnerImpl::getInstance().getRunner();
}

} // namespace test
} // namespace nlm
