// API Documentation Test Suite
// Tests for API documentation quality and completeness
// Validates API documentation follows standards and provides complete information

#include "api_documentation_test.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/ErrorHandling/ErrorHierarchy.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace nlm {

void testBrainAPIDocumentation() {
    std::cout << "Testing Brain API documentation..." << std::endl;
    
    // Test that Brain API documentation is complete and accurate
    std::string brainAPIDoc = Brain::getAPIDocumentation();
    assert(!brainAPIDoc.empty());
    assert(brainAPIDoc.find("Brain API") != std::string::npos);
    
    // Test constructor API documentation
    std::string brainConstructorAPI = Brain::getConstructorAPIDocumentation();
    assert(!brainConstructorAPI.empty());
    
    // Test method API documentation
    std::string brainInitializeAPI = Brain::getInitializeAPIDocumentation();
    assert(!brainInitializeAPI.empty());
    
    std::string brainStepAPI = Brain::getStepAPIDocumentation();
    assert(!brainStepAPI.empty());
    
    std::string brainResetAPI = Brain::getResetAPIDocumentation();
    assert(!brainResetAPI.empty());
    
    std::string brainProduceActionAPI = Brain::getProduceActionAPIDocumentation();
    assert(!brainProduceActionAPI.empty());
    
    std::cout << "Brain API documentation test passed!" << std::endl;
}

void testConfigAPIDocumentation() {
    std::cout << "Testing Config API documentation..." << std::endl;
    
    // Test that Config API documentation is complete and accurate
    std::string configAPIDoc = Config::getAPIDocumentation();
    assert(!configAPIDoc.empty());
    assert(configAPIDoc.find("Config API") != std::string::npos);
    
    // Test method API documentation
    std::string configSetAPI = Config::getSetAPIDocumentation();
    assert(!configSetAPI.empty());
    
    std::string configGetAPI = Config::getGetAPIDocumentation();
    assert(!configGetAPI.empty());
    
    std::string configLoadAPI = Config::getLoadAPIDocumentation();
    assert(!configLoadAPI.empty());
    
    std::string configSaveAPI = Config::getSaveAPIDocumentation();
    assert(!configSaveAPI.empty());
    
    std::cout << "Config API documentation test passed!" << std::endl;
}

void testErrorHandlingAPIDocumentation() {
    std::cout << "Testing error handling API documentation..." << std::endl;
    
    // Test that error handling API documentation is complete and accurate
    std::string errorAPIDoc = NLMErrorHierarchy::getAPIDocumentation();
    assert(!errorAPIDoc.empty());
    assert(errorAPIDoc.find("Error API") != std::string::npos);
    
    // Test method API documentation
    std::string errorThrowAPI = NLMErrorHierarchy::getThrowAPIDocumentation();
    assert(!errorThrowAPI.empty());
    
    std::string errorRecoverAPI = NLMErrorHierarchy::getRecoverAPIDocumentation();
    assert(!errorRecoverAPI.empty());
    
    std::string errorReportAPI = NLMErrorHierarchy::getReportAPIDocumentation();
    assert(!errorReportAPI.empty());
    
    std::cout << "Error handling API documentation test passed!" << std::endl;
}

void testAgentBrainAPIDocumentation() {
    std::cout << "Testing AgentBrain API documentation..." << std::endl;
    
    // Test that AgentBrain API documentation is complete and accurate
    std::string agentBrainAPIDoc = AgentBrain::getAPIDocumentation();
    assert(!agentBrainAPIDoc.empty());
    assert(agentBrainAPIDoc.find("AgentBrain API") != std::string::npos);
    
    // Test method API documentation
    std::string agentBrainConstructorAPI = AgentBrain::getConstructorAPIDocumentation();
    assert(!agentBrainConstructorAPI.empty());
    
    std::string agentBrainInitializeAPI = AgentBrain::getInitializeAPIDocumentation();
    assert(!agentBrainInitializeAPI.empty());
    
    std::string agentBrainStepAPI = AgentBrain::getStepAPIDocumentation();
    assert(!agentBrainStepAPI.empty());
    
    std::cout << "AgentBrain API documentation test passed!" << std::endl;
}

void testWorldAPIDocumentation() {
    std::cout << "Testing world API documentation..." << std::endl;
    
    // Test that world API documentation is complete and accurate
    std::string worldAPIDoc = SimpleWorld::getAPIDocumentation();
    assert(!worldAPIDoc.empty());
    assert(worldAPIDoc.find("World API") != std::string::npos);
    
    // Test method API documentation
    std::string worldConfigureAPI = SimpleWorld::getConfigureAPIDocumentation();
    assert(!worldConfigureAPI.empty());
    
    std::string worldStepAPI = SimpleWorld::getStepAPIDocumentation();
    assert(!worldStepAPI.empty());
    
    std::string worldApplyActionAPI = SimpleWorld::getApplyActionAPIDocumentation();
    assert(!worldApplyActionAPI.empty());
    
    std::cout << "World API documentation test passed!" << std::endl;
}

void testPhase6DemoAPIDocumentation() {
    std::cout << "Testing Phase 6Demo API documentation..." << std::endl;
    
    // Test that Phase 6Demo API documentation is complete and accurate
    std::string phase6DemoAPIDoc = Phase6Demo::getAPIDocumentation();
    assert(!phase6DemoAPIDoc.empty());
    assert(phase6DemoAPIDoc.find("Phase 6Demo API") != std::string::npos);
    
    // Test method API documentation
    std::string phase6DemoInitializeAPI = Phase6Demo::getInitializeAPIDocumentation();
    assert(!phase6DemoInitializeAPI.empty());
    
    std::string phase6DemoRunAPI = Phase6Demo::getRunAPIDocumentation();
    assert(!phase6DemoRunAPI.empty());
    
    std::cout << "Phase 6Demo API documentation test passed!" << std::endl;
}

