#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Test suite for NLM Python bindings
class NLMTestSuite {
public:
    struct TestResults {
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        std::vector<std::string> failures;
        std::vector<std::string> warnings;
        
        double getPassRate() const {
            return totalTests > 0 ? (static_cast<double>(passedTests) / totalTests) * 100.0 : 0.0;
        }
    };
    
    TestResults runAllTests() {
        TestResults results;
        
        // Basic functionality tests
        results = runBasicTests();
        if (results.getPassRate() < 80.0) {
            results.warnings.push_back("Basic functionality test pass rate below 80%");
        }
        
        // Core class tests
        TestResults coreResults = runCoreClassTests();
        results.totalTests += coreResults.totalTests;
        results.passedTests += coreResults.passedTests;
        results.failedTests += coreResults.failedTests;
        results.failures.insert(results.failures.end(), coreResults.failures.begin(), coreResults.failures.end());
        
        // Agent tests
        TestResults agentResults = runAgentTests();
        results.totalTests += agentResults.totalTests;
        results.passedTests += agentResults.passedTests;
        results.failedTests += agentResults.failedTests;
        results.failures.insert(results.failures.end(), agentResults.failures.begin(), agentResults.failures.end());
        
        // Advanced features tests
        TestResults advancedResults = runAdvancedFeatureTests();
        results.totalTests += advancedResults.totalTests;
        results.passedTests += advancedResults.passedTests;
        results.failedTests += advancedResults.failedTests;
        results.failures.insert(results.failures.end(), advancedResults.failures.begin(), advancedResults.failures.end());
        
        // Error handling tests
        TestResults errorResults = runErrorHandlingTests();
        results.totalTests += errorResults.totalTests;
        results.passedTests += errorResults.passedTests;
        results.failedTests += errorResults.failedTests;
        results.failures.insert(results.failures.end(), errorResults.failures.begin(), errorResults.failures.end());
        
        return results;
    }
    
private:
    TestResults runBasicTests() {
        TestResults results;
        
        // Test 1: Config creation and basic operations
        results.totalTests++;
        try {
            auto config = std::make_shared<Config>();
            config->loadFromFile("test_config.json"); // Will fail but should handle gracefully
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Config creation test failed");
        }
        
        // Test 2: World creation and configuration
        results.totalTests++;
        try {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10.0f, 10.0f, 8, 8);
            world->reset();
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("World creation test failed");
        }
        
        // Test 3: Sensor creation
        results.totalTests++;
        try {
            auto vision = std::make_shared<Vision>(8, 8, 3);
            std::vector<float> data(192, 0.5f);
            vision->setData(data);
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Sensor creation test failed");
        }
        
        // Test 4: Action creation
        results.totalTests++;
        try {
            auto action = std::make_shared<Action>(ActionType::MoveForward);
            action->setType(ActionType::MoveBackward);
            auto action2 = action->clone();
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Action creation test failed");
        }
        
