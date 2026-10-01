#include "framework/TestFramework.hpp"

int main(int argc, char** argv) {
    nlm::framework::TestFramework testFramework;
    
    // Load configuration
    testFramework.loadConfiguration("configs/default.cfg");
    testFramework.loadFromCommandLine(argc, argv);
    
    // Setup brain
    if (!testFramework.setupBrain()) {
        std::cerr << "Failed to setup brain!" << std::endl;
        return 1;
    }
    
    // Run all tests
    bool success = testFramework.runAllTests();
    
    // Report results
    testFramework.reportResults();
    
    return success ? 0 : 1;
}
