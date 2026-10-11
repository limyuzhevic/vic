// Python Integration Benchmarks
// Performance benchmarks for Python bindings

#include <benchmark/benchmark.h>
#include "core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <cmath>

namespace nlm::tests {

// Python benchmark for Config class
static void BM_ConfigConstructor(benchmark::State& state) {
    for (auto _ : state) {
        auto config = std::make_shared<Config>();
        benchmark::DoNotOptimize(config);
    }
}

static void BM_ConfigSetGet(benchmark::State& state) {
    for (auto _ : state) {
        auto config = std::make_shared<Config>();
        
        for (int i = 0; i < 100; ++i) {
            config->set(std::to_string(i), i);
            auto val = config->get<int>(std::to_string(i));
            benchmark::DoNotOptimize(val);
        }
    }
}

static void BM_ConfigJsonSerialization(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    
    for (int i = 0; i < 100; ++i) {
        config->set(std::to_string(i), i);
    }
    
    for (auto _ : state) {
        std::string json_str = config->toJson();
        benchmark::DoNotOptimize(json_str);
    }
}

// Python benchmark for Brain class
static void BM_BrainCreation(benchmark::State& state) {
    for (auto _ : state) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", state.range(0));
        config->set("region_count", 2);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        benchmark::DoNotOptimize(brain);
    }
}

static void BM_BrainStep(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        brain->step(state.range(1));
    }
}

static void BM_BrainAction(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        brain->step(state.range(1));
        auto action = brain->produceAction();
        benchmark::DoNotOptimize(action);
    }
}

// Python benchmark for Visualization
static void BM_VisualizationCreate(benchmark::State& state) {
    for (auto _ : state) {
        auto visualization = std::make_shared<VisualizationInterface>();
        visualization->initialize();
        benchmark::DoNotOptimize(visualization);
    }
}

static void BM_VisualizationUpdate(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto visualization = std::make_shared<VisualizationInterface>();
    visualization->initialize();
    
    for (auto _ : state) {
        brain->step(state.range(0));
        visualization->update();
    }
}

static void BM_VisualizationRender(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto visualization = std::make_shared<VisualizationInterface>();
    visualization->initialize();
    
    for (auto _ : state) {
        visualization->render();
    }
}

// Python benchmark for AgentBrain
static void BM_AgentBrainProcess(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(800, 600, 400, 300);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    auto percept = world->getSensoryPercept();
    
    for (auto _ : state) {
        agent->processSensoryInput(percept);
    }
}

// Register benchmarks
BENCHMARK(BM_ConfigConstructor);
BENCHMARK(BM_ConfigSetGet);
BENCHMARK(BM_ConfigJsonSerialization);

BENCHMARK(BM_BrainCreation)->Arg(100)->Arg(500)->Arg(1000);
BENCHMARK(BM_BrainStep)->Arg(0)->Arg(100)->Arg(500)->Arg(1000);
BENCHMARK(BM_BrainAction)->Arg(100)->Arg(500)->Arg(1000);

BENCHMARK(BM_VisualizationCreate);
BENCHMARK(BM_VisualizationUpdate)->Arg(0)->Arg(100)->Arg(500);
BENCHMARK(BM_VisualizationRender);

BENCHMARK(BM_AgentBrainProcess);

// Performance benchmark for Python integration
static void BM_PythonIntegration(benchmark::State& state) {
    // Simulate Python integration overhead
    for (auto _ : state) {
        // Python-style operations
        auto config = std::make_shared<Config>();
        config->set("test", "value");
        config->set("count", 42);
        
        // Simulate Python binding overhead
        std::string repr = config->__repr__();
        benchmark::DoNotOptimize(repr);
        
        // Simulate Python list operations
        std::vector<std::string> keys = config->getKeys();
        benchmark::DoNotOptimize(keys);
    }
}

BENCHMARK(BM_PythonIntegration);

} // namespace nlm::tests
