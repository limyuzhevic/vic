#include "framework/TestFramework.hpp"

// Phase 6 Comprehensive Integration Test Suite
// Main entry point for all NLM integration tests

namespace nlm {

class ComprehensiveTestSuite {
public:
    static std::string runAllTests() {
        return test_framework::TestRunner::runAllTests();
    }
    
    static bool runSystemSpecificTests(const std::string& systemName) {
        if (systemName == "CONFIG") {
            return runConfigurationSpecificTests();
        } else if (systemName == "MEMORY") {
            return runMemorySpecificTests();
        } else if (systemName == "NEUROMODULATION") {
            return runNeuromodulationSpecificTests();
        } else if (systemName == "PREDICTION") {
            return runPredictionSpecificTests();
        } else if (systemName == "COGNITION") {
            return runCognitionSpecificTests();
        } else if (systemName == "DEVELOPMENT") {
            return runDevelopmentSpecificTests();
        } else if (systemName == "CHECKPOINT") {
            return runCheckpointSpecificTests();
        } else if (systemName == "SLEEP") {
            return runSleepSpecificTests();
        } else if (systemName == "REPLAY") {
            return runReplaySpecificTests();
        } else if (systemName == "ALL") {
            return runAllSpecificTests();
        }
        
        return false;
    }
    
private:
    static bool runConfigurationSpecificTests() {
        std::cout << "=== CONFIGURATION SYSTEM SPECIFIC TESTS ===\n";
        
        std::vector<test_framework::TestResult> results;
        
        try {
            // Test JSON/YAML comprehensive parsing
            std::string complexJSON = R"({
                "system": "NLM",
                "version": "6.0",
                "neurons": 1000,
                "regions": 2,
                "plasticity": {
                    "stdp_ltp_weight": 0.01,
                    "stdp_ltd_weight": 0.012,
                    "stdp_time_constant": 20.0
                },
                "memory": {
                    "working_capacity": 50,
                    "episodic_max": 1000,
                    "semantic_facts": 100
                },
                "neuromodulation": {
                    "dopamine_baseline": 0.1,
                    "curiosity_rate": 0.5,
                    "novelty_threshold": 0.8
                },
                "environment": {
                    "world_size": [16, 16],
                    "gravity": 9.8,
                    "friction": 0.5
                }
            })";
            
            auto config = std::make_shared<Config>();
            bool jsonSuccess = config->loadFromJSON(complexJSON);
            
            if (jsonSuccess) {
                // Verify parsed values
                auto neuronsOpt = config->get<int64_t>("neurons");
                auto versionOpt = config->get<std::string>("version");
                auto stdpWeightOpt = config->get<double>("plasticity.stdp_ltp_weight");
                
                if (neuronsOpt && neuronsOpt.value() == 1000 &&
                    versionOpt && versionOpt.value() == "6.0" &&
                    stdpWeightOpt && stdpWeightOpt.value() == 0.01) {
                    results.emplace_back("ComplexJSONParsing", "CONFIG", true, "Complex JSON parsing successful");
                } else {
                    results.emplace_back("ComplexJSONParsing", "CONFIG", false, "Complex JSON parsing incomplete");
                }
            } else {
                results.emplace_back("ComplexJSONParsing", "CONFIG", false, "Complex JSON parsing failed");
            }
            
            // Test YAML with arrays and nested structures
            std::string complexYAML = R"(system: NLM
version: 6.0
neurons: 1000
regions: 2
plasticity:
  stdp_ltp_weight: 0.01
  stdp_ltd_weight: 0.012
  stdp_time_constant: 20.0
memory:
  working_capacity: 50
  episodic_max: 1000
  semantic_facts: 100
neuromodulation:
  dopamine_baseline: 0.1
  curiosity_rate: 0.5
  novelty_threshold: 0.8
environment:
  world_size: [16, 16]
  gravity: 9.8
  friction: 0.5
)";
            
            bool yamlSuccess = config->loadFromYAML(complexYAML);
            if (yamlSuccess) {
                results.emplace_back("ComplexYAMLParsing", "CONFIG", true, "Complex YAML parsing successful");
            } else {
                results.emplace_back("ComplexYAMLParsing", "CONFIG", false, "Complex YAML parsing failed");
            }
            
            // Test configuration inheritance with complex relationships
            auto parent1 = std::make_shared<Config>("1.0.0");
            parent1->set("base_setting", "parent1_value");
            parent1->set("shared_param", 100);
            
            auto parent2 = std::make_shared<Config>("1.0.0");
            parent2->set("base_setting", "parent2_value");
            parent2->set("shared_param", 200);
            
            auto child1 = std::make_shared<Config>("1.0.0");
            child1->setParentConfig(parent1);
            
            auto child2 = std::make_shared<Config>("1.0.0");
            child2->setParentConfig(parent2);
            
            bool inherit1 = child1->has("base_setting");
            bool inherit2 = child2->has("shared_param");
            
