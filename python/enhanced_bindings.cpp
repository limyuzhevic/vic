// Enhanced NLM Python API - Enhanced Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

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

// Forward declarations for helper functions
class PythonBrain;
class PythonWorld;
class PythonAgent;

// ========== HIGH-LEVEL ABSTRACTION CLASSES ==========

class PythonBrain {
public:
    std::shared_ptr<Brain> brain_;
    
    PythonBrain(std::shared_ptr<Config> config = nullptr) {
        if (!config) config = std::make_shared<Config>();
        brain_ = std::make_shared<Brain>(config);
    }
    
    // Simple initialization with one-liner
    void initialize() {
        brain_->initialize();
    }
    
    // Run simulation for N steps
    void simulate(int steps) {
        for (int i = 0; i < steps; ++i) {
            brain_->step(i);
        }
    }
    
    // One-step simulation with optional sensory input
    void step(int step = 0, const std::vector<float>& input = {}) {
        if (!input.empty()) {
            // Create a simple vision input
            auto vision = std::make_shared<Vision>(8, 8, 3);
            vision->setData(input);
            brain_->receiveSensoryInput(*vision);
        }
        brain_->step(step);
    }
    
    // Get key statistics in one call
    std::map<std::string, double> getStats() {
        std::map<std::string, double> stats;
        
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["development_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        
        // Get memory stats if available
        if (brain_->getWorkingMemory()) {
            stats["working_memory_traces"] = brain_->getWorkingMemory()->getActiveTraces();
        }
        if (brain_->getEpisodicMemory()) {
            stats["episodic_memory_episodes"] = brain_->getEpisodicMemory()->getEpisodeCount();
        }
        
        return stats;
    }
    
    // Enable all learning features
    void enableLearning() {
        // This would be implemented with proper AgentBrain interface
        // For now, it's a placeholder for the enhanced API
    }
    
    // Save brain state with automatic filename
    bool save(const std::string& name = "brain_checkpoint") {
        std::string filename = name + "_" + 
            std::to_string(std::time(nullptr)) + ".nlm";
        return brain_->save(filename);
    }
    
    // Get brain as shared pointer for advanced usage
    std::shared_ptr<Brain> getBrain() {
        return brain_;
    }
};

class PythonWorld {
public:
    std::shared_ptr<SimpleWorld> world_;
    
    PythonWorld() {
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(20, 20, 8, 8);
        world_->reset();
    }
    
    // Configure world with simple parameters
    void setup(int width = 20, int height = 20, 
               int visionWidth = 8, int visionHeight = 8) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    // Set agent starting position
    void setAgentPosition(double x, double y) {
        world_->setAgentStart(x, y);
    }
    
    // Update world and get observation
    std::vector<float> update(double timestep = 0.1) {
        world_->update(timestep);
        auto percept = world_->getSensoryPercept();
        return percept->getVision()->getData();
    }
    
    // Apply action to world
    bool applyAction(int actionType, double currentTime) {
        // Convert action type to MotorCommand
        MotorCommand cmd;
        cmd.setType(static_cast<MotorCommand::Type>(actionType));
        return world_->applyMotorCommand(cmd, currentTime);
    }
    
    // Get current world state
    std::map<std::string, double> getState() {
        std::map<std::string, double> state;
        
        auto body = world_->getAgentBody();
        state["x"] = body->x;
        state["y"] = body->y;
        state["orientation"] = body->orientation;
        state["energy"] = body->energy;
        state["health"] = body->health;
        
        state["time"] = world_->getSimulationTime();
        state["width"] = world_->getWidth();
        state["height"] = world_->getHeight();
        
        return state;
    }
    
    // Get world as shared pointer for advanced usage
    std::shared_ptr<SimpleWorld> getWorld() {
        return world_;
    }
};

class PythonAgent {
public:
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<PythonWorld> world_;
    
    PythonAgent(std::shared_ptr<PythonBrain> brain) {
        agent_ = std::make_shared<AgentBrain>(brain->getBrain());
        world_ = std::make_shared<PythonWorld>();
    }
    
    // Initialize agent with world
    void initialize() {
        agent_->initialize(world_->getWorld().get());
    }
    
    // Run agent episode
    std::map<std::string, double> runEpisode(int maxSteps = 1000) {
        std::map<std::string, double> results;
        double totalReward = 0.0;
        
        for (int step = 0; step < maxSteps; ++step) {
            // Get observation
            auto percept = world_->getWorld()->getSensoryPercept();
            
            // Process sensory input
            agent_->processSensoryInput(*percept);
            
            // Brain step
            agent_->getBrain()->step(step);
            
            // Get action
            MotorCommand action = agent_->decodeMotorCommand();
            
            // Apply action
            world_->getWorld()->applyMotorCommand(action, world_->getWorld()->getSimulationTime());
            
            // Get reward
            double reward = 0.0;  // Simplified - would get from world
            totalReward += reward;
            
            // Apply reward modulation
            agent_->applyRewardModulation(reward, 0.0);
            
            // Update development
            agent_->updateDevelopment(0.1);
        }
        
        results["total_reward"] = totalReward / maxSteps;
        results["final_x"] = world_->getState()["x"];
        results["final_y"] = world_->getState()["y"];
        
        return results;
    }
    
