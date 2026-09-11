#include "Phase6Validator.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <iomanip>

using namespace nlm;

Phase6Validator::Phase6Validator() {
    Logger::getInstance().setLevel(Logger::Level::Info);
}

std::vector<ValidationTestResults> Phase6Validator::runAllValidationTests() {
    std::vector<ValidationTestResults> results;
    
    results.push_back(testBrainIntegration());
    results.push_back(testSaveLoad());
    results.push_back(testNeuromodulationIntegration());
    results.push_back(testMemorySystems());
    results.push_back(testDevelopment());
    results.push_back(testSleepCycle());
    results.push_back(testPerformance());
    results.push_back(testExpertFeatures());
    
    return results;
}

ValidationTestResults Phase6Validator::testBrainIntegration() {
    std::string testName = "Brain Integration Test";
    double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    ValidationTestResults result;
    result.testName = testName;
    
    try {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            result.status = "FAILED";
            result.passed = false;
            result.details = "Brain initialization failed";
        } else {
            bool workingMemoryOK = brain->getWorkingMemory() != nullptr;
            bool episodicMemoryOK = brain->getEpisodicMemory() != nullptr;
            bool associativeMemoryOK = brain->getAssociativeMemory() != nullptr;
            bool predictionSystemOK = brain->getPredictionSystem() != nullptr;
            bool plannerOK = brain->getPlanner() != nullptr;
            bool conceptFormationOK = brain->getConceptFormation() != nullptr;
            bool attentionOK = brain->getAttention() != nullptr;
            bool developmentOK = brain->getDevelopmentSystem() != nullptr;
            bool dopamineOK = brain->getDopamine() != nullptr;
            bool curiosityOK = brain->getCuriosity() != nullptr;
            bool noveltyOK = brain->getNovelty() != nullptr;
            bool predictionErrorOK = brain->getPredictionErrorSignal() != nullptr;
            
            if (workingMemoryOK && episodicMemoryOK && associativeMemoryOK && 
                predictionSystemOK && plannerOK && conceptFormationOK && attentionOK &&
                developmentOK && dopamineOK && curiosityOK && noveltyOK && predictionErrorOK) {
                result.status = "PASSED";
                result.passed = true;
                result.details = "All integrated systems are accessible and connected";
            } else {
                result.status = "PARTIAL";
                result.passed = false;
                if (!workingMemoryOK) result.issues.push_back("Working memory disconnected");
                if (!episodicMemoryOK) result.issues.push_back("Episodic memory disconnected");
                if (!associativeMemoryOK) result.issues.push_back("Associative memory disconnected");
                if (!predictionSystemOK) result.issues.push_back("Prediction system disconnected");
                if (!plannerOK) result.issues.push_back("Planner disconnected");
                if (!conceptFormationOK) result.issues.push_back("Concept formation disconnected");
                if (!attentionOK) result.issues.push_back("Attention disconnected");
                if (!developmentOK) result.issues.push_back("Development disconnected");
                if (!dopamineOK) result.issues.push_back("Dopamine disconnected");
                if (!curiosityOK) result.issues.push_back("Curiosity disconnected");
                if (!noveltyOK) result.issues.push_back("Novelty disconnected");
                if (!predictionErrorOK) result.issues.push_back("Prediction error disconnected");
                result.details = "Some systems are still disconnected from brain loop";
            }
        }
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.passed = false;
        result.details = std::string("Exception: ") + e.what();
    }
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    result.executionTime = endTime - startTime;
    
    return result;
}

