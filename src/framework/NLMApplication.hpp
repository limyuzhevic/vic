#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>

namespace nlm {

// Forward declarations
class Config;
class Logger;
class Brain;
class TestFramework;

// Application result codes

// Application result codes
enum class ApplicationResult {
    Success = 0,
    ConfigurationError = 1,
    InitializationError = 2,
    ExecutionError = 3,
    CleanupError = 4,
    UnknownError = 255
};

// Application phases
enum class ApplicationPhase {
    Initialization,
    Configuration,
    BrainSetup,
    Testing,
    Cleanup,
    Shutdown
};

// Application status
class ApplicationStatus {
public:
    enum class Phase {
        Initialization,
        Configuration,
        BrainSetup,
        Testing,
        Cleanup,
        Shutdown,
        Unknown
    };
    
    Phase currentPhase;
    std::string currentTask;
    double progress; // 0.0 to 1.0
    std::map<std::string, std::string> metadata;
    
    ApplicationStatus() : currentPhase(Phase::Initialization), 
                         currentTask("Starting"), progress(0.0) {}
};
struct TestResult {
    enum class Status { PASSED, FAILED, SKIPPED };
    
    std::string name;
    Status status;
    std::string message;
    double executionTime;
    
    TestResult(const std::string& name, Status status, const std::string& message = "")
        : name(name), status(status), message(message), executionTime(0.0) {}
    
    bool passed() const { return status == Status::PASSED; }
    bool failed() const { return status == Status::FAILED; }
    bool skipped() const { return status == Status::SKIPPED; }
};

// Test framework interface
class TestFramework {
public:
    virtual ~TestFramework() = default;
    virtual void registerTest(const std::string& name, std::function<TestResult(std::shared_ptr<Brain>)> test) = 0;
    virtual std::vector<TestResult> runAllTests(std::shared_ptr<Brain> brain) = 0;
    virtual void clearTests() = 0;
    virtual size_t getTestCount() const = 0;
};

// Main application framework
class NLMApplication {
public:
    NLMApplication();
    virtual ~NLMApplication();
    
    // Non-copyable, movable
    NLMApplication(const NLMApplication&) = delete;
    NLMApplication& operator=(const NLMApplication&) = delete;
    NLMApplication(NLMApplication&&) noexcept;
    NLMApplication& operator=(NLMApplication&&) noexcept;
    
    // Core application lifecycle
    ApplicationResult run(int argc, char** argv);
    ApplicationResult run(const std::vector<std::string>& args);
    
    // Component management
    void setConfig(std::shared_ptr<Config> config) { config_ = config; }
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    void setLogger(std::shared_ptr<Logger> logger) { logger_ = logger; }
    std::shared_ptr<Logger> getLogger() const { return logger_; }
    
    // Test management
    void registerTest(const std::string& name, std::function<TestResult(std::shared_ptr<Brain>)> test);
    std::vector<TestResult> runAllTests(std::shared_ptr<Brain> brain);
    
    // Phase execution
    ApplicationResult executePhase(ApplicationPhase phase);
    ApplicationResult executeConfigurationPhase(int argc, char** argv);
    ApplicationResult executeBrainSetupPhase();
    ApplicationResult executeTestingPhase(std::shared_ptr<Brain> brain);
    ApplicationResult executeCleanupPhase();
    
    // Error handling and recovery
    ApplicationResult handleError(const std::string& context, const std::exception& e);
    bool shouldContinueOnError() const { return continueOnError_; }
    void setContinueOnError(bool continue) { continueOnError_ = continue; }
    
    // Configuration profiles
    ApplicationResult loadDefaultConfig();
    ApplicationResult loadPhase2Config();
    ApplicationResult loadExperimentConfig();
    
    // Status and reporting
    ApplicationStatus getStatus() const { return status_; }
    std::string getStatusString() const;
    
    // Resource management
    bool hasBrain() const { return brain_ != nullptr; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
    // Configuration loading utilities
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createPhase2Config();
    static std::shared_ptr<Config> createExperimentConfig();
    
    // Command line argument parsing
    struct CommandLineOptions {
        std::string configFile;
        std::string logFile;
        std::string outputFile;
        std::string testFilter;
        std::vector<std::string> customArgs;
        bool verbose;
        bool quiet;
        bool help;
        bool version;
        
        CommandLineOptions() : verbose(false), quiet(false), help(false), version(false) {}
    };
    
    static CommandLineOptions parseCommandLine(int argc, char** argv);
    
protected:
    // Virtual methods for customization
    virtual ApplicationResult onInitialization();
    virtual ApplicationResult onConfiguration();
    virtual ApplicationResult onBrainSetup();
    virtual ApplicationResult onTesting(std::shared_ptr<Brain> brain);
    virtual ApplicationResult onCleanup();
    virtual ApplicationResult onShutdown();
    
    virtual void onPhaseChanged(ApplicationPhase oldPhase, ApplicationPhase newPhase);
    virtual void onError(const std::string& context, const std::exception& e);
    virtual void onProgress(double progress, const std::string& task);
    
    // Helper methods
    void logStatus() const;
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    ApplicationStatus status_;
    bool continueOnError_;
    std::shared_ptr<TestFramework> testFramework_;
};

} // namespace nlm
