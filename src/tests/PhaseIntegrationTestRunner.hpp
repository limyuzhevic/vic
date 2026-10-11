#pragma once

#include "TestFramework.hpp"
#include "TestFixtures.hpp"
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <map>

namespace nlm {

class PhaseIntegrationTestRunner {
public:
    enum class NLMPhase {
        PHASE_1,           // Neural Development & Architecture
        PHASE_2,           // Real Neural Computation Tests
        PHASE_3,           // Advanced Learning & Plasticity
        PHASE_4,           // Memory System Integration
        PHASE_5,           // Prediction & Planning
        PHASE_6,           // Advanced Cognition & Social Behavior
        PHASE_7,           // Production & Deployment
        VALIDATION,        // System Validation
        BENCHMARK,         // Performance Benchmarking
        ANALYSIS          // Data Analysis & Optimization
    };
    
    struct PhaseTestContext {
        std::string testName;
        NLMPhase phase;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::string status;
        std::string errorMessage;
        double executionTime;
        double memoryUsage;
        double cpuUsage;
        std::map<std::string, std::string> metrics;
        std::map<std::string, std::string> componentResults;
        
        PhaseTestContext(const std::string& name, NLMPhase p)
            : testName(name), phase(p), startTime(std::chrono::system_clock::now()),
              endTime(), executionTime(0.0), memoryUsage(0.0), cpuUsage(0.0) {}
    };
    
    PhaseIntegrationTestRunner();
    ~PhaseIntegrationTestRunner() = default;
    
    static PhaseIntegrationTestRunner& getInstance() {
        static PhaseIntegrationTestRunner instance;
        return instance;
    }
    
    void setPhaseOrder(const std::vector<NLMPhase>& phaseOrder);
    void enablePhase(NLMPhase phase);
    void disablePhase(NLMPhase phase);
    bool isPhaseEnabled(NLMPhase phase) const;
    
    void addPhaseTest(NLMPhase phase, std::unique_ptr<TestFramework::TestCase> testCase);
    void addPhaseTestSuite(NLMPhase phase, const std::string& suiteName,
                          const std::vector<std::unique_ptr<TestFramework::TestCase>>& testCases);
    
    void runPhase(NLMPhase phase);
    void runAllPhases();
    void runPhaseRange(NLMPhase startPhase, NLMPhase endPhase);
    
    const std::vector<PhaseTestContext>& getPhaseResults() const { return phaseResults; }
    std::vector<PhaseTestContext> getPhaseResultsByPhase(NLMPhase phase) const;
    std::vector<PhaseTestContext> getPhaseResultsByStatus(const std::string& status) const;
    
    std::string generatePhaseReport(NLMPhase phase) const;
    std::string generateOverallPhaseReport() const;
    std::string generatePhaseSummaryReport() const;
    
    // Phase-specific test factories
    template<typename T, typename... Args>
    std::unique_ptr<T> createPhaseTest(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    // Phase progression and state management
    bool hasPhaseCompleted(NLMPhase phase) const;
    bool hasAllPhasesCompleted() const;
    void markPhaseCompleted(NLMPhase phase);
    NLMPhase getCurrentPhase() const { return currentPhase; }
    
    // Component state validation
    bool validateComponentReadiness(NLMPhase phase) const;
    bool validateSystemState(NLMPhase phase) const;
    
    // Data collection and analysis
    std::map<std::string, double> collectPhaseMetrics(NLMPhase phase) const;
    std::string analyzePhaseResults(NLMPhase phase) const;
    std::string analyzeCrossPhaseTrends() const;
    
protected:
    int maxConcurrency;
    double testTimeoutSeconds;
    std::vector<NLMPhase> enabledPhases;
    std::map<NLMPhase, std::vector<std::unique_ptr<TestFramework::TestCase>>> phaseTests;
    std::vector<PhaseTestContext> phaseResults;
    std::map<NLMPhase, bool> phaseCompleted;
    std::mutex resultsMutex;
    
    NLMPhase currentPhase;
    
    virtual void executePhaseTests(NLMPhase phase);
    virtual void validatePhaseResults(NLMPhase phase, PhaseTestContext& context);
    virtual void cleanupPhaseResults(NLMPhase phase);
    
    virtual std::string getPhaseName(NLMPhase phase) const;
    virtual bool validatePhasePreconditions(NLMPhase phase) const;
    virtual bool validatePhasePostconditions(NLMPhase phase) const;
};

// Phase-specific test base classes
class Phase1TestBase : public TestFramework::TestCase {
public:
    Phase1TestBase(const std::string& name);
    
    virtual bool validateNeuralArchitecture() const = 0;
    virtual bool validateBasicFunctionality() const = 0;
    virtual bool validateSystemStability() const = 0;
    
protected:
    bool setupPhase1Components() const;
    bool teardownPhase1Components() const;
    virtual void runPhase1SpecificTests() = 0;
    
    void setup() override;
    void teardown() override;
    void run() override;
};

class Phase2TestBase : public TestFramework::TestCase {
public:
    Phase2TestBase(const std::string& name);
    
    virtual bool validateNeuralComputation() const = 0;
    virtual bool validatePlasticityMechanisms() const = 0;
    virtual bool validateMemoryFormation() const = 0;
    
protected:
    bool setupPhase2Components() const;
    bool teardownPhase2Components() const;
    virtual void runPhase2SpecificTests() = 0;
    
    void setup() override;
    void teardown() override;
    void run() override;
};

class Phase3TestBase : public TestFramework::TestCase {
public:
    Phase3TestBase(const std::string& name);
    
