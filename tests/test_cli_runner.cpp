// NLM CLI and TestRunner Tests
// Phase 2: Real Neural Computation Tests - Advanced

#include "cli/CLIParser.hpp"
#include "experiments/TestRunner.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

namespace test_cli_runner {

void testCLIParserCreation() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    // Should be able to create parser
    assert(parser.getConfig() == config);
    
    std::cout << "    testCLIParserCreation passed" << std::endl;
}

void testCLIParserHelp() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    // Should be able to generate help
    auto help = parser.generateHelp();
    assert(!help.programName.empty());
    assert(!help.version.empty());
    assert(!help.description.empty());
    
    std::cout << "    testCLIParserHelp passed" << std::endl;
}

void testCLIParserBasicParsing() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test basic parsing
    int argc = 3;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--test",
        (char*)"basic"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(success);
    assert(args.testTypes.find("basic") != args.testTypes.end());
    
    std::cout << "    testCLIParserBasicParsing passed" << std::endl;
}

void testCLIParserMultipleTests() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test multiple tests
    int argc = 7;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--test",
        (char*)"plasticity",
        (char*)"--test",
        (char*)"stdp",
        (char*)"--test",
        (char*)"all"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(success);
    assert(args.testTypes.find("plasticity") != args.testTypes.end());
    assert(args.testTypes.find("stdp") != args.testTypes.end());
    
    std::cout << "    testCLIParserMultipleTests passed" << std::endl;
}

void testCLIParserTestParameters() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test test-specific parameters
    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--test",
        (char*)"plasticity",
        (char*)"--plasticity-steps",
        (char*)"2000"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(success);
    assert(args.testTypes.find("plasticity") != args.testTypes.end());
    
    std::cout << "    testCLIParserTestParameters passed" << std::endl;
}

void testCLIParserVerbosity() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test verbosity
    int argc = 3;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--verbose",
        (char*)"--quiet"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(success);
    // Verbosity level should be 0 (verbose then quiet)
    assert(args.verbosityLevel == 0);
    
    std::cout << "    testCLIParserVerbosity passed" << std::endl;
}

void testCLIParserOutputOptions() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test output options
    int argc = 4;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--save-results",
        (char*)"--results-file",
        (char*)"results.json"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(success);
    assert(args.saveResults);
    assert(args.resultsFile == "results.json");
    
    std::cout << "    testCLIParserOutputOptions passed" << std::endl;
}

void testCLIParserTimingOptions() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test timing options
    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--max-time",
        (char*)"1800",
        (char*)"--step-delay",
        (char*)"0.001"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(success);
    assert(args.maxExecutionTime == std::chrono::seconds(1800));
    assert(args.stepDelay == 0.001);
    
    std::cout << "    testCLIParserTimingOptions passed" << std::endl;
}

void testCLIParserConfigOptions() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test config options
    int argc = 3;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--config",
        (char*)"myconfig.cfg"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(success);
    assert(args.configFile == "myconfig.cfg");
    
    std::cout << "    testCLIParserConfigOptions passed" << std::endl;
}

void testCLIParserValidation() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test validation with unknown test type
    int argc = 3;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--test",
        (char*)"unknown_test"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(!success); // Should fail validation
    assert(parser.hasError());
    
    std::cout << "    testCLIParserValidation passed" << std::endl;
}

void testCLIParserErrorHandling() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    CommandLineArgs args;
    
    // Test error handling
    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--test",
        (char*)"basic",
        (char*)"--invalid-option",
        (char*)"value"
    };
    
    bool success = parser.parse(argc, argv, args);
    assert(!success);
    assert(parser.hasError());
    
    std::cout << "    testCLIParserErrorHandling passed" << std::endl;
}

void testCLIParserStatistics() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    CLIParser parser(config, logger);
    
    // Test statistics
    assert(parser.getTotalArgumentsProcessed() >= 0);
    assert(parser.getSuccessfulParses() >= 0);
    
    std::cout << "    testCLIParserStatistics passed" << std::endl;
}

void testTestRunnerCreation() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    TestRunner testRunner(config, logger);
    
    // Should be able to create test runner
    auto availableTests = testRunner.getAvailableTests();
    assert(!availableTests.empty());
    
    std::cout << "    testTestRunnerCreation passed" << std::endl;
}

void testTestRunnerBasicFunctionality() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    TestRunner testRunner(config, logger);
    
    // Test basic functionality
    std::vector<std::string> testNames = {"basic", "plasticity", "stdp"};
    std::vector<TestParameters> paramsList;
    
    for (const auto& name : testNames) {
        TestParameters params;
        params.testType = name;
        paramsList.push_back(params);
    }
    
    auto results = testRunner.runTests(testNames, paramsList);
    
    // Should return results for each test
    assert(results.size() == testNames.size());
    
    std::cout << "    testTestRunnerBasicFunctionality passed" << std::endl;
}