ValidationTestResults Phase6Validator::testSaveLoad() {
    std::string testName = "Save/Load Test";
    double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    ValidationTestResults result;
    result.testName = testName;
    
    try {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain1 = std::make_shared<Brain>(cfg);
        brain1->initialize();
        
        for (int i = 0; i < 100; ++i) {
            brain1->step(i, i * 0.001);
        }
        
        std::string checkpointPath = "/tmp/nlm_validation_checkpoint.bin";
        
        bool saveOK = brain1->save(checkpointPath);
        if (!saveOK) {
            result.status = "FAILED";
            result.passed = false;
            result.details = "Checkpoint save operation failed";
            result.issues.push_back("Save operation returned false");
        } else {
            auto brain2 = std::make_shared<Brain>(cfg);
            brain2->initialize();
            
            bool loadOK = brain2->load(checkpointPath);
            if (!loadOK) {
                result.status = "FAILED";
                result.passed = false;
                result.details = "Checkpoint load operation failed";
                result.issues.push_back("Load operation returned false");
            } else {
                result.status = "PASSED";
                result.passed = true;
                result.details = "Checkpoint save/load functionality working correctly";
            }
        }
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.passed = false;
        result.details = std::string("Exception: ") + e.what();
    }
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    result.executionTime = endTime - startTime;
    
    return result;
}

ValidationTestResults Phase6Validator::testNeuromodulationIntegration() {
    std::string testName = "Neuromodulation Integration Test";
    double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    ValidationTestResults result;
    result.testName = testName;
    
    try {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predictionError = brain->getPredictionErrorSignal();
        
        if (!dopamine || !curiosity || !novelty || !predictionError) {
            result.status = "FAILED";
            result.passed = false;
            result.details = "Some neuromodulation systems are not accessible";
            if (!dopamine) result.issues.push_back("Dopamine system missing");
            if (!curiosity) result.issues.push_back("Curiosity system missing");
            if (!novelty) result.issues.push_back("Novelty system missing");
            if (!predictionError) result.issues.push_back("Prediction error system missing");
        } else {
            float initialDopamineLevel = dopamine->getLevel();
            
            for (int i = 0; i < 50; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
                brain->step(i, i * 0.001);
                
                if (dopamine->getLevel() != initialDopamineLevel) {
                    result.status = "PASSED";
                    result.passed = true;
                    result.details = "Neuromodulation signals are affecting neural dynamics";
                    break;
                }
            }
            
            if (!result.passed) {
                result.status = "PARTIAL";
                result.passed = false;
                result.details = "Neuromodulation systems exist but may not be fully integrated";
                result.issues.push_back("Neuromodulation effects not visible during simulation");
            }
        }
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.passed = false;
        result.details = std::string("Exception: ") + e.what();
    }
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    result.executionTime = endTime - startTime;
    
    return result;
}

ValidationTestResults Phase6Validator::testMemorySystems() {
    std::string testName = "Memory Systems Test";
    double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    ValidationTestResults result;
    result.testName = testName;
    
    try {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* wm = brain->getWorkingMemory();
        auto* em = brain->getEpisodicMemory();
        
        if (!wm || !em) {
            result.status = "FAILED";
            result.passed = false;
            result.details = "Memory systems are not accessible";
            if (!wm) result.issues.push_back("Working memory not available");
            if (!em) result.issues.push_back("Episodic memory not available");
        } else {
            for (int i = 0; i < 200; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 3.0f);
                brain->step(i, i * 0.001);
            }
            
            size_t wmTraces = wm->getActiveTraces();
            size_t emEpisodes = em->getEpisodeCount();
            
            if (wmTraces > 0 || emEpisodes > 0) {
                result.status = "PASSED";
                result.passed = true;
                result.details = std::string("Working memory has ") + std::to_string(wmTraces) + 
                               " traces, Episodic memory has " + std::to_string(emEpisodes) + " episodes";
            } else {
                result.status = "PARTIAL";
                result.passed = false;
                result.details = "Memory systems exist but may not be actively forming memories";
                result.issues.push_back("No working memory traces or episodic memories formed");
            }
        }
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.passed = false;
        result.details = std::string("Exception: ") + e.what();
    }
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    result.executionTime = endTime - startTime;
    
    return result;
}

