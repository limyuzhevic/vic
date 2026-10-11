#pragma once

#include "TestFramework.hpp"
#include "TestFixtures.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <map>

namespace nlm {

class NLMComponentTestSuite {
public:
    enum class ComponentType {
        NEURAL_DYNAMICS,
        PLASTICITY,
        MEMORY_SYSTEM,
        PREDICTION,
        COGNITION,
        NEUROMODULATION,
        DEVELOPMENT,
        MULTI_AGENT,
        PERFORMANCE,
        MEMORY_LEAK,
        INTEGRATION,
        REGRESSION
    };
    
    struct ComponentTest {
        std::string name;
        ComponentType type;
        std::string description;
        TestFramework::TestLevel level;
        std::chrono::duration<double> timeout;
        std::chrono::duration<double> memoryLimit;
        std::vector<std::string> tags;
        bool enabled;
        std::map<std::string, std::string> parameters;
        
        ComponentTest(const std::string& n, ComponentType t, const std::string& desc)
            : name(n), type(t), description(desc), level(TestFramework::TestLevel::UNIT),
              timeout(10.0), memoryLimit(1024.0), enabled(true) {}
    };
    
    struct ComponentTestResult {
        std::string testName;
        ComponentType type;
        TestFramework::TestStatus status;
        std::chrono::duration<double> duration;
        std::chrono::duration<double> memoryUsage;
        std::string failureMessage;
        std::string errorMessage;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::map<std::string, std::string> metrics;
        std::map<std::string, std::string> parameters;
    };
    
    NLMComponentTestSuite();
    ~NLMComponentTestSuite() = default;
    
    static NLMComponentTestSuite& getInstance() {
        static NLMComponentTestSuite instance;
        return instance;
    }
    
    void addComponentTest(const ComponentTest& test);
    void removeComponentTest(const std::string& name);
    ComponentTest& getComponentTest(const std::string& name);
    
    void addTestFixture(const std::string& componentName, std::shared_ptr<TestFixture> fixture);
    std::shared_ptr<TestFixture> getTestFixture(const std::string& componentName);
    
    std::vector<ComponentTestResult> runComponentTests(const std::string& componentName = "", 
                                                      ComponentType type = ComponentType::NEURAL_DYNAMICS);
    std::vector<ComponentTestResult> runAllComponentTests();
    
    void generateComponentTestReport(const std::string& title = "NLM Component Test Report");
    
