#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

// Helper function to create a default Python exception
void registerPythonExceptions(py::module& m) {
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "Error");
}

// Helper function to get a value with proper error handling
py::object getWithErrorHandling(py::object obj, const std::string& methodName, const std::string& errorMsg) {
    try {
        return obj.attr(methodName.c_str)();
    } catch (const py::error_already_set& e) {
        throw std::runtime_error(errorMsg + std::string(" - ") + e.what());
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python-friendly interface for the NLM C++ neural simulation framework.
        
        This module provides simplified, Pythonic APIs for creating and working with
        artificial neural networks, brains, and simulated environments.
        
        Key Features:
        - Simple brain creation and management
        - High-level simulation control
        - Easy world interaction
        - Built-in error handling
        - Comprehensive documentation
        
        Example Usage:
        ```python
        import pynlm
        
        # Create a simple brain
        brain = pynlm.create_brain()
        brain.initialize()
        
        # Run a simulation
        for i in range(100):
            brain.step(i)
        
        print(f"Total neurons: {brain.get_total_neuron_count()}")
        ```
    )pbdoc";

    // Register Python exceptions
    registerPythonExceptions(m);

    // Simple configuration helper
    m.def("create_default_config", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    m.def("load_config", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from: " + filepath);
        }
        return config;
    }, "Load configuration from file (JSON or simple key=value format)", py::arg("filepath"));

    m.def("save_config", [](const std::shared_ptr<Config>& config, const std::string& filepath) {
        if (!config->saveToFile(filepath)) {
            throw std::runtime_error("Failed to save configuration to: " + filepath);
        }
    }, "Save configuration to file (JSON or simple key=value format)", 
          py::arg("config"), py::arg("filepath"));

    // Convenience function to create a brain with default settings
    m.def("create_brain", [](const std::shared_ptr<Config>& config) {
        return std::make_shared<Brain>(config);
    }, "Create a brain with the given configuration", py::arg("config") = nullptr);

    m.def("create_brain", []() {
        auto config = std::make_shared<Config>();
        // Set some sensible defaults
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    // Enhanced brain class with Pythonic methods
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             "Perform a simulation step", py::arg("currentStep"))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             "Perform a simulation step with timestamp", py::arg("currentStep"), py::arg("currentTime"))
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, "Save brain state to file", py::arg("filepath"))
        .def("load", &Brain::load, "Load brain state from file", py::arg("filepath"))
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             "Inject sensory input into the brain", py::arg("input"))
        .def("inject_current", &Brain::injectCurrent,
             "Inject current directly into a specific neuron", py::arg("neuron"), py::arg("current"))
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             "Inject current into all neurons of a specific type", py::arg("type"), py::arg("current"))
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             "Set developmental stage", py::arg("stage"))
        .def("get_config", &Brain::getConfig,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        .def("__repr__", [](const std::shared_ptr<Brain>& brain) {
            return "<Brain: " + std::to_string(brain->getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain->getTotalSynapseCount()) + 
                   " synapses>";
        });

    // Enhanced AgentBrain class with Pythonic methods
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize,
             "Initialize with world", py::arg("world"))
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             "Process sensory percept and inject into brain", py::arg("percept"))
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             "Apply reward-based neuromodulation", py::arg("reward"), py::arg("predictedReward"))
        .def("update_development", &AgentBrain::updateDevelopment,
             "Update development system", py::arg("timestep"))
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             "Get the underlying brain")
        // Subsystem enable/disable with Pythonic boolean parameters
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             "Enable/disable reward modulation", py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             "Enable/disable structural plasticity", py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             "Enable/disable development", py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             "Enable/disable curiosity", py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Enhanced SimpleWorld class with Pythonic methods
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             "Configure world dimensions", py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8)
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             "Set agent starting position", py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update,
             "Update world simulation", py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             "Apply motor command to world", py::arg("cmd"), py::arg("currentTime"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             "Get sensory percept from world (returns reference)", py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             "Get agent body state (returns reference)", py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject,
             "Add world object", py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject,
             "Remove world object by position", py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition,
             "Check if position is valid", py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy,
             "Set maximum energy", py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate,
             "Set energy decay rate", py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed,
             "Set random seed", py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed");

    // Simplified convenience functions for common use cases
    m.def("run_simple_simulation", [](int numNeurons = 100, int numSteps = 500) {
        auto brain = pynlm::create_brain();
        brain->initialize();
        
        for (int step = 0; step < numSteps; ++step) {
            // Inject some random input to stimulate activity
            for (int i = 0; i < 10 && i < numNeurons / 10; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        return brain;
    }, "Run a simple simulation and return the brain", 
          py::arg("numNeurons") = 100, py::arg("numSteps") = 500);

    m.def("create_agent_simulation", [](int worldSize = 10, int steps = 200) {
        auto brain = pynlm::create_brain();
        brain->initialize();
        
        auto world = pynlm::create_simple_world();
        world->configure(worldSize, worldSize, 8, 8);
        world->reset();
        world->set_agent_start(worldSize/2, worldSize/2);
        
        auto agent = pynlm::create_agent_brain(brain);
        agent->initialize(*world);
        
        // Enable learning systems
        agent->enable_reward_modulation(true);
        agent->enable_structural_plasticity(true);
        agent->enable_development(true);
        agent->enable_curiosity(true);
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward based on position
            float reward = 0.0f;
            auto body = world->getAgentBody();
            if (body.x >= 0 && body.x < worldSize && body.y >= 0 && body.y < worldSize) {
                reward = 1.0f;  // Small reward for staying in bounds
            }
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete agent simulation", 
          py::arg("worldSize") = 10, py::arg("steps") = 200);

    // Export constants and enums for easy access
    m.attr("VERSION") = "0.1.0";
    m.attr("COPYRIGHT") = "© 2026 NLM Project";

    // Create submodule for constants
    auto constants = py::module_::create_submodule(m, "constants");
    
    constants.attr("NEURON_TYPES") = py::module_::create_submodule(constants, "neuron_types");
    constants.attr("ACTION_TYPES") = py::module_::create_submodule(constants, "action_types");
    constants.attr("WORLD_OBJECT_TYPES") = py::module_::create_submodule(constants, "world_object_types");
    
    // Export enum values for easy access
    constants.attr("EXCITATORY") = NeuronType::Excitatory;
    constants.attr("INHIBITORY") = NeuronType::Inhibitory;
    constants.attr("SENSORY") = NeuronType::Sensory;
    constants.attr("MOTOR") = NeuronType::Motor;
    constants.attr("INTERNAL") = NeuronType::Internal;
    constants.attr("MODULATORY") = NeuronType::Modulatory;
    
    constants.attr("MOVE_FORWARD") = ActionType::MoveForward;
    constants.attr("MOVE_BACKWARD") = ActionType::MoveBackward;
    constants.attr("TURN_LEFT") = ActionType::TurnLeft;
    constants.attr("TURN_RIGHT") = ActionType::TurnRight;
    constants.attr("INTERACT") = ActionType::Interact;
    constants.attr("EAT") = ActionType::Eat;
    constants.attr("DRINK") = ActionType::Drink;
    constants.attr("REST") = ActionType::Rest;
    constants.attr("WAIT") = ActionType::Wait;
    
    constants.attr("RESOURCE") = WorldObjectType::Resource;
    constants.attr("HAZARD") = WorldObjectType::Hazard;
    constants.attr("WALL") = WorldObjectType::Wall;
    constants.attr("MARKER") = WorldObjectType::Marker;
    
    constants.attr("INITIAL") = DevelopmentalStage::Initial;
    constants.attr("CRITICAL_PERIOD") = DevelopmentalStage::CriticalPeriod;
    constants.attr("MATURATION") = DevelopmentalStage::Maturation;
    constants.attr("ADULT") = DevelopmentalStage::Adult;
    constants.attr("AGING") = DevelopmentalStage::Aging;

    // Add examples to the module
    m.def("get_example_code", []() {
        return R"(
        # Simple Brain Example
        import pynlm
        
        brain = pynlm.create_brain()
        brain.initialize()
        
        for i in range(100):
            brain.step(i)
        
        print(f"Neurons: {brain.get_total_neuron_count()}")
        print(f"Firing: {brain.get_firing_neuron_count()}")
        
        # Save brain state
        brain.save("my_brain.bin")
        
        # Load brain state
        brain2 = pynlm.create_brain()
        brain2.load("my_brain.bin")
        
        print("Example complete!")
        )";
    }, "Get example code for common use cases");

    // Create shortcuts for common operations
    m.def("quick_test", []() {
        auto brain = pynlm::create_brain();
        brain->initialize();
        
        // Test basic functionality
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        
        for (int i = 0; i < 50; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto spikes = brain->getTotalSpikeCount();
        auto rate = brain->getAverageFiringRate();
        
        return std::make_tuple(spikes, rate);
    }, "Run a quick test of the NLM system");

    // Create a simple training loop helper
    m.def("training_loop", [](int episodes = 10, int stepsPerEpisode = 100, 
                          float explorationRate = 0.1) {
        auto brain = pynlm::create_brain();
        brain->initialize();
        
        for (int episode = 0; episode < episodes; ++episode) {
            // Simple exploration/exploitation strategy
            bool explore = (rand() % 100) < (explorationRate * 100);
            
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // Generate some random input
                brain->injectCurrentToNeurons(NeuronType::Sensory, 
                    explore ? 40.0f : 20.0f);
                
                brain->step(step, step * 0.001);
            }
        }
        
        return brain;
    }, "Run a simple training loop", 
          py::arg("episodes") = 10, py::arg("stepsPerEpisode") = 100,
          py::arg("explorationRate") = 0.1);

    // Export helpful constants
    m.attr("DEFAULT_NEURON_COUNT") = 500;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001;

    // Create utility module
    auto utils = py::module_::create_submodule(m, "utils");
    
    utils.def("validate_config", [](const std::shared_ptr<Config>& config) {
        // Simple validation - check for required keys
        std::vector<std::string> warnings;
        
        if (!config->has("neuron_count")) {
            warnings.push_back("Configuration missing 'neuron_count', using default");
        }
        
        if (!config->has("connection_probability")) {
            warnings.push_back("Configuration missing 'connection_probability', using default");
        }
        
        return warnings;
    }, "Validate configuration and return warnings", py::arg("config"));

    utils.def("create_random_config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("random_seed", static_cast<int64_t>(42));
        return config;
    }, "Create a configuration with random parameters");

    utils.def("compare_brains", [](const std::shared_ptr<Brain>& brain1, const std::shared_ptr<Brain>& brain2) {
        std::map<std::string, std::string> comparisons;
        
        comparisons["neuron_count"] = std::to_string(brain1->getTotalNeuronCount()) + " vs " + std::to_string(brain2->getTotalNeuronCount());
        comparisons["synapse_count"] = std::to_string(brain1->getTotalSynapseCount()) + " vs " + std::to_string(brain2->getTotalSynapseCount());
        comparisons["spike_count"] = std::to_string(brain1->getTotalSpikeCount()) + " vs " + std::to_string(brain2->getTotalSpikeCount());
        comparisons["firing_rate"] = std::to_string(brain1->getAverageFiringRate()) + " vs " + std::to_string(brain2->getAverageFiringRate());
        
        return comparisons;
    }, "Compare two brains and return metrics comparison", 
          py::arg("brain1"), py::arg("brain2"));

    utils.def("benchmark_config", [](const std::shared_ptr<Config>& config, int steps = 1000) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return std::make_tuple(brain, totalTime, steps / totalTime);
    }, "Benchmark configuration and return brain, time, and steps/sec", 
          py::arg("config"), py::arg("steps") = 1000);

    // Add helpful constants to utils
    utils.attr("MIN_NEURONS") = 10;
    utils.attr("MAX_NEURONS") = 10000;
    utils.attr("MIN_STEPS") = 10;
    utils.attr("MAX_STEPS") = 100000;

    // Export a simple high-level API for users
    m.def("simple_brain", []() {
        return pynlm::create_brain();
    }, "Create a simple brain with default settings");

    m.def("simple_world", []() {
        return pynlm::create_simple_world();
    }, "Create a simple world with default settings");

    m.def("simple_agent", [](const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world) {
        return pynlm::create_agent_brain(brain);
    }, "Create a simple agent with brain and world", 
          py::arg("brain"), py::arg("world") = nullptr);

    m.def("quick_save_brain", [](const std::shared_ptr<Brain>& brain, const std::string& filename) {
        brain->save(filename);
    }, "Quick save brain to file", py::arg("brain"), py::arg("filename"));

    m.def("quick_load_brain", [](const std::string& filename) {
        auto brain = pynlm::create_brain();
        brain->load(filename);
        return brain;
    }, "Quick load brain from file", py::arg("filename"));

    // Add a simple training example function
    m.def("training_example", []() {
        auto brain = pynlm::create_brain();
        brain->initialize();
        
        std::cout << "Training Example:" << std::endl;
        std::cout << "  Step 0: Initializing brain" << std::endl;
        
        for (int step = 0; step < 100; ++step) {
            // Add some input pattern
            if (step % 10 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 25 == 0) {
                std::cout << "  Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        std::cout << "  Final spike count: " << brain->getTotalSpikeCount() << std::endl;
        
        return brain;
    }, "Run a simple training example with output");

} // namespace nlm