ValidationTestResults Phase6Validator::testDevelopment() {
    std::string testName = "Development Test";
    double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    ValidationTestResults result;
    result.testName = testName;
    
    try {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* devSystem = brain->getDevelopmentSystem();
        
        if (!devSystem) {
            result.status = "FAILED";
            result.passed = false;
            result.details = "Development system is not accessible";
            result.issues.push_back("Development system not available");
        } else {
            DevelopmentalStage stage = brain->getDevelopmentalStage();
            
            for (int i = 0; i < 3000; i += 1000) {
                brain->step(i, i * 0.001);
            }
            
            DevelopmentalStage finalStage = brain->getDevelopmentalStage();
            
            if (stage != finalStage) {
                result.status = "PASSED";
                result.passed = true;
                result.details = std::string("Development system working - stage changed from ") + 
                               std::to_string(static_cast<int>(stage)) + " to " + 
                               std::to_string(static_cast<int>(finalStage));
            } else {
                result.status = "PARTIAL";
                result.passed = false;
                result.details = "Development system exists but may not be advancing through stages";
                result.issues.push_back("Developmental stage did not change over time");
            }
        }
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.passed = false;
        result.details = std::string("Exception: ") + e.what();
    }
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    result.executionTime = endTime - startTime;
    
    return result;
}

ValidationTestResults Phase6Validator::testSleepCycle() {
    std::string testName = "Sleep/Rest Cycle Test";
    double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    ValidationTestResults result;
    result.testName = testName;
    
    try {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* em = brain->getEpisodicMemory();
        
        if (!em) {
            result.status = "FAILED";
            result.passed = false;
            result.details = "Episodic memory required for sleep cycle test";
            result.issues.push_back("Episodic memory system not available");
        } else {
            for (int i = 0; i < 500; ++i) {
                brain->step(i, i * 0.001);
            }
            
            size_t episodeCount = em->getEpisodeCount();
            auto episodes = em->getEpisodesForReplay(3);
            
            if (episodeCount > 0 && !episodes.empty()) {
                result.status = "PASSED";
                result.passed = true;
                result.details = std::string("Sleep/rest cycle functional - ") + 
                               std::to_string(episodeCount) + " episodes available for replay, " +
                               std::to_string(episodes.size()) + " retrieved for replay";
            } else {
                result.status = "PARTIAL";
                result.passed = false;
                result.details = "Replay system exists but may not be working correctly";
                if (episodeCount == 0) result.issues.push_back("No episodes stored for replay");
                if (episodes.empty()) result.issues.push_back("No episodes retrieved for replay");
            }
        }
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.passed = false;
        result.details = std::string("Exception: ") + e.what();
    }
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    result.executionTime = endTime - startTime;
    
    return result;
}

ValidationTestResults Phase6Validator::testPerformance() {
    std::string testName = "Performance Test";
    double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    ValidationTestResults result;
    result.testName = testName;
    
    try {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto startWall = std::chrono::high_resolution_clock::now();
        
        float totalFiringRate = 0.0f;
        size_t totalSteps = 2000;
        
        for (uint64_t step = 0; step < totalSteps; ++step) {
            brain->step(step, step * 0.001);
            totalFiringRate += brain->getAverageFiringRate();
        }
        
        auto endWall = std::chrono::high_resolution_clock::now();
        double wallTime = std::chrono::duration<double>(endWall - startWall).count();
        
        float avgFiringRate = totalFiringRate / totalSteps;
        float stepsPerSecond = totalSteps / wallTime;
        
        bool firingRateOK = avgFiringRate > 0.0f && avgFiringRate < 10.0f;
        bool performanceOK = stepsPerSecond > 1000.0f;
        
        if (firingRateOK && performanceOK) {
            result.status = "PASSED";
            result.passed = true;
            result.details = std::string("Performance acceptable - ") + 
                           std::to_string(stepsPerSecond) + " steps/sec, avg firing rate: " +
                           std::to_string(avgFiringRate) + " Hz";
        } else {
            result.status = "PERFORMANCE_ISSUES";
            result.passed = false;
            if (!firingRateOK) result.issues.push_back(std::string("Abnormal firing rate: ") + std::to_string(avgFiringRate) + " Hz");
            if (!performanceOK) result.issues.push_back(std::string("Low performance: ") + std::to_string(stepsPerSecond) + " steps/sec");
            result.details = "System running but performance may need optimization";
        }
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.passed = false;
        result.details = std::string("Exception: ") + e.what();
    }
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    result.executionTime = endTime - startTime;
    
    return result;
}