    // Enable all subsystems
    void enableAll() {
        agent_->enableRewardModulation(true);
        agent_->enableStructuralPlasticity(true);
        agent_->enableDevelopment(true);
        agent_->enableCuriosity(true);
    }
};

// ========== COMMAND LINE INTERFACE ==========

void setupCommandLine(py::module_& m) {
    m.def("createBrain", [](const std::string& configFile) {
        auto config = std::make_shared<Config>();
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        return std::make_shared<PythonBrain>(config);
    }, py::arg("configFile") = "",
        "Create a brain with optional config file");
    
    m.def("createWorld", [](int width, int height, int visionWidth, int visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create a world with specified dimensions");
    
    m.def("createAgent", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");
    
    m.def("runSimulation", [](std::shared_ptr<PythonBrain> brain, int steps) {
        brain->simulate(steps);
        return brain->getStats();
    }, py::arg("brain"), py::arg("steps"),
       "Run a simulation and return statistics");
    
    m.def("runCompleteExperiment", [](std::shared_ptr<PythonBrain> brain,
                                    std::shared_ptr<PythonWorld> world,
                                    int maxSteps) {
        auto agent = std::make_shared<PythonAgent>(brain);
        agent->initialize();
        agent->enableAll();
        return agent->runEpisode(maxSteps);
    }, py::arg("brain"), py::arg("world"), py::arg("maxSteps"),
       "Run a complete experiment with agent in world");
}

// ========== MAIN BINDING FUNCTIONS ==========

PYBIND11_MODULE(enhanced_pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python API
        -------------------------------------------------
        A user-friendly Python interface for the NLM neural simulation framework.
        
        This enhanced API provides:
        - Simple one-liner operations for common tasks
        - High-level abstractions for complex scenarios
        - Command-line interface helpers
        - Comprehensive example scripts
        - Better error handling and documentation
        
        Quick Start:
        >>> import enhanced_pynlm as nlm
        >>> brain = nlm.createBrain()
        >>> brain.initialize()
        >>> brain.simulate(100)
        >>> stats = brain.getStats()
    )pbdoc";
    
    // Register basic types and classes
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register enhanced Python classes
    py::class_<PythonBrain>(m, "Brain", R"pbdoc(High-level brain abstraction with user-friendly methods)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &PythonBrain::initialize, "Initialize the brain")
        .def("step", &PythonBrain::step, py::arg("step") = 0, py::arg("input") = std::vector<float>(),
             "Run one simulation step with optional sensory input")
        .def("simulate", &PythonBrain::simulate, py::arg("steps"), "Run multiple simulation steps")
        .def("getStats", &PythonBrain::getStats, "Get brain statistics as a dictionary")
        .def("save", &PythonBrain::save, py::arg("name") = "brain_checkpoint", 
             "Save brain state with automatic filename")
        .def("getBrain", &PythonBrain::getBrain, "Get the underlying brain object");
    
    py::class_<PythonWorld>(m, "World", R"pbdoc(High-level world abstraction with simple interface)pbdoc")
        .def(py::init<>())
        .def("setup", &PythonWorld::setup, py::arg("width") = 20, py::arg("height") = 20,
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             "Configure world dimensions")
        .def("setAgentPosition", &PythonWorld::setAgentPosition, "Set agent starting position")
        .def("update", &PythonWorld::update, py::arg("timestep") = 0.1, "Update world and return observation")
        .def("applyAction", &PythonWorld::applyAction, py::arg("actionType"), py::arg("currentTime"),
             "Apply action to world")
        .def("getState", &PythonWorld::getState, "Get world state as dictionary")
        .def("getWorld", &PythonWorld::getWorld, "Get the underlying world object");
    
    py::class_<PythonAgent>(m, "Agent", R"pbdoc(High-level agent with brain-world interface)pbdoc")
        .def(py::init<std::shared_ptr<PythonBrain>>())
        .def("initialize", &PythonAgent::initialize, "Initialize agent with world")
        .def("runEpisode", &PythonAgent::runEpisode, py::arg("maxSteps") = 1000,
             "Run complete agent episode and return results")
        .def("enableAll", &PythonAgent::enableAll, "Enable all learning subsystems");
    
    // Command line interface functions
    setupCommandLine(m);
    
    // Export original basic API functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Export constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Export enumeration types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("Interact", ActionType::Interact)
        .export_values();
}

} // namespace nlm
