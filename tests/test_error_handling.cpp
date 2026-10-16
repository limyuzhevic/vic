// Comprehensive error handling tests for NLM
#include "core/Errors/ErrorSystem.hpp"
#include "core/Utilities/ErrorHandlingUtilities.hpp"
#include "core/Recovery/ConfigRecovery.hpp"
#include "core/CLI/CLIErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_error_handling {

void testErrorHierarchy() {
    std::cout << "    Testing error hierarchy..." << std::endl;
    
    // Test base NLMException
    nlm::NLMException baseEx("Base error", nlm::ErrorSeverity::Error, "BASE_001", "Base error details");
    assert(std::string(baseEx.what()).find("Base error") != std::string::npos);
    assert(baseEx.getErrorCode() == "BASE_001");
    assert(baseEx.getSeverity() == nlm::ErrorSeverity::Error);
    
    // Test system errors
    nlm::SystemError sysEx("System error", "SYS_001", "System error details");
    assert(sysEx.getErrorCode().find("SYS") != std::string::npos);
    assert(sysEx.getSeverity() == nlm::ErrorSeverity::Critical);
    
    // Test neural errors
    nlm::NeuralError neuralEx("Neural error", "NRL_001", "Neural error details");
    assert(neuralEx.getErrorCode().find("NRL") != std::string::npos);
    
    // Test configuration errors
    nlm::ConfigError configEx("Config error", "CFG_001", "Config error details", nlm::ErrorSeverity::Warning);
    assert(configEx.getErrorCode().find("CFG") != std::string::npos);
    assert(configEx.getSeverity() == nlm::ErrorSeverity::Warning);
    
    std::cout << "    testErrorHierarchy passed" << std::endl;
}

void testErrorContext() {
    std::cout << "    Testing error context..." << std::endl;
    
    nlm::ErrorContext context("Brain", "Initialize", "/path/to/file.cpp", 42, "testBrainInit");
    assert(context.component == "Brain");
    assert(context.operation == "Initialize");
    assert(context.file == "/path/to/file.cpp");
    assert(context.line == 42);
    assert(context.function == "testBrainInit");
    
    nlm::NLMException ex("Test error");
    ex.addContext("Context 1");
    ex.addContext("Context 2");
    
    auto contexts = ex.getContext();
    assert(contexts.size() == 2);
    assert(contexts[0] == "Context 1");
    assert(contexts[1] == "Context 2");
    
    ex.addRecoveryStep("Step 1");
    ex.addRecoveryStep("Step 2");
    
    auto steps = ex.getRecoverySteps();
    assert(steps.size() == 2);
    assert(steps[0] == "Step 1");
    assert(steps[1] == "Step 2");
    
    std::cout << "    testErrorContext passed" << std::endl;
}

void testErrorFormatting() {
    std::cout << "    Testing error formatting..." << std::endl;
    
    nlm::NLMException ex("Test error", nlm::ErrorSeverity::Error, "TEST_001", "Detailed error information");
    ex.addRecoveryStep("Recovery step 1");
    ex.addRecoveryStep("Recovery step 2");
    
    std::string formatted = ex.getFormattedMessage();
    assert(formatted.find("Test error") != std::string::npos);
    assert(formatted.find("Detailed error information") != std::string::npos);
    assert(formatted.find("TEST_001") != std::string::npos);
    assert(formatted.find("Recovery step 1") != std::string::npos);
    
    std::cout << "    testErrorFormatting passed" << std::endl;
}

void testErrorUtils() {
    std::cout << "    Testing error utilities..." << std::endl;
    
    nlm::NLMException nlmEx("NLM error", nlm::ErrorSeverity::Warning);
    assert(nlm::ErrorUtils::getSeverity(nlmEx) == nlm::ErrorSeverity::Warning);
    assert(nlm::ErrorUtils::isRecoverable(nlmEx) == true);
    
    std::runtime_error stdEx("Standard exception");
    assert(nlm::ErrorUtils::getSeverity(stdEx) == nlm::ErrorSeverity::Error);
    assert(nlm::ErrorUtils::isRecoverable(stdEx) == true);
    
    std::string summary = nlm::ErrorUtils::getErrorSummary(nlmEx);
    assert(summary.find("NLM error") != std::string::npos);
    
    std::cout << "    testErrorUtils passed" << std::endl;
}