    // Component-specific test factories
    template<typename T, typename... Args>
    std::unique_ptr<T> createComponentTest(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    // Component state validation
    bool validateComponentReadiness(ComponentType component) const;
    bool validateComponentDependencies(ComponentType component) const;
    
    // Component performance measurement
    double measureComponentPerformance(ComponentType component, std::function<void()> benchmarkFunc) const;
    size_t measureComponentMemory(ComponentType component, std::function<void()> benchmarkFunc) const;
    
    // Component testing utilities
    std::string getComponentName(ComponentType component) const;
    std::string getComponentDescription(ComponentType component) const;
    
    // Component-specific testing
    std::vector<ComponentTestResult> runNeuralDynamicsTests();
    std::vector<ComponentTestResult> runPlasticityTests();
    std::vector<ComponentTestResult> runMemorySystemTests();
    std::vector<ComponentTestResult> runPredictionTests();
    std::vector<ComponentTestResult> runCognitionTests();
    std::vector<ComponentTestResult> runNeuromodulationTests();
    std::vector<ComponentTestResult> runDevelopmentTests();
    std::vector<ComponentTestResult> runMultiAgentTests();
    std::vector<ComponentTestResult> runPerformanceTests();
    std::vector<ComponentTestResult> runMemoryLeakTests();
    std::vector<ComponentTestResult> runIntegrationTests();
    std::vector<ComponentTestResult> runRegressionTests();
    
protected:
    std::mutex testsMutex;
    std::vector<ComponentTest> componentTests;
    std::map<std::string, std::shared_ptr<TestFixture>> componentFixtures;
    std::vector<ComponentTestResult> testResults;
    
    virtual void executeComponentTests(ComponentType component);
    virtual void validateComponentTestResults(ComponentTestResult& result);
    virtual void cleanupComponentTestResults(ComponentType component);
};

// Neural Dynamics Test Suite
class NeuralDynamicsTestSuite : public NLMComponentTestSuite {
public:
    NeuralDynamicsTestSuite();
    ~NeuralDynamicsTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createNeuralDynamicsTests() const;
    std::vector<ComponentTestResult> runNeuronTests() const;
    std::vector<ComponentTestResult> runSynapticTests() const;
    std::vector<ComponentTestResult> runNetworkTests() const;
    std::vector<ComponentTestResult> runDynamicsTests() const;
};

// Plasticity Test Suite
class PlasticityTestSuite : public NLMComponentTestSuite {
public:
    PlasticityTestSuite();
    ~PlasticityTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createPlasticityTests() const;
    std::vector<ComponentTestResult> runSTDPTests() const;
    std::vector<ComponentTestResult> runHebbianTests() const;
    std::vector<ComponentTestResult> runStructuralPlasticityTests() const;
    std::vector<ComponentTestResult> runPlasticityIntegrationTests() const;
};

// Memory System Test Suite
class MemorySystemTestSuite : public NLMComponentTestSuite {
public:
    MemorySystemTestSuite();
    ~MemorySystemTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createMemorySystemTests() const;
    std::vector<ComponentTestResult> runWorkingMemoryTests() const;
    std::vector<ComponentTestResult> runEpisodicMemoryTests() const;
    std::vector<ComponentTestResult> runAssociativeMemoryTests() const;
    std::vector<ComponentTestResult> runMemoryIntegrationTests() const;
};

// Prediction System Test Suite
class PredictionSystemTestSuite : public NLMComponentTestSuite {
public:
    PredictionSystemTestSuite();
    ~PredictionSystemTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createPredictionTests() const;
    std::vector<ComponentTestResult> runTemporalPredictionTests() const;
    std::vector<ComponentTestResult> runPatternPredictionTests() const;
    std::vector<ComponentTestResult> runPredictionAccuracyTests() const;
    std::vector<ComponentTestResult> runPredictionIntegrationTests() const;
};

// Cognition System Test Suite
class CognitionSystemTestSuite : public NLMComponentTestSuite {
public:
    CognitionSystemTestSuite();
    ~CognitionSystemTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createCognitionTests() const;
    std::vector<ComponentTestResult> runPlanningTests() const;
    std::vector<ComponentTestResult> runConceptFormationTests() const;
    std::vector<ComponentTestResult> runAttentionTests() const;
    std::vector<ComponentTestResult> runCognitionIntegrationTests() const;
};

// Neuromodulation Test Suite
class NeuromodulationTestSuite : public NLMComponentTestSuite {
public:
    NeuromodulationTestSuite();
    ~NeuromodulationTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createNeuromodulationTests() const;
    std::vector<ComponentTestResult> runDopamineTests() const;
    std::vector<ComponentTestResult> runCuriosityTests() const;
    std::vector<ComponentTestResult> runNoveltyTests() const;
    std::vector<ComponentTestResult> runPredictionErrorTests() const;
    std::vector<ComponentTestResult> runNeuromodulationIntegrationTests() const;
};

// Development Test Suite
class DevelopmentTestSuite : public NLMComponentTestSuite {
public:
    DevelopmentTestSuite();
    ~DevelopmentTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createDevelopmentTests() const;
    std::vector<ComponentTestResult> runSynaptogenesisTests() const;
    std::vector<ComponentTestResult> runMaturationTests() const;
    std::vector<ComponentTestResult> runPruningTests() const;
    std::vector<ComponentTestResult> runDevelopmentIntegrationTests() const;
};

// Multi-Agent Test Suite
class MultiAgentTestSuite : public NLMComponentTestSuite {
public:
    MultiAgentTestSuite();
    ~MultiAgentTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createMultiAgentTests() const;
    std::vector<ComponentTestResult> runAgentInteractionTests() const;
    std::vector<ComponentTestResult> runSocialBehaviorTests() const;
    std::vector<ComponentTestResult> runAgentCommunicationTests() const;
    std::vector<ComponentTestResult> runMultiAgentIntegrationTests() const;
};

// Performance Test Suite
class PerformanceTestSuite : public NLMComponentTestSuite {
public:
    PerformanceTestSuite();
    ~PerformanceTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createPerformanceTests() const;
    std::vector<ComponentTestResult> runUnitPerformanceTests() const;
    std::vector<ComponentTestResult> runIntegrationPerformanceTests() const;
    std::vector<ComponentTestResult> runStressPerformanceTests() const;
    std::vector<ComponentTestResult> runScalabilityTests() const;
};

// Memory Leak Test Suite
class MemoryLeakTestSuite : public NLMComponentTestSuite {
public:
    MemoryLeakTestSuite();
    ~MemoryLeakTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createMemoryLeakTests() const;
    std::vector<ComponentTestResult> runMemoryAllocationTests() const;
    std::vector<ComponentTestResult> runMemoryDeallocationTests() const;
    std::vector<ComponentTestResult> runMemoryLeakDetectionTests() const;
    std::vector<ComponentTestResult> runMemoryCleanupTests() const;
};

// Integration Test Suite
class IntegrationTestSuite : public NLMComponentTestSuite {
public:
    IntegrationTestSuite();
    ~IntegrationTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createIntegrationTests() const;
    std::vector<ComponentTestResult> runBrainMemoryIntegrationTests() const;
    std::vector<ComponentTestResult> runBrainPlasticityIntegrationTests() const;
    std::vector<ComponentTestResult> runBrainNeuromodulationIntegrationTests() const;
    std::vector<ComponentTestResult> runBrainCognitionIntegrationTests() const;
    std::vector<ComponentTestResult> runCrossSystemIntegrationTests() const;
};

// Regression Test Suite
class RegressionTestSuite : public NLMComponentTestSuite {
public:
    RegressionTestSuite();
    ~RegressionTestSuite() override;
    
    std::vector<ComponentTestResult> runAllTests() override;
    
protected:
    std::vector<ComponentTest> createRegressionTests() const;
    std::vector<ComponentTestResult> runFunctionalRegressionTests() const;
    std::vector<ComponentTestResult> runPerformanceRegressionTests() const;
    std::vector<ComponentTestResult> runMemoryRegressionTests() const;
    std::vector<ComponentTestResult> runRegressionComparisonTests() const;
};

} // namespace nlm
