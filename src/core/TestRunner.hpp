#include "TestSuite.hpp"
#include <iostream>
#include <vector>
#include <memory>

namespace nlm {

class TestRunner {
public:
    TestRunner() = default;
    ~TestRunner() = default;
    
    void addSuite(std::unique_ptr<TestSuite> suite) {
        suites_.push_back(std::move(suite));
    }
    
    bool runAll() {
        std::cout << "Running all test suites..." << std::endl;
        std::cout << "=" << std::string(30, '=') << std::endl;
        
        size_t totalPassed = 0;
        size_t totalFailed = 0;
        
        for (size_t i = 0; i < suites_.size(); ++i) {
            std::cout << "Suite " << (i + 1) << "/" << suites_.size() << ": "
                      << suites_[i]->getName() << std::endl;
            std::cout << std::string(30, '-') << std::endl;
            
            try {
                suites_[i]->run();
                totalPassed += suites_[i]->getPassedCount();
                totalFailed += suites_[i]->getFailedCount();
            } catch (const std::exception& e) {
                std::cout << "Suite failed with exception: " << e.what() << std::endl;
                totalFailed += suites_[i]->getFailedCount();
            }
            
            std::cout << std::endl;
        }
        
        std::cout << "Overall test summary:" << std::endl;
        std::cout << "  Passed: " << totalPassed << std::endl;
        std::cout << "  Failed: " << totalFailed << std::endl;
        std::cout << "  Total:  " << (totalPassed + totalFailed) << std::endl;
        
        return totalFailed == 0;
    }
    
private:
    std::vector<std::unique_ptr<TestSuite>> suites_;
};

} // namespace nlm