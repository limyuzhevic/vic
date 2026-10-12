// NLM Agent System with Enhanced CLI
// Provides advanced command-line interface for NLM agents
// Supports multiple modes: demo, training, evaluation, debugging

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include "memory/Memory.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <map>
#include <algorithm>

using namespace nlm;

void printBanner(const std::string& title = "NLM Agent System") {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM Agent System - Enhanced Neural Learning Machine        ║
    ║                                                               ║
    ║     Advanced Agent Architecture with Full Memory Integration    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << "\n=== NLM Agent System Usage ===" << std::endl;
    std::cout << "nlm_agent [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --mode MODE              Operation mode: demo, train, eval, debug (default: demo)" << std::endl;
    std::cout << "  --episodes N             Number of training episodes (default: 100)" << std::endl;
    std::cout << "  --steps-per-episode N     Steps per episode (default: 1000)" << std::endl;
    std::cout << "  --world-width N           World width in meters (default: 20.0)" << std::endl;
    std::cout << "  --world-height N          World height in meters (default: 20.0)" << std::endl;
    std::cout << "  --vision-width N          Vision sensor width (default: 16)" << std::endl;
    std::cout << "  --vision-height N         Vision sensor height (default: 16)" << std::endl;
    std::cout << "  --brain-regions N         Number of neural regions (default: 3)" << std::endl;
    std::cout << "  --neurons-per-region N    Neurons per region (default: 200)" << std::endl;
    std::cout << "  --seed N                  Random seed for reproducibility (default: 42)" << std::endl;
    std::cout << "  --checkpoint PATH          Save brain state to file" << std::endl;
    std::cout << "  --load PATH               Load brain state from file" << std::endl;
    std::cout << "  --memory-capacity N       Working memory capacity (default: 500)" << std::endl;
    std::cout << "  --memory-decay R           Working memory decay rate (default: 0.01)" << std::endl;
    std::cout << "  --enable-dev              Enable developmental plasticity" << std::endl;
    std::cout << "  --enable-curiosity        Enable curiosity-driven exploration" << std::endl;
    std::cout << "  --enable-reward           Enable reward modulation" << std::endl;
    std::cout << "  --log-level L             Logging level: trace, debug, info, warn, error, fatal" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_agent --mode demo --world-width 30 --world-height 30" << std::endl;
    std::cout << "  nlm_agent --mode train --episodes 1000 --steps-per-episode 2000 --enable-curiosity" << std::endl;
    std::cout << "  nlm_agent --mode eval --load checkpoint.brain --episodes 50" << std::endl;
    std::cout << "  nlm_agent --mode debug --log-level debug --enable-dev" << std::endl;
}

class NLMArgumentParser {
public:
    struct Arguments {
        std::string mode = "demo";
        size_t episodes = 100;
        size_t stepsPerEpisode = 1000;
        float worldWidth = 20.0f;
        float worldHeight = 20.0f;
        size_t visionWidth = 16;
        size_t visionHeight = 16;
        size_t brainRegions = 3;
        size_t neuronsPerRegion = 200;
        uint64_t seed = 42;
        std::string checkpointPath;
        std::string loadPath;
        size_t memoryCapacity = 500;
        float memoryDecay = 0.01f;
        bool enableDev = true;
        bool enableCuriosity = true;
        bool enableReward = true;
        std::string logLevel = "info";
        bool showHelp = false;
    };

    static Arguments parse(int argc, char** argv) {
        Arguments args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                args.showHelp = true;
            } else if (arg.find("--") == 0) {
                std::string key = arg.substr(2);
                size_t eqPos = key.find('=');
                
                if (eqPos != std::string::npos) {
                    std::string value = key.substr(eqPos + 1);
                    key = key.substr(0, eqPos);
                    processArgument(args, key, value);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    processArgument(args, key, argv[++i]);
                } else {
                    processArgument(args, key, "true");
                }
            }
        }
        
        return args;
    }
    