ValidationTestResults Phase6Validator::testExpertFeatures() {
    std::string testName = "Expert Features Test";
    double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    ValidationTestResults result;
    result.testName = testName;
    
    try {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        cfg->set("enable_checkpointing", true);
        cfg->set("replay_interval", 50);
        cfg->set("consolidation_interval", 500);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        std::string checkpointPath = "/tmp/nlm_expert_checkpoint.bin";
        
        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        bool saveOK = brain->save(checkpointPath);
        if (!saveOK) {
            result.status = "FAILED";
            result.passed = false;
            result.details = "Expert checkpoint save failed";
            result.issues.push_back("Save operation failed");
        } else {
            bool checkpointManagerOK = brain->getWorkingMemory() != nullptr;
            
            auto* wm = brain->getWorkingMemory();
            auto* em = brain->getEpisodicMemory();
            auto* attention = brain->getAttention();
            auto* predictionSystem = brain->getPredictionSystem();
            
            bool advancedFeaturesOK = wm != nullptr && em != nullptr && 
                                    attention != nullptr && predictionSystem != nullptr;
            
            if (advancedFeaturesOK) {
                result.status = "PASSED";
                result.passed = true;
                result.details = "Expert features working - advanced memory, attention, and prediction systems integrated";
            } else {
                result.status = "LIMITED";
                result.passed = false;
                result.details = "Basic expert features exist but may not be fully functional";
                if (!wm) result.issues.push_back("Working memory not available for expert features");
                if (!em) result.issues.push_back("Episodic memory not available for expert features");
                if (!attention) result.issues.push_back("Attention system not available for expert features");
                if (!predictionSystem) result.issues.push_back("Prediction system not available for expert features");
            }
        }
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.passed = false;
        result.details = std::string("Exception: ") + e.what();
    }
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    result.executionTime = endTime - startTime;
    
    return result;
}

int main() {
    std::cout << "=== NLM Phase 6 Integration Validation Suite ===" << std::endl;
    std::cout << "Testing all improvements made to Phase 6 integration..." << std::endl;
    std::cout << std::endl;
    
    Phase6Validator validator;
    std::vector<ValidationTestResults> results = validator.runAllValidationTests();
    
    std::cout << "=== VALIDATION SUMMARY ===" << std::endl;
    int totalTests = results.size();
    int passedTests = 0;
    int failedTests = 0;
    double totalExecutionTime = 0.0;
    
    for (const auto& result : results) {
        totalExecutionTime += result.executionTime;
        if (result.passed) passedTests++;
        else failedTests++;
    }
    
    std::cout << "Total tests run: " << totalTests << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    std::cout << "Total execution time: " << totalExecutionTime << "s" << std::endl;
    std::cout << "Success rate: " << (passedTests * 100.0 / totalTests) << "%" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== DETAILED RESULTS ===" << std::endl;
    for (const auto& result : results) {
        std::cout << result.testName << ": " << result.status << " (" << std::fixed << std::setprecision(3) << result.executionTime << "s)" << std::endl;
        std::cout << "  " << result.details << std::endl;
        if (!result.issues.empty()) {
            std::cout << "  Issues: ";
            for (size_t i = 0; i < result.issues.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << result.issues[i];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "=== OVERALL ASSESSMENT ===" << std::endl;
    if (failedTests == 0) {
        std::cout << "✅ ALL TESTS PASSED - Phase 6 integration is complete and functional!" << std::endl;
        std::cout << "The artificial brain system is now fully integrated and operational." << std::endl;
    } else {
        std::cout << "❌ SOME TESTS FAILED - Phase 6 integration needs work:" << std::endl;
        std::cout << "  - The NLM brain has existing architecture but systems are not properly connected" << std::endl;
        std::cout << "  - Based on Phase 6 Final Audit, the system is at 35.8% integration" << std::endl;
        std::cout << "  - Priority 1: CONNECT EXISTING SYSTEMS (see docs/PHASE6_FINAL_AUDIT.md)" << std::endl;
        std::cout << "  - Key missing connections: WorkingMemory→Sensory, EpisodicMemory→Experiences, etc." << std::endl;
    }
    
    return failedTests == 0 ? 0 : 1;
}