    virtual bool validateAdvancedLearning() const = 0;
    virtual bool validateMultiTaskLearning() const = 0;
    virtual bool validateTransferLearning() const = 0;
    
protected:
    bool setupPhase3Components() const;
    bool teardownPhase3Components() const;
    virtual void runPhase3SpecificTests() = 0;
    
    void setup() override;
    void teardown() override;
    void run() override;
};

class Phase4TestBase : public TestFramework::TestCase {
public:
    Phase4TestBase(const std::string& name);
    
    virtual bool validateWorkingMemory() const = 0;
    virtual bool validateEpisodicMemory() const = 0;
    virtual bool validateAssociativeMemory() const = 0;
    
protected:
    bool setupPhase4Components() const;
    bool teardownPhase4Components() const;
    virtual void runPhase4SpecificTests() = 0;
    
    void setup() override;
    void teardown() override;
    void run() override;
};

class Phase5TestBase : public TestFramework::TestCase {
public:
    Phase5TestBase(const std::string& name);
    
    virtual bool validatePredictionSystems() const = 0;
    virtual bool validatePlanningSystems() const = 0;
    virtual bool validateTemporalProcessing() const = 0;
    
protected:
    bool setupPhase5Components() const;
    bool teardownPhase5Components() const;
    virtual void runPhase5SpecificTests() = 0;
    
    void setup() override;
    void teardown() override;
    void run() override;
};

class Phase6TestBase : public TestFramework::TestCase {
public:
    Phase6TestBase(const std::string& name);
    
    virtual bool validateSocialBehavior() const = 0;
    virtual bool validateAdvancedCognition() const = 0;
    virtual bool validateConsciousness() const = 0;
    
protected:
    bool setupPhase6Components() const;
    bool teardownPhase6Components() const;
    virtual void runPhase6SpecificTests() = 0;
    
    void setup() override;
    void teardown() override;
    void run() override;
};

class Phase7TestBase : public TestFramework::TestCase {
public:
    Phase7TestBase(const std::string& name);
    
    virtual bool validateProductionReadiness() const = 0;
    virtual bool validateScalability() const = 0;
    virtual bool validateSafety() const = 0;
    
protected:
    bool setupPhase7Components() const;
    bool teardownPhase7Components() const;
    virtual void runPhase7SpecificTests() = 0;
    
    void setup() override;
    void teardown() override;
    void run() override;
};

class ValidationTestBase : public TestFramework::TestCase {
public:
    ValidationTestBase(const std::string& name);
    
    virtual bool validateSystemIntegrity() const = 0;
    virtual bool validatePerformanceTargets() const = 0;
    virtual bool validateQualityMetrics() const = 0;
    
protected:
    bool setupValidationComponents() const;
    bool teardownValidationComponents() const;
    virtual void runValidationSpecificTests() = 0;
    
    void setup() override;
    void teardown() override;
    void run() override;
};

// Specific phase test implementations
class Phase1IntegrationTest : public Phase1TestBase {
public:
    Phase1IntegrationTest();
    
    bool validateNeuralArchitecture() const override;
    bool validateBasicFunctionality() const override;
    bool validateSystemStability() const override;
    
protected:
    void runPhase1SpecificTests() override;
};

class Phase2IntegrationTest : public Phase2TestBase {
public:
    Phase2IntegrationTest();
    
    bool validateNeuralComputation() const override;
    bool validatePlasticityMechanisms() const override;
    bool validateMemoryFormation() const override;
    
protected:
    void runPhase2SpecificTests() override;
};

class Phase3IntegrationTest : public Phase3TestBase {
public:
    Phase3IntegrationTest();
    
    bool validateAdvancedLearning() const override;
    bool validateMultiTaskLearning() const override;
    bool validateTransferLearning() const override;
    
protected:
    void runPhase3SpecificTests() override;
};

class Phase4IntegrationTest : public Phase4TestBase {
public:
    Phase4IntegrationTest();
    
    bool validateWorkingMemory() const override;
    bool validateEpisodicMemory() const override;
    bool validateAssociativeMemory() const override;
    
protected:
    void runPhase4SpecificTests() override;
};

class Phase5IntegrationTest : public Phase5TestBase {
public:
    Phase5IntegrationTest();
    
    bool validatePredictionSystems() const override;
    bool validatePlanningSystems() const override;
    bool validateTemporalProcessing() const override;
    
protected:
    void runPhase5SpecificTests() override;
};

class Phase6IntegrationTest : public Phase6TestBase {
public:
    Phase6IntegrationTest();
    
    bool validateSocialBehavior() const override;
    bool validateAdvancedCognition() const override;
    bool validateConsciousness() const override;
    
protected:
    void runPhase6SpecificTests() override;
};

class Phase7IntegrationTest : public Phase7TestBase {
public:
    Phase7IntegrationTest();
    
    bool validateProductionReadiness() const override;
    bool validateScalability() const override;
    bool validateSafety() const override;
    
protected:
    void runPhase7SpecificTests() override;
};

class ValidationIntegrationTest : public ValidationTestBase {
public:
    ValidationIntegrationTest();
    
    bool validateSystemIntegrity() const override;
    bool validatePerformanceTargets() const override;
    bool validateQualityMetrics() const override;
    
protected:
    void runValidationSpecificTests() override;
};

// Cross-phase integration test
class CrossPhaseIntegrationTest : public TestFramework::TestCase {
public:
    CrossPhaseIntegrationTest();
    
    bool validatePhaseProgression() const;
    bool validateComponentTransferability() const;
    bool validateKnowledgeRetention() const;
    
protected:
    bool setupCrossPhaseComponents() const;
    bool teardownCrossPhaseComponents() const;
    void run() override;
};

} // namespace nlm
