// Performance Tests
// Comprehensive performance benchmarks and stress tests for NLM systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace test_performance {

// Benchmark helper functions
struct BenchmarkResult {
    std::string name;
    size_t neurons;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double memoryUsage;    // approximate
};

BenchmarkResult benchmarkBrain(size_t neuronCount, size_t steps, const std::string& testName) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (size_t step = 0; step < steps; ++step) {
        // Inject some input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
        
        brain.step(step, step * 0.001);
    }
    
    // End timing
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results
    BenchmarkResult result;
    result.name = testName;
    result.neurons = brain.getTotalNeuronCount();
    result.synapses = brain.getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain.getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    result.memoryUsage = static_cast<double>(result.neurons) * 0.001; // Approximate
    
    return result;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                              NLM Performance Benchmarks                             ║" << std::endl;
    std::cout << "╠════════════════════════════════╦════════════════╦════════════════════╦════════════════════════╣" << std::endl;
    std::cout << "║ Test Name                     ║ Neurons       ║ Steps/sec         ║ Spikes/step          ║" << std::endl;
    std::cout << "╠════════════════════════════════╬════════════════╬════════════════════╬════════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::left << std::setw(30) << r.name
                  << " ║ " << std::setw(12) << r.neurons
                  << " ║ " << std::setw(18) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════════════════════════╩════════════════╩════════════════════╩════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Performance Tests
void testBasicPerformance() {
    std::vector<BenchmarkResult> results;
    
    // Test different network sizes
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // Small network
        {500, 1000},      // Medium network
        {1000, 500},      // Large network
        {2000, 200},      // Very large network
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        BenchmarkResult result = benchmarkBrain(neurons, steps, 
            "NeuralDynamics - " + std::to_string(neurons) + " neurons");
        results.push_back(result);
    }
    
    printBenchmarkResults(results);
    
    std::cout << "    testBasicPerformance passed" << std::endl;
}

void testStressPerformance() {
    std::cout << "  Stress Test: Large-scale neural networks" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run stress test
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 1000; ++step) {
        // Heavy input
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 50.0f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 30.0f);
        
        brain.step(step, step * 0.001);
        
        // Progress check
        if (step % 200 == 0) {
            std::cout << "    Stress step " << step << " completed" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "  Stress test completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    std::cout << "  Average throughput: " << std::fixed << std::setprecision(0) << 1000.0 / totalTime << " steps/sec" << std::endl;
    
    std::cout << "    testStressPerformance passed" << std::endl;
}

