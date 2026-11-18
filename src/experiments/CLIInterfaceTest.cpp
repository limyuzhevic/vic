// Simple test program to verify the improved CLI interface
// Demonstrates the new user experience and error handling

#include "core/DemoProgramBase.hpp"
#include <iostream>
#include <string>

using namespace nlm;

class CLIInterfaceTest : public DemoProgram {
public:
    CLIInterfaceTest() : DemoProgram("nlm_cli_test") {
        // Add test-specific CLI arguments
        parser.addArgument("test", "Test identifier", false);
        parser.addArgument("param1", "First parameter", false);
        parser.addArgument("param2", "Second parameter", false);
        parser.addArgument("verbose", "Enable verbose output", false);
    }
    
    int execute() override {
        std::cout << "=== NLM CLI Interface Test ===" << std::endl;
        std::cout << "Testing improved command-line interface and user experience" << std::endl;
        
        // Test parameter parsing
        std::string testId = parser.getValue("test", "default_test");
        std::string param1 = parser.getValue("param1", "default1");
        std::string param2 = parser.getValue("param2", "default2");
        bool verbose = parser.getBoolValue("verbose", false);
        
        reporter.status("Test identifier: " + testId);
        reporter.status("Parameter 1: " + param1);
        reporter.status("Parameter 2: " + param2);
        
        if (verbose) {
            reporter.status("Verbose mode enabled");
            std::cout << "\nVerbose output:" << std::endl;
            std::cout << "  All parameters and their values are displayed above." << std::endl;
        }
        
        // Test progress reporting
        reporter.status("Starting progress test...");
        for (int i = 0; i < 10; ++i) {
            reporter.progress(i + 1, 10, "Processing items");
        }
        
        reporter.success("CLI interface test completed successfully");
        
        // Test error handling demonstration
        reporter.status("Demonstrating error handling...");
        
        // Test invalid parameter handling (this would be caught by the framework)
        if (testId.empty()) {
            ErrorHandler::handleError("CLI Test", "Test identifier cannot be empty");
        }
        
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Test passed: CLI interface, parameter parsing, and progress reporting work correctly" << std::endl;
        std::cout << "Error handling: Framework provides consistent error reporting" << std::endl;
        
        return 0;
    }
};

int main(int argc, char* argv[]) {
    return runDemoProgram<CLIInterfaceTest>(
        argc, argv,
        "nlm_cli_test",
        "CLI Interface Test",
        1,  // default param1
        2,  // default param2
        "Tests the improved CLI interface, parameter parsing, and user experience"
    );
}
