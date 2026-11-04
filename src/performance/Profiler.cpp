#include "performance/Profiler.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>

namespace nlm {

// Profiler implementation
void NLMProfiler::recordSystemMetrics() {
    CPUMonitor cpu;
    MemoryMonitor memory;
    
    PerformanceMetric cpuMetric("cpu_usage", MetricType::CPU, cpu.getCpuUsage(), "%");
    PerformanceMetric memoryMetric("memory_usage", MetricType::Memory, 
                                  memory.getMemoryUsage(), "%");
    
    SystemProfiler profiler;
    profiler.recordMetric(currentProfile_, cpuMetric);
    profiler.recordMetric(currentProfile_, memoryMetric);
}

// Convenience functions implementation
void example_usage() {
    // Start system profiling
    NLM_PROFILER_START("brain_simulation", "Profile brain simulation performance");
    
    // Start memory tracking
    NLM_PROFILER_START_MEMORY();
    
    // Start code profiling for a specific session
    NLM_PROFILER_START_CODE("neuron_computation");
    
    // Simulate some work
    double totalTime = 0.0;
    for (int i = 0; i < 1000; ++i) {
        // Simulate computation
        double x = std::sin(i * 0.01) * std::cos(i * 0.02);
        totalTime += x;
        
        // Profile individual iterations (commented out to avoid overhead)
        // NLM_PROFILE_FUNCTION("compute_neuron", "src/brain/Neuron.cpp", __LINE__, 0.001, 0.0, 0.0, 1);
    }
    
    // Use scope-based profiling
    NLM_PROFILE_SCOPE(neuron_step)
    {
        // Simulate neuron step computation
        double computation = std::sqrt(2.0) * std::log(10.0);
    }
    
    // Stop profiling
    NLM_PROFILER_STOP;
    NLM_PROFILER_STOP_MEMORY;
    NLM_PROFILER_STOP_CODE;
    
    // Generate report
    std::cout << nlm::NLMProfilerManager::generateReport();
}

// Example of how to use the factory with profiling
void demonstrate_factory_with_profiling() {
    NLM_PROFILER_START("factory_demo", "Demonstrate factory pattern with profiling");
    
    // Use dependency injection container
    auto& container = NLMContainer::getInstance();
    
    // Create brain with default configuration
    auto brain = FactoryUtils::createBrain();
    
    // Create logger
    auto logger = FactoryUtils::getLogger();
    
    // Log something
    logger->info("Brain created with factory pattern");
    
    // Create configuration
    auto config = FactoryUtils::getConfig();
    config->set("random_seed", 12345LL, ConfigSource::Runtime);
    
    // Use STDP plasticity rule
    auto stdp = FactoryUtils::getPlasticityRule<STDP>();
    
    // Simulate some operations
    for (int i = 0; i < 100; ++i) {
        // Simulate neural computation
        double value = std::sin(i * 0.1) * std::cos(i * 0.2);
        logger->debug("Computation step " + std::to_string(i) + ": " + std::to_string(value));
        
        // Profile function calls
        NLM_PROFILE_FUNCTION("compute_neuron_step", "src/brain/Neuron.cpp", __LINE__, 0.001, 0.0, 0.0, 1);
    }
    
    // Stop profiling
    NLM_PROFILER_STOP;
    
    // Generate report
    std::cout << nlm::NLMProfilerManager::generateReport();
}

// Example of advanced component creation
void demonstrate_advanced_components() {
    NLM_PROFILER_START("advanced_components", "Demonstrate advanced component creation");
    
    auto& container = NLMContainer::getInstance();
    
    // Create a custom brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 5000LL, ConfigSource::Default);
    config->set("region_count", 2LL, ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    config->set("simulation_timestep", 0.0005f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.02f, ConfigSource::Default);
    
    // Create brain with custom configuration
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create memory systems
    auto workingMemory = std::make_shared<WorkingMemory>();
    workingMemory->setCapacity(1000);
    workingMemory->setDecayRate(0.9f);
    
    auto episodicMemory = std::make_shared<EpisodicMemory>();
    episodicMemory->setMaxEpisodes(5000);
    episodicMemory->setReplayProbability(0.2f);
    
    // Create neuromodulators
    auto dopamine = std::make_shared<Dopamine>();
    dopamine->setBaseline(0.2f);
    dopamine->setPeak(1.0f);
    
    auto curiosity = std::make_shared<Curiosity>();
    curiosity->setBaseline(0.1f);
    curiosity->setThreshold(0.3f);
    
    // Create development system
    auto development = std::make_shared<DevelopmentSystem>();
    development->setMaturationRate(0.002f);
    
    // Simulate complex interactions
    for (int step = 0; step < 1000; ++step) {
        // Apply neuromodulation
        float dopamineEffect = dopamine->getEffect();
        float curiosityEffect = curiosity->getEffect();
        
        // Update development stage
        development->update(step);
        
        // Simulate neural activity
        double firingRate = 10.0 * std::exp(-step * 0.001);
        
        // Update memory systems
        workingMemory->update(firingRate);
        episodicMemory->encode("episode_" + std::to_string(step), firingRate);
        
        // Profile memory operations
        NLM_PROFILE_FUNCTION("update_memory_systems", "src/memory/WorkingMemory.cpp", __LINE__, 0.0005, 0.0, 0.0, 1);
    }
    
    // Stop profiling
    NLM_PROFILER_STOP;
    
    // Generate report
    std::cout << nlm::NLMProfilerManager::generateReport();
}

// Example of performance benchmarking
void demonstrate_benchmarking() {
    NLM_PROFILER_START("benchmark_neural_network", "Benchmark neural network performance");
    
    SystemProfiler profiler;
    
    // Measure baseline performance
    auto baselineProfile = profiler.startProfile("baseline", "Baseline neural network performance");
    
    // Simulate baseline computation
    double baselineTime = 0.0;
    for (int i = 0; i < 100; ++i) {
        double result = std::sin(i * 0.1) * std::cos(i * 0.2);
        baselineTime += result;
        NLM_PROFILE_FUNCTION("baseline_computation", "benchmark.cpp", __LINE__, 0.001, 0.0, 0.0, 1);
    }
    
    profiler.endProfile(baselineProfile);
    
    // Measure optimized performance
    auto optimizedProfile = profiler.startProfile("optimized", "Optimized neural network performance");
    
    // Simulate optimized computation
    double optimizedTime = 0.0;
    for (int i = 0; i < 100; ++i) {
        double result = std::sin(i * 0.1) * std::cos(i * 0.2);
        optimizedTime += result;
        NLM_PROFILE_FUNCTION("optimized_computation", "benchmark.cpp", __LINE__, 0.0005, 0.0, 0.0, 1);
    }
    
    profiler.endProfile(optimizedProfile);
    
    // Compare profiles
    std::string comparison = profiler.compareProfiles(baselineProfile, optimizedProfile);
    std::cout << comparison;
    
    // Generate analysis report
    std::string analysis = profiler.generateAnalysisReport(baselineProfile, optimizedProfile);
    std::cout << "\nPerformance Analysis:\n" << analysis;
    
    // Stop benchmarking
    NLM_PROFILER_STOP;
}

} // namespace nlm