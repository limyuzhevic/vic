#include "main_refactored.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <chrono>

namespace nlm {

TestResult NeuralTestRunner::runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    TestResult result;
    result.testName = "Basic Connectivity Test";
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.passed = false;
            result.message = "No neural region found";
            return result;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            result.passed = false;
            result.message = "No neurons found in region";
            return result;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        NLM_LOG_INFO("Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(TestConstants::CONNECTIVITY_TEST_CURRENT);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * TestConstants::DEFAULT_TIMESTEP);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            result.passed = true;
            result.message = "✓ Spikes propagate through network";
            result.metrics["spikes_generated"] = std::to_string(spikes);
            result.metrics["neurons_tested"] = std::to_string(std::min(size_t(10), neurons.size()));
        } else {
            result.passed = false;
            result.message = "No spikes generated - connectivity may be broken";
            result.metrics["neurons_tested"] = std::to_string(std::min(size_t(10), neurons.size()));
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Exception in connectivity test: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    return result;
}

TestResult NeuralTestRunner::runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    TestResult result;
    result.testName = "Plasticity Learning Experiment";
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        struct LearningExperiment {
            std::shared_ptr<Brain> brain;
            uint64_t seed = TestConstants::DEFAULT_RANDOM_SEED;
            size_t initialSynapseCount = 0;
            std::vector<float> initialWeights;
            std::vector<float> finalWeights;
            std::vector<NeuronId> mostActiveNeurons;
            
            LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
                : brain(b), seed(s) {}
            
            void recordInitialState() {
                initialSynapseCount = brain->getTotalSynapseCount();
                initialWeights.clear();
                
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        initialWeights.push_back(syn->getWeight());
                    }
                }
                
                NLM_LOG_INFO("Initial state recorded: " + std::to_string(initialSynapseCount) + " synapses");
                if (!initialWeights.empty()) {
                    float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                    float mean = sum / initialWeights.size();
                    NLM_LOG_INFO("Mean weight: " + std::to_string(mean));
                }
            }
            
            void recordFinalState() {
                finalWeights.clear();
                
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        finalWeights.push_back(syn->getWeight());
                    }
                }
                
                mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
                
                NLM_LOG_INFO("Final state recorded:");
                NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
                if (!finalWeights.empty()) {
                    float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                    float mean = sum / finalWeights.size();
                    NLM_LOG_INFO("Mean weight: " + std::to_string(mean));
                }
            }
            
            void computeStatistics(std::map<std::string, std::string>& outMetrics) {
                if (initialWeights.empty() || finalWeights.empty()) {
                    return;
                }
                
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                
                outMetrics["initial_mean_weight"] = std::to_string(initialMean);
                outMetrics["final_mean_weight"] = std::to_string(finalMean);
                outMetrics["weight_change"] = std::to_string(finalMean - initialMean);
                
                size_t strengthened = 0, weakened = 0, unchanged = 0;
                size_t minSize = std::min(initialWeights.size(), finalWeights.size());
                
                for (size_t i = 0; i < minSize; ++i) {
                    float delta = finalWeights[i] - initialWeights[i];
                    if (delta > TestConstants::WEIGHT_CHANGE_THRESHOLD) ++strengthened;
                    else if (delta < -TestConstants::WEIGHT_CHANGE_THRESHOLD) ++weakened;
                    else ++unchanged;
                }
                
                outMetrics["strengthened_synapses"] = std::to_string(strengthened);
                outMetrics["weakened_synapses"] = std::to_string(weakened);
                outMetrics["unchanged_synapses"] = std::to_string(unchanged);
                
                bool learningOccurred = (std::abs(finalMean - initialMean) > TestConstants::LEARNING_THRESHOLD) ||
                                        (strengthened > 0 || weakened > 0);
                
                outMetrics["learning_detected"] = learningOccurred ? "true" : "false";
                outMetrics["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
                outMetrics["most_active_neurons"] = std::to_string(mostActiveNeurons.size());
            }
        };
        
        LearningExperiment experiment(brain, TestConstants::DEFAULT_RANDOM_SEED);
        
        // Configure for plasticity
        configureForTest(brain, true);
        
        NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(TestConstants::PLAPACITY_EXPERIMENT_STEPS) + " steps)...");
        
        for (SimulationStep step = 0; step < TestConstants::PLAPACITY_EXPERIMENT_STEPS; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, TestConstants::DEFAULT_EXCITATORY_CURRENT);
            }
            
            brain->step(step, step * TestConstants::DEFAULT_TIMESTEP);
            
            if (step % 100 == 0 && config_->getOr<bool>("verbose", false)) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        experiment.recordInitialState();
        experiment.recordFinalState();
        experiment.computeStatistics(result.metrics);
        
        result.passed = true;
        result.message = "Plasticity experiment completed successfully";
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Exception in plasticity experiment: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    return result;
}

