#pragma once

#include "TestFramework.hpp"
#include "TestFixtures.hpp"
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>

namespace nlm {

class IntegrationTestRunner {
public:
    enum class TestPhase {
        PREPARATION,
        EXECUTION,
        VALIDATION,
        CLEANUP
    };
    
    struct IntegrationTestContext {
        std::string testName;
        TestPhase phase;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::string status;
        std::string errorMessage;
        double executionTime;
        std::unordered_map<std::string, std::string> metadata;
        
        IntegrationTestContext(const std::string& name) 
            : testName(name), phase(TestPhase::PREPARATION), startTime(std::chrono::system_clock::now()),
              endTime(), executionTime(0.0) {}
    };
    
    IntegrationTestRunner();
    ~IntegrationTestRunner() = default;
    
    static IntegrationTestRunner& getInstance() {
        static IntegrationTestRunner instance;
        return instance;
    }
    
    void addIntegrationTest(std::unique_ptr<TestFramework::TestCase> testCase);
    void addIntegrationTestSuite(const std::string& suiteName, 
                                const std::vector<std::unique_ptr<TestFramework::TestCase>>& testCases);
    
    void setMaxConcurrency(int maxConcurrency) { maxConcurrency_ = maxConcurrency; }
    void setTestTimeout(double timeoutSeconds) { testTimeout_ = timeoutSeconds; }
    void enableComponent(const std::string& componentName);
    void disableComponent(const std::string& componentName);
    bool isComponentEnabled(const std::string& componentName) const;
    
    void runIntegrationTests(const std::string& category = "", const std::string& level = "");
    void runSequentialIntegrationTests();
    void runParallelIntegrationTests();
    
    const std::vector<IntegrationTestContext>& getTestContexts() const { return testContexts; }
    std::vector<IntegrationTestContext> getTestContextsByPhase(TestPhase phase) const;
    std::vector<IntegrationTestContext> getTestContextsByStatus(const std::string& status) const;
    
    void generateIntegrationReport(const std::string& title = "Integration Test Report");
    
    // Component-specific integration test factories
    template<typename T, typename... Args>
    std::unique_ptr<T> createIntegrationTest(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    // Performance measurement utilities
    static double measureExecutionTime(std::function<void()> func);
    static size_t measureMemoryUsage(std::function<void()> func);
    
    // Component state validation
    virtual bool validateComponentState(const std::string& componentName) const;
    virtual bool validateSystemConsistency() const;
    
protected:
    int maxConcurrency_;
    double testTimeout_;
    std::vector<std::unique_ptr<TestFramework::TestCase>> integrationTests;
    std::vector<IntegrationTestContext> testContexts;
    std::unordered_map<std::string, bool> enabledComponents;
    std::mutex contextsMutex;
    
    virtual void executeTestPhase(IntegrationTestContext& context, TestFramework::TestCase& testCase);
    virtual void validateTestResults(IntegrationTestContext& context);
    virtual void cleanupTestPhase(IntegrationTestContext& context);
    
    virtual std::string getComponentName(const TestFramework::TestCase& testCase) const;
};

// Multi-component integration test base class
class MultiComponentIntegrationTest : public TestFramework::TestCase {
public:
    MultiComponentIntegrationTest(const std::string& name, TestLevel level, TestCategory category);
    
    void addDependency(const std::string& componentName);
    void addDependency(const std::string& componentName, std::function<bool()> validator);
    
    const std::vector<std::string>& getDependencies() const { return dependencies; }
    
    bool validateDependencies() const;
    
protected:
    std::vector<std::string> dependencies;
    std::vector<std::pair<std::string, std::function<bool()>>> dependencyValidators;
    
    virtual bool setupComponents() = 0;
    virtual bool teardownComponents() = 0;
    
    void setup() override;
    void teardown() override;
};

// Brain memory integration test
class BrainMemoryIntegrationTest : public MultiComponentIntegrationTest {
public:
    BrainMemoryIntegrationTest(const std::string& name = "BrainMemoryIntegration");
    
    void run() override;
    
protected:
    bool setupComponents() override;
    bool teardownComponents() override;
    
    bool validateWorkingEpisodicIntegration() const;
    bool validateEpisodicAssociativeIntegration() const;
    bool validateMemorySystemConsistency() const;
};

// Brain plasticity integration test
class BrainPlasticityIntegrationTest : public MultiComponentIntegrationTest {
public:
    BrainPlasticityIntegrationTest(const std::string& name = "BrainPlasticityIntegration");
    
    void run() override;
    
protected:
    bool setupComponents() override;
    bool teardownComponents() override;
    
    bool validateSTDPIntegration() const;
    bool validateHebbianIntegration() const;
    bool validateStructuralIntegration() const;
    bool validatePlasticityHomeostasis() const;
};

// Brain neuromodulation integration test
class BrainNeuromodulationIntegrationTest : public MultiComponentIntegrationTest {
public:
    BrainNeuromodulationIntegrationTest(const std::string& name = "BrainNeuromodulationIntegration");
    
    void run() override;
    
protected:
    bool setupComponents() override;
    bool teardownComponents() override;
    
    bool validateDopamineIntegration() const;
    bool validateCuriosityIntegration() const;
    bool validateNoveltyIntegration() const;
    bool validatePredictionErrorIntegration() const;
};

// Brain cognition integration test
class BrainCognitionIntegrationTest : public MultiComponentIntegrationTest {
public:
    BrainCognitionIntegrationTest(const std::string& name = "BrainCognitionIntegration");
    
    void run() override;
    
protected:
    bool setupComponents() override;
    bool teardownComponents() override;
    
    bool validatePlannerIntegration() const;
    bool validateConceptFormationIntegration() const;
    bool validateAttentionIntegration() const;
    bool validateCognitiveCoherence() const;
};

// Cross-component stress test
class CrossComponentStressTest : public TestFramework::TestCase {
public:
    CrossComponentStressTest(const std::string& name = "CrossComponentStress");
    
    void run() override;
    
protected:
    bool setupComponents() override;
    bool teardownComponents() override;
    
    void simulateHighLoad() const;
    bool validateSystemStability() const;
    bool checkResourceLeaks() const;
};

// System recovery test
class SystemRecoveryTest : public TestFramework::TestCase {
public:
    SystemRecoveryTest(const std::string& name = "SystemRecovery");
    
    void run() override;
    
protected:
    bool setupComponents() override;
    bool teardownComponents() override;
    
    void simulateFailureAndRecovery() const;
    bool validateRecoveryIntegrity() const;
    bool validateDataPersistence() const;
};

} // namespace nlm
