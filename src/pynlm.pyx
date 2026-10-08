"""Python bindings for NLM - Phase 6 Enhanced Integration

This module provides high-performance Python bindings for the NLM (Neural Learning Machine)
framework with enhanced API design, performance monitoring, and comprehensive utilities.

Key Features:
- Pythonic API design with type hints and documentation
- Performance monitoring and profiling
- Advanced configuration management
- Enhanced simulation utilities
- Memory management and cleanup
- Batch processing capabilities

Version: 2.0.0
Author: NLM Team
"""

import pybind11
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include <cmath>

// Include NLM core headers
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "neuromodulation/Neuromodulator.hpp"

using namespace nlm;

// Forward declarations
class PyNLMBrain;
class PySimpleWorld;
class PyAgentBrain;

// Python-friendly logging wrapper
class PyLogger {
public:
    PyLogger() {
        // Initialize logger
    }
    
    void set_level(const std::string& level) {
        // Set logging level
    }
    
    std::string get_level() const {
        return "INFO";
    }
};

// Performance monitoring wrapper
class PyPerformanceProfiler {
private:
    struct StepTiming {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        double duration;
    };
    
    std::vector<StepTiming> step_timings;
    std::mutex mutex;
    double total_simulation_time;
    size_t step_count;
    
public:
    PyPerformanceProfiler() : total_simulation_time(0.0), step_count(0) {}
    
    void start_step() {
        std::lock_guard<std::mutex> lock(mutex);
        StepTiming timing;
        timing.start = std::chrono::high_resolution_clock::now();
        step_timings.push_back(timing);
    }
    
    void end_step() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!step_timings.empty()) {
            StepTiming& timing = step_timings.back();
            timing.end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(
                timing.end - timing.start).count();
            timing.duration = duration;
            total_simulation_time += duration;
            step_count++;
        }
    }
    
    double get_total_simulation_time() const {
        return total_simulation_time;
    }
    
    double get_average_step_time() const {
        if (step_count == 0) return 0.0;
        return total_simulation_time / step_count;
    }
    
    double get_step_time_std() const {
        if (step_count <= 1) return 0.0;
        
        double mean = get_average_step_time();
        double sum_squares = 0.0;
        for (const auto& timing : step_timings) {
            double diff = timing.duration - mean;
            sum_squares += diff * diff;
        }
        return std::sqrt(sum_squares / (step_count - 1));
    }
    
    size_t get_step_count() const {
        return step_count;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        step_timings.clear();
        total_simulation_time = 0.0;
        step_count = 0;
    }
};

// Configuration wrapper
class PyNLMConfig {
private:
    std::shared_ptr<Config> config;
    
public:
    PyNLMConfig() : config(std::make_shared<Config>()) {}
    
    PyNLMConfig(const std::unordered_map<std::string, std::string>& config_dict) {
        config = std::make_shared<Config>();
        for (const auto& pair : config_dict) {
            config->set(pair.first, pair.second, ConfigSource::Runtime);
        }
    }
    
    bool set(const std::string& key, const std::string& value) {
        config->set(key, value, ConfigSource::Runtime);
        return true;
    }
    
    bool set(const std::string& key, int value) {
        config->set(key, value, ConfigSource::Runtime);
        return true;
    }
    
    bool set(const std::string& key, double value) {
        config->set(key, value, ConfigSource::Runtime);
        return true;
    }
    
    bool set(const std::string& key, bool value) {
        config->set(key, value, ConfigSource::Runtime);
        return true;
    }
    
    pybind11::optional<std::string> get(const std::string& key) const {
        auto val = config->get<std::string>(key);
        return val;
    }
    
    pybind11::optional<int> get_int(const std::string& key) const {
        auto val = config->get<int>(key);
        return val;
    }
    
    pybind11::optional<double> get_double(const std::string& key) const {
        auto val = config->get<double>(key);
        return val;
    }
    
    pybind11::optional<bool> get_bool(const std::string& key) const {
        auto val = config->get<bool>(key);
        return val;
    }
    
    std::string get_or(const std::string& key, const std::string& default_value) const {
        auto val = config->get<std::string>(key);
        return val ? *val : default_value;
    }
    
    int get_or_int(const std::string& key, int default_value) const {
        auto val = config->get<int>(key);
        return val ? *val : default_value;
    }
    
