// Simulation utilities implementation
#include "NLMPythonSimulationUtils.h"
#include "NLMPythonTypes.h"
#include <map>
#include <functional>
#include <chrono>

// Simulation statistics tracker
class SimulationStats {
public:
    struct Stats {
        size_t steps = 0;
        size_t totalSpikes = 0;
        size_t firingNeurons = 0;
        double averageFiringRate = 0.0;
        double timeElapsed = 0.0;
        std::map<size_t, size_t> spikeHistory;
    };
    
    void reset() {
        stats.steps = 0;
        stats.totalSpikes = 0;
        stats.firingNeurons = 0;
        stats.averageFiringRate = 0.0;
        stats.timeElapsed = 0.0;
        stats.spikeHistory.clear();
    }
    
    void step(const std::shared_ptr<Brain>& brain, double timestep) {
        stats.steps++;
        stats.timeElapsed += timestep;
        
        if (brain) {
            stats.totalSpikes = brain->getTotalSpikeCount();
            stats.firingNeurons = brain->getFiringNeuronCount();
            stats.averageFiringRate = brain->getAverageFiringRate();
            stats.spikeHistory[stats.steps] = stats.totalSpikes;
        }
    }
    
    const Stats& getStats() const { return stats; }
    
    py::dict toDict() const {
        py::dict result;
        result["steps"] = stats.steps;
        result["total_spikes"] = stats.totalSpikes;
        result["firing_neurons"] = stats.firingNeurons;
        result["average_firing_rate"] = stats.averageFiringRate;
        result["time_elapsed"] = stats.timeElapsed;
        return result;
    }
    
private:
    Stats stats;
};

// Simulation runner with callbacks
class SimulationRunner {
public:
    using ProgressCallback = std::function<void(size_t step, const SimulationStats::Stats& stats)>;
    using StepCallback = std::function<void(size_t step, double time, const std::shared_ptr<Brain>& brain)>;
    
    SimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world = nullptr,
                     std::shared_ptr<AgentBrain> agent = nullptr)
        : brain(brain), world(world), agent(agent), currentStep(0), currentTime(0.0) {}
    
    void runSteps(size_t numSteps, double timestep = 0.001, 
                  const ProgressCallback& progressCallback = nullptr,
                  const StepCallback& stepCallback = nullptr) {
        if (!brain) return;
        
        SimulationStats stats;
        
        for (size_t step = 0; step < numSteps; ++step) {
            currentStep = step;
            currentTime = step * timestep;
            
            // Run simulation step
            brain->step(step, currentTime);
            
            // World update if available
            if (world) {
                world->update(timestep);
                if (agent) {
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, currentTime);
                }
            }
            
            // Update statistics
            stats.step(brain, timestep);
            
            // Call callbacks
            if (progressCallback) {
                progressCallback(step, stats.getStats());
            }
            if (stepCallback) {
                stepCallback(step, currentTime, brain);
            }
        }
    }
    
    // Batch simulation runs
    std::vector<SimulationStats::Stats> runBatch(size_t numRuns, size_t stepsPerRun, 
                                                 double timestep = 0.001) {
        std::vector<SimulationStats::Stats> results;
        
        for (size_t run = 0; run < numRuns; ++run) {
            if (brain) {
                brain->reset();
            }
            
            SimulationStats stats;
            
            for (size_t step = 0; step < stepsPerRun; ++step) {
                if (brain) {
                    brain->step(step, step * timestep);
                    stats.step(brain, timestep);
                }
            }
            
            results.push_back(stats.getStats());
        }
        
        return results;
    }
    
    // Run until a condition is met
    size_t runUntilCondition(std::function<bool(const std::shared_ptr<Brain>&, double)> condition,
                             double timestep = 0.001) {
        if (!brain) return 0;
        
        SimulationStats stats;
        size_t step = 0;
        
        while (!condition(brain, step * timestep)) {
            brain->step(step, step * timestep);
            if (world) {
                world->update(timestep);
            }
            stats.step(brain, timestep);
            step++;
            
            // Safety check to prevent infinite loops
            if (step > 1000000) {
                break;
            }
        }
        
        return step;
    }
    
    // Get current statistics
    SimulationStats::Stats getCurrentStats() {
        SimulationStats stats;
        if (brain) {
            stats.step(brain, 0.001);
        }
        return stats.getStats();
    }
    
    // Reset simulation
    void reset() {
        if (brain) {
            brain->reset();
        }
        currentStep = 0;
        currentTime = 0.0;
    }
    
    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
    // Get underlying world
    std::shared_ptr<SimpleWorld> getWorld() const { return world; }
    
    // Get underlying agent
    std::shared_ptr<AgentBrain> getAgent() const { return agent; }
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    size_t currentStep;
    double currentTime;
};