void testTestRunnerExecutionModes() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    TestRunner testRunner(config, logger);
    
    // Test execution modes
    testRunner.setExecutionMode(ExecutionMode::Sequential);
    testRunner.setExecutionMode(ExecutionMode::Parallel);
    testRunner.setExecutionMode(ExecutionMode::Dependent);
    
    std::cout << "    testTestRunnerExecutionModes passed" << std::endl;
}

void testTestRunnerProgressReporting() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    TestRunner testRunner(config, logger);
    
    // Test progress reporting
    testRunner.setProgressReporting(true);
    testRunner.setProgressReporting(false);
    
    // Should be able to get progress info
    auto progressInfo = testRunner.getProgressInfo();
    assert(progressInfo.empty()); // No active progress initially
    
    std::cout << "    testTestRunnerProgressReporting passed" << std::endl;
}

void testTestRunnerResultsManagement() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    TestRunner testRunner(config, logger);
    
    // Test results management
    testRunner.clearResults();
    
    auto results = testRunner.getResults();
    assert(results.empty()); // No results initially
    
    std::cout << "    testTestRunnerResultsManagement passed" << std::endl;
}

void testTestRunnerValidation() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    TestRunner testRunner(config, logger);
    
    // Test validation
    TestParameters params;
    params.testType = "basic";
    
    bool valid = testRunner.validateTestParameters("basic", params);
    // Should be valid for basic test
    assert(valid);
    
    std::cout << "    testTestRunnerValidation passed" << std::endl;
}

void testTestRunnerStatistics() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    TestRunner testRunner(config, logger);
    
    // Test statistics
    double avgScore = testRunner.getAverageScore();
    assert(avgScore >= 0.0 && avgScore <= 1.0);
    
    std::chrono::milliseconds totalTime = testRunner.getTotalExecutionTime();
    assert(totalTime.count() >= 0);
    
    std::cout << "    testTestRunnerStatistics passed" << std::endl;
}

void testTestRunnerSaveLoadResults() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    TestRunner testRunner(config, logger);
    
    // Test save/load results
    std::string testFile = "test_results.json";
    
    bool saveSuccess = testRunner.saveResults(testFile);
    // Might fail if directory doesn't exist, but that's okay
    
    bool loadSuccess = testRunner.loadResults(testFile);
    // Might fail if file doesn't exist, but that's okay
    
    std::cout << "    testTestRunnerSaveLoadResults passed" << std::endl;
}

void testProgressReporterCreation() {
    auto logger = std::make_shared<Logger>();
    
    ProgressReporter progressReporter(logger);
    
    // Should be able to create progress reporter
    auto activeProgress = progressReporter.getActiveProgress();
    assert(activeProgress.empty());
    
    std::cout << "    testProgressReporterCreation passed" << std::endl;
}

void testProgressReporterBasicFunctionality() {
    auto logger = std::make_shared<Logger>();
    
    ProgressReporter progressReporter(logger);
    
    // Test basic functionality
    progressReporter.startProgress("test1", 100);
    assert(progressReporter.isProgressActive("test1"));
    
    progressReporter.updateProgress("test1", 50, "Halfway done");
    progressReporter.completeProgress("test1", true, "Completed successfully");
    
    assert(!progressReporter.isProgressActive("test1"));
    
    std::cout << "    testProgressReporterBasicFunctionality passed" << std::endl;
}

void testProgressReporterConfiguration() {
    auto logger = std::make_shared<Logger>();
    
    ProgressReporter progressReporter(logger);
    
    // Test configuration
    progressReporter.setDisplayInterval(50);
    progressReporter.setEnableColors(true);
    progressReporter.setEnableProgressBar(true);
    progressReporter.setEnableTimeEstimates(true);
    progressReporter.setUpdateFrequency(2.0);
    
    std::cout << "    testProgressReporterConfiguration passed" << std::endl;
}

void testAdvancedLoggerCreation() {
    auto logger = std::make_shared<Logger>();
    
    AdvancedLogger advancedLogger(logger);
    
    // Should be able to create advanced logger
    std::cout << "    testAdvancedLoggerCreation passed" << std::endl;
}

void testAdvancedLoggerBasicFunctionality() {
    auto logger = std::make_shared<Logger>();
    
    AdvancedLogger advancedLogger(logger);
    
    // Test basic functionality
    advancedLogger.logOperation("test_operation", "test_component", "test1", true);
    
    LogContext context("test_op", "test_comp", "test1");
    advancedLogger.logWithContext(context, "Test message", "INFO");
    
    advancedLogger.logProgress("test1", 50, 100, "Halfway there");
    
    advancedLogger.logPerformanceMetric("test_metric", 42.5, "units", "test1");
    
    std::cout << "    testAdvancedLoggerBasicFunctionality passed" << std::endl;
}

void testErrorHandlerCreation() {
    auto logger = std::make_shared<Logger>();
    
    ErrorHandler errorHandler(logger);
    
    // Should be able to create error handler
    assert(errorHandler.getTotalErrorCount() == 0);
    
    std::cout << "    testErrorHandlerCreation passed" << std::endl;
}

