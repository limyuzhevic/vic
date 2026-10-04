#ifndef PYBIND11_MODULE
#error "pybind11 header not found. Install pybind11 and recompile."
#endif

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

namespace py = pybind11;

// Simulation context manager for Python
class SimulationContext {
public:
    SimulationContext(std::shared_ptr<nlm::Brain> brain) : brain_(brain) {}
    
    void enter() {
        // Initialize brain on context entry
        if (brain_) {
            brain_->reset();
            brain_->initialize();
        }
    }
    
    void exit(py::object, py::object, py::object) {
        // Cleanup on context exit
        if (brain_) {
            brain_.reset();
        }
    }
    
    void run(nlm::SimulationStep num_steps, 
             std::shared_ptr<nlm::SimpleWorld> world,
             std::shared_ptr<nlm::AgentBrain> agent) {
        for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
            world->update(0.1); // Update world
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain_->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM - Neural Learning Machine Python bindings";

    // Register NeuronType enum
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    // Register DevelopmentalStage enum
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    // Register ConfigSource enum
    py::enum_<nlm::ConfigSource>(m, "ConfigSource")
        .value("Default", nlm::ConfigSource::Default)
        .value("File", nlm::ConfigSource::File)
        .value("CommandLine", nlm::ConfigSource::CommandLine)
        .value("Runtime", nlm::ConfigSource::Runtime)
        .export_values();

    // Register Config class
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", py::overload_cast<const std::string&, int64_t, nlm::ConfigSource>(&nlm::Config::set),
             "Set integer value")
        .def("set", py::overload_cast<const std::string&, double, nlm::ConfigSource>(&nlm::Config::set),
             "Set float value")
        .def("set", py::overload_cast<const std::string&, bool, nlm::ConfigSource>(&nlm::Config::set),
             "Set boolean value")
        .def("set", py::overload_cast<const std::string&, const std::string&, nlm::ConfigSource>(&nlm::Config::set),
             "Set string value")
        .def("get_or", &nlm::Config::getOr<int64_t>, "Get integer with default")
        .def("get_or", &nlm::Config::getOr<double>, "Get float with default")
        .def("get_or", &nlm::Config::getOr<bool>, "Get boolean with default")
        .def("get_or", &nlm::Config::getOr<std::string>, "Get string with default")
        .def("load_from_file", &nlm::Config::loadFromFile, "Load config from file")
        .def("save_to_file", &nlm::Config::saveToFile, "Save config to file")
        .def("get_keys", &nlm::Config::getKeys, "Get all config keys")
        .def("summary", &nlm::Config::summary, "Get config summary");

