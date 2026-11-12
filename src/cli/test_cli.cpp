// NLM CLI Test Suite
// Comprehensive tests for the unified CLI functionality

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <cstring>

// Include CLI components
#include "nlm/CliCommand.hpp"
#include "nlm/Subcommands/RunCommand.hpp"
#include "nlm/Subcommands/AnalyzeCommand.hpp"
#include "nlm/Subcommands/ExportCommand.hpp"
#include "nlm/Subcommands/VisualizeCommand.hpp"
#include "nlm/Subcommands/ConfigCommand.hpp"
#include "nlm/Subcommands/ToolsCommand.hpp"

namespace nlm {

    class CLITestSuite {
    public:
        CLITestSuite() {
            // Initialize CLI commands
            initializeCLICommands();
        }
        
        void runAllTests() {
            std::cout << "=== NLM CLI Test Suite ===" << std::endl << std::endl;
            
            testCommandRegistration();
            testHelpSystem();
            testRunCommand();
            testAnalyzeCommand();
            testExportCommand();
            testVisualizeCommand();
            testConfigCommand();
            testToolsCommand();
            
            std::cout << std::endl;
            std::cout << "=== All CLI Tests Passed ===" << std::endl;
        }
        
    private:
        void initializeCLICommands() {
            #include "nlm/SubcommandsRegistration.cpp"
        }
        
        void testCommandRegistration() {
            std::cout << "Testing command registration..." << std::endl;
            
            // Test that all commands are registered
            std::vector<std::string> expectedCommands = {
                "run", "analyze", "export", "visualize", "config", "tools"
            };
            
            std::vector<std::string> registeredCommands = CommandRegistry::getInstance().getCommandNames();
            
            for (const auto& expected : expectedCommands) {
                bool found = false;
                for (const auto& registered : registeredCommands) {
                    if (expected == registered) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::cerr << "Error: Command '" << expected << "' not registered." << std::endl;
                    assert(false);
                }
            }
            
            std::cout << "  ✓ All commands registered successfully" << std::endl;
        }
        
        void testHelpSystem() {
            std::cout << "Testing help system..." << std::endl;
            
            // Test general help
            std::stringstream output;
            CommandRegistry::getInstance().printHelp(output);
            std::string helpOutput = output.str();
            
            assert(helpOutput.find("NLM - Neural Learning Machine CLI") != std::string::npos);
            assert(helpOutput.find("Usage: nlm <command> [options] [args]") != std::string::npos);
            assert(helpOutput.find("run - Run demonstrations and experiments") != std::string::npos);
            
            std::cout << "  ✓ Help system working correctly" << std::endl;
        }
        
        void testRunCommand() {
            std::cout << "Testing Run command..." << std::endl;
            
            // Test help
            std::vector<std::string> helpArgs = {"--help"};
            RunCommand runCommand;
            // This would normally execute, but we'll just verify the command exists
            assert(runCommand.getName() == "run");
            assert(runCommand.getDescription() == "Run demonstrations and experiments");
            
            std::cout << "  ✓ Run command structure valid" << std::endl;
        }
        
        void testAnalyzeCommand() {
            std::cout << "Testing Analyze command..." << std::endl;
            
            // Test command structure
            std::vector<std::string> analyzeArgs = {"connectivity", "--help"};
            AnalyzeCommand analyzeCommand;
            
            assert(analyzeCommand.getName() == "analyze");
            assert(analyzeCommand.getDescription() == "Analyze brain state and activity");
            
            std::cout << "  ✓ Analyze command structure valid" << std::endl;
        }
        
        void testExportCommand() {
            std::cout << "Testing Export command..." << std::endl;
            
            // Test command structure
            ExportCommand exportCommand;
            
            assert(exportCommand.getName() == "export");
            assert(exportCommand.getDescription() == "Export data and results");
            
            std::cout << "  ✓ Export command structure valid" << std::endl;
        }
        
        void testVisualizeCommand() {
            std::cout << "Testing Visualize command..." << std::endl;
            
            // Test command structure
            VisualizeCommand visualizeCommand;
            
            assert(visualizeCommand.getName() == "visualize");
            assert(visualizeCommand.getDescription() == "Visualize brain data and results");
            
            std::cout << "  ✓ Visualize command structure valid" << std::endl;
        }
        
        void testConfigCommand() {
            std::cout << "Testing Config command..." << std::endl;
            
            // Test command structure
            ConfigCommand configCommand;
            
            assert(configCommand.getName() == "config");
            assert(configCommand.getDescription() == "Manage configuration");
            
            std::cout << "  ✓ Config command structure valid" << std::endl;
        }
        
        void testToolsCommand() {
            std::cout << "Testing Tools command..." << std::endl;
            
            // Test command structure
            ToolsCommand toolsCommand;
            
            assert(toolsCommand.getName() == "tools");
            assert(toolsCommand.getDescription() == "Advanced tools and utilities");
            
            std::cout << "  ✓ Tools command structure valid" << std::endl;
        }
    };
    
} // namespace nlm

int main() {
    nlm::CLITestSuite testSuite;
    testSuite.runAllTests();
    
    return 0;
}