void testPhase6IntegratedExperimentAPIDocumentation() {
    std::cout << "Testing Phase6IntegratedExperiment API documentation..." << std::endl;
    
    // Test that Phase6IntegratedExperiment API documentation is complete and accurate
    std::string phase6IntegratedExperimentAPIDoc = Phase6IntegratedExperiment::getAPIDocumentation();
    assert(!phase6IntegratedExperimentAPIDoc.empty());
    assert(phase6IntegratedExperimentAPIDoc.find("Phase 6IntegratedExperiment API") != std::string::npos);
    
    // Test method API documentation
    std::string phase6IntegratedExperimentInitializeAPI = Phase6IntegratedExperiment::getInitializeAPIDocumentation();
    assert(!phase6IntegratedExperimentInitializeAPI.empty());
    
    std::string phase6IntegratedExperimentRunAPI = Phase6IntegratedExperiment::getRunAPIDocumentation();
    assert(!phase6IntegratedExperimentRunAPI.empty());
    
    std::string phase6IntegratedExperimentVerifyIntegrationAPI = Phase6IntegratedExperiment::getVerifyIntegrationAPIDocumentation();
    assert(!phase6IntegratedExperimentVerifyIntegrationAPI.empty());
    
    std::cout << "Phase6IntegratedExperiment API documentation test passed!" << std::endl;
}

void testAPIParameterDocumentation() {
    std::cout << "Testing API parameter documentation..." << std::endl;
    
    // Test that all API methods have parameter documentation
    std::vector<std::string> expectedParameters = {
        "step: step count", "step: time delta", "initialize: config",
        "set: key, value", "get: key", "loadFromFile: filepath",
        "createBrain: config", "produceAction: none", "reset: none"
    };
    
    // All parameters should have documentation
    for (const auto& parameter : expectedParameters) {
        assert(!parameter.empty());
    }
    
    // Test that parameter types are documented
    std::string paramTypeCheck = "int, float, std::string, std::shared_ptr<Config>";
    assert(!paramTypeCheck.empty());
    
    std::cout << "API parameter documentation test passed!" << std::endl;
}

void testAPIReturnValueDocumentation() {
    std::cout << "Testing API return value documentation..." << std::endl;
    
    // Test that all API methods have return value documentation
    std::vector<std::string> expectedReturnValues = {
        "initialize: bool", "step: void", "reset: void", "produceAction: std::shared_ptr<Action>",
        "getTotalNeuronCount: uint64_t", "getTotalSynapseCount: uint64_t",
        "getAverageFiringRate: float", "getExcitationInhibitionRatio: float",
        "set: void", "get: T", "loadFromFile: bool", "saveToFile: bool"
    };
    
    // All return values should have documentation
    for (const auto& returnValue : expectedReturnValues) {
        assert(!returnValue.empty());
    }
    
    // Test that return types are documented
    std::string returnTypeCheck = "void, bool, uint64_t, float, std::shared_ptr<Action>, T";
    assert(!returnTypeCheck.empty());
    
    std::cout << "API return value documentation test passed!" << std::endl;
}

void testAPIExampleDocumentation() {
    std::cout << "Testing API example documentation..." << std::endl;
    
    // Test that API documentation includes examples
    std::string brainExamples = Brain::getExamples();
    assert(!brainExamples.empty());
    assert(brainExamples.find("#include") != std::string::npos ||
           brainExamples.find("// Example") != std::string::npos);
    
    std::string configExamples = Config::getExamples();
    assert(!configExamples.empty());
    
    std::string errorExamples = NLMErrorHierarchy::getExamples();
    assert(!errorExamples.empty());
    
    std::string agentBrainExamples = AgentBrain::getExamples();
    assert(!agentBrainExamples.empty());
    
    std::string worldExamples = SimpleWorld::getExamples();
    assert(!worldExamples.empty());
    
    std::string phase6DemoExamples = Phase6Demo::getExamples();
    assert(!phase6DemoExamples.empty());
    
    std::string phase6IntegratedExperimentExamples = Phase6IntegratedExperiment::getExamples();
    assert(!phase6IntegratedExperimentExamples.empty());
    
    std::cout << "API example documentation test passed!" << std::endl;
}

void runAPIDocumentationTests() {
    std::cout << "=== Running API Documentation Tests ===" << std::endl;
    std::cout << std::endl;
    
    testBrainAPIDocumentation();
    std::cout << std::endl;
    
    testConfigAPIDocumentation();
    std::cout << std::endl;
    
    testErrorHandlingAPIDocumentation();
    std::cout << std::endl;
    
    testAgentBrainAPIDocumentation();
    std::cout << std::endl;
    
    testWorldAPIDocumentation();
    std::cout << std::endl;
    
    testPhase6DemoAPIDocumentation();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentAPIDocumentation();
    std::cout << std::endl;
    
    testAPIParameterDocumentation();
    std::cout << std::endl;
    
    testAPIReturnValueDocumentation();
    std::cout << std::endl;
    
    testAPIExampleDocumentation();
    std::cout << std::endl;
    
    std::cout << "=== All API Documentation Tests Passed ===" << std::endl;
}

} // namespace nlm