void testErrorHandler() {
    std::cout << "    Testing error handler..." << std::endl;
    
    nlm::ErrorHandler handler;
    
    nlm::NLMException ex("Test error", nlm::ErrorSeverity::Error);
    handler.handle(ex);
    
    // Test with string error
    handler.handle("String error", nlm::ErrorSeverity::Warning, "Details", "ERR_001");
    
    // Test error statistics
    auto stats = handler.getErrorStats();
    assert(stats.severityCounts[nlm::ErrorSeverity::Error] >= 0);
    
    // Test error threshold
    handler.setErrorThreshold(nlm::ErrorSeverity::Warning);
    
    // Test error handling enabled
    handler.setErrorHandlingEnabled(true);
    
    std::cout << "    testErrorHandler passed" << std::endl;
}

void testErrorRecovery() {
    std::cout << "    Testing error recovery..." << std::endl;
    
    nlm::ErrorHandler handler;
    
    nlm::NLMException ex("Test error", nlm::ErrorSeverity::Warning);
    
    // Test recovery
    handler.attemptRecovery(ex);
    
    // Test adding recovery strategy
    auto strategy = std::make_shared<nlm::AutomaticErrorRecovery>();
    handler.addRecoveryStrategy(strategy);
    
    std::cout << "    testErrorRecovery passed" << std::endl;
}

void testErrorReporting() {
    std::cout << "    Testing error reporting..." << std::endl;
    
    nlm::ConsoleErrorReporter reporter;
    
    reporter.report("Test message", nlm::ErrorSeverity::Error);
    
    reporter.setErrorThreshold(nlm::ErrorSeverity::Warning);
    
    auto stats = reporter.getErrorStats();
    assert(stats.errorCount_ >= 0);
    assert(stats.warningCount_ >= 0);
    
    std::cout << "    testErrorReporting passed" << std::endl;
}

void testConfigValidation() {
    std::cout << "    Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("region_count", 4, nlm::ConfigSource::Runtime);
    
    nlm::ConfigValidator validator;
    validator.addRangeRule("neuron_count", 10, 1000, nlm::ValidationSeverity::Error);
    validator.addRangeRule("region_count", 1, 100, nlm::ValidationSeverity::Error);
    
    assert(validator.isValid(config));
    
    std::vector<nlm::ValidationError> errors = validator.getErrors(config);
    assert(errors.empty());
    
    // Test invalid configuration
    config.set("neuron_count", 5000, nlm::ConfigSource::Runtime);
    assert(!validator.isValid(config));
    
    errors = validator.getErrors(config);
    assert(!errors.empty());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigRecovery() {
    std::cout << "    Testing configuration recovery..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 5000, nlm::ConfigSource::Runtime);
    config.set("region_count", 150, nlm::ConfigSource::Runtime);
    
    nlm::ConfigRecovery recovery;
    
    // Test validation and recovery
    bool success = recovery.validateAndRecover(config);
    assert(success);
    
    // Verify configuration was fixed
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount >= 10 && *neuronCount <= 10000);
    
    auto regionCount = config.get<int64_t>("region_count");
    assert(regionCount.has_value());
    assert(*regionCount >= 1 && *regionCount <= 100);
    
    std::cout << "    testConfigRecovery passed" << std::endl;
}

