// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration of an artificial developmental brain.
//
// This is the new Phase 6 main entry point using the enhanced application framework.
// Demonstrates the improved architecture with:
// - Modular application structure
// - Enhanced configuration system
// - Extensible test framework
// - Comprehensive error handling
// - Phase-specific configuration profiles

#include "framework/NLMApplication.hpp"
#include "framework/TestFramework.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "test_utils/test_utils.hpp"
#include <iostream>

using namespace nlm;

// Custom application that demonstrates the new features
class Phase6Application : public NLMApplication {
public:
    Phase6Application() : NLMApplication() {
        // Register built-in tests using the test framework utilities
        registerTest("Basic Connectivity", test_utils::createConnectivityTest());
        registerTest("Plasticity Learning", test_utils::createPlasticityTest());
        registerTest("STDP Verification", test_utils::createSTDPTest());
        
        // Enable detailed logging for the application
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        auto fileLogger = std::make_shared<FileLogger>("nlm_phase6.log", LogLevel::Info);
        
        getLogger()->addLogger(consoleLogger);
        getLogger()->addLogger(fileLogger);
    }
    
protected:
    ApplicationResult onInitialization() override {
        logStatus();
        onProgress(1.0, "Phase 6 application initialized");
        return ApplicationResult::Success;
    }
    
    ApplicationResult onConfiguration() override {
        onProgress(1.0, "Phase 6 configuration loaded");
        return ApplicationResult::Success;
    }
    
    ApplicationResult onBrainSetup() override {
        onProgress(1.0, "Phase 6 brain setup completed");
        return ApplicationResult::Success;
    }
    
    ApplicationResult onTesting(std::shared_ptr<Brain> brain) override {
        onProgress(0.0, "Running Phase 6 integration tests");
        
        // Run tests using the enhanced test framework
        auto testResults = runAllTests(brain);
        
        // Log detailed test results
        size_t passed = 0, failed = 0, skipped = 0;
        for (const auto& result : testResults) {
            switch (result.status) {
                case TestResult::Status::PASSED: passed++; break;
                case TestResult::Status::FAILED: failed++; break;
                case TestResult::Status::SKIPPED: skipped++; break;
            }
        }
        
        onProgress(1.0, "Phase 6 testing completed: " + std::to_string(passed) + 
                    " passed, " + std::to_string(failed) + " failed, " + 
                    std::to_string(skipped) + " skipped");
        
        // Log test summary
        if (logger_) {
            logger_->info("=== Phase 6 Test Results ===");
            logger_->info("Total tests: " + std::to_string(testResults.size()));
            logger_->info("Passed: " + std::to_string(passed));
            logger_->info("Failed: " + std::to_string(failed));
            logger_->info("Skipped: " + std::to_string(skipped));
            
            if (failed > 0) {
                logger_->error("Phase 6 integration test failures detected");
                return ApplicationResult::ExecutionError;
            }
        }
        
        return ApplicationResult::Success;
    }
    
    ApplicationResult onCleanup() override {
        onProgress(1.0, "Phase 6 cleanup completed");
        return ApplicationResult::Success;
    }
    
    ApplicationResult onShutdown() override {
        logStatus();
        
        if (logger_) {
            logger_->info("=== Phase 6 Complete ===");
            logger_->info("The NLM Phase 6 brain is now a functioning artificial neural substrate");
            logger_->info("capable of changing its own synaptic connections through experience.");
        }
        
        return ApplicationResult::Success;
    }
    
    void onPhaseChanged(ApplicationPhase oldPhase, ApplicationPhase newPhase) override {
        if (logger_) {
            std::string phaseName;
            switch (newPhase) {
                case ApplicationPhase::Initialization: phaseName = "Initialization"; break;
                case ApplicationPhase::Configuration: phaseName = "Configuration"; break;
                case ApplicationPhase::BrainSetup: phaseName = "Brain Setup"; break;
                case ApplicationPhase::Testing: phaseName = "Testing"; break;
                case ApplicationPhase::Cleanup: phaseName = "Cleanup"; break;
                case ApplicationPhase::Shutdown: phaseName = "Shutdown"; break;
            }
            logger_->info("Phase changed: " + phaseName);
        }
    }
    
    void onError(const std::string& context, const std::exception& e) override {
        if (logger_) {
            logger_->error("Error in " + context + ": " + e.what());
        } else {
            std::cerr << "Error in " << context << ": " << e.what() << std::endl;
        }
    }
    
    void onProgress(double progress, const std::string& task) override {
        status_.currentTask = task;
        status_.progress = progress;
        
        if (logger_) {
            logger_->info("Progress: " + std::to_string(progress * 100) + "% - " + task);
        } else {
            std::cout << "Progress: " << (progress * 100) << "% - " << task << std::endl;
        }
    }
};

int main(int argc, char** argv) {
    try {
        Phase6Application app;
        
        // Parse command line arguments
        auto options = NLMApplication::parseCommandLine(argc, argv);
        
        // Show help if requested
        if (options.help) {
            std::cout << "NLM Phase 6 - Neural Learning Machine" << std::endl;
            std::cout << "Usage: nlm_phase6 [--config=file] [--verbose] [--quiet] [--help] [--version]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --config=file   Load configuration from file" << std::endl;
            std::cout << "  --verbose       Enable verbose logging" << std::endl;
            std::cout << "  --quiet         Suppress informational messages" << std::endl;
            std::cout << "  --help          Show this help message" << std::endl;
            std::cout << "  --version       Show version information" << std::endl;
            return 0;
        }
        
        // Show version if requested
        if (options.version) {
            std::cout << "NLM Phase 6 - Neural Learning Machine v0.1.0" << std::endl;
            std::cout << "Artificial Developmental Brain with Real Neural Computation" << std::endl;
            return 0;
        }
        
        // Set up logging based on options
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(options.quiet ? LogLevel::Warning : LogLevel::Info);
        
        if (options.verbose) {
            consoleLogger->setLevel(LogLevel::Debug);
        }
        
        logger->addLogger(consoleLogger);
        app.setLogger(logger);
        
        // Run the application
        auto result = app.run(argc, argv);
        
        return static_cast<int>(result);
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return static_cast<int>(ApplicationResult::UnknownError);
    }
}