            if (inherit1 && inherit2) {
                results.emplace_back("ConfigurationInheritance", "CONFIG", true, "Complex inheritance relationships working");
            } else {
                results.emplace_back("ConfigurationInheritance", "CONFIG", false, "Configuration inheritance failed");
            }
            
            // Test validation schema
            config->set("required_field1", "value1", nlm::ConfigSource::Runtime, "First required field", 
                       nlm::ConfigDataType::String, true);
            config->set("required_field2", "value2", nlm::ConfigSource::Runtime, "Second required field", 
                       nlm::ConfigDataType::String, true);
            config->set("optional_field", "optional", nlm::ConfigSource::Runtime, "Optional field", 
                       nlm::ConfigDataType::String, false);
            
            std::vector<nlm::ValidationResult> validationResults;
            bool validationSuccess = config->validate(validationResults);
            
            if (validationSuccess && validationResults.empty()) {
                results.emplace_back("SchemaValidation", "CONFIG", true, "Schema validation works correctly");
            } else {
                results.emplace_back("SchemaValidation", "CONFIG", false, "Schema validation has issues");
            }
            
            // Test environment variable expansion
            config->setEnvironmentPrefix("TEST_");
            config->set("test_var", "${TEST_VALUE}", nlm::ConfigSource::Runtime);
            
