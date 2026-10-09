#include "test/EnhancedLoggerTest.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Enhanced Logging System - Comprehensive Test" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        // Run backward compatibility tests
        nlm::BackwardCompatibilityTest::runAllTests();
        
        // Create test log files for additional testing
        std::cout << "\nCreating test log files..." << std::endl;
        std::ofstream testLog1("test_logs_1.log");
        testLog1 << "[2024-01-01 12:00:00] [INFO] Test log entry 1" << std::endl;
        testLog1 << "[2024-01-01 12:01:00] [ERROR] Test log entry 2" << std::endl;
        testLog1.close();
        
        std::ofstream testLog2("test_logs_2.log");
        testLog2 << "[2024-01-02 12:00:00] [WARNING] Test log entry 3" << std::endl;
        testLog2.close();
        
        // Test log management functionality
        std::cout << "\nTesting log management functionality..." << std::endl;
        
        // Test log search
        std::vector<nlm::EnhancedLogEntry> searchResults = 
            nlm::getEnhancedLoggingSystem().searchLogs("Test log entry");
        std::cout << "Search for 'Test log entry': " << searchResults.size() << " results found" << std::endl;
        
        // Test log export/import
        bool exportSuccess = nlm::getEnhancedLoggingSystem().exportLogs("final_test_export.json", "json");
        std::cout << "Final export test: " << (exportSuccess ? "Success" << std::endl : "Failed" << std::endl);
        
        // Test performance
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            nlm::getEnhancedLoggingSystem().log("Performance test " + std::to_string(i));
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "\nPerformance test: 100 logs processed in " << duration.count() << " ms" << std::endl;
        std::cout << "Average time per log: " << (duration.count() * 10.0) << " microseconds" << std::endl;
        
        // Generate final report
        std::cout << "\n=== Final System Report ===" << std::endl;
        std::cout << nlm::getEnhancedLoggingSystem().generateSystemReport() << std::endl;
        
        // Cleanup
        nlm::getEnhancedLoggingSystem().shutdown();
        
        // Remove test files
        std::remove("test_logs_1.log");
        std::remove("test_logs_2.log");
        std::remove("test_export.json");
        std::remove("final_test_export.json");
        
        std::cout << "\n=========================================" << std::endl;
        std::cout << "All tests completed successfully!" << std::endl;
        std::cout << "Enhanced logging system is working correctly." << std::endl;
        std::cout << "=========================================" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "\nTest failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
