#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Version: 0.1.0
        Author: NLM Authors
        License: MIT
    )pbdoc";

    // Add version attribute
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__license__") = "MIT";
    
    // Add module-level constants
    m.attr("NEURON_TYPES") = py::cast(std::unordered_map<std::string, NeuronType>({
        {"excitatory", NeuronType::Excitatory},
        {"inhibitory", NeuronType::Inhibitory},
        {"modulatory", NeuronType::Modulatory},
        {"sensory", NeuronType::Sensory},
        {"motor", NeuronType::Motor},
        {"internal", NeuronType::Internal}
    }));
    
    m.attr("ACTION_TYPES") = py::cast(std::unordered_map<std::string, ActionType>({
        {"move_forward", ActionType::MoveForward},
        {"move_backward", ActionType::MoveBackward},
        {"move_left", ActionType::MoveLeft},
        {"move_right", ActionType::MoveRight},
        {"turn_left", ActionType::TurnLeft},
        {"turn_right", ActionType::TurnRight},
        {"look", ActionType::Look},
        {"look_up", ActionType::LookUp},
        {"look_down", ActionType::LookDown},
        {"interact", ActionType::Interact},
        {"eat", ActionType::Eat},
        {"drink", ActionType::Drink},
        {"rest", ActionType::Rest},
        {"wait", ActionType::Wait},
        {"custom", ActionType::Custom}
    }));
    
    // High-level convenience functions
    m.def("create_default_brain", [](const std::string& config_file = "") -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config_file") = "",
         "Create a default brain with optional configuration file.");
    
    m.def("create_simple_simulation", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return py::make_tuple(brain, world, agent);
    }, "Create a complete simple simulation with brain, world, and agent.");
    
    m.def("run_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, size_t num_steps) {
        if (!agent->isDevelopmentEnabled()) {
            agent->enableDevelopment(true);
        }
        if (!agent->isRewardModulationEnabled()) {
            agent->enableRewardModulation(true);
        }
        
        for (size_t i = 0; i < num_steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward calculation
            float reward = 0.0f;
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        return py::dict(
            "total_spikes", brain->getTotalSpikeCount(),
            "final_stage", brain->getDevelopmentalStage(),
            "reward_level", agent->getNeuromodulationLevel()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
         "Run a complete simulation episode.");
    
    // Add Pythonic methods to Brain class
    py::class_<Brain> brain_class(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        This class provides the main interface for running neural simulations,
        including spiking neurons, plasticity, and integrated cognitive systems.
    )pbdoc");
    
    brain_class
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create a new brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds, "Get all region IDs")
        .def("get_regions", &Brain::getRegions, py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("log_status", &Brain::logStatus, "Log brain status");
    
    // Add Pythonic methods to Config class
    py::class_<Config> config_class(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc");
    
    config_class
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"), "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, const std::vector<std::string>& args) {
            int argc = args.size();
            std::vector<char*> argv;
            for (const auto& arg : args) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            return self.loadFromArgs(argc, argv.data());
        }, py::arg("args"), "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"), "Save configuration to a JSON file")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            return std::string();
        }, py::arg("key"), "Get configuration value as string (default empty)")
        .def("get_int", [](const Config& self, const std::string& key, int default_val = 0) {
            auto value = self.get<int>(key);
            return value ? *value : default_val;
        }, py::arg("key"), py::arg("default_val") = 0, "Get configuration value as int")
        .def("get_float", [](const Config& self, const std::string& key, float default_val = 0.0f) {
            auto value = self.get<float>(key);
            return value ? *value : default_val;
        }, py::arg("key"), py::arg("default_val") = 0.0f, "Get configuration value as float")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration");
    
    // Add Pythonic methods to SimpleWorld class
    py::class_<SimpleWorld> world_class(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a virtual environment where agents can interact,
        sense, and act based on neural computation.
    )pbdoc");
    
    world_class
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove world object")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed");
    
    // Add Pythonic methods to AgentBrain class
    py::class_<AgentBrain> agent_class(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Provides the connection between neural computation and environmental interaction,
        handling sensory input, motor output, and neuromodulation.
    )pbdoc");
    
    agent_class
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");
    
    // Create default instances (factories)
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("create_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("create_agent", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Add convenience enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Add constants for invalid IDs
    // Add advanced simulation control functions
    m.def("create_advanced_simulation", [](const std::string& config_file = "") {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("enable_structural_plasticity", true, ConfigSource::Default);
        config->set("enable_reward_modulation", true, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Default);
        config->set("enable_curiosity", true, ConfigSource::Default);
        config->set("enable_prediction", true, ConfigSource::Default);
        
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 12, 12);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return py::make_tuple(brain, world, agent, config);
    }, "Create an advanced simulation with enhanced features.");
    
    m.def("run_batch_simulation", [](const std::vector<std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>> simulations,
                                   size_t steps_per_episode, size_t num_episodes) {
        py::list results;
        
        for (size_t episode = 0; episode < num_episodes; ++episode) {
            py::dict episode_result;
            episode_result["episode"] = episode;
            
            for (size_t sim_idx = 0; sim_idx < simulations.size(); ++sim_idx) {
                auto& [brain, world, agent] = simulations[sim_idx];
                
                // Run simulation episode
                for (size_t step = 0; step < steps_per_episode; ++step) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    brain->step(step);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Simple reward calculation
                    float reward = 0.0f;
                    auto internal = percept.getInternal();
                    if (!internal.empty()) {
                        reward = internal[0];
                    }
                    agent->applyRewardModulation(reward, 0.0f);
                    agent->updateDevelopment(0.1);
                }
                
                // Store episode results
                episode_result["simulation_" + std::to_string(sim_idx)] = py::dict(
                    "final_spikes", brain->getTotalSpikeCount(),
                    "final_stage", brain->getDevelopmentalStage(),
                    "reward_level", agent->getNeuromodulationLevel(),
                    "memory_traces", agent->getBrain()->getWorkingMemory() ? 
                        agent->getBrain()->getWorkingMemory()->getActiveTraces() : 0
                );
            }
            
            results.append(episode_result);
        }
        
        return results;
    }, py::arg("simulations"), py::arg("steps_per_episode"), py::arg("num_episodes"),
         "Run multiple simulations in batch mode.");
    
    m.def("save_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (brain) {
            return brain->save(filepath);
        }
        return false;
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file.");
    
    m.def("load_brain_state", [](const std::string& filepath, const std::string& config_file = "") {
        auto config = std::make_shared<Config>();
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(filepath)) {
            brain->initialize();
            return brain;
        }
        return std::shared_ptr<Brain>(nullptr);
    }, py::arg("filepath"), py::arg("config_file") = "", "Load brain state from file.");
    
    m.def("benchmark_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                     std::shared_ptr<AgentBrain> agent, size_t steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict benchmark_result;
        benchmark_result["steps_completed"] = steps;
        benchmark_result["total_spikes"] = brain->getTotalSpikeCount();
        benchmark_result["elapsed_time_ms"] = duration.count();
        benchmark_result["steps_per_second"] = steps / (duration.count() / 1000.0);
        benchmark_result["spikes_per_second"] = brain->getTotalSpikeCount() / (duration.count() / 1000.0);
        
        return benchmark_result;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         "Run simulation and return benchmark results.");
    
    m.def("export_simulation_data", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                      const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        file << "NLM Simulation Data Export" << std::endl;
        file << "===========================" << std::endl;
        file << "Generated at: " << std::chrono::system_clock::now() << std::endl;
        file << std::endl;
        
        // Brain statistics
        file << "Brain Statistics:" << std::endl;
        file << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        file << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        file << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        file << std::endl;
        
        // World statistics
        file << "World Statistics:" << std::endl;
        file << "  Width: " << world->getWidth() << std::endl;
        file << "  Height: " << world->getHeight() << std::endl;
        file << "  Simulation time: " << world->getSimulationTime() << std::endl;
        file << "  Max energy: " << world->getMaxEnergy() << std::endl;
        file << "  Energy decay rate: " << world->getEnergyDecayRate() << std::endl;
        file << std::endl;
        
        // Get current percept
        auto percept = world->getSensoryPercept();
        file << "Sensory Input:" << std::endl;
        file << "  Vision (" << percept.getVisionWidth() << "x" << percept.getVisionHeight() << "): ";
        auto vision = percept.getVision();
        for (size_t i = 0; i < vision.size(); ++i) {
            file << vision[i];
            if (i < vision.size() - 1) file << ", ";
        }
        file << std::endl;
        file << "  Touch: ";
        auto touch = percept.getTouch();
        for (size_t i = 0; i < touch.size(); ++i) {
            file << touch[i];
            if (i < touch.size() - 1) file << ", ";
        }
        file << std::endl;
        file << "  Internal: ";
        auto internal = percept.getInternal();
        for (size_t i = 0; i < internal.size(); ++i) {
            file << internal[i];
            if (i < internal.size() - 1) file << ", ";
        }
        file << std::endl;
        
        file << std::endl;
        file << "Export completed successfully." << std::endl;
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("filepath"),
         "Export simulation data to a text file.");
    
    m.def("get_memory_usage", [](std::shared_ptr<Brain> brain) {
        py::dict memory_info;
        
        if (brain) {
            memory_info["total_neurons"] = brain->getTotalNeuronCount();
            memory_info["total_synapses"] = brain->getTotalSynapseCount();
            memory_info["working_memory_traces"] = brain->getWorkingMemory() ? 
                brain->getWorkingMemory()->getActiveTraces() : 0;
            memory_info["episodic_memory_episodes"] = brain->getEpisodicMemory() ? 
                brain->getEpisodicMemory()->getEpisodeCount() : 0;
            memory_info["associative_memory_patterns"] = brain->getAssociativeMemory() ? 
                brain->getAssociativeMemory()->getPatternCount() : 0;
            
            // Estimate memory usage (approximate)
            size_t estimated_memory = (brain->getTotalNeuronCount() * 100) +  // Neurons
                                    (brain->getTotalSynapseCount() * 50) +  // Synapses
                                    (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() * 20 : 0) +  // Working memory
                                    (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() * 1000 : 0);  // Episodic memory
            
            memory_info["estimated_memory_kb"] = estimated_memory / 1024;
        }
        
        return memory_info;
    }, py::arg("brain"), "Get memory usage information for the brain.");
    
    m.def("optimize_brain", [](std::shared_ptr<Brain> brain) {
        if (!brain) return;
        
        // Apply optimization heuristics
        auto* working_memory = brain->getWorkingMemory();
        if (working_memory) {
            // Clear old memory traces to free memory
            working_memory->clear();
        }
        
        // Reset spike system for clean state
        if (auto* spike_system = brain->getSpikeSystem()) {
            spike_system->reset();
        }
        
        // Reinitialize with optimal configuration
        brain->reset();
        
        return true;
    }, py::arg("brain"), "Optimize brain memory usage and performance.");
    
    m.def("simulate_with_checkpoint", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent, size_t steps,
                                         const std::string& checkpoint_file) {
        if (brain->load(checkpoint_file)) {
            // Loaded checkpoint, continue simulation
            for (size_t i = 0; i < steps; ++i) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            // Save checkpoint after simulation
            brain->save(checkpoint_file);
            
            return py::dict(
                "checkpoint_loaded", true,
                "checkpoint_saved", true,
                "final_spikes", brain->getTotalSpikeCount()
            );
        } else {
            // Failed to load checkpoint, create new simulation
            for (size_t i = 0; i < steps; ++i) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            // Save checkpoint for future runs
            brain->save(checkpoint_file);
            
            return py::dict(
                "checkpoint_loaded", false,
                "checkpoint_saved", true,
                "final_spikes", brain->getTotalSpikeCount()
            );
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         py::arg("checkpoint_file"), "Simulate with checkpoint support.");
    
    // High-level convenience functions
    m.def("create_default_brain", [](const std::string& config_file = "") -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config_file") = "",
         "Create a default brain with optional configuration file.");
    
    m.def("create_simple_simulation", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return py::make_tuple(brain, world, agent);
    }, "Create a complete simple simulation with brain, world, and agent.");
    
    m.def("run_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, size_t num_steps) {
        if (!agent->isDevelopmentEnabled()) {
            agent->enableDevelopment(true);
        }
        if (!agent->isRewardModulationEnabled()) {
            agent->enableRewardModulation(true);
        }
        
        for (size_t i = 0; i < num_steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward calculation
            float reward = 0.0f;
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        return py::dict(
            "total_spikes", brain->getTotalSpikeCount(),
            "final_stage", brain->getDevelopmentalStage(),
            "reward_level", agent->getNeuromodulationLevel()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
         "Run a complete simulation episode.");
    
    // Add context manager support for Brain
    py::class_<BrainWrapper>(m, "BrainWrapper", R"pbdoc(
        Python wrapper for Brain with context manager support.
        
        This class provides automatic cleanup and resource management
        for Brain instances when used in 'with' statements.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", &BrainWrapper::enter)
        .def("__exit__", &BrainWrapper::exit)
        .def("step", &BrainWrapper::step, py::arg("step"), "Perform a simulation step")
        .def("run", &BrainWrapper::run, py::arg("steps"), py::arg("time_step") = 0.001f,
              "Run multiple simulation steps")
        .def("reset", &BrainWrapper::reset, "Reset brain state")
        .def("get_stats", &BrainWrapper::get_stats, "Get brain statistics as dictionary")
        .def("close", &BrainWrapper::close, "Explicitly close the brain");
    
    // Add context manager support for Agent
    py::class_<AgentWrapper>(m, "AgentWrapper", R"pbdoc(
        Python wrapper for AgentBrain with context manager support.
        
        Provides automatic cleanup and convenience methods for agent interactions.
    )pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>())
        .def("__enter__", &AgentWrapper::enter)
        .def("__exit__", &AgentWrapper::exit)
        .def("process_observation", &AgentWrapper::process_observation, 
              py::arg("percept"), "Process sensory observation")
        .def("take_action", &AgentWrapper::take_action, py::arg("world"),
              "Take action based on current brain state")
        .def("get_status", &AgentWrapper::get_status, "Get agent status as dictionary")
        .def("close", &AgentWrapper::close, "Explicitly close the agent");
    
    // Add world wrapper with context manager
    py::class_<WorldWrapper>(m, "WorldWrapper", R"pbdoc(
        Python wrapper for SimpleWorld with context manager support.
        
        Provides automatic cleanup and convenience methods for world management.
    )pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>())
        .def("__enter__", &WorldWrapper::enter)
        .def("__exit__", &WorldWrapper::exit)
        .def("update", &WorldWrapper::update, py::arg("dt"), "Update world state")
        .def("reset", &WorldWrapper::reset, "Reset world to initial state")
        .def("set_agent_position", &WorldWrapper::set_agent_position,
              py::arg("x"), py::arg("y"), "Set agent position")
        .def("get_observation", &WorldWrapper::get_observation, "Get current observation")
        .def("get_agent_body", &WorldWrapper::get_agent_body, "Get agent body state")
        .def("add_resource", &WorldWrapper::add_resource, py::arg("x"), py::arg("y"),
              py::arg("value"), "Add resource to world")
        .def("close", &WorldWrapper::close, "Explicitly close the world");
    
    // Add convenience methods to existing classes
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
              "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, const std::vector<std::string>& args) {
            int argc = args.size();
            std::vector<char*> argv;
            for (const auto& arg : args) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            return self.loadFromArgs(argc, argv.data());
        }, py::arg("args"), "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
              "Save configuration to a JSON file")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
              "Set configuration value")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            return std::string();
        }, py::arg("key"), "Get configuration value as string (default empty)")
        .def("get_int", [](const Config& self, const std::string& key, int default_val = 0) {
            auto value = self.get<int>(key);
            return value ? *value : default_val;
        }, py::arg("key"), py::arg("default_val") = 0, "Get configuration value as int")
        .def("get_float", [](const Config& self, const std::string& key, float default_val = 0.0f) {
            auto value = self.get<float>(key);
            return value ? *value : default_val;
        }, py::arg("key"), py::arg("default_val") = 0.0f, "Get configuration value as float")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
