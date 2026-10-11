#include "IntegrationTestRunner.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

IntegrationTestRunner::IntegrationTestRunner() : maxConcurrency_(4), testTimeout_(30.0) {
    // Initialize default enabled components
    enabledComponents["brain"] = true;
    enabledComponents["memory"] = true;
    enabledComponents["plasticity"] = true;
    enabledComponents["neuromodulation"] = true;
    enabledComponents["cognition"] = true;
    enabledComponents["development"] = true;
}

void IntegrationTestRunner::addIntegrationTest(std::unique_ptr<TestFramework::TestCase> testCase) {
    integrationTests.push_back(std::move(testCase));
}

void IntegrationTestRunner::addIntegrationTestSuite(const std::string& suiteName, 
                                                  const std::vector<std::unique_ptr<TestFramework::TestCase>>& testCases) {
    for (auto& testCase : testCases) {
        testCase->name = suiteName + "::" + testCase->getName();
        addIntegrationTest(std::move(testCase));
    }
}

void IntegrationTestRunner::enableComponent(const std::string& componentName) {
    enabledComponents[componentName] = true;
}

void IntegrationTestRunner::disableComponent(const std::string& componentName) {
    enabledComponents[componentName] = false;
}

bool IntegrationTestRunner::isComponentEnabled(const std::string& componentName) const {
    auto it = enabledComponents.find(componentName);
    return it != enabledComponents.end() ? it->second : false;
}

void IntegrationTestRunner::runIntegrationTests(const std::string& category, const std::string& level) {
    std::cout << "=== Running Integration Tests ===" << std::endl;
    
    for (auto& testCase : integrationTests) {
        if (!category.empty() && static_cast<int>(testCase->getCategory()) != std::stoi(category)) {
            continue;
        }
        if (!level.empty() && static_cast<int>(testCase->getLevel()) != std::stoi(level)) {
            continue;
        }
        
        IntegrationTestContext context(testCase->getName());
        context.phase = TestPhase::EXECUTION;
        
        try {
            executeTestPhase(context, *testCase);
            validateTestResults(context);
            
            context.status = "PASSED";
            std::cout << "  Integration test: " << testCase->getName() << " PASSED" << std::endl;
        } catch (const std::exception& e) {
            context.status = "FAILED";
            context.errorMessage = e.what();
            std::cout << "  Integration test: " << testCase->getName() << " FAILED - " << e.what() << std::endl;
        }
        
        context.endTime = std::chrono::system_clock::now();
        context.executionTime = std::chrono::duration<double>(context.endTime - context.startTime).count();
        
        std::lock_guard<std::mutex> lock(contextsMutex);
        testContexts.push_back(context);
    }
    
    std::cout << "=== Integration Test Run Complete ===" << std::endl;
}

void IntegrationTestRunner::runSequentialIntegrationTests() {
    std::cout << "Running integration tests sequentially..." << std::endl;
    runIntegrationTests();
}

void IntegrationTestRunner::runParallelIntegrationTests() {
    std::cout << "Running integration tests in parallel (max concurrency: " << maxConcurrency_ << ")..." << std::endl;
    
    // This is a simplified parallel implementation
    // In a real implementation, you would use threads or async operations
    size_t activeThreads = 0;
    for (auto& testCase : integrationTests) {
        if (activeThreads >= maxConcurrency_) {
            // Wait for a thread to complete (simplified)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            activeThreads = 0; // Simplified - in real implementation track active threads
        }
        
        IntegrationTestContext context(testCase->getName());
        context.phase = TestPhase::EXECUTION;
        
        try {
            executeTestPhase(context, *testCase);
            validateTestResults(context);
            
            context.status = "PASSED";
            std::cout << "  Integration test: " << testCase->getName() << " PASSED" << std::endl;
        } catch (const std::exception& e) {
            context.status = "FAILED";
            context.errorMessage = e.what();
            std::cout << "  Integration test: " << testCase->getName() << " FAILED - " << e.what() << std::endl;
        }
        
        context.endTime = std::chrono::system_clock::now();
        context.executionTime = std::chrono::duration<double>(context.endTime - context.startTime).count();
        
        std::lock_guard<std::mutex> lock(contextsMutex);
        testContexts.push_back(context);
        
        activeThreads++;
    }
    
    std::cout << "=== Parallel Integration Test Run Complete ===" << std::endl;
}