private:
    static void processArgument(Arguments& args, const std::string& key, const std::string& value) {
        if (key == "mode") {
            args.mode = value;
        } else if (key == "episodes") {
            args.episodes = std::stoul(value);
        } else if (key == "steps-per-episode") {
            args.stepsPerEpisode = std::stoul(value);
        } else if (key == "world-width") {
            args.worldWidth = std::stof(value);
        } else if (key == "world-height") {
            args.worldHeight = std::stof(value);
        } else if (key == "vision-width") {
            args.visionWidth = std::stoul(value);
        } else if (key == "vision-height") {
            args.visionHeight = std::stoul(value);
        } else if (key == "brain-regions") {
            args.brainRegions = std::stoul(value);
        } else if (key == "neurons-per-region") {
            args.neuronsPerRegion = std::stoul(value);
        } else if (key == "seed") {
            args.seed = std::stoull(value);
        } else if (key == "checkpoint") {
            args.checkpointPath = value;
        } else if (key == "load") {
            args.loadPath = value;
        } else if (key == "memory-capacity") {
            args.memoryCapacity = std::stoul(value);
        } else if (key == "memory-decay") {
            args.memoryDecay = std::stof(value);
        } else if (key == "enable-dev") {
            args.enableDev = (value == "true" || value == "1" || value == "yes");
        } else if (key == "enable-curiosity") {
            args.enableCuriosity = (value == "true" || value == "1" || value == "yes");
        } else if (key == "enable-reward") {
            args.enableReward = (value == "true" || value == "1" || value == "yes");
        } else if (key == "log-level") {
            args.logLevel = value;
        }
    }
};

void setupLogging(const std::string& level) {
    auto logger = std::make_shared<Logger>();
    
    LogLevel logLevel;
    if (level == "trace") logLevel = LogLevel::Trace;
    else if (level == "debug") logLevel = LogLevel::Debug;
    else if (level == "info") logLevel = LogLevel::Info;
    else if (level == "warn") logLevel = LogLevel::Warn;
    else if (level == "error") logLevel = LogLevel::Error;
    else if (level == "fatal") logLevel = LogLevel::Fatal;
    else logLevel = LogLevel::Info;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Logging level set to: " + level);
}

class NLMExperiment {
public:
    struct ExperimentResults {
        double totalReward;
        double averageStepReward;
        size_t totalSteps;
        size_t successfulActions;
        float finalMemoryUsage;
        float finalSkillProficiency;
        double learningCurve[10]; // Average reward per episode
    };

    NLMExperiment(const NLMArgumentParser::Arguments& args)
        : args_(args), world_(std::make_unique<SimpleWorld>()), 
          agentBody_(std::make_unique<AgentBody>()),
          agentBrain_(nullptr), brain_(nullptr),
          workingMemory_(std::make_unique<WorkingMemory>()),
          episodicMemory_(std::make_unique<EpisodicMemory>()),
          semanticMemory_(std::make_unique<SemanticMemory>()),
          proceduralMemory_(std::make_unique<ProceduralMemory>()),
          associativeMemory_(std::make_unique<AssociativeMemory>()) {}

    bool run() {
        printBanner();
        
        // Setup logging
        setupLogging(args_.logLevel);
        
        NLM_LOG_INFO("Starting NLM Agent Experiment: " + args_.mode);
        NLM_LOG_INFO("Configuration: " + std::to_string(args_.episodes) + " episodes, " +
                    std::to_string(args_.stepsPerEpisode) + " steps per episode");
        
        // Configure world
        world_->configure(args_.worldWidth, args_.worldHeight, 
                         args_.visionWidth, args_.visionHeight);
        world_->setAgentStart(args_.worldWidth / 2.0f, args_.worldHeight / 2.0f);
        world_->setRandomSeed(args_.seed);
        
        // Initialize brain
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(args_.seed), ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(args_.brainRegions * args_.neuronsPerRegion), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(args_.brainRegions), ConfigSource::CommandLine);
        
        brain_ = std::make_shared<Brain>(config);
        
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        // Initialize agent brain
        agentBrain_ = std::make_unique<AgentBrain>(brain_);
        agentBrain_->initialize(*world_);
        