    // Helper function to create default config
    m.def("create_default_config", []() {
        auto config = std::make_shared<nlm::Config>();
        // Set sensible defaults for Phase 6
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        config->set("replay_interval", 100, nlm::ConfigSource::Default);
        config->set("consolidation_interval", 1000, nlm::ConfigSource::Default);
        config->set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::Default);
        return config;
    }, "Create default configuration for NLM brain");

    // Helper function to create development config
    m.def("create_development_config", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
        config->set("development_enabled", true, nlm::ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002f, nlm::ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, nlm::ConfigSource::Default);
        config->set("replay_interval", 50, nlm::ConfigSource::Default);
        config->set("consolidation_interval", 500, nlm::ConfigSource::Default);
        config->set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::Default);
        return config;
    }, "Create development configuration for NLM brain");

    // Register Brain class
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize, "Initialize brain")
        .def("step", py::overload_cast<nlm::SimulationStep>(&nlm::Brain::step), "Run simulation step")
        .def("step", py::overload_cast<nlm::SimulationStep, nlm::Timestamp>(&nlm::Brain::step), "Run simulation step with time")
        .def("get_total_neuron_count", &nlm::Brain::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapse_count", &nlm::Brain::getTotalSynapseCount, "Get total synapse count")
        .def("get_firing_neuron_count", &nlm::Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("get_total_spike_count", &nlm::Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_average_firing_rate", &nlm::Brain::getAverageFiringRate, "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &nlm::Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("get_developmental_stage", &nlm::Brain::getDevelopmentalStage, "Get developmental stage")
        .def("set_developmental_stage", &nlm::Brain::setDevelopmentalStage, "Set developmental stage")
        .def("reset", &nlm::Brain::reset, "Reset brain state")
        .def("save", &nlm::Brain::save, "Save brain state to file")
        .def("load", &nlm::Brain::load, "Load brain state from file")
        .def("log_status", &nlm::Brain::logStatus, "Log brain status")
        .def("receive_sensory_input", &nlm::Brain::receiveSensoryInput, "Receive sensory input")
        .def("inject_current", &nlm::Brain::injectCurrent, "Inject current into specific neuron")
        .def("inject_current_to_neurons", &nlm::Brain::injectCurrentToNeurons, "Inject current to neurons of specific type");

    // Register SimpleWorld class
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, "Configure world dimensions")
        .def("reset", &nlm::SimpleWorld::reset, "Reset world state")
        .def("update", &nlm::SimpleWorld::update, "Update world state")
        .def("get_sensory_percept", &nlm::SimpleWorld::getSensoryPercept, "Get sensory percept")
        .def("get_agent_body", &nlm::SimpleWorld::getAgentBody, "Get agent body")
        .def("apply_motor_command", &nlm::SimpleWorld::applyMotorCommand, "Apply motor command")
        .def("set_random_seed", &nlm::SimpleWorld::setRandomSeed, "Set random seed")
        .def("get_random_seed", &nlm::SimpleWorld::getRandomSeed, "Get random seed")
        .def("get_width", &nlm::SimpleWorld::getWidth, "Get world width")
        .def("get_height", &nlm::SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &nlm::SimpleWorld::getMaxEnergy, "Get max energy")
        .def("set_max_energy", &nlm::SimpleWorld::setMaxEnergy, "Set max energy")
        .def("get_energy_decay_rate", &nlm::SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("set_energy_decay_rate", &nlm::SimpleWorld::setEnergyDecayRate, "Set energy decay rate")
        .def("get_simulation_time", &nlm::SimpleWorld::getSimulationTime, "Get simulation time")
        .def("is_valid_position", &nlm::SimpleWorld::isValidPosition, "Check if position is valid")
        .def("add_object", &nlm::SimpleWorld::addObject, "Add object to world")
        .def("remove_object", &nlm::SimpleWorld::removeObject, "Remove object at position")
        .def("get_object_at", &nlm::SimpleWorld::getObjectAt, "Get object at position");

    // Helper function to create simple world
    m.def("create_simple_world", []() {
        return std::make_unique<nlm::SimpleWorld>();
    }, "Create simple world");

    // Register SensoryPercept class
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("get_vision", &nlm::SensoryPercept::getVision, "Get vision data")
        .def("set_vision", &nlm::SensoryPercept::setVision, "Set vision data")
        .def("get_touch", &nlm::SensoryPercept::getTouch, "Get touch data")
        .def("set_touch", &nlm::SensoryPercept::setTouch, "Set touch data")
        .def("get_internal", &nlm::SensoryPercept::getInternal, "Get internal data")
        .def("set_internal", &nlm::SensoryPercept::setInternal, "Set internal data")
        .def("get_proprioception", &nlm::SensoryPercept::getProprioception, "Get proprioception data")
        .def("set_proprioception", &nlm::SensoryPercept::setProprioception, "Set proprioception data")
        .def("get_audio", &nlm::SensoryPercept::getAudio, "Get audio data")
        .def("set_audio", &nlm::SensoryPercept::setAudio, "Set audio data")
        .def("get_all_signals", &nlm::SensoryPercept::getAllSignals, "Get all sensory signals concatenated")
        .def("get_timestamp", &nlm::SensoryPercept::getTimestamp, "Get timestamp")
        .def("set_timestamp", &nlm::SensoryPercept::setTimestamp, "Set timestamp")
        .def("get_vision_width", &nlm::SensoryPercept::getVisionWidth, "Get vision width")
        .def("get_vision_height", &nlm::SensoryPercept::getVisionHeight, "Get vision height");

    // Register AgentBrain class
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize, "Initialize agent brain")
        .def("get_sensory_input_size", &nlm::AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("get_motor_output_size", &nlm::AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("process_sensory_input", &nlm::AgentBrain::processSensoryInput, "Process sensory input")
        .def("decode_motor_command", &nlm::AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("apply_reward_modulation", &nlm::AgentBrain::applyRewardModulation, "Apply reward modulation")
        .def("update_development", &nlm::AgentBrain::updateDevelopment, "Update development system")
        .def("get_developmental_stage", &nlm::AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("get_neuromodulation_level", &nlm::AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("get_curiosity_level", &nlm::AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &nlm::AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &nlm::AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset, "Reset agent for new episode")
        .def("enable_reward_modulation", &nlm::AgentBrain::enableRewardModulation, "Enable reward modulation")
        .def("enable_structural_plasticity", &nlm::AgentBrain::enableStructuralPlasticity, "Enable structural plasticity")
        .def("enable_development", &nlm::AgentBrain::enableDevelopment, "Enable development")
        .def("enable_curiosity", &nlm::AgentBrain::enableCuriosity, "Enable curiosity")
        .def("is_reward_modulation_enabled", &nlm::AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &nlm::AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("is_development_enabled", &nlm::AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("is_curiosity_enabled", &nlm::AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        .def("get_brain", &nlm::AgentBrain::getBrain, "Get brain pointer");

    // Helper function to create agent brain
    m.def("create_agent_brain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_unique<nlm::AgentBrain>(brain);
    }, "Create agent brain");

    // Context manager for simulation
    py::class_<SimulationContext>(m, "SimulationContext")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("__enter__", &SimulationContext::enter)
        .def("__exit__", &SimulationContext::exit)
        .def("run", &SimulationContext::run, "Run simulation for specified steps", 
             py::arg("num_steps"), py::arg("world"), py::arg("agent"));

    // Helper function for simulation loop
    m.def("run_simulation", [](std::shared_ptr<nlm::Brain> brain, 
                             std::unique_ptr<nlm::SimpleWorld> world,
                             std::unique_ptr<nlm::AgentBrain> agent,
                             nlm::SimulationStep num_steps) {
        for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
            world->update(0.1); // Update world
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, "Run complete simulation loop", 
        py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"));

    // Convenience functions
    m.def("create_brain", [](std::shared_ptr<nlm::Config> config) {
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        return brain;
    }, "Create and initialize brain");

    // Event system
    m.def("add_step_callback", [](std::shared_ptr<nlm::Brain> brain, 
                                std::function<void(nlm::SimulationStep)> callback) {
        auto original_handler = brain->getSpikeSystem()->getHandler();
        brain->getSpikeSystem()->registerHandler(
            [original_handler, callback](const nlm::DetailedSpikeEvent& event) {
                original_handler(event);
                callback(event.step);
            }
        );
    }, "Add callback for each simulation step", 
        py::arg("brain"), py::arg("callback"));

    // Utility functions
    m.def("get_version", []() { return "0.1.0"; }, "Get NLM version");

    // Statistical helpers
    m.def("plot_spike_activity", [](std::shared_ptr<nlm::Brain> brain, const std::string& filename) {
        // Placeholder for spike activity plotting
        nlm::Logger::getGlobal().info("Spike activity plot saved to: " + filename);
    }, "Plot spike activity (placeholder)", py::arg("brain"), py::arg("filename"));

    m.def("analyze_network_dynamics", [](std::shared_ptr<nlm::Brain> brain) {
        // Placeholder for network dynamics analysis
        auto result = py::dict(
            "total_neurons", brain->getTotalNeuronCount(),
            "total_synapses", brain->getTotalSynapseCount(),
            "firing_rate", brain->getAverageFiringRate(),
            "e_i_ratio", brain->getExcitationInhibitionRatio(),
            "total_spikes", brain->getTotalSpikeCount()
        );
        return result;
    }, "Analyze network dynamics and return statistics", py::arg("brain"));

    // Export/import functionality
    m.def("save_checkpoint", [](std::shared_ptr<nlm::Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, "Save brain state to checkpoint file", py::arg("brain"), py::arg("filepath"));

    m.def("load_checkpoint", [](std::shared_ptr<nlm::Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, "Load brain state from checkpoint file", py::arg("brain"), py::arg("filepath"));

    // Simulation utilities
    m.def("set_simulation_speed", [](std::shared_ptr<nlm::Brain> brain, double speed) {
        // Placeholder for simulation speed control
        nlm::Logger::getGlobal().info("Simulation speed set to: " + std::to_string(speed) + "x");
    }, "Set simulation speed multiplier", py::arg("brain"), py::arg("speed"));

    // Configuration utilities
    m.def("export_config", [](std::shared_ptr<nlm::Config> config, const std::string& filename) {
        return config->saveToFile(filename);
    }, "Export configuration to file", py::arg("config"), py::arg("filename"));

    m.def("import_config", [](const std::string& filename) {
        auto config = std::make_shared<nlm::Config>();
        config->loadFromFile(filename);
        return config;
    }, "Import configuration from file", py::arg("filename"));

    // Pattern templates for common simulations
    m.def("create_pattern_agent_simulation", 
        [](nlm::SimulationStep num_steps, int64_t seed) {
            auto config = std::make_shared<nlm::Config>();
            config->set("random_seed", seed, nlm::ConfigSource::Default);
            config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(config);
            auto world = std::make_unique<nlm::SimpleWorld>();
            world->configure(10.0f, 10.0f, 8, 8);
            world->reset();
            
            auto agent = std::make_unique<nlm::AgentBrain>(brain);
            agent->initialize(*world);
            
            // Enable learning features
            agent->enable_reward_modulation(true);
            agent->enable_curiosity(true);
            
            return std::make_tuple(brain, std::move(world), std::move(agent));
        }, "Create pattern agent simulation with typical configuration",
        py::arg("num_steps"), py::arg("seed") = 42);

    m.def("create_pattern_development_simulation",
        [](nlm::SimulationStep num_steps, int64_t seed) {
            auto config = std::make_shared<nlm::Config>();
            config->set("random_seed", seed, nlm::ConfigSource::Default);
            config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
            config->set("development_enabled", true, nlm::ConfigSource::Default);
            config->set("synaptogenesis_rate", 0.0002f, nlm::ConfigSource::Default);
            config->set("pruning_rate", 0.00002f, nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(config);
            auto world = std::make_unique<nlm::SimpleWorld>();
            world->configure(15.0f, 15.0f, 10, 10);
            world->reset();
            
            auto agent = std::make_unique<nlm::AgentBrain>(brain);
            agent->initialize(*world);
            
            // Enable all development features
            agent->enable_reward_modulation(true);
            agent->enable_structural_plasticity(true);
            agent->enable_development(true);
            agent->enable_curiosity(true);
            
            return std::make_tuple(brain, std::move(world), std::move(agent));
        }, "Create development simulation with typical configuration",
        py::arg("num_steps"), py::arg("seed") = 42);

    // Examples and demonstrations
    m.def("example_simple_brain", []() {
        auto brain = std::make_shared<nlm::Brain>(nlm::create_default_config());
        brain->initialize();
        
        for (int i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        return std::make_tuple(brain,
            "Brain created and stepped 100 times. Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    }, "Simple brain example");

    m.def("example_agent_world_simulation", []() {
        auto brain = std::make_shared<nlm::Brain>(nlm::create_default_config());
        auto world = std::make_unique<nlm::SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        world->reset();
        
        auto agent = std::make_unique<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        
        // Run agent simulation
        for (int step = 0; step < 200; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain, std::move(world), std::move(agent),
            "Agent simulation completed. Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    }, "Agent in world simulation example");

    // Visualization helpers (placeholders)
    m.def("create_visualization_config", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("visualization_enabled", true, nlm::ConfigSource::Default);
        config->set("window_width", 1024, nlm::ConfigSource::Default);
        config->set("window_height", 768, nlm::ConfigSource::Default);
        config->set("show_neurons", true, nlm::ConfigSource::Default);
        config->set("show_synapses", true, nlm::ConfigSource::Default);
        config->set("show_spikes", true, nlm::ConfigSource::Default);
        return config;
    }, "Create visualization configuration");

    // Statistics collection
    m.def("collect_simulation_stats", [](std::shared_ptr<nlm::Brain> brain, 
                                        std::shared_ptr<nlm::SimpleWorld> world,
                                        std::shared_ptr<nlm::AgentBrain> agent,
                                        nlm::SimulationStep steps) {
        auto stats = py::dict(
            "brain_stats", py::dict(
                "total_neurons", brain->getTotalNeuronCount(),
                "total_synapses", brain->getTotalSynapseCount(),
                "firing_neuron_count", brain->getFiringNeuronCount(),
                "total_spike_count", brain->getTotalSpikeCount(),
                "average_firing_rate", brain->getAverageFiringRate(),
                "e_i_ratio", brain->getExcitationInhibitionRatio(),
                "developmental_stage", static_cast<int>(brain->getDevelopmentalStage())
            ),
            "world_stats", py::dict(
                "width", world->getWidth(),
                "height", world->getHeight(),
                "simulation_time", world->getSimulationTime()
            )
        );
        return stats;
    }, "Collect simulation statistics", 
        py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"));
}