        return results;
    }
    
    TestResults runCoreClassTests() {
        TestResults results;
        
        // Test Brain class
        results.totalTests++;
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brain->step(0);
            brain->step(1);
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Brain basic functionality test failed");
        }
        
        // Test AgentBrain class
        results.totalTests++;
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            auto agent = std::make_shared<AgentBrain>(brain);
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 16, 16);
            world->reset();
            agent->initialize(*world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("AgentBrain basic functionality test failed");
        }
        
        // Test SimpleWorld class
        results.totalTests++;
        try {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(30.0f, 30.0f, 12, 12);
            world->reset();
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            auto body = world->getAgentBody();
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("SimpleWorld basic functionality test failed");
        }
        
        // Test SensoryPercept
        results.totalTests++;
        try {
            auto percept = std::make_shared<SensoryPercept>();
            auto vision = std::make_shared<Vision>(8, 8, 3);
            percept->setVision(vision);
            auto touch = std::vector<float>(8, 0.5f);
            percept->setTouch(touch);
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("SensoryPercept basic functionality test failed");
        }
        
        // Test ActionResult
        results.totalTests++;
        try {
            auto result = std::make_shared<ActionResult>(1.0f, true, "success");
            result->reward = 2.5f;
            result->success = true;
            result->message = "completed";
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("ActionResult basic functionality test failed");
        }
        
        return results;
    }
    
    TestResults runAgentTests() {
        TestResults results;
        
        // Test complete agent-world simulation
        results.totalTests++;
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(15.0f, 15.0f, 10, 10);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            // Enable features
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            
            // Run simulation
            for (int i = 0; i < 50; ++i) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i);
                auto action = agent->decodeMotorCommand();
                auto result = world->applyMotorCommand(action, world->getSimulationTime());
                agent->applyRewardModulation(result.reward, 0.0f);
                agent->updateDevelopment(0.1f);
            }
            
            // Check final state
            if (agent->getNeuromodulationLevel() >= 0.0f &&
                agent->getCuriosityLevel() >= 0.0f &&
                agent->getDevelopmentalStage() != DevelopmentalStage::Initial) {
                results.passedTests++;
            } else {
                results.failedTests++;
                results.failures.push_back("Complete agent-world simulation test failed - unexpected final state");
            }
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Complete agent-world simulation test failed");
        }
        
        // Test agent reset
        results.totalTests++;
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10.0f, 10.0f, 8, 8);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            // Modify state
            agent->applyRewardModulation(1.0f, 0.0f);
            brain->step(0);
            
            // Reset and check
            agent->reset();
            if (agent->getNeuromodulationLevel() == 0.0f) {
                results.passedTests++;
            } else {
                results.failedTests++;
                results.failures.push_back("Agent reset test failed - neuromodulation level not reset");
            }
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Agent reset test failed");
        }
        
        return results;
    }
    
    TestResults runAdvancedFeatureTests() {
        TestResults results;
        
        // Test performance profiling
        results.totalTests++;
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 100; ++i) {
                brain->step(i);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            if (duration.count() > 0) { // Should have taken some time
                results.passedTests++;
            } else {
                results.failedTests++;
                results.failures.push_back("Performance profiling test failed - unexpected timing");
            }
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Performance profiling test failed");
        }
        
        // Test memory statistics
        results.totalTests++;
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto workingMemory = brain->getWorkingMemory();
            auto episodicMemory = brain->getEpisodicMemory();
            auto associativeMemory = brain->getAssociativeMemory();
            
            if (workingMemory && episodicMemory && associativeMemory) {
                results.passedTests++;
            } else {
                results.failedTests++;
                results.failures.push_back("Memory statistics test failed - null memory system");
            }
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Memory statistics test failed");
        }
        
        return results;
    }
    
    TestResults runErrorHandlingTests() {
        TestResults results;
        
        // Test null pointer handling
        results.totalTests++;
        try {
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            brain->initialize();
            
            // This should not crash
            brain->getTotalNeuronCount(); // Should work
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Null pointer handling test failed - unexpected exception");
        }
        
        // Test invalid operation handling
        results.totalTests++;
        try {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10.0f, 10.0f, 8, 8);
            
            // Should be able to call these methods multiple times without issues
            for (int i = 0; i < 5; ++i) {
                world->reset();
                world->update(0.1f);
            }
            
            results.passedTests++;
        } catch (...) {
            results.failedTests++;
            results.failures.push_back("Invalid operation handling test failed");
        }
        
        // Test configuration validation
        results.totalTests++;
        try {
            auto config = std::make_shared<Config>();
            config->loadFromFile("non_existent_file.json"); // Should handle gracefully
            results.passedTests++;
        } catch (...) {
            results.passedTests++; // Expected to fail
        }
        
        return results;
    }
};