        // Configure memory systems
        workingMemory_->getCurrentSize(); // This will need to be modified to accept capacity
        
        // Load checkpoint if specified
        if (!args_.loadPath.empty()) {
            if (!brain_->load(args_.loadPath)) {
                NLM_LOG_WARN("Failed to load checkpoint from: " + args_.loadPath);
            } else {
                NLM_LOG_INFO("Successfully loaded checkpoint from: " + args_.loadPath);
            }
        }
        
        // Run experiment based on mode
        if (args_.mode == "demo") {
            runDemoMode();
        } else if (args_.mode == "train") {
            runTrainingMode();
        } else if (args_.mode == "eval") {
            runEvaluationMode();
        } else if (args_.mode == "debug") {
            runDebugMode();
        } else {
            NLM_LOG_ERROR("Unknown mode: " + args_.mode);
            return false;
        }
        
        // Save checkpoint if specified
        if (!args_.checkpointPath.empty()) {
            if (brain_->save(args_.checkpointPath)) {
                NLM_LOG_INFO("Successfully saved checkpoint to: " + args_.checkpointPath);
            } else {
                NLM_LOG_ERROR("Failed to save checkpoint to: " + args_.checkpointPath);
            }
        }
        
        return true;
    }
    
    const ExperimentResults& getResults() const { return results_; }
    