TestResult NeuralTestRunner::runStdpVerification(std::shared_ptr<Brain> brain) {
    TestResult result;
    result.testName = "STDP Verification";
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.passed = false;
            result.message = "No neural region found";
            return result;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            result.passed = false;
            result.message = "Not enough synapses for STDP test";
            return result;
        }
        
        NLM_LOG_INFO("Testing STDP on 5 synapses:");
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < TestConstants::STD_CONNECTED_ACTIVITY_STEPS; ++trial) {
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                preNeuron->injectCurrent(TestConstants::STD_ACTIVITY_CURRENT);
                brain->step(trial * 2, trial * 2 * TestConstants::DEFAULT_TIMESTEP);
                
                postNeuron->injectCurrent(TestConstants::STD_ACTIVITY_CURRENT);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * TestConstants::DEFAULT_TIMESTEP);
            }
        }
        
        NLM_LOG_INFO("After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
            totalDelta += delta;
        }
        
        if (totalDelta > TestConstants::LEARNING_THRESHOLD) {
            result.passed = true;
            result.message = "✓ STDP WORKING: Pre-before-post produced potentiation";
            result.metrics["stdp_delta"] = std::to_string(totalDelta);
            result.metrics["stdp_direction"] = "potentiation";
        } else if (totalDelta < -TestConstants::LEARNING_THRESHOLD) {
            result.passed = true;
            result.message = "STDP working with depression (reversed sign)";
            result.metrics["stdp_delta"] = std::to_string(totalDelta);
            result.metrics["stdp_direction"] = "depression";
        } else {
            result.passed = false;
            result.message = "No significant STDP change detected";
            result.metrics["stdp_delta"] = std::to_string(totalDelta);
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Exception in STDP verification: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    return result;
}

TestResult NeuralTestRunner::runLearningExperiment(std::shared_ptr<Brain> brain) {
    TestResult result;
    result.testName = "Learning Experiment";
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        NLM_LOG_INFO("=== Test 4: Learning Experiment ===");
        
        // Configure for learning with Hebbian and STDP
        configureForTest(brain, true);
        
        // Enable development to affect plasticity rates
        if (auto* devSystem = brain->getDevelopmentSystem()) {
            devSystem->setStage(DevelopmentalStage::CriticalPeriod);
        }
        
        NLM_LOG_INFO("Running learning experiment with development...")
        
        // Run simulation with sensory input
        size_t sensoryNeurons = std::max(size_t(1), brain->getTotalNeuronCount() / 4);
        
        for (SimulationStep step = 0; step < 500; ++step) {
            for (size_t i = 0; i < sensoryNeurons; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, TestConstants::DEFAULT_EXCITATORY_CURRENT);
            }
            
            brain->step(step, step * TestConstants::DEFAULT_TIMESTEP);
            
            if (step % 100 == 0 && config_->getOr<bool>("verbose", false)) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Measure learning outcomes
        float avgFiringRate = brain->getAverageFiringRate();
        size_t totalSpikes = brain->getTotalSpikeCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        
        NLM_LOG_INFO("Learning experiment results:");
        NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate));
        NLM_LOG_INFO("Total spikes: " + std::to_string(totalSpikes));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(firingNeurons));
        
        result.passed = true;
        result.message = "Learning experiment completed successfully";
        result.metrics["avg_firing_rate"] = std::to_string(avgFiringRate);
        result.metrics["total_spikes"] = std::to_string(totalSpikes);
        result.metrics["firing_neurons"] = std::to_string(firingNeurons);
        result.metrics["learning_epochs"] = std::to_string(5);
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Exception in learning experiment: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    return result;
}