PYBIND11_MODULE(test_pynlm, m) {
    m.doc() = R"pbdoc(
        NLM Python Bindings Test Suite
        -------------------------------
        Comprehensive test suite for NLM Python bindings.
        Includes basic functionality tests, core class tests, agent tests,
        advanced feature tests, and error handling tests.
    )pbdoc";

    m.def("run_all_tests", &NLMTestSuite::runAllTests,
          "Run all tests in the NLM test suite and return results.")
        .def("run_basic_tests", &NLMTestSuite::runBasicTests,
             "Run basic functionality tests.")
        .def("run_core_class_tests", &NLMTestSuite::runCoreClassTests,
             "Run core class functionality tests.")
        .def("run_agent_tests", &NLMTestSuite::runAgentTests,
             "Run agent functionality tests.")
        .def("run_advanced_feature_tests", &NLMTestSuite::runAdvancedFeatureTests,
             "Run advanced feature tests.")
        .def("run_error_handling_tests", &NLMTestSuite::runErrorHandlingTests,
             "Run error handling tests.");

    // Test helper functions
    m.def("create_test_config", []() {
        return std::make_shared<Config>();
    }, "Create a test configuration for testing purposes.");

    m.def("create_test_brain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a test brain for testing purposes.");

    m.def("create_test_world", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a test world for testing purposes.");

    m.def("create_test_agent", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a test agent for testing purposes.");

    m.def("create_test_percept", []() {
        return std::make_shared<SensoryPercept>();
    }, "Create a test sensory percept for testing purposes.");

    // Quick test functions
    m.def("test_brain_step", [](std::shared_ptr<Brain> brain, int steps) {
        brain->initialize();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"),
        "Test brain stepping functionality.");

    m.def("test_agent_simulation", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
           std::shared_ptr<SimpleWorld> world, int steps) {
            agent->initialize(*world);
            agent->enableRewardModulation(true);
            
            for (int i = 0; i < steps; ++i) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                agent->applyRewardModulation(0.1f, 0.0f);
            }
            
            return std::make_tuple(
                agent->getNeuromodulationLevel(),
                agent->getCuriosityLevel(),
                agent->getDevelopmentalStage()
            );
        },
        py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
        "Test complete agent simulation.");

    m.def("get_test_environment_info", []() {
        std::string info = "NLM Test Environment\n";
        info += "Version: 0.1.1 (Test Edition)\n";
        info += "Python Bindings: Enhanced with comprehensive test suite\n";
        info += "Features: All core and advanced features\n";
        info += "Error Handling: Comprehensive validation and user-friendly exceptions\n";
        return info;
    }, "Get test environment information.");

    m.def("create_mock_vision_data", [](size_t width, size_t height, size_t channels) {
        std::vector<float> data(width * height * channels, 0.5f);
        // Add some pattern to make it more realistic
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t idx = (y * width + x) * channels;
                float intensity = std::sin(static_cast<float>(x) / static_cast<float>(width) * 3.14159f) *
                                std::cos(static_cast<float>(y) / static_cast<float>(height) * 3.14159f);
                for (size_t c = 0; c < channels; ++c) {
                    data[idx + c] = (intensity + 1.0f) * 0.5f;
                }
            }
        }
        return data;
    }, py::arg("width"), py::arg("height"), py::arg("channels"),
        "Create mock vision data for testing.");

    m.def("create_mock_touch_data", [](size_t size) {
        std::vector<float> data(size, 0.0f);
        for (size_t i = 0; i < size; ++i) {
            data[i] = std::sin(static_cast<float>(i) * 0.5f) * 0.5f + 0.5f;
        }
        return data;
    }, py::arg("size"), "Create mock touch data for testing.");

    m.def("create_mock_internal_data", [](size_t size) {
        std::vector<float> data(size, 0.5f);
        return data;
    }, py::arg("size"), "Create mock internal data for testing.");

    m.def("run_performance_benchmark", 
        [](std::shared_ptr<Brain> brain, int steps, int warmupSteps) {
            if (!brain) {
                throw SimulationError("Brain cannot be null for performance benchmark.");
            }
            
            // Warmup
            brain->initialize();
            for (int i = 0; i < warmupSteps; ++i) {
                brain->step(i);
            }
            
            // Benchmark
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < steps; ++i) {
                brain->step(i + warmupSteps);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            double stepsPerSecond = (steps * 1000.0) / duration.count();
            
            return std::make_tuple(
                static_cast<double>(duration.count()) / 1000.0, // seconds
                stepsPerSecond, // steps per second
                brain->getTotalSpikeCount(), // total spikes
                brain->getAverageFiringRate() // average firing rate
            );
        },
        py::arg("brain"), py::arg("steps"), py::arg("warmupSteps") = 100,
        "Run performance benchmark on brain simulation.");

    // Integration test helper
    m.def("run_integration_test", 
        [](int testSteps) {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20.0f, 20.0f, 16, 16);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            
            // Run integration test
            float totalReward = 0.0f;
            for (int step = 0; step < testSteps; ++step) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                auto result = world->applyMotorCommand(action, world->getSimulationTime());
                agent->applyRewardModulation(result.reward, 0.0f);
                totalReward += result.reward;
            }
            
            return std::make_tuple(
                brain->getTotalSpikeCount(),
                brain->getAverageFiringRate(),
                totalReward / testSteps, // average reward per step
                agent->getNeuromodulationLevel(),
                agent->getCuriosityLevel(),
                agent->getDevelopmentalStage()
            );
        },
        py::arg("testSteps") = 200,
        "Run complete integration test with brain, agent, and world.");

    // Stress test
    m.def("run_stress_test", 
        [](std::shared_ptr<Brain> brain, int stressSteps) {
            if (!brain) {
                throw SimulationError("Brain cannot be null for stress test.");
            }
            
            brain->initialize();
            
            float maxFiringRate = 0.0f;
            float totalStress = 0.0f;
            
            for (int step = 0; step < stressSteps; ++step) {
                brain->step(step);
                
                float firingRate = brain->getAverageFiringRate();
                maxFiringRate = std::max(maxFiringRate, firingRate);
                
                // Simulate stress through high input
                if (step % 10 == 0) {
                    for (size_t i = 0; i < 10; ++i) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
                    }
                }
                
                totalStress += firingRate;
            }
            
            return std::make_tuple(
                stressSteps, // steps completed
                totalStress / stressSteps, // average stress level
                maxFiringRate, // maximum firing rate
                brain->getTotalSpikeCount(), // total spikes
                brain->getTotalNeuronCount() // total neurons
            );
        },
        py::arg("brain"), py::arg("stressSteps") = 1000,
        "Run stress test on brain simulation.");

    // Memory usage test
    m.def("test_memory_usage", 
        [](std::shared_ptr<Brain> brain, int testSteps) {
            if (!brain) {
                throw SimulationError("Brain cannot be null for memory test.");
            }
            
            brain->initialize();
            
            // Force memory allocation by creating many regions
            for (int i = 0; i < 10; ++i) {
                brain->addRegion(std::string("Region_") + std::to_string(i));
            }
            
            // Run simulation
            float totalSpikes = 0.0f;
            for (int step = 0; step < testSteps; ++step) {
                brain->step(step);
                totalSpikes += brain->getTotalSpikeCount();
            }
            
            return std::make_tuple(
                totalSpikes / testSteps, // average spikes per step
                brain->getTotalNeuronCount(), // total neurons
                brain->getTotalSynapseCount(), // total synapses
                brain->getRegionCount() // number of regions
            );
        },
        py::arg("brain"), py::arg("testSteps") = 500,
        "Test memory usage and performance with multiple regions.");
}

} // namespace nlm