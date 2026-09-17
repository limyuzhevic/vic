#include "ExperimentInterfaces.hpp"
#include "Phase6ConfigManager.hpp"
#include "ExperimentFactory.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <thread>
#include <algorithm>

namespace nlm {

// Phase 6 experiment validator implementation
class Phase6ExperimentValidator : public IExperimentValidator {
public:
    Phase6ExperimentValidator(std::shared_ptr<Phase6ConfigManager> configManager);
    ~Phase6ExperimentValidator() override;
    
    bool validateConfiguration(const std::string& configName) const override;
    bool validateSetup(const std::string& configName) const override;
    std::vector<std::string> getValidationErrors() const override;
    std::vector<std::string> getValidationWarnings() const override;
    
    // Validation rules
    bool validateNeuronCount(size_t count) const;
    bool validateMaxSteps(uint64_t steps) const;
    bool validateWorldDimensions(float width, float height) const;
    bool validateConnectionProbability(float probability) const;
    
    // Set validation rules
    void setStrictValidation(bool strict) { strictValidation_ = strict; }
    bool getStrictValidation() const { return strictValidation_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Phase 6 experiment runner implementation
class Phase6ExperimentRunner : public IExperimentRunner {
public:
    Phase6ExperimentRunner(std::shared_ptr<Phase6ConfigManager> configManager);
    ~Phase6ExperimentRunner() override;
    
    bool run(const std::string& configName) override;
    std::vector<double> getResults() const override;
    std::vector<std::string> getMetrics() const override;
    std::string getStatus() const override;
    bool isComplete() const override;
    void stop() override;
    
    // Configuration management
    bool setConfiguration(const std::string& configName);
    
    // Result management
    void addResult(const std::string& metric, double value);
    void setStatus(const std::string& status);
    
    // Execution control
    void setExecutionInterval(double interval) { executionInterval_ = interval; }
    double getExecutionInterval() const { return executionInterval_; }
    
    // Validation integration
    void setValidator(std::shared_ptr<IExperimentValidator> validator);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool strictExecution_;
    double executionInterval_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point endTime_;
};

// Validator registry
class ExperimentValidatorRegistry {
public:
    ExperimentValidatorRegistry();
    ~ExperimentValidatorRegistry() = default;
    
    // Disable copying
    ExperimentValidatorRegistry(const ExperimentValidatorRegistry&) = delete;
    ExperimentValidatorRegistry& operator=(const ExperimentValidatorRegistry&) = delete;
    
    // Singleton access
    static ExperimentValidatorRegistry& getInstance();
    
    // Register validator
    void registerValidator(const std::string& name, std::unique_ptr<IExperimentValidator> validator);
    
    // Get validator
    std::shared_ptr<IExperimentValidator> getValidator(const std::string& name);
    
    // Get all registered validators
    std::vector<std::string> getRegisteredValidators() const;
    
    // Check if validator is registered
    bool hasValidator(const std::string& name) const;
    
    // Remove validator
    void unregisterValidator(const std::string& name);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Runner registry
class ExperimentRunnerRegistry {
public:
    ExperimentRunnerRegistry();
    ~ExperimentRunnerRegistry() = default;
    
    // Disable copying
    ExperimentRunnerRegistry(const ExperimentRunnerRegistry&) = delete;
    ExperimentRunnerRegistry& operator=(const ExperimentRunnerRegistry&) = delete;
    
    // Singleton access
    static ExperimentRunnerRegistry& getInstance();
    
    // Register runner
    void registerRunner(const std::string& name, std::unique_ptr<IExperimentRunner> runner);
    
    // Get runner
    std::shared_ptr<IExperimentRunner> getRunner(const std::string& name);
    
    // Get all registered runners
    std::vector<std::string> getRegisteredRunners() const;
    
    // Check if runner is registered
    bool hasRunner(const std::string& name) const;
    
    // Remove runner
    void unregisterRunner(const std::string& name);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