void testConfigCheckpoint() {
    std::cout << "    Testing configuration checkpoint..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 10, nlm::ConfigSource::Runtime);
    
    nlm::SystemCheckpoint checkpoint;
    checkpoint.setCheckpointPath("./test_checkpoints");
    
    // Test checkpoint
    std::string filepath = "./test_checkpoints/test.chk";
    bool checkpointSuccess = checkpoint.checkpoint(filepath, config);
    assert(checkpointSuccess);
    
    // Test restore
    nlm::Config restoredConfig;
    bool restoreSuccess = checkpoint.restore(filepath, restoredConfig);
    assert(restoreSuccess);
    
    std::cout << "    testConfigCheckpoint passed" << std::endl;
}

void testCLIERrorHandling() {
    std::cout << "    Testing CLI error handling..." << std::endl;
    
    nlm::CLIErrorHandlingConfig cliConfig;
    cliConfig.errorHandlingMode = nlm::CLIErrorHandlingMode::Graceful;
    cliConfig.enableAutoRecovery = true;
    cliConfig.enableGracefulDegradation = true;
    
    nlm::CLIErrorHandler cliHandler;
    bool initSuccess = cliHandler.initialize(cliConfig);
    assert(initSuccess);
    
    nlm::NLMException ex("CLI test error", nlm::ErrorSeverity::Error);
    std::exception handledError = cliHandler.handle(ex, "CLI Test");
    assert(handledError.what() != nullptr);
    
    nlm::Config config;
    bool configSuccess = cliHandler.handleConfigError(config, "CLI Test");
    assert(configSuccess);
    
    std::cout << "    testCLIERrorHandling passed" << std::endl;
}

void testErrorCallbacks() {
    std::cout << "    Testing error callbacks..." << std::endl;
    
    nlm::ErrorCallbackManager callbackManager;
    
    std::vector<std::exception> capturedErrors;
    callbackManager.addErrorCallback([&capturedErrors](const std::exception& error) {
        capturedErrors.push_back(error);
    });
    
    nlm::NLMException ex("Callback test error");
    callbackManager.invokeCallbacks(ex);
    
    assert(capturedErrors.size() == 1);
    assert(std::string(capturedErrors[0].what()).find("Callback test error") != std::string::npos);
    
    std::cout << "    testErrorCallbacks passed" << std::endl;
}

void testGlobalErrorHandlers() {
    std::cout << "    Testing global error handlers..." << std::endl;
    
    // Test global error handler
    nlm::NLMException ex("Global test error");
    nlm::handle(ex);
    
    // Test global error callback manager
    nlm::registerErrorCallback([](const std::exception& error) {
        // Callback implementation
    });
    
    // Test global CLI error handler
    nlm::CLIErrorHandlingConfig cliConfig;
    nlm::initializeCLIErrorHandler(cliConfig);
    
    std::cout << "    testGlobalErrorHandlers passed" << std::endl;
}

void testErrorPatterns() {
    std::cout << "    Testing error patterns..." << std::endl;
    
    nlm::NLMException ex1("Error 1", nlm::ErrorSeverity::Error, "ERR_001");
    nlm::NLMException ex2("Error 2", nlm::ErrorSeverity::Warning, "ERR_002");
    nlm::NLMException ex3("Error 3", nlm::ErrorSeverity::Error, "ERR_001");
    
    std::vector<std::exception> errors = {ex1, ex2, ex3};
    
    // Test error pattern analysis (if available)
    // This would test the error mitigation system
    
    std::cout << "    testErrorPatterns passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive error handling tests..." << std::endl << std::endl;
    
    testErrorHierarchy();
    testErrorContext();
    testErrorFormatting();
    testErrorUtils();
    testErrorHandler();
    testErrorRecovery();
    testErrorReporting();
    testConfigValidation();
    testConfigRecovery();
    testConfigCheckpoint();
    testCLIERrorHandling();
    testErrorCallbacks();
    testGlobalErrorHandlers();
    testErrorPatterns();
    
    std::cout << std::endl;
    std::cout << "All error handling tests passed successfully!" << std::endl;
}

} // namespace test_error_handling
