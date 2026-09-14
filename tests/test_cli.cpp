#include "cli.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

using namespace nlm;

namespace test_cli {
    
    void testParserBasic() {
        CommandLineParser parser;
        CLIOptions options;
        
        // Test help
        int argc = 2;
        char* argv[3];
        argv[0] = "nlm";
        argv[1] = "--help";
        argv[2] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.help == true);
    }
    
    void testParserVersion() {
        CommandLineParser parser;
        CLIOptions options;
        
        int argc = 2;
        char* argv[3];
        argv[0] = "nlm";
        argv[1] = "--version";
        argv[2] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.version == true);
        assert(!parser.getVersion().empty());
    }
    
    void testParserOutput() {
        CommandLineParser parser;
        CLIOptions options;
        
        int argc = 3;
        char* argv[4];
        argv[0] = "nlm";
        argv[1] = "--output";
        argv[2] = "json";
        argv[3] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.outputFormat == "json");
    }
    
    void testParserTest() {
        CommandLineParser parser;
        CLIOptions options;
        
        int argc = 3;
        char* argv[4];
        argv[0] = "nlm";
        argv[1] = "--test";
        argv[2] = "connectivity";
        argv[3] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.testNames.size() == 1);
        assert(options.testNames[0] == "connectivity");
    }
    
    void testParserCategory() {
        CommandLineParser parser;
        CLIOptions options;
        
        int argc = 3;
        char* argv[4];
        argv[0] = "nlm";
        argv[1] = "--category";
        argv[2] = "basic";
        argv[3] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.testCategories.size() == 1);
        assert(options.testCategories[0] == "basic");
    }
    
    void testParserMode() {
        CommandLineParser parser;
        CLIOptions options;
        
        int argc = 3;
        char* argv[4];
        argv[0] = "nlm";
        argv[1] = "--mode";
        argv[2] = "benchmark";
        argv[3] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.mode == "benchmark");
    }
    
    void testParserConfig() {
        CommandLineParser parser;
        CLIOptions options;
        
        int argc = 3;
        char* argv[4];
        argv[0] = "nlm";
        argv[1] = "--config";
        argv[2] = "my_config.cfg";
        argv[3] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.configFile == "my_config.cfg");
    }
    
    void testParserSteps() {
        CommandLineParser parser;
        CLIOptions options;
        
        int argc = 3;
        char* argv[4];
        argv[0] = "nlm";
        argv[1] = "--steps";
        argv[2] = "5000";
        argv[3] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.maxSteps == 5000);
    }
    
    void testParserQuiet() {
        CommandLineParser parser;
        CLIOptions options;
        
        int argc = 2;
        char* argv[3];
        argv[0] = "nlm";
        argv[1] = "--quiet";
        argv[2] = nullptr;
        
        assert(parser.parse(argc, argv, options));
        assert(options.quiet == true);
    }
    
    void testHelp() {
        CommandLineParser parser;
        std::string help = parser.getHelp();
        assert(!help.empty());
        assert(help.find("Usage:") != std::string::npos);
        assert(help.find("Test Selection") != std::string::npos);
        assert(help.find("--help") != std::string::npos);
    }
    
    void testVersion() {
        CommandLineParser parser;
        std::string version = parser.getVersion();
        assert(!version.empty());
        assert(version.find("NLM") != std::string::npos);
    }
    
    void testAvailableTests() {
        CommandLineParser parser;
        std::vector<TestInfo> tests = parser.getAvailableTests();
        assert(tests.size() > 0);
        
        bool hasConnectivity = false;
        for (const auto& test : tests) {
            if (test.name == "connectivity") {
                hasConnectivity = true;
                break;
            }
        }
        assert(hasConnectivity);
    }
    
    void testAvailableCategories() {
        CommandLineParser parser;
        std::vector<std::string> categories = parser.getAvailableCategories();
        assert(categories.size() > 0);
        
        bool hasBasic = false;
        for (const auto& category : categories) {
            if (category == "basic") {
                hasBasic = true;
                break;
            }
        }
        assert(hasBasic);
    }
    
    void testExecutorHelp() {
        CLIExecutor executor;
        CLIOptions options;
        options.help = true;
        
        CommandResult result = executor.execute(options);
        assert(result.exitCode == 0);
    }
    
    void testExecutorVersion() {
        CLIExecutor executor;
        CLIOptions options;
        options.version = true;
        
        CommandResult result = executor.execute(options);
        assert(result.exitCode == 0);
    }
    
    void testExecutorRunMode() {
        CLIExecutor executor;
        CLIOptions options;
        options.mode = "run";
        options.testNames = {"connectivity", "plasticity"};
        
        CommandResult result = executor.execute(options);
        assert(result.exitCode == 0);
        assert(result.success);
        assert(result.output.find("Running NLM in mode: run") != std::string::npos);
    }
    
    void testExecutorBenchmarkMode() {
        CLIExecutor executor;
        CLIOptions options;
        options.mode = "benchmark";
        
        CommandResult result = executor.execute(options);
        assert(result.exitCode == 0);
        assert(result.success);
        assert(result.output.find("Running NLM benchmark") != std::string::npos);
    }
    
    void testExecutorProfileMode() {
        CLIExecutor executor;
        CLIOptions options;
        options.mode = "profile";
        
        CommandResult result = executor.execute(options);
        assert(result.exitCode == 0);
        assert(result.success);
        assert(result.output.find("Running NLM profile") != std::string::npos);
    }
    
    void testExecutorValidationMode() {
        CLIExecutor executor;
        CLIOptions options;
        options.mode = "validate";
        
        CommandResult result = executor.execute(options);
        assert(result.exitCode == 0);
        assert(result.success);
        assert(result.output.find("Running NLM validation") != std::string::npos);
    }
    
    void testOutputFormatter() {
        std::string testData = "Test output data";
        
        assert(CLIExecutor::OutputFormatter::formatConsole(testData) == testData);
        assert(CLIExecutor::OutputFormatter::formatJSON(testData) == "{\"data\": \"Test output data\"}");
        assert(CLIExecutor::OutputFormatter::formatCSV(testData) == "\"Test output data\"");
        assert(CLIExecutor::OutputFormatter::formatHTML(testData) == "<div>Test output data</div>");
    }
    
    void testExecutorExecuteTest() {
        CLIExecutor executor;
        CommandResult result = executor.executeTest("connectivity");
        assert(result.exitCode == 0);
        assert(result.success);
        assert(result.output.find("Executing test: connectivity") != std::string::npos);
    }
    
    void testExecutorExecuteBenchmark() {
        CLIExecutor executor;
        CLIOptions options;
        options.benchmarkMetrics = "time,memory";
        
        CommandResult result = executor.executeBenchmark(options);
        assert(result.exitCode == 0);
        assert(result.success);
        assert(result.output.find("Executing benchmark with metrics") != std::string::npos);
    }
    
    void testExecutorExecuteProfile() {
        CLIExecutor executor;
        CLIOptions options;
        options.profileOutput = "profile.html";
        
        CommandResult result = executor.executeProfile(options);
        assert(result.exitCode == 0);
        assert(result.success);
        assert(result.output.find("Executing profile with output") != std::string::npos);
    }
    
    void testExecutorExecuteValidation() {
        CLIExecutor executor;
        CLIOptions options;
        options.validationSchema = "schema.xml";
        
        CommandResult result = executor.executeValidation(options);
        assert(result.exitCode == 0);
        assert(result.success);
        assert(result.output.find("Executing validation with schema") != std::string::npos);
    }
    
    void testLoadTests() {
        CLIExecutor executor;
        CLIOptions options;
        options.testNames = {"connectivity", "plasticity"};
        
        std::vector<std::string> tests = executor.loadTests(options);
        assert(tests.size() == 2);
        assert(std::find(tests.begin(), tests.end(), "connectivity") != tests.end());
        assert(std::find(tests.begin(), tests.end(), "plasticity") != tests.end());
    }
    
    void testFilterTests() {
        CLIExecutor executor;
        std::vector<std::string> allTests = {"connectivity", "plasticity", "stdp", "brain"};
        CLIOptions options;
        options.testCategories = {"basic", "plasticity"};
        
        std::vector<std::string> filtered = executor.filterTests(allTests, options);
        assert(filtered.size() >= 1);
        assert(std::find(filtered.begin(), filtered.end(), "connectivity") != filtered.end());
        assert(std::find(filtered.begin(), filtered.end(), "plasticity") != filtered.end());
    }
    
    void testFilterTestsByPattern() {
        CLIExecutor executor;
        std::vector<std::string> allTests = {"connectivity", "plasticity", "stdp", "brain"};
        CLIOptions options;
        options.testPattern = "con";
        
        std::vector<std::string> filtered = executor.filterTests(allTests, options);
        assert(filtered.size() == 1);
        assert(filtered[0] == "connectivity");
    }
    
    void runAllTests() {
        std::cout << "=== Running CLI Tests ===" << std::endl;
        
        try {
            testParserBasic();
            std::cout << "✓ testParserBasic" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserBasic: " << e.what() << std::endl;
        }
        
        try {
            testParserVersion();
            std::cout << "✓ testParserVersion" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserVersion: " << e.what() << std::endl;
        }
        
        try {
            testParserOutput();
            std::cout << "✓ testParserOutput" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserOutput: " << e.what() << std::endl;
        }
        
        try {
            testParserTest();
            std::cout << "✓ testParserTest" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserTest: " << e.what() << std::endl;
        }
        
        try {
            testParserCategory();
            std::cout << "✓ testParserCategory" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserCategory: " << e.what() << std::endl;
        }
        
        try {
            testParserMode();
            std::cout << "✓ testParserMode" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserMode: " << e.what() << std::endl;
        }
        
        try {
            testParserConfig();
            std::cout << "✓ testParserConfig" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserConfig: " << e.what() << std::endl;
        }
        
        try {
            testParserSteps();
            std::cout << "✓ testParserSteps" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserSteps: " << e.what() << std::endl;
        }
        
        try {
            testParserQuiet();
            std::cout << "✓ testParserQuiet" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testParserQuiet: " << e.what() << std::endl;
        }
        
        try {
            testHelp();
            std::cout << "✓ testHelp" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testHelp: " << e.what() << std::endl;
        }
        
        try {
            testVersion();
            std::cout << "✓ testVersion" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testVersion: " << e.what() << std::endl;
        }
        
        try {
            testAvailableTests();
            std::cout << "✓ testAvailableTests" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testAvailableTests: " << e.what() << std::endl;
        }
        
        try {
            testAvailableCategories();
            std::cout << "✓ testAvailableCategories" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testAvailableCategories: " << e.what() << std::endl;
        }
        
        try {
            testExecutorHelp();
            std::cout << "✓ testExecutorHelp" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorHelp: " << e.what() << std::endl;
        }
        
        try {
            testExecutorVersion();
            std::cout << "✓ testExecutorVersion" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorVersion: " << e.what() << std::endl;
        }
        
        try {
            testExecutorRunMode();
            std::cout << "✓ testExecutorRunMode" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorRunMode: " << e.what() << std::endl;
        }
        
        try {
            testExecutorBenchmarkMode();
            std::cout << "✓ testExecutorBenchmarkMode" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorBenchmarkMode: " << e.what() << std::endl;
        }
        
        try {
            testExecutorProfileMode();
            std::cout << "✓ testExecutorProfileMode" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorProfileMode: " << e.what() << std::endl;
        }
        
        try {
            testExecutorValidationMode();
            std::cout << "✓ testExecutorValidationMode" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorValidationMode: " << e.what() << std::endl;
        }
        
        try {
            testOutputFormatter();
            std::cout << "✓ testOutputFormatter" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testOutputFormatter: " << e.what() << std::endl;
        }
        
        try {
            testExecutorExecuteTest();
            std::cout << "✓ testExecutorExecuteTest" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorExecuteTest: " << e.what() << std::endl;
        }
        
        try {
            testExecutorExecuteBenchmark();
            std::cout << "✓ testExecutorExecuteBenchmark" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorExecuteBenchmark: " << e.what() << std::endl;
        }
        
        try {
            testExecutorExecuteProfile();
            std::cout << "✓ testExecutorExecuteProfile" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorExecuteProfile: " << e.what() << std::endl;
        }
        
        try {
            testExecutorExecuteValidation();
            std::cout << "✓ testExecutorExecuteValidation" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testExecutorExecuteValidation: " << e.what() << std::endl;
        }
        
        try {
            testLoadTests();
            std::cout << "✓ testLoadTests" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testLoadTests: " << e.what() << std::endl;
        }
        
        try {
            testFilterTests();
            std::cout << "✓ testFilterTests" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testFilterTests: " << e.what() << std::endl;
        }
        
        try {
            testFilterTestsByPattern();
            std::cout << "✓ testFilterTestsByPattern" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ testFilterTestsByPattern: " << e.what() << std::endl;
        }
        
        std::cout << "=== CLI Tests Complete ===" << std::endl;
    }

} // namespace test_cli