private:
    void runDemoMode() {
        NLM_LOG_INFO("=== DEMO MODE ===");
        NLM_LOG_INFO("Running basic agent demonstration with all memory systems");
        
        for (size_t episode = 0; episode < args_.episodes; ++episode) {
            NLM_LOG_INFO("Episode " + std::to_string(episode + 1) + "/" + std::to_string(args_.episodes));
            
            runEpisode(episode);
            
            // Collect episodic memory data
            auto recentEpisodes = episodicMemory_->getRecentEpisodes(5);
            if (!recentEpisodes.empty()) {
                NLM_LOG_INFO("Recent episodic memory: " + std::to_string(recentEpisodes.size()) + " episodes stored");
            }
        }
    }
    
    void runTrainingMode() {
        NLM_LOG_INFO("=== TRAINING MODE ===");
        NLM_LOG_INFO("Training agent with curiosity and reward modulation");
        
        for (size_t episode = 0; episode < args_.episodes; ++episode) {
            NLM_LOG_INFO("Training Episode " + std::to_string(episode + 1) + "/" + std::to_string(args_.episodes));
            
            runEpisode(episode);
            
            // Apply procedural learning based on experience
            if (episode % 100 == 0) {
                learnFromExperience();
            }
            
            // Log progress
            if (episode % 20 == 0) {
                logTrainingProgress(episode);
            }
        }
    }
    
    void runEvaluationMode() {
        NLM_LOG_INFO("=== EVALUATION MODE ===");
        NLM_LOG_INFO("Evaluating trained agent performance");
        
        double totalReward = 0.0;
        size_t totalActions = 0;
        
        for (size_t episode = 0; episode < args_.episodes; ++episode) {
            NLM_LOG_INFO("Evaluation Episode " + std::to_string(episode + 1) + "/" + std::to_string(args_.episodes));
            
            runEpisode(episode);
            
            // For evaluation, we just want to see final performance
            // No learning or adaptation during evaluation
        }
    }
    
    void runDebugMode() {
        NLM_LOG_INFO("=== DEBUG MODE ===");
        NLM_LOG_INFO("Running detailed diagnostic and debugging");
        
        // Run shorter demonstration with detailed logging
        for (size_t episode = 0; episode < std::min(args_.episodes, 10u); ++episode) {
            NLM_LOG_INFO("Debug Episode " + std::to_string(episode + 1) + "/10");
            
            runEpisode(episode);
            
            // Log detailed memory state
            logMemoryDebugInfo(episode);
        }
    }
    
    void runEpisode(size_t episode) {
        // Reset agent and world for new episode
        brain_->reset();
        agentBrain_->reset();
        world_->reset();
        world_->setAgentStart(args_.worldWidth / 2.0f, args_.worldHeight / 2.0f);
        
        double episodeReward = 0.0;
        size_t stepsCompleted = 0;
        
        for (size_t step = 0; step < args_.stepsPerEpisode; ++step) {
            // Get sensory input
            const auto& percept = world_->getSensoryPercept();
            
            // Process through agent brain
            agentBrain_->processSensoryInput(percept);
            
            // Get motor command
            MotorCommand command = agentBrain_->decodeMotorCommand();
            
            // Apply command to world
            ActionResult result = world_->applyMotorCommand(command, world_->getSimulationTime());
            
            // Record experience
            recordExperience(percept, command, result, step);
            
            // Apply neuromodulation if enabled
            if (args_.enableReward) {
                agentBrain_->applyRewardModulation(result.reward, agentBrain_->getPredictionError());
            }
            
            // Update development if enabled
            if (args_.enableDev) {
                agentBrain_->updateDevelopment(world_->getSimulationTime());
            }
            
            episodeReward += result.reward;
            stepsCompleted++;
            
            // Progress world
            world_->update(world_->getSimulationTime());
            
            // Log step progress in debug mode
            if (args_.mode == "debug" && step % 100 == 0) {
                NLM_LOG_DEBUG("Step " + std::to_string(step) + ": Reward = " + std::to_string(result.reward));
            }
        }
        
        // Update results
        results_.totalReward += episodeReward;
        results_.totalSteps += stepsCompleted;
        results_.learningCurve[episode % 10] += episodeReward;
        
        // Log episode summary
        NLM_LOG_INFO("Episode " + std::to_string(episode + 1) + " complete: Reward = " + std::to_string(episodeReward));
    }
    
    void recordExperience(const SensoryPercept& percept, MotorCommand command, 
                          const ActionResult& result, size_t step) {
        // Create episodic memory entry
        EpisodicMemoryItem episode;
        episode.timestamp = step;
        
        // Extract relevant features from perception
        std::vector<float> perceptFeatures;
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < std::min(size_t(10), vision.size()); ++i) {
            perceptFeatures.push_back(vision[i]);
        }
        
        episode.values = perceptFeatures;
        episode.metadata = "action_" + std::to_string(static_cast<int>(command)) + 
                         "," + std::to_string(result.reward);
        
        // Store in episodic memory
        episodicMemory_->storeEpisode(episode);
        
        // Store semantic knowledge
        if (result.success) {
            semanticMemory_->storeFact("action_success_" + std::to_string(static_cast<int>(command)),
                                     std::to_string(result.reward));
        }
        
        // Learn procedural skill
        std::vector<NeuronId> skillPattern = {static_cast<NeuronId>(command), static_cast<NeuronId>(step % 1000)};
        proceduralMemory_->learnSkill(std::to_string(static_cast<int>(command)), skillPattern);
        
        // Update associative memory
        for (size_t i = 0; i < perceptFeatures.size() && i < 5; ++i) {
            if (perceptFeatures[i] > 0.5f) {
                associateNeuron(static_cast<NeuronId>(i), static_cast<NeuronId>(command), 0.3f);
            }
        }
    }
    
    void associateNeuron(NeuronId a, NeuronId b, float strength) {
        associativeMemory_->associate(a, b, strength);
        
        // Create Hebbian-like association
        if (a != b) {
            // Bidirectional association
            associativeMemory_->associate(b, a, strength * 0.9f);
        }
    }
    
    void learnFromExperience() {
        NLM_LOG_INFO("Learning from accumulated experience...");
        
        // Consolidate episodic memory
        episodicMemory_->consolidate(0.6f); // Low threshold for consolidation
        
        // Apply semantic consolidation
        // (would need additional semantic consolidation methods)
        
        // Strengthen frequently used procedural skills
        auto skills = proceduralMemory_->getAllSkills();
        for (auto* skill : skills) {
            if (skill->proficiency < 0.8f) {
                proceduralMemory_->updateProficiency(skill->name, 0.05f);
            }
        }
        
        NLM_LOG_INFO("Experience learning completed");
    }
    
    void logTrainingProgress(size_t episode) {
        double avgReward = results_.totalReward / (episode + 1);
        NLM_LOG_INFO("Training Progress - Episode " + std::to_string(episode + 1) + ": Avg Reward = " + std::to_string(avgReward));
        
        // Log memory system statistics
        NLM_LOG_INFO("Memory Stats - Working: " + std::to_string(workingMemory_->getCurrentSize()) + "/" + 
                    std::to_string(workingMemory_->getCapacity()) + 
                    ", Episodic: " + std::to_string(episodicMemory_->getEpisodeCount()) + 
                    ", Skills: " + std::to_string(proceduralMemory_->getAllSkillNames().size()));
    }
    
    void logMemoryDebugInfo(size_t episode) {
        NLM_LOG_DEBUG("=== Memory Debug Info (Episode " + std::to_string(episode + 1) + ") ===");
        
        // Working memory
        NLM_LOG_DEBUG("Working Memory: " + std::to_string(workingMemory_->getCurrentSize()) + "/" + 
                     std::to_string(workingMemory_->getCapacity()) + " items");
        
        // Episodic memory
        NLM_LOG_DEBUG("Episodic Memory: " + std::to_string(episodicMemory_->getEpisodeCount()) + " episodes");
        
        // Semantic memory
        NLM_LOG_DEBUG("Semantic Memory: " + std::to_string(semanticMemory_->getAllFacts().size()) + " facts");
        
        // Procedural memory
        auto skillNames = proceduralMemory_->getAllSkillNames();
        NLM_LOG_DEBUG("Procedural Memory: " + std::to_string(skillNames.size()) + " skills");
        
        for (const auto& skillName : skillNames) {
            float proficiency = proceduralMemory_->getSkillProficiency(skillName);
            NLM_LOG_DEBUG("  - " + skillName + ": " + std::to_string(proficiency));
        }
        
        // Associative memory
        auto associations = associativeMemory_->getAllAssociations();
        NLM_LOG_DEBUG("Associative Memory: " + std::to_string(associations.size()) + " associations");
        
        // Developmental stage
        if (agentBrain_) {
            NLM_LOG_DEBUG("Agent Developmental Stage: " + std::to_string(static_cast<int>(agentBrain_->getDevelopmentalStage())));
            NLM_LOG_DEBUG("Agent Neuromodulation Level: " + std::to_string(agentBrain_->getNeuromodulationLevel()));
            NLM_LOG_DEBUG("Agent Curiosity Level: " + std::to_string(agentBrain_->getCuriosityLevel()));
        }
    }
    
    NLMArgumentParser::Arguments args_;
    std::unique_ptr<SimpleWorld> world_;
    std::unique_ptr<AgentBody> agentBody_;
    std::unique_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<WorkingMemory> workingMemory_;
    std::unique_ptr<EpisodicMemory> episodicMemory_;
    std::unique_ptr<SemanticMemory> semanticMemory_;
    std::unique_ptr<ProceduralMemory> proceduralMemory_;
    std::unique_ptr<AssociativeMemory> associativeMemory_;
    ExperimentResults results_;
};

int main(int argc, char** argv) {
    auto args = NLMArgumentParser::parse(argc, argv);
    
    if (args.showHelp) {
        printUsage();
        return 0;
    }
    
    NLMExperiment experiment(args);
    
    if (!experiment.run()) {
        std::cerr << "Experiment failed!" << std::endl;
        return 1;
    }
    
    // Print final results
    std::cout << "\n=== Experiment Complete ===" << std::endl;
    std::cout << "Total Reward: " << experiment.getResults().totalReward << std::endl;
    std::cout << "Average Reward per Step: " << experiment.getResults().averageStepReward << std::endl;
    std::cout << "Total Steps: " << experiment.getResults().totalSteps << std::endl;
    
    return 0;
}