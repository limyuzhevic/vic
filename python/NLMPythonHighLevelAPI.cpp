#include "NLMPythonHighLevelAPI.h"
#include "NLMPythonTypes.h"
#include <algorithm>
#include <sstream>

namespace nlm {
class BrainFactory {
public:
    struct BrainPreset {
        std::string name;
        std::string description;
        json parameters;
        std::map<std::string, std::string> tags;
        
        std::string getName() const { return name; }
        std::string getDescription() const { return description; }
        json getParameters() const { return parameters; }
    };
    
    static std::vector<BrainPreset> getAvailablePresets() {
        std::vector<BrainPreset> presets;
        
        // Minimal brain preset
        BrainPreset minimal;
        minimal.name = "minimal";
        minimal.description = "Minimal brain with just enough neurons for basic activity";
        minimal.parameters = {
            {"brain.neuron_count", 100},
            {"brain.synapse_density", 0.05},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", false},
            {"plasticity.structural.enable", false}
        };
        presets.push_back(minimal);
        
        // Active brain preset
        BrainPreset active;
        active.name = "active";
        active.description = "Brain optimized for high activity and learning";
        active.parameters = {
            {"brain.neuron_count", 1000},
            {"brain.synapse_density", 0.1},
            {"plasticity.stdp.enable", true},
            {"plasticity.stdp.learning_rate", 0.01},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", true},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true}
        };
        presets.push_back(active);
        
        // Stable brain preset
        BrainPreset stable;
        stable.name = "stable";
        stable.description = "Brain optimized for stable, predictable behavior";
        stable.parameters = {
            {"brain.neuron_count", 500},
            {"brain.synapse_density", 0.08},
            {"plasticity.stdp.enable", true},
            {"plasticity.stdp.learning_rate", 0.001},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", false},
            {"development.stage", "Adult"},
            {"neuromod.curiosity.enable", false}
        };
        presets.push_back(stable);
        
        // Social brain preset
        BrainPreset social;
        social.name = "social";
        social.description = "Brain optimized for social learning and interaction";
        social.parameters = {
            {"brain.neuron_count", 1500},
            {"brain.synapse_density", 0.15},
            {"plasticity.stdp.enable", true},
            {"plasticity.stdp.learning_rate", 0.005},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", true},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"memory.episodic.enable", true},
            {"memory.associative.enable", true}
        };
        presets.push_back(social);
        
        // Exploration brain preset
        BrainPreset exploration;
        exploration.name = "exploration";
        exploration.description = "Brain optimized for exploration and discovery";
        exploration.parameters = {
            {"brain.neuron_count", 2000},
            {"brain.synapse_density", 0.2},
            {"plasticity.stdp.enable", true},
            {"plasticity.stdp.learning_rate", 0.02},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", true},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"development.stage", "CriticalPeriod"},
            {"memory.working.capacity", 50}
        };
        presets.push_back(exploration);
        
        return presets;
    }
    
    // Create brain from preset
    static std::shared_ptr<Brain> createFromPreset(const std::string& presetName,
                                                    const std::string& configFile = "") {
        auto presets = getAvailablePresets();
        for (const auto& preset : presets) {
            if (preset.name == presetName) {
                // Create default config
                auto config = std::make_shared<Config>();
                
                // Load preset parameters
                for (auto it = preset.parameters.begin(); it != preset.parameters.end(); ++it) {
                    std::string key = it.key();
                    json value = it.value();
                    
                    // Convert to appropriate C++ type
                    if (value.is_number_integer()) {
                        config->set(key, value.get<int64_t>(), ConfigSource::Default);
                    } else if (value.is_number_float()) {
                        config->set(key, value.get<double>(), ConfigSource::Default);
                    } else if (value.is_string()) {
                        config->set(key, value.get<std::string>(), ConfigSource::Default);
                    } else if (value.is_boolean()) {
                        config->set(key, value.get<bool>(), ConfigSource::Default);
                    }
                }
                
                // Save to file if specified
                if (!configFile.empty()) {
                    config->saveToFile(configFile);
                }
                
                // Create brain
                return std::make_shared<Brain>(config);
            }
        }
        
        // Preset not found, create minimal brain
        return createMinimalBrain();
    }
    