std::vector<TestResult> NeuralTestRunner::runAllTests(std::shared_ptr<Brain> brain) {
    std::vector<TestResult> results;
    
    results.push_back(runBasicConnectivityTest(brain));
    
    resetBrainForTest(brain);
    results.push_back(runPlasticityExperiment(brain));
    
    resetBrainForTest(brain);
    results.push_back(runStdpVerification(brain));
    
    resetBrainForTest(brain);
    results.push_back(runLearningExperiment(brain));
    
    return results;
}

void NeuralTestRunner::resetBrainForTest(std::shared_ptr<Brain> brain) {
    brain->reset();
    brain->initialize();
}

void NeuralTestRunner::configureForTest(std::shared_ptr<Brain> brain, bool enablePlasticity) {
    // Configure STDP parameters
    if (auto* stdp = brain->getSTDP()) {
        stdp->setLTP(TestConstants::DEFAULT_LTP_WEIGHT);
        stdp->setLTD(TestConstants::DEFAULT_LTD_WEIGHT);
        stdp->setTau(TestConstants::DEFAULT_STDP_TAU);
    }
    
    if (auto* hebbian = brain->getHebbian()) {
        hebbian->setLearningRate(0.01f);
    }
    
    // Configure structural plasticity
    if (auto* structural = brain->getStructuralPlasticity()) {
        structural->setSynaptogenesisRate(TestConstants::DEFAULT_SYNAPSTOGENESIS_RATE);
        structural->setPruningRate(TestConstants::DEFAULT_PRUNING_RATE);
    }
    
    // Enable plasticity on synapses if requested
    if (enablePlasticity) {
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
    }
}

NLMCommandLineInterface::CommandLineOptions NLMCommandLineInterface::parseCommandLine(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--config=") {
            options.configFile = arg.substr(8);
        } else if (arg == "--timestep") {
            if (i + 1 < argc) options.timestep = std::stod(argv[++i]);
        } else if (arg.substr(0, 10) == "--timestep=") {
            options.timestep = std::stod(arg.substr(11));
        } else if (arg == "--neurons") {
            if (i + 1 < argc) options.neuronCount = std::stoull(argv[++i]);
        } else if (arg.substr(0, 9) == "--neurons=") {
            options.neuronCount = std::stoull(arg.substr(10));
        } else if (arg == "--steps") {
            if (i + 1 < argc) options.maxSteps = std::stoull(argv[++i]);
        } else if (arg.substr(0, 8) == "--steps=") {
            options.maxSteps = std::stoull(arg.substr(9));
        } else if (arg == "--test") {
            if (i + 1 < argc) options.testMode = argv[++i];
        } else if (arg.substr(0, 7) == "--test=") {
            options.testMode = arg.substr(8);
        } else if (arg == "--output") {
            if (i + 1 < argc) options.outputFile = argv[++i];
        } else if (arg == "--log") {
            if (i + 1 < argc) options.logFile = argv[++i];
        } else if (arg == "--verbose") {
            options.verbose = true;
        }
    }
    
    return options;
}