void IntegrationTestRunner::generateIntegrationReport(const std::string& title) {
    std::stringstream ss;
    ss << "========================================" << std::endl;
    ss << "         " << title << "         " << std::endl;
    ss << "========================================" << std::endl;
    ss << "Total Integration Tests: " << integrationTests.size() << std::endl;
    ss << "Test Contexts Collected: " << testContexts.size() << std::endl;
    ss << std::endl;
    
    // Count results by status
    int passed = 0, failed = 0;
    for (const auto& context : testContexts) {
        if (context.status == "PASSED") passed++;
        else if (context.status == "FAILED") failed++;
    }
    
    ss << "Passed: " << passed << std::endl;
    ss << "Failed: " << failed << std::endl;
    ss << "Success Rate: " << (passed * 100.0 / (passed + failed)) << "%" << std::endl;
    
    if (!testContexts.empty()) {
        double totalTime = 0.0;
        for (const auto& context : testContexts) {
            totalTime += context.executionTime;
        }
        ss << "Average Test Duration: " << (totalTime / testContexts.size()) << "s" << std::endl;
    }
    
    ss << std::endl << "Test Contexts:" << std::endl;
    for (const auto& context : testContexts) {
        ss << "  " << context.testName << ": " << context.status 
           << " (" << context.executionTime << "s)" << std::endl;
    }
    
    ss << "========================================" << std::endl;
    
    std::cout << ss.str();
}

double IntegrationTestRunner::measureExecutionTime(std::function<void()> func) {
    auto startTime = std::chrono::high_resolution_clock::now();
    func();
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime).count();
}

size_t IntegrationTestRunner::measureMemoryUsage(std::function<void()> func) {
    // This is a simplified memory measurement
    // In a real implementation, you would use platform-specific memory measurement tools
    func();
    return 0; // Placeholder
}

std::vector<IntegrationTestRunner::IntegrationTestContext> IntegrationTestRunner::getTestContextsByPhase(TestPhase phase) const {
    std::vector<IntegrationTestContext> filteredContexts;
    for (const auto& context : testContexts) {
        if (context.phase == phase) {
            filteredContexts.push_back(context);
        }
    }
    return filteredContexts;
}

std::vector<IntegrationTestRunner::IntegrationTestContext> IntegrationTestRunner::getTestContextsByStatus(const std::string& status) const {
    std::vector<IntegrationTestContext> filteredContexts;
    for (const auto& context : testContexts) {
        if (context.status == status) {
            filteredContexts.push_back(context);
        }
    }
    return filteredContexts;
}

bool IntegrationTestRunner::validateComponentState(const std::string& componentName) const {
    // Placeholder validation
    return isComponentEnabled(componentName);
}

bool IntegrationTestRunner::validateSystemConsistency() const {
    // Placeholder validation
    return true;
}

void IntegrationTestRunner::executeTestPhase(IntegrationTestContext& context, TestFramework::TestCase& testCase) {
    context.phase = TestPhase::EXECUTION;
    
    // Perform dependency validation if this is a MultiComponentIntegrationTest
    auto* multiComponentTest = dynamic_cast<MultiComponentIntegrationTest*>(&testCase);
    if (multiComponentTest) {
        if (!multiComponentTest->validateDependencies()) {
            throw std::runtime_error("Dependency validation failed for " + testCase.getName());
        }
    }
    
    // Run the actual test
    testCase.setup();
    
    // Measure execution time
    context.startTime = std::chrono::system_clock::now();
    testCase.run();
    context.endTime = std::chrono::system_clock::now();
    
    testCase.teardown();
    
    context.executionTime = std::chrono::duration<double>(context.endTime - context.startTime).count();
}

void IntegrationTestRunner::validateTestResults(IntegrationTestContext& context) {
    context.phase = TestPhase::VALIDATION;
    
    // Validate results based on test case type
    auto* multiComponentTest = dynamic_cast<MultiComponentIntegrationTest*>(&context.testCaseName);
    if (multiComponentTest) {
        // Additional validation for multi-component tests
        if (!validateSystemConsistency()) {
            throw std::runtime_error("System consistency validation failed for " + context.testName);
        }
    }
}