    // Create minimal brain
    static std::shared_ptr<Brain> createMinimalBrain() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100, ConfigSource::Default);
        config->set("brain.synapse_density", 0.05, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }
    
    // Create brain from configuration file
    static std::shared_ptr<Brain> createFromFile(const std::string& configFile) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        return std::make_shared<Brain>(config);
    }
    
    // Create brain with custom parameters
    static std::shared_ptr<Brain> createBrain(const json& parameters, const std::string& configFile = "") {
        auto config = std::make_shared<Config>();
        
        // Apply parameters
        for (auto it = parameters.begin(); it != parameters.end(); ++it) {
            std::string key = it.key();
            json value = it.value();
            
            if (value.is_number_integer()) {
                config->set(key, value.get<int64_t>(), ConfigSource::User);
            } else if (value.is_number_float()) {
                config->set(key, value.get<double>(), ConfigSource::User);
            } else if (value.is_string()) {
                config->set(key, value.get<std::string>(), ConfigSource::User);
            } else if (value.is_boolean()) {
                config->set(key, value.get<bool>(), ConfigSource::User);
            }
        }
        
        // Save to file if specified
        if (!configFile.empty()) {
            config->saveToFile(configFile);
        }
        
        return std::make_shared<Brain>(config);
    }
};

// World setup utilities
class WorldSetup {
public:
    // Create simple world with default settings
    static std::shared_ptr<SimpleWorld> createSimpleWorld() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8); // width, height, visionWidth, visionHeight
        world->setRandomSeed(42);
        world->setMaxEnergy(100.0);
        world->setEnergyDecayRate(0.1);
        return world;
    }
    
    // Create world with custom settings
    static std::shared_ptr<SimpleWorld> createCustomWorld(size_t width, size_t height,
                                                          size_t visionWidth, size_t visionHeight,
                                                          double maxEnergy = 100.0,
                                                          double energyDecayRate = 0.1,
                                                          size_t randomSeed = 42) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setRandomSeed(randomSeed);
        world->setMaxEnergy(maxEnergy);
        world->setEnergyDecayRate(energyDecayRate);
        return world;
    }
    
    // Create world with objects
    static std::shared_ptr<SimpleWorld> createWorldWithObjects(
        size_t width, size_t height,
        const std::vector<WorldObject>& objects) {
        auto world = createSimpleWorld();
        
        // Add objects to world
        for (const auto& obj : objects) {
            world->addObject(obj);
        }
        
        return world;
    }
    
    // Create maze world
    static std::shared_ptr<SimpleWorld> createMazeWorld(size_t width, size_t height) {
        auto world = createSimpleWorld();
        
        // Create walls
        for (size_t x = 0; x < width; ++x) {
            for (size_t y = 0; y < height; ++y) {
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    // Add wall
                    WorldObject wall;
                    wall.x = static_cast<float>(x);
                    wall.y = static_cast<float>(y);
                    wall.type = WorldObjectType::Wall;
                    wall.radius = 0.3f;
                    world->addObject(wall);
                }
            }
        }
        
        return world;
    }
    
    // Create resource world
    static std::shared_ptr<SimpleWorld> createResourceWorld(size_t width, size_t height,
                                                            size_t numResources = 10) {
        auto world = createSimpleWorld();
        
        // Create resources
        std::mt19937 rng(42);
        std::uniform_int_distribution<size_t> distX(2, width - 3);
        std::uniform_int_distribution<size_t> distY(2, height - 3);
        
        for (size_t i = 0; i < numResources; ++i) {
            WorldObject resource;
            resource.x = static_cast<float>(distX(rng));
            resource.y = static_cast<float>(distY(rng));
            resource.type = WorldObjectType::Resource;
            resource.value = 1.0f;
            resource.radius = 0.2f;
            world->addObject(resource);
        }
        
        return world;
    }
};