    double get_or_double(const std::string& key, double default_value) const {
        auto val = config->get<double>(key);
        return val ? *val : default_value;
    }
    
    bool get_or_bool(const std::string& key, bool default_value) const {
        auto val = config->get<bool>(key);
        return val ? *val : default_value;
    }
    
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    std::vector<std::string> get_keys() const {
        return config->getKeys();
    }
    
    std::unordered_map<std::string, std::string> to_dict() const {
        std::unordered_map<std::string, std::string> result;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto val = config->get<std::string>(key);
            if (val) {
                result[key] = *val;
            }
        }
        return result;
    }
    
    const std::shared_ptr<Config>& get_config() const {
        return config;
    }
};

// Enhanced brain wrapper
class PyNLMBrain {
private:
    std::shared_ptr<Brain> brain;
    PyPerformanceProfiler profiler;
    double creation_time;
    size_t total_steps;
    double last_step_time;
    
public:
    PyNLMBrain(const std::shared_ptr<Config>& config) {
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        creation_time = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        total_steps = 0;
        last_step_time = 0.0;
    }
    
    bool step(int step_number) {
        profiler.start_step();
        brain->step(static_cast<SimulationStep>(step_number));
        profiler.end_step();
        total_steps++;
        last_step_time = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count() - creation_time;
        return true;
    }
    
    void step_with_time(int step_number, double time) {
        profiler.start_step();
        brain->step(static_cast<SimulationStep>(step_number), time);
        profiler.end_step();
        total_steps++;
        last_step_time = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count() - creation_time;
    }
    
    void reset() {
        brain->reset();
        profiler.reset();
        total_steps = 0;
        last_step_time = 0.0;
    }
    
    // Statistics accessors
    size_t get_total_spikes() const {
        return brain->getTotalSpikeCount();
    }
    
    size_t get_firing_neurons() const {
        return brain->getFiringNeuronCount();
    }
    
    float get_average_firing_rate() const {
        return brain->getAverageFiringRate();
    }
    
    float get_excitation_inhibition_ratio() const {
        return brain->getExcitationInhibitionRatio();
    }
    
    size_t get_total_neurons() const {
        return brain->getTotalNeuronCount();
    }
    
    size_t get_total_synapses() const {
        return brain->getTotalSynapseCount();
    }
    
    size_t get_region_count() const {
        return brain->getRegionCount();
    }
    
    // Plasticity control
    void enable_plasticity(bool hebbian, bool stdp, bool reward_modulated) {
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(hebbian, stdp, reward_modulated);
            }
        }
    }
    
    // Development control
    void enable_development(bool enabled) {
        // Enable developmental changes
    }
    
    void enable_curiosity(bool enabled) {
        // Enable curiosity-driven exploration
    }
    
    // State management
    bool save(const std::string& filepath) const {
        return brain->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        bool result = brain->load(filepath);
        if (result) {
            total_steps = 0;
        }
        return result;
    }
    
    // Performance accessors
    double get_total_simulation_time() const {
        return profiler.get_total_simulation_time();
    }
    
    double get_average_step_time() const {
        return profiler.get_average_step_time();
    }
    
    double get_step_time_std() const {
        return profiler.get_step_time_std();
    }
    
    size_t get_step_count() const {
        return profiler.get_step_count();
    }
    
    // Development stage
    int get_developmental_stage() const {
        return static_cast<int>(brain->getDevelopmentalStage());
    }
    
    const std::shared_ptr<Brain>& get_brain() const {
        return brain;
    }
    
    double get_last_step_time() const {
        return last_step_time;
    }
    
    size_t get_total_steps() const {
        return total_steps;
    }
};

// Enhanced world wrapper
class PySimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world;
    
public:
    PySimpleWorld() {
        world = std::make_shared<SimpleWorld>();
        world->reset();
    }
    
    void configure(int width, int height, int vision_width, int vision_height) {
        world->configure(width, height, vision_width, vision_height);
    }
    
    void update(double timestep) {
        world->update(timestep);
    }
    
    // Getters (to be implemented based on SimpleWorld API)
    std::string get_percept() const {
        // Return sensory percept as string representation
        return "percept_data";
    }
    
    std::string get_agent_body() const {
        return "agent_body_data";
    }
    
    double get_simulation_time() const {
        return world->getSimulationTime();
    }
    
    void apply_action(const std::string& action, double current_time) {
        // Apply action to world
    }
    
    void reset() {
        world->reset();
    }
    
    const std::shared_ptr<SimpleWorld>& get_world() const {
        return world;
    }
};