void testMemoryPerformance() {
    std::cout << "  Memory Test: Working with large memory systems" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initialize memory systems
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Performance test for memory operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Store many items in working memory
    for (int i = 0; i < 1000; ++i) {
        std::vector<float> pattern(10);
        for (int j = 0; j < 10; ++j) {
            pattern[j] = static_cast<float>(sin(i * 0.1f + j * 0.05f));
        }
        workingMemory->store(pattern, 1.0f);
    }
    
    // Retrieve items
    for (int i = 0; i < 500; ++i) {
        std::vector<float> retrieved = workingMemory->retrieve();
        assert(retrieved.size() == 10);
    }
    
    // Store episodes in episodic memory
    for (int i = 0; i < 100; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.reward = static_cast<float>(i);
        episode.sensoryState = std::vector<float>(5, static_cast<float>(i) * 0.1f);
        episodicMemory->storeEpisode(episode);
    }
    
    // Retrieve recent episodes
    for (int i = 0; i < 50; ++i) {
        std::vector<const nlm::EpisodicMemoryItem*> episodes = episodicMemory->getRecentEpisodes(5);
        assert(!episodes.empty());
    }
    
    // Performance test for associative memory
    for (int i = 0; i < 200; ++i) {
        std::vector<float> patternA(5);
        std::vector<float> patternB(5);
        for (int j = 0; j < 5; ++j) {
            patternA[j] = static_cast<float>(i * 0.2f + j * 0.1f);
            patternB[j] = static_cast<float>(i * 0.3f + j * 0.05f);
        }
        associativeMemory->associate(patternA, patternB, 0.8f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "  Memory operations completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    std::cout << "  Operations per second: " << std::fixed << std::setprecision(0) << (1000 + 500 + 200) / totalTime << std::endl;
    
    std::cout << "    testMemoryPerformance passed" << std::endl;
}

void testNeuromodulationPerformance() {
    std::cout << "  Neuromodulation Test: Signal processing and response" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predError != nullptr);
    
    // Performance test for neuromodulation updates
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 2000; ++step) {
        // Simulate neuromodulatory signals
        float reward = static_cast<float>(sin(step * 0.05f) + 1.0f) * 0.5f;
        float prediction = static_cast<float>(cos(step * 0.03f));
        float actual = prediction + static_cast<float>(sin(step * 0.1f)) * 0.1f;
        float noveltySignal = static_cast<float>(pow(sin(step * 0.02f), 2));
        
        dopamine->signalReward(reward);
        dopamine->signalRewardPredictionError(actual - prediction);
        
        curiosity->update(noveltySignal, actual - prediction, 0.01f);
        novelty->setLevel(noveltySignal);
        predError->computeError(prediction, actual);
        
        if (step % 400 == 0) {
            std::cout << "    Neuromodulation step " << step << " completed" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "  Neuromodulation processing completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    std::cout << "  Neuromodulation updates per second: " << std::fixed << std::setprecision(0) << 2000.0 / totalTime << std::endl;
    
    std::cout << "    testNeuromodulationPerformance passed" << std::endl;
}

void testDevelopmentPerformance() {
    std::cout << "  Development Test: Plasticity and maturation processes" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Performance test for developmental processes
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 1000; ++step) {
        devSystem->update(&brain, step);
        brain.step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "    Development step " << step << " completed" << std::endl;
            std::cout << "    Current stage: " << devSystem->getStageName() << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "  Developmental processes completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    std::cout << "  Development updates per second: " << std::fixed << std::setprecision(0) << 1000.0 / totalTime << std::endl;
    
    std::cout << "    testDevelopmentPerformance passed" << std::endl;
}

void testPredictionPerformance() {
    std::cout << "  Prediction Test: Temporal and action consequence prediction" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Performance test for prediction systems
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 1500; ++step) {
        // Create realistic sensory sequence
        std::vector<float> sensoryState(5);
        for (int i = 0; i < 5; ++i) {
            sensoryState[i] = static_cast<float>(sin(step * 0.05f + i * 0.1f));
        }
        
        brain.recordSensoryState(sensoryState, step);
        std::vector<float> prediction = brain.generatePrediction(step + 1);
        float error = brain.updateWithObservation(sensoryState, step + 1);
        
        // Test action consequence prediction
        if (step % 5 == 0) {
            std::vector<nlm::ActionType> actions = {nlm::ActionType::MoveForward};
            nlm::PlanningCandidate candidate = brain.evaluateSequence(actions, sensoryState);
        }
        
        if (step % 300 == 0) {
            std::cout << "    Prediction step " << step << " completed" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "  Prediction processes completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    std::cout << "  Prediction updates per second: " << std::fixed << std::setprecision(0) << 1500.0 / totalTime << std::endl;
    
    std::cout << "    testPredictionPerformance passed" << std::endl;
}

void testMemoryManagementPerformance() {
    std::cout << "  Memory Management Test: Resource allocation and cleanup" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Performance test for memory management
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int episode = 0; episode < 20; ++episode) {
        // Store and clear working memory
        for (int i = 0; i < 100; ++i) {
            std::vector<float> pattern(8);
            for (int j = 0; j < 8; ++j) {
                pattern[j] = static_cast<float>(rand() / static_cast<float>(RAND_MAX));
            }
            workingMemory->store(pattern, 1.0f);
        }
        
        workingMemory->clear();
        
        // Store and consolidate episodic memory
        for (int i = 0; i < 50; ++i) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = episode * 100 + i;
            episode.reward = static_cast<float>(i * 2);
            episode.sensoryState = std::vector<float>(3, static_cast<float>(i) * 0.1f);
            episodicMemory->storeEpisode(episode);
        }
        
        episodicMemory->consolidate(0.5f); // Keep episodes with relevance > 0.5
        episodicMemory->clear();
        
        // Store and clear associative memory
        for (int i = 0; i < 30; ++i) {
            std::vector<float> patternA(4);
            std::vector<float> patternB(4);
            for (int j = 0; j < 4; ++j) {
                patternA[j] = static_cast<float>(i * 0.3f + j * 0.1f);
                patternB[j] = static_cast<float>(i * 0.4f + j * 0.05f);
            }
            associativeMemory->associate(patternA, patternB, 0.7f);
        }
        
        associativeMemory->clear();
        
        if (episode % 5 == 0) {
            std::cout << "    Memory management episode " << episode << " completed" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "  Memory management completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    std::cout << "  Memory cycles per second: " << std::fixed << std::setprecision(0) << 20.0 / totalTime << std::endl;
    
    std::cout << "    testMemoryManagementPerformance passed" << std::endl;
}

void testSystemScaling() {
    std::cout << "  System Scaling Test: Performance with increasing complexity" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Test scaling with different configurations
    std::vector<std::tuple<size_t, size_t, size_t, std::string>> scalingTests = {
        {100, 1, 1000, "Minimal System"},
        {500, 2, 1000, "Small System"},
        {1000, 5, 500, "Medium System"},
        {2000, 10, 200, "Large System"},
    };
    
    for (const auto& [neurons, regions, steps, name] : scalingTests) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neurons), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regions), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
            brain.step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BenchmarkResult result;
        result.name = name;
        result.neurons = brain.getTotalNeuronCount();
        result.synapses = brain.getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain.getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        results.push_back(result);
        
        std::cout << "  " << name << ": " << std::fixed << std::setprecision(0) << result.stepsPerSecond << " steps/sec" << std::endl;
    }
    
    printBenchmarkResults(results);
    
    std::cout << "    testSystemScaling passed" << std::endl;
}