void registerSimulationUtils(py::module_& m) {
    // Simulation statistics tracker
    py::class_<SimulationStats>(m, "SimulationStats", "Track simulation statistics and progress")
        .def(py::init<>())
        .def("reset", &SimulationStats::reset, "Reset statistics tracker")
        .def("step", &SimulationStats::step, py::arg("brain"), py::arg("timestep"),
             "Update statistics with brain simulation step")
        .def("to_dict", &SimulationStats::toDict, "Convert statistics to dictionary");
    
    // Simulation runner with callbacks
    py::class_<SimulationRunner>(m, "SimulationRunner", 
                                 "Run simulations with progress tracking and callbacks")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world") = nullptr, py::arg("agent") = nullptr,
             "Initialize simulation runner with brain, optional world and agent")
        .def("run_steps", &SimulationRunner::runSteps,
             py::arg("num_steps"), py::arg("timestep") = 0.001,
             py::arg("progress_callback") = nullptr,
             py::arg("step_callback") = nullptr,
             "Run simulation for specified number of steps with optional callbacks")
        .def("run_batch", &SimulationRunner::runBatch,
             py::arg("num_runs"), py::arg("steps_per_run"), py::arg("timestep") = 0.001,
             "Run multiple simulation trials and return results")
        .def("run_until_condition", &SimulationRunner::runUntilCondition,
             py::arg("condition"), py::arg("timestep") = 0.001,
             "Run simulation until specified condition is met")
        .def("get_current_stats", &SimulationRunner::getCurrentStats,
             "Get current simulation statistics")
        .def("reset", &SimulationRunner::reset, "Reset simulation to initial state")
        .def("get_brain", &SimulationRunner::getBrain,
             "Get the underlying brain instance")
        .def("get_world", &SimulationRunner::getWorld,
             "Get the underlying world instance (if any)")
        .def("get_agent", &SimulationRunner::getAgent,
             "Get the underlying agent instance (if any)");
    
    // Utility functions for simulation control
    m.def("run_simulation_loop", [](std::shared_ptr<Brain> brain, size_t steps, 
                                   std::shared_ptr<SimpleWorld> world = nullptr,
                                   std::shared_ptr<AgentBrain> agent = nullptr,
                                   double timestep = 0.001) {
        SimulationRunner runner(brain, world, agent);
        
        std::vector<SimulationStats::Stats> stats;
        
        auto progressCallback = [&stats](size_t step, const SimulationStats::Stats& stepStats) {
            stats.push_back(stepStats);
        };
        
        runner.runSteps(steps, timestep, progressCallback);
        
        return stats;
    }, py::arg("brain"), py::arg("steps"), py::arg("world") = nullptr,
       py::arg("agent") = nullptr, py::arg("timestep") = 0.001,
       "Run a complete simulation with optional world and agent integration");

    // Convenience simulation utilities
    m.def("create_simulation", [](std::shared_ptr<Brain> brain) {
        return SimulationRunner(brain);
    }, py::arg("brain"), "Create a simulation runner for the given brain");

    m.def("create_simulation_with_world", [](std::shared_ptr<Brain> brain, 
                                             std::shared_ptr<SimpleWorld> world,
                                             std::shared_ptr<AgentBrain> agent) {
        return SimulationRunner(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent") = nullptr,
       "Create a simulation runner with world and optional agent integration");

    // Simulation condition helpers
    m.def("condition_step_count", [](size_t targetSteps) {
        return [targetSteps](const std::shared_ptr<Brain>&, double time) {
            return static_cast<size_t>(time / 0.001) >= targetSteps;
        };
    }, py::arg("target_steps"), "Create a condition that triggers after specified step count");

    m.def("condition_max_spikes", [](size_t maxSpikes) {
        return [maxSpikes](const std::shared_ptr<Brain>& brain, double time) {
            return brain && brain->getTotalSpikeCount() >= maxSpikes;
        };
    }, py::arg("max_spikes"), "Create a condition that triggers after reaching max spike count");

    m.def("condition_time_elapsed", [](double maxTime) {
        return [maxTime](const std::shared_ptr<Brain>&, double time) {
            return time >= maxTime;
        };
    }, py::arg("max_time"), "Create a condition that triggers after elapsed time exceeds limit");
}