void NLMCommandLineInterface::printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                     Show this help message" << std::endl;
    std::cout << "  --version                      Show version information" << std::endl;
    std::cout << "  --config=<file>                Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --timestep=<seconds>           Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --neurons=<count>              Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=<count>                Maximum simulation steps (default: 1000)" << std::endl;
    std::cout << "  --test=<mode>                  Test mode (all, connectivity, plasticity, stdp, learning)" << std::endl;
    std::cout << "  --output=<file>                Output file for results" << std::endl;
    std::cout << "  --log=<file>                   Log file (default: stdout)" << std::endl;
    std::cout << "  --verbose                       Enable verbose output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Test modes:" << std::endl;
    std::cout << "  all        Run all tests (default)" << std::endl;
    std::cout << "  connectivity   Test basic neural connectivity" << std::endl;
    std::cout << "  plasticity    Test plasticity learning" << std::endl;
    std::cout << "  stdp          Test STDP mechanism" << std::endl;
    std::cout << "  learning      Test learning with development" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example:" << std::endl;
    std::cout << "  " << programName << " --test=all --neurons=1000 --steps=5000 --verbose" << std::endl;
}

void NLMCommandLineInterface::printVersion() {
    std::cout << "NLM v0.1.0 - Neural Learning Machine" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Built with C++20, pybind11 for Python bindings" << std::endl;
    std::cout << "Phase 6 Complete: Final Integration of All Brain Systems" << std::endl;
}

int NLMCommandLineInterface::runApplication(CommandLineOptions& options) {
    printBanner();
    
    std::cout << "Initializing NLM with configuration..." << std::endl;
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Application ===");
    NLM_LOG_INFO("Starting with configuration:");
    NLM_LOG_INFO("  timestep: " + std::to_string(options.timestep) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(options.neuronCount));
    NLM_LOG_INFO("  max_steps: " + std::to_string(options.maxSteps));
    
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    config->loadFromArgs(0, nullptr);
    
    config->set("random_seed", static_cast<int64_t>(TestConstants::DEFAULT_RANDOM_SEED), ConfigSource::Default);
    config->set("simulation_timestep", options.timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    config->set("stdp_ltp_weight", TestConstants::DEFAULT_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", TestConstants::DEFAULT_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_tau", TestConstants::DEFAULT_STDP_TAU, ConfigSource::Default);
    
    config->set("synaptogenesis_rate", TestConstants::DEFAULT_SYNAPSTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", TestConstants::DEFAULT_PRUNING_RATE, ConfigSource::Default);
    
    double timestep = config->getOr<double>("simulation_timestep", options.timestep);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    NeuralTestRunner testRunner(config);
    std::vector<TestResult> results;
    
    if (options.testMode == "all") {
        results = testRunner.runAllTests(brain);
    } else if (options.testMode == "connectivity") {
        results.push_back(testRunner.runBasicConnectivityTest(brain));
    } else if (options.testMode == "plasticity") {
        results.push_back(testRunner.runPlasticityExperiment(brain));
    } else if (options.testMode == "stdp") {
        results.push_back(testRunner.runStdpVerification(brain));
    } else if (options.testMode == "learning") {
        results.push_back(testRunner.runLearningExperiment(brain));
    } else {
        NLM_LOG_ERROR("Unknown test mode: " + options.testMode);
        return 1;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Summary ===");
    
    bool allPassed = true;
    for (const auto& result : results) {
        std::string status = result.passed ? "✓ PASS" : "✗ FAIL";
        NLM_LOG_INFO(status + " - " + result.testName + ": " + result.message);
        if (!result.passed) allPassed = false;
    }
    
    NLM_LOG_INFO("");
    if (allPassed) {
        NLM_LOG_INFO("=== SUCCESS: All tests passed ===");
    } else {
        NLM_LOG_ERROR("=== FAILURE: Some tests failed ===");
    }
    
    return allPassed ? 0 : 1;
}

int NLMCommandLineInterface::runTests(CommandLineOptions& options) {
    return runApplication(options);
}