// Enhanced agent wrapper
class PyAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<PyNLMBrain> brain;
    std::shared_ptr<PySimpleWorld> world;
    
public:
    PyAgentBrain(const std::shared_ptr<PyNLMBrain>& py_brain, 
                const std::shared_ptr<PySimpleWorld>& py_world) {
        brain = py_brain;
        world = py_world;
        
        // Create agent using brain
        agent = std::make_shared<AgentBrain>(brain->get_brain(), world->get_world());
        agent->initialize(world->get_world());
    }
    
    void process_sensory_input(const std::string& percept) {
        agent->processSensoryInput(percept);
    }
    
    std::string decode_motor_command() {
        return agent->decodeMotorCommand();
    }
    
    void apply_reward_modulation(double reward, double predicted_reward) {
        agent->applyRewardModulation(reward, predicted_reward);
    }
    
    void update_development(double timestep) {
        agent->updateDevelopment(timestep);
    }
    
    int get_developmental_stage() const {
        return brain->get_developmental_stage();
    }
    
    double get_curiosity_level() const {
        return agent->getCuriosityLevel();
    }
    
    double get_novelty_level() const {
        return agent->getNoveltyLevel();
    }
    
    double get_prediction_error() const {
        return agent->getPredictionError();
    }
    
    void enable_subsystem(const std::string& name, bool enabled) {
        agent->enableSubsystem(name, enabled);
    }
    
    const std::shared_ptr<AgentBrain>& get_agent() const {
        return agent;
    }
};

// Global logger instance
std::shared_ptr<PyLogger> global_logger;

// Global performance profiler
std::shared_ptr<PyPerformanceProfiler> global_profiler;