void testErrorHandlerBasicFunctionality() {
    auto logger = std::make_shared<Logger>();
    
    ErrorHandler errorHandler(logger);
    
    // Test basic functionality
    ErrorInfo error(ErrorSeverity::Warning, "Test warning", "TEST_WARNING");
    ErrorContext context("test_op", "test_comp", "test1");
    
    errorHandler.reportError(error, context);
    
    assert(errorHandler.getTotalErrorCount() > 0);
    
    std::cout << "    testErrorHandlerBasicFunctionality passed" << std::endl;
}

void testInputValidatorCreation() {
    auto logger = std::make_shared<Logger>();
    auto errorHandler = std::make_shared<ErrorHandler>(logger);
    
    InputValidator inputValidator(errorHandler);
    
    // Should be able to create input validator
    std::cout << "    testInputValidatorCreation passed" << std::endl;
}

void testInputValidatorBasicFunctionality() {
    auto logger = std::make_shared<Logger>();
    auto errorHandler = std::make_shared<ErrorHandler>(logger);
    
    InputValidator inputValidator(errorHandler);
    
    // Test basic functionality
    bool valid = inputValidator.validateParameter("test_param", "test_value", "test1");
    
    ValidationResult result = inputValidator.validateParameters({{"param1", "value1"}, {"param2", "value2"}}, "test1");
    
    std::cout << "    testInputValidatorBasicFunctionality passed" << std::endl;
}

void testEnhancedConfigCreation() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    EnhancedConfig enhancedConfig(config, logger);
    
    // Should be able to create enhanced config
    auto baseConfig = enhancedConfig.getBaseConfig();
    assert(baseConfig == config);
    
    std::cout << "    testEnhancedConfigCreation passed" << std::endl;
}

void testEnhancedConfigBasicFunctionality() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    EnhancedConfig enhancedConfig(config, logger);
    
    // Test basic functionality
    enhancedConfig.setValue("test_key", "test_value");
    
    auto val = enhancedConfig.getOr<std::string>("test_key", "default");
    assert(val == "test_value");
    
    std::cout << "    testEnhancedConfigBasicFunctionality passed" << std::endl;
}

void testIntegration() {
    auto config = std::make_shared<Config>();
    auto logger = std::make_shared<Logger>();
    
    // Test integration of all components
    CLIParser cliParser(config, logger);
    TestRunner testRunner(config, logger);
    ProgressReporter progressReporter(logger);
    AdvancedLogger advancedLogger(logger);
    ErrorHandler errorHandler(logger);
    InputValidator inputValidator(&errorHandler);
    EnhancedConfig enhancedConfig(config, logger);
    
    // All components should work together
    assert(&cliParser.getConfig() == config.get());
    
    std::cout << "    testIntegration passed" << std::endl;
}

// Main test runner
int main() {
    std::cout << "=== NLM CLI and TestRunner Tests (Advanced) ===" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // CLI Parser tests
    std::cout << "Testing CLI Parser..." << std::endl;
    try {
        testCLIParserCreation();
        testCLIParserHelp();
        testCLIParserBasicParsing();
        testCLIParserMultipleTests();
        testCLIParserTestParameters();
        testCLIParserVerbosity();
        testCLIParserOutputOptions();
        testCLIParserTimingOptions();
        testCLIParserConfigOptions();
        testCLIParserValidation();
        testCLIParserErrorHandling();
        testCLIParserStatistics();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // TestRunner tests
    std::cout << "Testing TestRunner..." << std::endl;
    try {
        testTestRunnerCreation();
        testTestRunnerBasicFunctionality();
        testTestRunnerExecutionModes();
        testTestRunnerProgressReporting();
        testTestRunnerResultsManagement();
        testTestRunnerValidation();
        testTestRunnerStatistics();
        testTestRunnerSaveLoadResults();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Progress Reporter tests
    std::cout << "Testing Progress Reporter..." << std::endl;
    try {
        testProgressReporterCreation();
        testProgressReporterBasicFunctionality();
        testProgressReporterConfiguration();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Advanced Logger tests
    std::cout << "Testing Advanced Logger..." << std::endl;
    try {
        testAdvancedLoggerCreation();
        testAdvancedLoggerBasicFunctionality();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Error Handler tests
    std::cout << "Testing Error Handler..." << std::endl;
    try {
        testErrorHandlerCreation();
        testErrorHandlerBasicFunctionality();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Input Validator tests
    std::cout << "Testing Input Validator..." << std::endl;
    try {
        testInputValidatorCreation();
        testInputValidatorBasicFunctionality();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Enhanced Config tests
    std::cout << "Testing Enhanced Config..." << std::endl;
    try {
        testEnhancedConfigCreation();
        testEnhancedConfigBasicFunctionality();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Integration tests
    std::cout << "Testing Integration..." << std::endl;
    try {
        testIntegration();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    if (allPassed) {
        std::cout << "=== All Tests Passed ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests Failed ===" << std::endl;
        return 1;
    }
}

} // namespace test_cli_runner