// Agent setup utilities
class AgentSetup {
public:
    // Create agent with default settings
    static std::shared_ptr<AgentBrain> createAgent(std::shared_ptr<Brain> brain,
                                                    std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable common subsystems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return agent;
    }
    
    // Create agent with custom configuration
    static std::shared_ptr<AgentBrain> createCustomAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        bool rewardModulation = true,
        bool curiosity = true,
        bool development = true,
        bool structuralPlasticity = true,
        bool novelty = true) {
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable/disable subsystems
        agent->enableRewardModulation(rewardModulation);
        agent->enableCuriosity(curiosity);
        agent->enableDevelopment(development);
        agent->enableStructuralPlasticity(structuralPlasticity);
        agent->enableNoveltyDetection(novelty);
        
        return agent;
    }
    
    // Create learning agent
    static std::shared_ptr<AgentBrain> createLearningAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        const std::string& learningStyle = "curiosity") {
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all learning subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        agent->enableNoveltyDetection(true);
        
        // Configure based on learning style
        if (learningStyle == "exploration") {
            agent->setExplorationRate(0.8);
            agent->setExploitationRate(0.2);
        } else if (learningStyle == "exploitation") {
            agent->setExplorationRate(0.2);
            agent->setExploitationRate(0.8);
        } else {
            // Balanced
            agent->setExplorationRate(0.5);
            agent->setExploitationRate(0.5);
        }
        
        return agent;
    }
};

// Complete simulation workflows
class SimulationWorkflows {
public:
    // Run complete agent simulation workflow
    static std::vector<SimulationStats::Stats> runCompleteAgentSimulation(
        const std::string& brainPreset = "active",
        size_t worldWidth = 20,
        size_t worldHeight = 20,
        size_t simulationSteps = 1000,
        bool enableVisualization = false) {
        
        // Step 1: Create brain
        auto brain = BrainFactory::createFromPreset(brainPreset);
        brain->initialize();
        
        // Step 2: Create world
        auto world = WorldSetup::createSimpleWorld();
        world->configure(worldWidth, worldHeight, 8, 8);
        world->reset();
        world->setAgentStart(static_cast<float>(worldWidth / 2), static_cast<float>(worldHeight / 2));
        
        // Step 3: Create agent
        auto agent = AgentSetup::createLearningAgent(brain, world);
        
        // Step 4: Run simulation
        std::vector<SimulationStats::Stats> stats;
        
        SimulationRunner runner(brain, world, agent);
        
        auto progressCallback = [&stats](size_t step, const SimulationStats::Stats& stepStats) {
            stats.push_back(stepStats);
        };
        
        runner.runSteps(simulationSteps, 0.1, progressCallback);
        
        // Step 5: Create visualization if requested
        if (enableVisualization) {
            NeuralActivityVisualizer visualizer;
            visualizer.createComprehensiveVisualization(brain, stats, "complete_simulation.png");
        }
        
        return stats;
    }
    
    // Run exploration simulation
    static std::vector<SimulationStats::Stats> runExplorationSimulation(
        size_t simulationSteps = 2000,
        const std::string& explorationPreset = "exploration") {
        
        // Create exploration-optimized brain
        auto brain = BrainFactory::createFromPreset(explorationPreset);
        brain->initialize();
        
        // Create rich environment
        auto world = WorldSetup::createResourceWorld(30, 30, 20);
        
        // Create curious agent
        auto agent = AgentSetup::createLearningAgent(brain, world, "exploration");
        
        // Run simulation with exploration focus
        SimulationRunner runner(brain, world, agent);
        
        std::vector<SimulationStats::Stats> stats;
        
        auto progressCallback = [&stats](size_t step, const SimulationStats::Stats& stepStats) {
            stats.push_back(stepStats);
            
            // Log exploration metrics
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << "Neurons: " << stepStats.firingNeurons << ", "
                         << "Spikes: " << stepStats.totalSpikes << ", "
                         << "Rate: " << stepStats.averageFiringRate << "\n";
            }
        };
        