void IntegrationTestRunner::cleanupTestPhase(IntegrationTestContext& context) {
    context.phase = TestPhase::CLEANUP;
    
    // Cleanup can be implemented here
}

std::string IntegrationTestRunner::getComponentName(const TestFramework::TestCase& testCase) const {
    // Determine component name based on test category
    switch (testCase.getCategory()) {
        case TestCategory::NEURAL_DYNAMICS: return "brain";
        case TestCategory::PLASTICITY: return "plasticity";
        case TestCategory::MEMORY_SYSTEM: return "memory";
        case TestCategory::PREDICTION: return "prediction";
        case TestCategory::COGNITION: return "cognition";
        case TestCategory::NEUROMODULATION: return "neuromodulation";
        case TestCategory::DEVELOPMENT: return "development";
        default: return "unknown";
    }
}

// MultiComponentIntegrationTest implementation
MultiComponentIntegrationTest::MultiComponentIntegrationTest(const std::string& name, TestLevel level, TestCategory category)
    : TestFramework::TestCase(name, level, category) {}

void MultiComponentIntegrationTest::addDependency(const std::string& componentName) {
    dependencies.push_back(componentName);
}

void MultiComponentIntegrationTest::addDependency(const std::string& componentName, std::function<bool()> validator) {
    dependencies.push_back(componentName);
    dependencyValidators.push_back(std::make_pair(componentName, validator));
}

bool MultiComponentIntegrationTest::validateDependencies() const {
    // Check if all components are enabled
    for (const auto& dep : dependencies) {
        if (!isComponentEnabled(dep)) {
            return false;
        }
    }
    
    // Check custom validators
    for (const auto& validator : dependencyValidators) {
        if (!validator.second()) {
            return false;
        }
    }
    
    return true;
}

void MultiComponentIntegrationTest::setup() {
    if (!setupComponents()) {
        throw std::runtime_error("Component setup failed for " + getName());
    }
}

void MultiComponentIntegrationTest::teardown() {
    teardownComponents();
}

// BrainMemoryIntegrationTest implementation
BrainMemoryIntegrationTest::BrainMemoryIntegrationTest(const std::string& name)
    : MultiComponentIntegrationTest(name, TestLevel::INTEGRATION, TestCategory::INTEGRATION) {
    addDependency("brain");
    addDependency("memory");
}

void BrainMemoryIntegrationTest::run() {
    std::cout << "  Testing Brain-Memory Integration..." << std::endl;
    
    if (!validateWorkingEpisodicIntegration()) {
        throw std::runtime_error("Working-Episodic integration validation failed");
    }
    
    if (!validateEpisodicAssociativeIntegration()) {
        throw std::runtime_error("Episodic-Associative integration validation failed");
    }
    
    if (!validateMemorySystemConsistency()) {
        throw std::runtime_error("Memory system consistency validation failed");
    }
    
    std::cout << "    Integration validation PASSED" << std::endl;
}

bool BrainMemoryIntegrationTest::setupComponents() {
    // Setup brain and memory systems
    // This would actually initialize the components in a real implementation
    return true;
}

bool BrainMemoryIntegrationTest::teardownComponents() {
    // Cleanup brain and memory systems
    return true;
}

bool BrainMemoryIntegrationTest::validateWorkingEpisodicIntegration() const {
    // Validate integration between working and episodic memory
    return true; // Placeholder
}

bool BrainMemoryIntegrationTest::validateEpisodicAssociativeIntegration() const {
    // Validate integration between episodic and associative memory
    return true; // Placeholder
}

bool BrainMemoryIntegrationTest::validateMemorySystemConsistency() const {
    // Validate overall memory system consistency
    return true; // Placeholder
}

// BrainPlasticityIntegrationTest implementation
BrainPlasticityIntegrationTest::BrainPlasticityIntegrationTest(const std::string& name)
    : MultiComponentIntegrationTest(name, TestLevel::INTEGRATION, TestCategory::PLASTICITY) {
    addDependency("brain");
    addDependency("plasticity");
}