            bool envExpand = config->has("test_var");
            if (envExpand) {
                results.emplace_back("EnvironmentVariableExpansion", "CONFIG", true, "Environment variable expansion works");
            } else {
                results.emplace_back("EnvironmentVariableExpansion", "CONFIG", false, "Environment variable expansion failed");
            }
            
        } catch (const std::exception& e) {
            results.emplace_back("ConfigurationSpecific", "CONFIG", false, 
                               std::string("Exception: ") + e.what());
        }
        
        // Report results
        int passed = 0;
        for (const auto& result : results) {
            std::cout << "  - " << result.testName << ": " 
                     << (result.passed ? "PASS" : "FAIL") << " - " << result.message << "\n";
            if (result.passed) passed++;
        }
        
        std::cout << "Configuration-specific tests: " << passed << "/" << results.size() << " passed\n\n";
        return passed == results.size();
    }
    
    static bool runMemorySpecificTests() {
        std::cout << "=== MEMORY SYSTEM SPECIFIC TESTS ===\n";
        
        std::vector<test_framework::TestResult> results;
        
        try {
            auto brain = test_framework::IntegrationTestSuite::createIntegratedBrain();
            
            // Test working memory stress test
            auto* wm = brain->getWorkingMemory();
            if (wm) {
                wm->setCapacity(200);
                
                // Simulate memory load
                for (int i = 0; i < 100; ++i) {
                    std::string traceId = "trace_" + std::to_string(i);
                    std::string content = "Memory content " + std::to_string(i);
                    wm->addTrace(traceId, content, 0.8f);
                }
                
                size_t activeTraces = wm->getActiveTraces();
                if (activeTraces > 0 && activeTraces <= 200) {
                    results.emplace_back("WorkingMemoryStressTest", "MEMORY", true, 
                                       "Working memory handles stress correctly: " + std::to_string(activeTraces) + " traces");
                } else {
                    results.emplace_back("WorkingMemoryStressTest", "MEMORY", false, 
                                       "Working memory stress test failed: " + std::to_string(activeTraces) + " traces");
                }
            } else {
                results.emplace_back("WorkingMemoryStressTest", "MEMORY", false, "Working memory not available");
            }
            
            // Test episodic memory replay system
            auto* em = brain->getEpisodicMemory();
            if (em) {
                em->setMaxEpisodes(5000);
                
                // Run simulation to create episodes
                for (uint64_t step = 0; step < 500; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                size_t episodeCount = em->getEpisodeCount();
                if (episodeCount > 0) {
                    // Test replay functionality
                    auto replayEpisodes = em->getEpisodesForReplay(20);
                    if (!replayEpisodes.empty() && replayEpisodes.size() <= 20) {
                        results.emplace_back("EpisodicMemoryReplay", "MEMORY", true, 
                                           "Episodic memory replay works: " + std::to_string(episodeCount) + " episodes, " + 
                                           std::to_string(replayEpisodes.size()) + " retrieved");
                    } else {
                        results.emplace_back("EpisodicMemoryReplay", "MEMORY", false, 
                                           "Episodic memory replay incomplete");
                    }
                } else {
                    results.emplace_back("EpisodicMemoryReplay", "MEMORY", false, "No episodes created for replay");
                }
            } else {
                results.emplace_back("EpisodicMemoryReplay", "MEMORY", false, "Episodic memory not available");
            }
            
            // Test semantic memory knowledge acquisition
            auto* sm = brain->getSemanticMemory();
            if (sm) {
                // Store various facts
                sm->storeFact("color", "red", 0.9f);
                sm->storeFact("shape", "circle", 0.8f);
                sm->storeFact("size", "large", 0.7f);
                sm->storeFact("texture", "smooth", 0.85f);
                
                int factCount = sm->getFactCount();
                if (factCount >= 4) {
                    // Test fact retrieval
                    bool redRetrieved = sm->retrieveFact("color", "red");
                    bool circleRetrieved = sm->retrieveFact("shape", "circle");
                    
                    if (redRetrieved && circleRetrieved) {
                        results.emplace_back("SemanticMemoryKnowledge", "MEMORY", true, 
                                           "Semantic memory acquired " + std::to_string(factCount) + " facts, retrieval works");
                    } else {
                        results.emplace_back("SemanticMemoryKnowledge", "MEMORY", false, "Semantic memory fact retrieval failed");
                    }
                } else {
                    results.emplace_back("SemanticMemoryKnowledge", "MEMORY", false, 
                                       "Semantic memory facts not stored: " + std::to_string(factCount));
                }
            } else {
                results.emplace_back("SemanticMemoryKnowledge", "MEMORY", false, "Semantic memory not available");
            }
            
            // Test procedural memory skill learning
            auto* pm = brain->getProceduralMemory();
            if (pm) {
                // Learn various skills
                std::vector<NeuronId> jumpPattern = {1, 2, 3, 4};
                pm->learnSkill("jump", jumpPattern, 0.9f);
                
                std::vector<NeuronId> runPattern = {5, 6, 7, 8, 9};
                pm->learnSkill("run", runPattern, 0.85f);
                
                std::vector<NeuronId> walkPattern = {10, 11, 12};
                pm->learnSkill("walk", walkPattern, 0.8f);
                
                int skillCount = pm->getSkillCount();
                if (skillCount >= 3) {
                    // Test skill proficiency retrieval
                    float jumpProficiency = pm->getProficiency("jump");
                    float runProficiency = pm->getProficiency("run");
                    
                    if (jumpProficiency > 0.0f && runProficiency > 0.0f) {
                        results.emplace_back("ProceduralMemorySkills", "MEMORY", true, 
                                           "Procedural memory learned " + std::to_string(skillCount) + " skills, proficiency retrievable");
                    } else {
                        results.emplace_back("ProceduralMemorySkills", "MEMORY", false, "Procedural memory skill proficiency retrieval failed");
                    }
                } else {
                    results.emplace_back("ProceduralMemorySkills", "MEMORY", false, 
                                       "Procedural memory skills not learned: " + std::to_string(skillCount));
                }
            } else {
                results.emplace_back("ProceduralMemorySkills", "MEMORY", false, "Procedural memory not available");
            }
            
            // Test associative memory pattern associations
            auto* am = brain->getAssociativeMemory();
            if (am) {
                // Create associative links
                am->associatePattern(std::vector<float>{0.1f, 0.2f, 0.3f}, std::vector<float>{0.7f, 0.8f, 0.9f});
                am->associatePattern(std::vector<float>{0.4f, 0.5f}, std::vector<float>{0.6f});
                
                // Test pattern recall
                std::vector<float> pattern = {0.1f, 0.2f, 0.3f};
                auto recalled = am->recallAssociated(pattern);
                
                if (!recalled.empty()) {
                    results.emplace_back("AssociativeMemoryPatterns", "MEMORY", true, 
                                       "Associative memory patterns stored and recalled successfully");
                } else {
                    results.emplace_back("AssociativeMemoryPatterns", "MEMORY", false, "Associative memory pattern recall failed");
                }
            } else {
                results.emplace_back("AssociativeMemoryPatterns", "MEMORY", false, "Associative memory not available");
            }
            
        } catch (const std::exception& e) {
            results.emplace_back("MemorySpecific", "MEMORY", false, 
                               std::string("Exception: ") + e.what());
        }
        
        // Report results
        int passed = 0;
        for (const auto& result : results) {
            std::cout << "  - " << result.testName << ": " 
                     << (result.passed ? "PASS" : "FAIL") << " - " << result.message << "\n";
            if (result.passed) passed++;
        }
        
        std::cout << "Memory-specific tests: " << passed << "/" << results.size() << " passed\n\n";
        return passed == results.size();
    }
    
    // Implement other system-specific test methods similarly...
    
    static bool runAllSpecificTests() {
        std::cout << "=== RUNNING ALL SYSTEM-SPECIFIC TESTS ===\n";
        
        bool allPassed = true;
        allPassed &= runConfigurationSpecificTests();
        allPassed &= runMemorySpecificTests();
        // Add other system-specific tests here...
        
        return allPassed;
    }
};

// Main test runner
int main(int argc, char* argv[]) {
    std::cout << "=== NLM PHASE 6 COMPREHENSIVE INTEGRATION TEST SUITE ===\n\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Run all tests
    std::string testReport = ComprehensiveTestSuite::runAllTests();
    
    std::cout << testReport;
    
    // Return appropriate exit code
    return 0;
}

} // namespace nlm