        runner.runSteps(simulationSteps, 0.1, progressCallback);
        
        return stats;
    }
    
    // Run social learning simulation
    static std::vector<SimulationStats::Stats> runSocialLearningSimulation(
        size_t numAgents = 3,
        size_t simulationSteps = 1500,
        const std::string& socialPreset = "social") {
        
        // Create social brain for all agents
        auto brain = BrainFactory::createFromPreset(socialPreset);
        
        // Create shared world
        auto world = WorldSetup::createResourceWorld(40, 40, 30);
        
        // Create multiple agents
        std::vector<std::shared_ptr<AgentBrain>> agents;
        for (size_t i = 0; i < numAgents; ++i) {
            auto agent = AgentSetup::createLearningAgent(brain, world, "balanced");
            agents.push_back(agent);
        }
        
        // Run multi-agent simulation
        std::vector<SimulationStats::Stats> allStats;
        
        for (size_t step = 0; step < simulationSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Each agent processes sensory input and takes action
            for (size_t agentId = 0; agentId < agents.size(); ++agentId) {
                auto agent = agents[agentId];
                
                // Get agent's view of the world
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Brain thinks
                brain->step(step);
                
                // Agent decides action
                auto action = agent->decodeMotorCommand();
                
                // Apply action
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward modulation
                double reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
                agent->applyRewardModulation(reward, 0.0);
            }
            
            // Collect statistics
            SimulationStats::Stats stepStats;
            stepStats.steps = step + 1;
            stepStats.totalSpikes = brain->getTotalSpikeCount();
            stepStats.firingNeurons = brain->getFiringNeuronCount();
            stepStats.averageFiringRate = brain->getAverageFiringRate();
            stepStats.spikeHistory[stepStats.steps] = stepStats.totalSpikes;
            
            allStats.push_back(stepStats);
        }
        
        return allStats;
    }
    
    // Run development simulation
    static std::vector<SimulationStats::Stats> runDevelopmentSimulation(
        size_t simulationSteps = 3000,
        const std::string& developmentPreset = "stable") {
        
        // Create developing brain
        auto brain = BrainFactory::createFromPreset(developmentPreset);
        brain->setDevelopmentalStage(DevelopmentalStage::Initial);
        brain->initialize();
        
        // Create simple world
        auto world = WorldSetup::createSimpleWorld();
        world->configure(25, 25, 6, 6);
        world->reset();
        
        // Create developing agent
        auto agent = AgentSetup::createCustomAgent(brain, world, true, true, true, false, false);
        
        // Run development simulation
        std::vector<SimulationStats::Stats> stats;
        
        SimulationRunner runner(brain, world, agent);
        
        auto progressCallback = [&stats](size_t step, const SimulationStats::Stats& stepStats) {
            stats.push_back(stepStats);
            
            // Log development progress
            if (step % 200 == 0) {
                std::cout << "Development Step " << step << ": "
                         << "Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << ", "
                         << "Neurons: " << stepStats.firingNeurons << ", "
                         << "Learning Rate: " << brain->getPlasticityRate() << "\n";
            }
        };
        
        runner.runSteps(simulationSteps, 0.1, progressCallback);
        
        return stats;
    }
};

// Quick-start utilities
class QuickStart {
public:
    // One-shot brain and simulation setup
    static std::vector<SimulationStats::Stats> quickBrainSimulation(
        size_t numNeurons = 200,
        size_t simulationSteps = 500) {
        
        // Create brain with specified size
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", numNeurons, ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create simple world
        auto world = WorldSetup::createSimpleWorld();
        world->configure(15, 15, 6, 6);
        world->reset();
        
        // Create and run simulation
        SimulationRunner runner(brain, world);
        
        std::vector<SimulationStats::Stats> stats;
        
        auto progressCallback = [&stats](size_t step, const SimulationStats::Stats& stepStats) {
            stats.push_back(stepStats);
        };
        
        runner.runSteps(simulationSteps, 0.1, progressCallback);
        
        return stats;
    }
    