void testRealisticWorkload() {
    std::cout << "  Realistic Workload Test: Complex agent-environment interaction" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.08f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create realistic workload with multiple interacting components
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 2000; ++step) {
        // Simulate complex sensory input
        std::vector<float> sensoryInput(8);
        for (int i = 0; i < 8; ++i) {
            sensoryInput[i] = static_cast<float>(sin(step * 0.02f + i * 0.15f) * 0.5f + 0.5f);
        }
        
        brain.receiveSensoryInput(sensoryInput);
        
        // Apply neuromodulation
        auto* dopamine = brain.getDopamine();
        if (dopamine) {
            float reward = static_cast<float>(cos(step * 0.03f)) * 0.5f + 0.5f;
            dopamine->signalReward(reward);
        }
        
        // Update prediction systems
        auto* curiosity = brain.getCuriosity();
        auto* novelty = brain.getNovelty();
        if (curiosity && novelty) {
            float noveltySignal = static_cast<float>(pow(sin(step * 0.01f), 2));
            curiosity->update(noveltySignal, noveltySignal, 0.01f);
            novelty->setLevel(noveltySignal);
        }
        
        // Run main brain step
        brain.step(step, step * 0.001);
        
        // Production and learning
        std::unique_ptr<nlm::Action> action = brain.produceAction();
        if (action) {
            // Process action
        }
        
        // Developmental update
        auto* devSystem = brain.getDevelopmentSystem();
        if (devSystem) {
            devSystem->update(&brain, step);
        }
        
        if (step % 400 == 0) {
            std::cout << "    Realistic workload step " << step << " completed" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "  Realistic workload completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    std::cout << "  Workload throughput: " << std::fixed << std::setprecision(0) << 2000.0 / totalTime << " steps/sec" << std::endl;
    
    std::cout << "    testRealisticWorkload passed" << std::endl;
}

void runAll() {
    std::cout << "Running NLM Performance and Stress Tests..." << std::endl;
    
    try {
        testBasicPerformance();
        testStressPerformance();
        testMemoryPerformance();
        testNeuromodulationPerformance();
        testDevelopmentPerformance();
        testPredictionPerformance();
        testMemoryManagementPerformance();
        testSystemScaling();
        testRealisticWorkload();
        
        std::cout << "  All Performance and Stress tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_performance