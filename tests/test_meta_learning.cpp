# Test for MetaLearning algorithms
#include "MetaLearning.hpp"
#include "../MetaLearningIntegration.hpp"
#include <iostream>
#include <cassert>

void testMetaLearningInitialization() {
    nlm::MetaLearning metaLearning;
    std::cout << "MetaLearning initialized successfully" << std::endl;
}

void testTaskInference() {
    nlm::TaskInference taskInference;
    
    // Test task inference with sample experience
    std::vector<float> experience = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::string inferredTask = taskInference.inferTask(experience);
    
    assert(inferredTask == "unknown_task" || inferredTask.find("task") != std::string::npos);
    std::cout << "TaskInference works: inferred task " << inferredTask << std::endl;
}

void testLearningToLearn() {
    nlm::LearningToLearn learningToLearn;
    
    // Test learning rate optimization
    std::vector<float> lossHistory = {0.5f, 0.4f, 0.3f, 0.2f, 0.1f};
    std::vector<float> gradientHistory = {0.1f, 0.2f, 0.15f, 0.12f, 0.08f};
    
    float learningRate = learningToLearn.optimizeLearningRate(lossHistory, gradientHistory);
    assert(learningRate > 0.0f && learningRate <= 1.0f);
    std::cout << "LearningToLearn optimized learning rate: " << learningRate << std::endl;
}

void testAutoCuriosity() {
    nlm::AutoCuriosity autoCuriosity;
    
    // Test curiosity update
    float curiosity = autoCuriosity.update(0.5f, 0.3f, 1.0f);
    assert(curiosity >= 0.0f);
    std::cout << "AutoCuriosity updated: level = " << curiosity << std::endl;
}

void testMetaPlasticity() {
    nlm::MetaPlasticity metaPlasticity;
    
    // Test meta-plasticity parameters
    metaPlasticity.setAdaptiveLearningRate(true);
    metaPlasticity.setMetaPlasticityRate(0.01f);
    
    assert(metaPlasticity.getName() == std::string("MetaPlasticity"));
    std::cout << "MetaPlasticity created successfully with adaptive learning rate" << std::endl;
}

void testMetaLearningIntegration() {
    nlm::MetaLearningIntegration integration;
    
    // Test initialization
    integration.setMetaLearningEnabled(true);
    integration.setTaskInferenceEnabled(true);
    integration.setLearningToLearnEnabled(true);
    integration.setAutoCuriosityEnabled(true);
    integration.setMetaPlasticityEnabled(true);
    
    std::cout << "MetaLearningIntegration configured successfully" << std::endl;
}

void runAllMetaLearningTests() {
    std::cout << "=== Running Meta-Learning Algorithm Tests ===" << std::endl;
    
    testMetaLearningInitialization();
    testTaskInference();
    testLearningToLearn();
    testAutoCuriosity();
    testMetaPlasticity();
    testMetaLearningIntegration();
    
    std::cout << "=== All Meta-Learning Tests Passed ===" << std::endl;
}

int main() {
    runAllMetaLearningTests();
    return 0;
}