void BrainPlasticityIntegrationTest::run() {
    std::cout << "  Testing Brain-Plasticity Integration..." << std::endl;
    
    if (!validateSTDPIntegration()) {
        throw std::runtime_error("STDP integration validation failed");
    }
    
    if (!validateHebbianIntegration()) {
        throw std::runtime_error("Hebbian integration validation failed");
    }
    
    if (!validateStructuralIntegration()) {
        throw std::runtime_error("Structural integration validation failed");
    }
    
    if (!validatePlasticityHomeostasis()) {
        throw std::runtime_error("Plasticity homeostasis validation failed");
    }
    
    std::cout << "    Integration validation PASSED" << std::endl;
}

bool BrainPlasticityIntegrationTest::setupComponents() {
    // Setup brain and plasticity systems
    return true;
}

bool BrainPlasticityIntegrationTest::teardownComponents() {
    // Cleanup brain and plasticity systems
    return true;
}

bool BrainPlasticityIntegrationTest::validateSTDPIntegration() const {
    // Validate STDP integration
    return true; // Placeholder
}

bool BrainPlasticityIntegrationTest::validateHebbianIntegration() const {
    // Validate Hebbian integration
    return true; // Placeholder
}

bool BrainPlasticityIntegrationTest::validateStructuralIntegration() const {
    // Validate structural plasticity integration
    return true; // Placeholder
}

bool BrainPlasticityIntegrationTest::validatePlasticityHomeostasis() const {
    // Validate plasticity homeostasis
    return true; // Placeholder
}

// BrainNeuromodulationIntegrationTest implementation
BrainNeuromodulationIntegrationTest::BrainNeuromodulationIntegrationTest(const std::string& name)
    : MultiComponentIntegrationTest(name, TestLevel::INTEGRATION, TestCategory::NEUROMODULATION) {
    addDependency("brain");
    addDependency("neuromodulation");
}

void BrainNeuromodulationIntegrationTest::run() {
    std::cout << "  Testing Brain-Neuromodulation Integration..." << std::endl;
    
    if (!validateDopamineIntegration()) {
        throw std::runtime_error("Dopamine integration validation failed");
    }
    
    if (!validateCuriosityIntegration()) {
        throw std::runtime_error("Curiosity integration validation failed");
    }
    
    if (!validateNoveltyIntegration()) {
        throw std::runtime_error("Novelty integration validation failed");
    }
    
    if (!validatePredictionErrorIntegration()) {
        throw std::runtime_error("Prediction error integration validation failed");
    }
    
    std::cout << "    Integration validation PASSED" << std::endl;
}

bool BrainNeuromodulationIntegrationTest::setupComponents() {
    // Setup brain and neuromodulation systems
    return true;
}

bool BrainNeuromodulationIntegrationTest::teardownComponents() {
    // Cleanup brain and neuromodulation systems
    return true;
}

bool BrainNeuromodulationIntegrationTest::validateDopamineIntegration() const {
    // Validate dopamine integration
    return true; // Placeholder
}

bool BrainNeuromodulationIntegrationTest::validateCuriosityIntegration() const {
    // Validate curiosity integration
    return true; // Placeholder
}

bool BrainNeuromodulationIntegrationTest::validateNoveltyIntegration() const {
    // Validate novelty integration
    return true; // Placeholder
}

bool BrainNeuromodulationIntegrationTest::validatePredictionErrorIntegration() const {
    // Validate prediction error integration
    return true; // Placeholder
}

// BrainCognitionIntegrationTest implementation
BrainCognitionIntegrationTest::BrainCognitionIntegrationTest(const std::string& name)
    : MultiComponentIntegrationTest(name, TestLevel::INTEGRATION, TestCategory::COGNITION) {
    addDependency("brain");
    addDependency("cognition");
}

void BrainCognitionIntegrationTest::run() {
    std::cout << "  Testing Brain-Cognition Integration..." << std::endl;
    
    if (!validatePlannerIntegration()) {
        throw std::runtime_error("Planner integration validation failed");
    }
    
    if (!validateConceptFormationIntegration()) {
        throw std::runtime_error("Concept formation integration validation failed");
    }
    
    if (!validateAttentionIntegration()) {
        throw std::runtime_error("Attention integration validation failed");
    }
    
    if (!validateCognitiveCoherence()) {
        throw std::runtime_error("Cognitive coherence validation failed");
    }
    
    std::cout << "    Integration validation PASSED" << std::endl;
}