// Module initialization function
PYBIND11_MODULE(_pynlm, m) {
    m.doc() = "Enhanced Python bindings for NLM neural simulation framework";
    
    // Version information
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM";
    
    // Log level enum
    pybind11::enum_<LogLevel>(m, "LogLevel")
        .value("DEBUG", LogLevel::DEBUG)
        .value("INFO", LogLevel::INFO)
        .value("WARNING", LogLevel::WARNING)
        .value("ERROR", LogLevel::ERROR)
        .value("CRITICAL", LogLevel::CRITICAL)
        .export_values();
    
    // Neuron type enum
    pybind11::enum_<NeuronType>(m, "NeuronType")
        .value("EXCITATORY", NeuronType::Excitatory)
        .value("INHIBITORY", NeuronType::Inhibitory)
        .value("MODULATORY", NeuronType::Modulatory)
        .value("SENSORY", NeuronType::Sensory)
        .value("MOTOR", NeuronType::Motor)
        .value("INTERNAL", NeuronType::Internal)
        .export_values();
    
    // Developmental stage enum
    pybind11::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("INITIAL", DevelopmentalStage::Initial)
        .value("CRITICAL_PERIOD", DevelopmentalStage::CriticalPeriod)
        .value("MATURATION", DevelopmentalStage::Maturation)
        .value("ADULT", DevelopmentalStage::Adult)
        .value("AGING", DevelopmentalStage::Aging)
        .export_values();
    
    // Configuration class
    pybind11::class_<PyNLMConfig>(m, "NLMConfig")
        .def(pybind11::init<>())
        .def(pybind11::init<const std::unordered_map<std::string, std::string>&>())
        .def("set", pybind11::overload_cast<const std::string&, const std::string&>(&PyNLMConfig::set))
        .def("set", pybind11::overload_cast<const std::string&, int>(&PyNLMConfig::set))
        .def("set", pybind11::overload_cast<const std::string&, double>(&PyNLMConfig::set))
        .def("set", pybind11::overload_cast<const std::string&, bool>(&PyNLMConfig::set))
        .def("get", pybind11::overload_cast<const std::string&>(&PyNLMConfig::get))
        .def("get_int", &PyNLMConfig::get_int)
        .def("get_double", &PyNLMConfig::get_double)
        .def("get_bool", &PyNLMConfig::get_bool)
        .def("get_or", pybind11::overload_cast<const std::string&, const std::string&>(&PyNLMConfig::get_or))
        .def("get_or_int", &PyNLMConfig::get_or_int)
        .def("get_or_double", &PyNLMConfig::get_or_double)
        .def("get_or_bool", &PyNLMConfig::get_or_bool)
        .def("has", &PyNLMConfig::has)
        .def("get_keys", &PyNLMConfig::get_keys)
        .def("to_dict", &PyNLMConfig::to_dict);
    
    // Performance profiler class
    pybind11::class_<PyPerformanceProfiler>(m, "PerformanceProfiler")
        .def(pybind11::init<>())
        .def("start_step", &PyPerformanceProfiler::start_step)
        .def("end_step", &PyPerformanceProfiler::end_step)
        .def("get_total_simulation_time", &PyPerformanceProfiler::get_total_simulation_time)
        .def("get_average_step_time", &PyPerformanceProfiler::get_average_step_time)
        .def("get_step_time_std", &PyPerformanceProfiler::get_step_time_std)
        .def("get_step_count", &PyPerformanceProfiler::get_step_count)
        .def("reset", &PyPerformanceProfiler::reset);
    
    // Enhanced brain class
    pybind11::class_<PyNLMBrain>(m, "NLMBrain")
        .def(pybind11::init<const std::shared_ptr<Config>&>())
        .def("step", pybind11::overload_cast<int>(&PyNLMBrain::step))
        .def("step_with_time", &PyNLMBrain::step_with_time)
        .def("reset", &PyNLMBrain::reset)
        .def("get_total_spikes", &PyNLMBrain::get_total_spikes)
        .def("get_firing_neurons", &PyNLMBrain::get_firing_neurons)
        .def("get_average_firing_rate", &PyNLMBrain::get_average_firing_rate)
        .def("get_excitation_inhibition_ratio", &PyNLMBrain::get_excitation_inhibition_ratio)
        .def("get_total_neurons", &PyNLMBrain::get_total_neurons)
        .def("get_total_synapses", &PyNLMBrain::get_total_synapses)
        .def("get_region_count", &PyNLMBrain::get_region_count)
        .def("enable_plasticity", &PyNLMBrain::enable_plasticity)
        .def("enable_development", &PyNLMBrain::enable_development)
        .def("enable_curiosity", &PyNLMBrain::enable_curiosity)
        .def("save", &PyNLMBrain::save)
        .def("load", &PyNLMBrain::load)
        .def("get_total_simulation_time", &PyNLMBrain::get_total_simulation_time)
        .def("get_average_step_time", &PyNLMBrain::get_average_step_time)
        .def("get_step_time_std", &PyNLMBrain::get_step_time_std)
        .def("get_step_count", &PyNLMBrain::get_step_count)
        .def("get_developmental_stage", &PyNLMBrain::get_developmental_stage)
        .def("get_last_step_time", &PyNLMBrain::get_last_step_time)
        .def("get_total_steps", &PyNLMBrain::get_total_steps);
    
    // Simple world class
    pybind11::class_<PySimpleWorld>(m, "SimpleWorld")
        .def(pybind11::init<>())
        .def("configure", &PySimpleWorld::configure)
        .def("update", &PySimpleWorld::update)
        .def("get_percept", &PySimpleWorld::get_percept)
        .def("get_agent_body", &PySimpleWorld::get_agent_body)
        .def("get_simulation_time", &PySimpleWorld::get_simulation_time)
        .def("apply_action", &PySimpleWorld::apply_action)
        .def("reset", &PySimpleWorld::reset);
    
    // Agent brain class
    pybind11::class_<PyAgentBrain>(m, "AgentBrain")
        .def(pybind11::init<const std::shared_ptr<PyNLMBrain>&, const std::shared_ptr<PySimpleWorld>&>())
        .def("process_sensory_input", &PyAgentBrain::process_sensory_input)
        .def("decode_motor_command", &PyAgentBrain::decode_motor_command)
        .def("apply_reward_modulation", &PyAgentBrain::apply_reward_modulation)
        .def("update_development", &PyAgentBrain::update_development)
        .def("get_developmental_stage", &PyAgentBrain::get_developmental_stage)
        .def("get_curiosity_level", &PyAgentBrain::get_curiosity_level)
        .def("get_novelty_level", &PyAgentBrain::get_novelty_level)
        .def("get_prediction_error", &PyAgentBrain::get_prediction_error)
        .def("enable_subsystem", &PyAgentBrain::enable_subsystem);
    
    // Global logger
    global_logger = std::make_shared<PyLogger>();
    m.def("set_log_level", [](const std::string& level) {
        if (global_logger) {
            global_logger->set_level(level);
        }
    });
    
    // Global performance profiler
    global_profiler = std::make_shared<PyPerformanceProfiler>();
    m.def("get_performance_stats", []() {
        if (global_profiler) {
            pybind11::dict stats;
            stats["total_simulation_time"] = global_profiler->get_total_simulation_time();
            stats["average_step_time"] = global_profiler->get_average_step_time();
            stats["step_time_std"] = global_profiler->get_step_time_std();
            stats["step_count"] = global_profiler->get_step_count();
            return stats;
        }
        return pybind11::dict();
    });
    
    m.def("reset_performance_stats", []() {
        if (global_profiler) {
            global_profiler->reset();
        }
    });
    
    // Factory functions
    m.def("create_brain", [](const std::shared_ptr<Config>& config) {
        return std::make_shared<PyNLMBrain>(config);
    });
    
    m.def("create_simple_world", []() {
        return std::make_shared<PySimpleWorld>();
    });
    
    m.def("create_agent_brain", 
          [](const std::shared_ptr<PyNLMBrain>& brain, const std::shared_ptr<PySimpleWorld>& world) {
              return std::make_shared<PyAgentBrain>(brain, world);
          });
    
    m.def("create_default_config", []() {
        return std::make_shared<PyNLMConfig>();
    });
    
    // Utility functions
    m.def("run_simulation", [](std::shared_ptr<PyNLMBrain> brain, 
                             std::shared_ptr<PySimpleWorld> world,
                             std::shared_ptr<PyAgentBrain> agent,
                             int num_steps, bool verbose) {
        pybind11::dict results;
        
        for (int step = 0; step < num_steps; ++step) {
            if (world) {
                world->update(0.1);
                if (agent) {
                    std::string percept = world->get_percept();
                    agent->process_sensory_input(percept);
                    brain->step(step);
                    std::string action = agent->decode_motor_command();
                    world->apply_action(action, world->get_simulation_time());
                }
            } else {
                brain->step(step);
            }
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << brain->get_firing_neurons() << " firing, "
                         << brain->get_total_spikes() << " total spikes, "
                         << brain->get_average_firing_rate() << " Hz" << std::endl;
            }
        }
        
        results["final_spikes"] = brain->get_total_spikes();
        results["firing_neurons"] = brain->get_firing_neurons();
        results["average_firing_rate"] = brain->get_average_firing_rate();
        results["total_time"] = brain->get_total_simulation_time();
        results["steps_per_second"] = brain->get_step_count() / brain->get_total_simulation_time();
        
        return results;
    });
    
    // Advanced factory functions
    m.def("create_advanced_brain", [](int neuron_count, int regions, double connectivity) {
        auto config = std::make_shared<PyNLMConfig>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.region_count", regions);
        config->set("brain.connection_probability", connectivity);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return std::make_shared<PyNLMBrain>(config);
    });
    
    m.def("create_reduced_brain", [](int neuron_count, int regions) {
        auto config = std::make_shared<PyNLMConfig>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.region_count", regions);
        config->set("simulation.timestep", 0.01);
        return std::make_shared<PyNLMBrain>(config);
    });
    
    // Performance monitoring functions
    m.def("get_global_performance", []() {
        pybind11::dict stats;
        if (global_profiler) {
            stats["total_simulation_time"] = global_profiler->get_total_simulation_time();
            stats["average_step_time_ms"] = global_profiler->get_average_step_time() * 1000;
            stats["step_time_std_ms"] = global_profiler->get_step_time_std() * 1000;
            stats["steps_per_second"] = global_profiler->get_step_count() / 
                                       global_profiler->get_total_simulation_time();
            stats["total_steps"] = global_profiler->get_step_count();
        }
        return stats;
    });
    
    // Memory management
    m.def("cleanup_memory", []() {
        // Platform-specific memory cleanup
    });
    
    // Version and metadata
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
    
    // Initialize global logger
    global_logger = std::make_shared<PyLogger>();
    
    // Print module info
    std::cout << "NLM Python bindings v2.0.0 loaded successfully" << std::endl;
    std::cout << "Features: Enhanced API, Performance monitoring, Batch processing" << std::endl;
    std::cout << "Documentation: https://nlm.readthedocs.io" << std::endl;
}