    // Quick agent setup
    static std::vector<SimulationStats::Stats> quickAgentSetup(
        const std::string& preset = "active",
        size_t worldSize = 20,
        size_t steps = 800) {
        
        // Use the complete agent simulation workflow
        return SimulationWorkflows::runCompleteAgentSimulation(preset, worldSize, worldSize, steps);
    }
    
    // Quick exploration setup
    static std::vector<SimulationStats::Stats> quickExplorationSetup(
        size_t steps = 1500) {
        return SimulationWorkflows::runExplorationSimulation(steps, "exploration");
    }
    
    // Quick social setup
    static std::vector<SimulationStats::Stats> quickSocialSetup(
        size_t numAgents = 2,
        size_t steps = 1200) {
        return SimulationWorkflows::runSocialLearningSimulation(numAgents, steps, "social");
    }
    
    // Quick development setup
    static std::vector<SimulationStats::Stats> quickDevelopmentSetup(
        size_t steps = 2000) {
        return SimulationWorkflows::runDevelopmentSimulation(steps, "stable");
    }
};

void registerHighLevelAPI(py::module_& m) {
    // Brain factory
    py::class_<BrainFactory>(m, "BrainFactory", "Factory for creating brains with presets and configurations")
        .def_static("get_available_presets", &BrainFactory::getAvailablePresets,
                    "Get list of available brain presets")
        .def_static("create_from_preset", &BrainFactory::createFromPreset,
                    py::arg("preset_name"), py::arg("config_file") = "",
                    "Create brain from preset")
        .def_static("create_minimal_brain", &BrainFactory::createMinimalBrain,
                    "Create minimal brain")
        .def_static("create_from_file", &BrainFactory::createFromFile,
                    py::arg("config_file"),
                    "Create brain from configuration file")
        .def_static("create_brain", &BrainFactory::createBrain,
                    py::arg("parameters"), py::arg("config_file") = "",
                    "Create brain with custom parameters");
    
    // Brain factory preset structure
    py::class_<BrainFactory::BrainPreset>(m, "BrainPreset",
                                          "Brain preset configuration")
        .def_readonly("name", &BrainFactory::BrainPreset::name)
        .def_readonly("description", &BrainFactory::BrainPreset::description)
        .def_readonly("parameters", &BrainFactory::BrainPreset::parameters)
        .def_readonly("tags", &BrainFactory::BrainPreset::tags);
    
    // World setup
    py::class_<WorldSetup>(m, "WorldSetup", "Utilities for setting up worlds")
        .def_static("create_simple_world", &WorldSetup::createSimpleWorld,
                    "Create simple world with default settings")
        .def_static("create_custom_world", &WorldSetup::createCustomWorld,
                    py::arg("width"), py::arg("height"), py::arg("vision_width"),
                    py::arg("vision_height"), py::arg("max_energy") = 100.0,
                    py::arg("energy_decay_rate") = 0.1, py::arg("random_seed") = 42,
                    "Create world with custom settings")
        .def_static("create_maze_world", &WorldSetup::createMazeWorld,
                    py::arg("width"), py::arg("height"),
                    "Create maze world")
        .def_static("create_resource_world", &WorldSetup::createResourceWorld,
                    py::arg("width"), py::arg("height"), py::arg("num_resources") = 10,
                    "Create world with resources");
    
    // Agent setup
    py::class_<AgentSetup>(m, "AgentSetup", "Utilities for setting up agents")
        .def_static("create_agent", &AgentSetup::createAgent,
                    py::arg("brain"), py::arg("world"),
                    "Create agent with default settings")
        .def_static("create_custom_agent", &AgentSetup::createCustomAgent,
                    py::arg("brain"), py::arg("world"), py::arg("reward_modulation") = true,
                    py::arg("curiosity") = true, py::arg("development") = true,
                    py::arg("structural_plasticity") = true, py::arg("novelty") = true,
                    "Create agent with custom configuration")
        .def_static("create_learning_agent", &AgentSetup::createLearningAgent,
                    py::arg("brain"), py::arg("world"), py::arg("learning_style") = "curiosity",
                    "Create learning agent with specified style");
    
    // Simulation workflows
    py::class_<SimulationWorkflows>(m, "SimulationWorkflows", "Complete simulation workflows")
        .def_static("run_complete_agent_simulation", &SimulationWorkflows::runCompleteAgentSimulation,
                    py::arg("brain_preset") = "active", py::arg("world_width") = 20,
                    py::arg("world_height") = 20, py::arg("simulation_steps") = 1000,
                    py::arg("enable_visualization") = false,
                    "Run complete agent simulation workflow")
        .def_static("run_exploration_simulation", &SimulationWorkflows::runExplorationSimulation,
                    py::arg("simulation_steps") = 2000, py::arg("exploration_preset") = "exploration",
                    "Run exploration simulation")
        .def_static("run_social_learning_simulation", &SimulationWorkflows::runSocialLearningSimulation,
                    py::arg("num_agents") = 3, py::arg("simulation_steps") = 1500,
                    py::arg("social_preset") = "social",
                    "Run social learning simulation")
        .def_static("run_development_simulation", &SimulationWorkflows::runDevelopmentSimulation,
                    py::arg("simulation_steps") = 3000, py::arg("development_preset") = "stable",
                    "Run development simulation");
    
    // Quick start utilities
    py::class_<QuickStart>(m, "QuickStart", "Quick-start utilities for common tasks")
        .def_static("quick_brain_simulation", &QuickStart::quickBrainSimulation,
                    py::arg("num_neurons") = 200, py::arg("simulation_steps") = 500,
                    "Quick brain simulation setup")
        .def_static("quick_agent_setup", &QuickStart::quickAgentSetup,
                    py::arg("preset") = "active", py::arg("world_size") = 20,
                    py::arg("steps") = 800,
                    "Quick agent setup")
        .def_static("quick_exploration_setup", &QuickStart::quickExplorationSetup,
                    py::arg("steps") = 1500,
                    "Quick exploration setup")
        .def_static("quick_social_setup", &QuickStart::quickSocialSetup,
                    py::arg("num_agents") = 2, py::arg("steps") = 1200,
                    "Quick social setup")
        .def_static("quick_development_setup", &QuickStart::quickDevelopmentSetup,
                    py::arg("steps") = 2000,
                    "Quick development setup");
    
    // Convenience functions for high-level API
    m.def("run_agent_simulation", [](const std::string& preset = "active",
                                     size_t worldSize = 20,
                                     size_t steps = 1000) {
        return SimulationWorkflows::runCompleteAgentSimulation(preset, worldSize, worldSize, steps);
    }, py::arg("preset") = "active", py::arg("world_size") = 20,
       py::arg("steps") = 1000,
       "Run agent simulation with preset (convenience function)");

    m.def("create_agent_brain", [](const std::string& preset = "active") {
        return BrainFactory::createFromPreset(preset);
    }, py::arg("preset") = "active",
       "Create agent brain with preset (convenience function)");

    m.def("setup_agent_environment", [](size_t width = 20, size_t height = 20) {
        return WorldSetup::createSimpleWorld();
    }, py::arg("width") = 20, py::arg("height") = 20,
       "Setup agent environment (convenience function)");

    m.def("create_exploration_agent", [](const std::string& preset = "exploration") {
        auto brain = BrainFactory::createFromPreset(preset);
        brain->initialize();
        return SimulationWorkflows::runExplorationSimulation(1000, preset);
    }, py::arg("preset") = "exploration",
       "Create exploration agent (convenience function)");

    m.def("create_social_agents", [](size_t numAgents = 3, size_t steps = 1000) {
        return SimulationWorkflows::runSocialLearningSimulation(numAgents, steps, "social");
    }, py::arg("num_agents") = 3, py::arg("steps") = 1000,
       "Create social agents (convenience function)");
}