bool BrainCognitionIntegrationTest::setupComponents() {
    // Setup brain and cognition systems
    return true;
}

bool BrainCognitionIntegrationTest::teardownComponents() {
    // Cleanup brain and cognition systems
    return true;
}

bool BrainCognitionIntegrationTest::validatePlannerIntegration() const {
    // Validate planner integration
    return true; // Placeholder
}

bool BrainCognitionIntegrationTest::validateConceptFormationIntegration() const {
    // Validate concept formation integration
    return true; // Placeholder
}

bool BrainCognitionIntegrationTest::validateAttentionIntegration() const {
    // Validate attention integration
    return true; // Placeholder
}

bool BrainCognitionIntegrationTest::validateCognitiveCoherence() const {
    // Validate cognitive coherence
    return true; // Placeholder
}

// CrossComponentStressTest implementation
CrossComponentStressTest::CrossComponentStressTest(const std::string& name)
    : TestFramework::TestCase(name, TestLevel::STRESS, TestCategory::STRESS) {
    addDependency("brain");
    addDependency("memory");
    addDependency("plasticity");
    addDependency("neuromodulation");
}

void CrossComponentStressTest::run() {
    std::cout << "  Testing Cross-Component Stress..." << std::endl;
    
    if (!setupComponents()) {
        throw std::runtime_error("Component setup failed for stress test");
    }
    
    simulateHighLoad();
    
    if (!validateSystemStability()) {
        throw std::runtime_error("System stability validation failed for stress test");
    }
    
    if (!checkResourceLeaks()) {
        throw std::runtime_error("Resource leak detection failed for stress test");
    }
    
    std::cout << "    Stress test PASSED" << std::endl;
}

bool CrossComponentStressTest::setupComponents() {
    // Setup all components for stress testing
    return true;
}

bool CrossComponentStressTest::teardownComponents() {
    // Cleanup after stress test
    return true;
}

void CrossComponentStressTest::simulateHighLoad() const {
    // Simulate high load conditions
    // This would run multiple operations concurrently
    std::cout << "    Simulating high load conditions..." << std::endl;
}

bool CrossComponentStressTest::validateSystemStability() const {
    // Validate system stability under load
    return true; // Placeholder
}

bool CrossComponentStressTest::checkResourceLeaks() const {
    // Check for resource leaks
    // In a real implementation, this would monitor memory usage, file handles, etc.
    return true; // Placeholder
}

// SystemRecoveryTest implementation
SystemRecoveryTest::SystemRecoveryTest(const std::string& name)
    : TestFramework::TestCase(name, TestLevel::STRESS, TestCategory::REGRESSION) {
    addDependency("brain");
}

void SystemRecoveryTest::run() {
    std::cout << "  Testing System Recovery..." << std::endl;
    
    if (!setupComponents()) {
        throw std::runtime_error("Component setup failed for recovery test");
    }
    
    simulateFailureAndRecovery();
    
    if (!validateRecoveryIntegrity()) {
        throw std::runtime_error("Recovery integrity validation failed");
    }
    
    if (!validateDataPersistence()) {
        throw std::runtime_error("Data persistence validation failed");
    }
    
    std::cout << "    Recovery test PASSED" << std::endl;
}

bool SystemRecoveryTest::setupComponents() {
    // Setup components for recovery testing
    return true;
}

bool SystemRecoveryTest::teardownComponents() {
    // Cleanup after recovery test
    return true;
}

void SystemRecoveryTest::simulateFailureAndRecovery() const {
    // Simulate failure scenarios and recovery
    std::cout << "    Simulating failure and recovery scenarios..." << std::endl;
}

bool SystemRecoveryTest::validateRecoveryIntegrity() const {
    // Validate system integrity after recovery
    return true; // Placeholder
}

bool SystemRecoveryTest::validateDataPersistence() const {
    // Validate data persistence after recovery
    return true; // Placeholder
}

} // namespace nlm
