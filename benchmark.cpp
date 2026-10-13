// Performance Benchmark
// Benchmarks for NLM system performance

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

namespace benchmark {

void benchmarkConfigSystem() {
    std::cout << "=== Config System Benchmark ===" << std::endl;
    
    // Configuration creation benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        nlm::Config config;
        config.set("param_" + std::to_string(i), i);
        auto val = config.getOr<int>("param_" + std::to_string(i), -1);
        if (val != i) {
            std::cout << "Error: Benchmark failed" << std::endl;
            return;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double opsPerSecond = (10000.0 * 1000.0) / duration.count();
    std::cout << "Config Operations: 10,000 iterations in " << duration.count() << " ms (" 
              << opsPerSecond << " ops/sec)" << std::endl;
    
    // Configuration validation benchmark
    startTime = std::chrono::high_resolution_clock::now();
    
    nlm::ConfigSchema schema;
    schema.addField("test_param", 0, "Test parameter", nullptr, true);
    
    nlm::Config config;
    config.set("test_param", 42);
    
    for (int i = 0; i < 1000; ++i) {
        assert(config.validate(schema));
    }
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Validation Operations: 1,000 validations in " << duration.count() << " ms (" 
              << (1000.0 / (duration.count() / 1000.0)) << " validations/sec)" << std::endl;
}

void benchmarkBrainSimulation() {
    std::cout << "\n=== Brain Simulation Benchmark ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning); // Suppress info for benchmark
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Configure brain for benchmarking
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    // Brain initialization benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 50; ++i) {
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double opsPerSecond = (50.0 * 1000.0) / duration.count();
    std::cout << "Brain Initialization: 50 brains in " << duration.count() << " ms (" 
              << opsPerSecond << " brains/sec)" << std::endl;
    
    // Simulation step benchmark
    startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (1000.0 * 1000.0) / duration.count();
    std::cout << "Simulation Steps: 1,000 steps in " << duration.count() << " ms (" 
              << stepsPerSecond << " steps/sec)" << std::endl;
}

void benchmarkMemoryUsage() {
    std::cout << "\n=== Memory Usage Benchmark ===" << std::endl;
    
    // Large-scale configuration benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::Config config;
    
    for (int i = 0; i < 10000; ++i) {
        config.set("large_key_" + std::to_string(i), std::string("value_" + std::to_string(i)));
        config.set("large_int_" + std::to_string(i), i * 2);
        config.set("large_float_" + std::to_string(i), i * 1.5f);
    }
    
    // Verify all entries
    for (int i = 0; i < 10000; ++i) {
        assert(config.has("large_key_" + std::to_string(i)));
        assert(config.has("large_int_" + std::to_string(i)));
        assert(config.has("large_float_" + std::to_string(i)));
    }
    
    // Generate summary (memory-intensive operation)
    for (int i = 0; i < 10; ++i) {
        std::string summary = config.summary();
        assert(summary.find("30000 entries") != std::string::npos);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Large Config Operations: 30,000 entries in " << duration.count() << " ms" << std::endl;
}

void benchmarkThreading() {
    std::cout << "\n=== Threading Benchmark ===" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    std::atomic<size_t> totalOperations{0};
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&, i]() {
            nlm::Config config;
            for (int j = 0; j < 1000; ++j) {
                config.set("thread_key_" + std::to_string(i) + "_" + std::to_string(j), j);
                auto val = config.getOr<int>("thread_key_" + std::to_string(i) + "_" + std::to_string(j), -1);
                if (val == j) {
                    totalOperations++;
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Threading Operations: " << totalOperations.load() << " successful operations in " 
              << duration.count() << " ms (" << totalOperations.load() * 1000.0 / duration.count() 
              << " ops/sec)" << std::endl;
}

void runAll() {
    benchmarkConfigSystem();
    benchmarkBrainSimulation();
    benchmarkMemoryUsage();
    benchmarkThreading();
    
    std::cout << "\n=== All benchmarks completed! ===" << std::endl;
}

} // namespace benchmark