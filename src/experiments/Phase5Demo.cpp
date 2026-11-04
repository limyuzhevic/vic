// Phase 5 Demo — Improved CLI Version
// brain-personality / sandbox-teachable brain with enhanced configuration

// This improved demo provides a comprehensive command-line interface for:
// 1. Pattern probe: fresh brain analysis of synthetic percepts
// 2. Sandbox run: closed-loop learning in SimpleWorld  
// 3. Checkpoint round-trip: persistence and state survival
// 4. Configuration via command line and files
// 5. Flexible testing modes

#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace nlm;

// Command-line argument parsing (global namespace)
void printUsage() {
    std::cout << R"(
PHASE 5 DEMO - Brain-Personality / Sandbox-Teachable Brain

USAGE:
    nlm_phase5_demo [--help] [--config <file>] [--demo <mode>] [--test <system>]

DEMOS:
    --demo quick   - Quick pattern probe and sandbox run (default)
    --demo full    - Complete three-part demo
    --demo probe   - Pattern probe only
    --demo sandbox - Sandbox run only
    --demo checkpoint - Checkpoint round-trip only

TEST SYSTEMS:
    --test memory      - Test memory integration
    --test neuromod     - Test neuromodulation integration
    --test cognition   - Test cognition systems
    --test development - Test development system

OPTIONS:
    --neuron-count N    Number of neurons (default: 500)
    --steps S          Number of simulation steps (default: 1000)
    --world-size X,Y,Z  World dimensions (default: 20,20,16,16)
    --seed SEED        Random seed (default: 42)
    --curiosity        Enable curiosity-driven exploration
    --reward-mod        Enable reward modulation
    --checkpoint <file> Checkpoint file path
    --verbose           Enable verbose logging
    --quiet             Minimize output

EXAMPLES:
    nlm_phase5_demo --demo full --neuron-count 1000 --curiosity --reward-mod
    nlm_phase5_demo --demo probe --verbose
    nlm_phase5_demo --config my_config.cfg --demo quick
    nlm_phase5_demo --test memory --world-size 16,16,8,8
)" << std::endl;
}

// Create config with validation
std::shared_ptr<Config> createConfig(size_t neuronCount, int64_t seed, 
                                    float connectionProb, size_t steps,
                                    bool curiosity, bool rewardMod) {
    auto config = std::make_shared<Config>();
    
    // Validate parameters
    if (neuronCount == 0) {
        throw std::invalid_argument("Neuron count must be greater than 0");
    }
    if (steps == 0) {
        throw std::invalid_argument("Step count must be greater than 0");
    }
    
    // Set configuration
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", connectionProb, ConfigSource::Default);
    config->set("random_seed", seed, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    return config;
}

// Parse command line arguments
bool parseArgs(int argc, char* argv[], std::string& demoMode, 
               std::string& testSystem, bool& verbose, bool& quiet,
               size_t& neuronCount, size_t& steps, int64_t& seed,
               float& connectionProb, size_t& worldX, size_t& worldY,
               size_t& worldZ, bool& enableCuriosity, bool& enableRewardMod,
               std::string& checkpointPath) {
    
    // Default values
    demoMode = "quick";
    testSystem = "";
    verbose = false;
    quiet = false;
    neuronCount = 500;
    steps = 1000;
    seed = 42;
    connectionProb = 0.1f;
    worldX = worldY = worldZ = 16;
    enableCuriosity = false;
    enableRewardMod = false;
    checkpointPath = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage();
            return true;
        }
        else if (arg == "--demo" && i + 1 < argc) {
            demoMode = argv[++i];
            std::transform(demoMode.begin(), demoMode.end(), demoMode.begin(), ::tolower);
            if (demoMode != "quick" && demoMode != "full" && demoMode != "probe" && 
                demoMode != "sandbox" && demoMode != "checkpoint") {
                std::cerr << "ERROR: Invalid demo mode: " << demoMode << std::endl;
                return false;
            }
        }
        else if (arg == "--test" && i + 1 < argc) {
            testSystem = argv[++i];
            std::transform(testSystem.begin(), testSystem.end(), testSystem.begin(), ::tolower);
        }
        else if (arg == "--config" && i + 1 < argc) {
            // TODO: Implement config file loading
            std::cout << "Config file loading not yet implemented: " << argv[++i] << std::endl;
        }
        else if (arg == "--neuron-count" && i + 1 < argc) {
            char* end;
            long val = std::strtol(argv[++i], &end, 10);
            if (*end != '\0' || val <= 0) {
                std::cerr << "ERROR: Invalid neuron count: " << argv[i] << std::endl;
                return false;
            }
            neuronCount = static_cast<size_t>(val);
        }
        else if (arg == "--steps" && i + 1 < argc) {
            char* end;
            long val = std::strtol(argv[++i], &end, 10);
            if (*end != '\0' || val <= 0) {
                std::cerr << "ERROR: Invalid steps: " << argv[i] << std::endl;
                return false;
            }
            steps = static_cast<size_t>(val);
        }
        else if (arg == "--seed" && i + 1 < argc) {
            char* end;
            long val = std::strtol(argv[++i], &end, 10);
            if (*end != '\0') {
                std::cerr << "ERROR: Invalid seed: " << argv[i] << std::endl;
                return false;
            }
            seed = val;
        }
        else if (arg == "--world-size" && i + 1 < argc) {
            std::string dims = argv[++i];
            size_t pos1 = dims.find(',');
            size_t pos2 = dims.find(',', pos1 + 1);
            size_t pos3 = dims.find(',', pos2 + 1);
            if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
                std::cerr << "ERROR: Invalid world size format: " << dims << std::endl;
                return false;
            }
            worldX = std::stoull(dims.substr(0, pos1));
            worldY = std::stoull(dims.substr(pos1 + 1, pos2 - pos1 - 1));
            worldZ = std::stoull(dims.substr(pos2 + 1, pos3 - pos2 - 1));
        }
        else if (arg == "--curiosity") {
            enableCuriosity = true;
        }
        else if (arg == "--reward-mod") {
            enableRewardMod = true;
        }
        else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointPath = argv[++i];
        }
        else if (arg == "--verbose") {
            verbose = true;
        }
        else if (arg == "--quiet") {
            quiet = true;
        }
        else if (arg.find('=') != std::string::npos && arg.find('=') > 2 && arg.substr(0, 2) == "--") {
            std::cerr << "WARNING: Command line options with = not fully supported yet: " << arg << std::endl;
        }
        else if (arg[0] == '-') {
            std::cerr << "ERROR: Unknown option: " << arg << std::endl;
            printUsage();
            return false;
        }
    }
    
    return true;
}

const char* cmdName(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        default: return "Wait";
    }
}

SensoryPercept makePercept(const std::vector<float>& vision,
                           const std::vector<float>& touch) {
    SensoryPercept p;
    p.setVision(vision);
    p.setTouch(touch);
    p.setInternal({1.0f, 1.0f, 0.0f, 0.0f});
    p.setProprioception({0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f});
    p.setAudio({});
    return p;
}

// Pattern probe function
void probePattern(const char* label, const SensoryPercept& percept, 
                  size_t neuronCount, int64_t seed) {
    std::cout << "  " << label << ": ";
    
    auto config = createConfig(neuronCount, seed, 0.1f, 50, false, false);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableCuriosity(false);
    
    // Present pattern multiple times
    uint64_t step = 0;
    for (int action = 0; action < 50; ++action) {
        agent.processSensoryInput(percept);
        for (int i = 0; i < 10; brain->step(step++)) {}
    }
    
    MotorCommand cmd = agent.decodeMotorCommand();
    auto acts = agent.getMotorGroupActivities();
    
    // Calculate winner margin
    float best = 0.0f, second = 0.0f;
    for (size_t i = 0; i < 5 && i < acts.size(); ++i) {
        float r = acts[i].firingRate;
        if (r > best) { second = best; best = r; }
        else if (r > second) { second = r; }
    }
    
    float margin = best - second;
    std::cout << "winner=" << cmdName(cmd) 
              << " rawMargin=" << margin << " Hz"
              << " (best=" << best << "s, second=" << second << "s)" << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string demoMode;
    std::string testSystem;
    bool verbose = false;
    bool quiet = false;
    size_t neuronCount = 500;
    size_t steps = 1000;
    int64_t seed = 42;
    float connectionProb = 0.1f;
    size_t worldX = 16, worldY = 16, worldZ = 16;
    bool enableCuriosity = false;
    bool enableRewardMod = false;
    std::string checkpointPath = "";
    
    if (!parseArgs(argc, argv, demoMode, testSystem, verbose, quiet,
                   neuronCount, steps, seed, connectionProb, worldX, worldY, worldZ,
                   enableCuriosity, enableRewardMod, checkpointPath)) {
        return 1;
    }
    
    // Initialize logging
    Logger::getGlobal().setLevel(verbose ? LogLevel::Info : LogLevel::Error);
    
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 5 - BRAIN-PERSONALITY / SANDBOX DEMO\n";
    std::cout << "===========================================\n\n";
    
    if (!quiet) {
        std::cout << "Configuration: " << neuronCount << " neurons, " << steps << " steps, seed=" << seed << std::endl;
        if (enableCuriosity) std::cout << "Features: Curiosity-driven exploration enabled" << std::endl;
        if (enableRewardMod) std::cout << "Features: Reward modulation enabled" << std::endl;
        if (!checkpointPath.empty()) std::cout << "Checkpoint: " << checkpointPath << std::endl;
        std::cout << std::endl;
    }
    
    // Run demo mode
    if (demoMode == "probe" || demoMode == "quick" || demoMode == "full") {
        std::cout << "--- 1. Pattern probe (fresh brain per pattern, seed " << seed << ") ---\n";
        
        std::vector<float> emptyVision(16, 1.0f);
        std::vector<float> noTouch(8, 0.0f);
        probePattern("empty scene      ", makePercept(emptyVision, noTouch), neuronCount, seed);
        
        std::vector<float> brightVision(16, 1.0f);
        for (int i = 5; i <= 10; ++i) brightVision[i] = 0.7f;
        std::vector<float> fwdTouch(8, 0.0f);
        fwdTouch[0] = 0.8f;
        probePattern("bright ahead+touch", makePercept(brightVision, fwdTouch), neuronCount, seed);
        
        std::vector<float> darkVision(16, 1.0f);
        for (int i = 5; i <= 10; ++i) darkVision[i] = 0.25f;
        probePattern("dark ahead+touch  ", makePercept(darkVision, fwdTouch), neuronCount, seed);
        
        if (demoMode == "probe" || demoMode == "quick") {
            std::cout << "\nPattern probe demo complete.\n";
            return 0;
        }
    }
    
    if (demoMode == "sandbox" || demoMode == "quick" || demoMode == "full") {
        std::cout << "\n--- 2. Sandbox run (" << steps << " steps) ---\n";
        
        auto config = createConfig(neuronCount, seed, connectionProb, steps,
                                   enableCuriosity, enableRewardMod);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        SimpleWorld world;
        world.configure(static_cast<float>(worldX), static_cast<float>(worldY), worldZ, worldZ);
        world.setRandomSeed(seed);
        world.reset();
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(enableRewardMod);
        agent.enableCuriosity(enableCuriosity);
        
        float totalReward = 0.0f;
        int resources = 0, hazards = 0, emptyInteracts = 0;
        std::set<std::pair<int, int>> visited;
        uint64_t brainStep = 0;
        
        for (int step = 0; step < steps; ++step) {
            const SensoryPercept& percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            for (int i = 0; i < 10; brain->step(brainStep++)) {}
            MotorCommand cmd = agent.decodeMotorCommand();
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            agent.applyRewardModulation(result.reward, 0.0f);
            world.update(0.1);
            totalReward += result.reward;
            
            if (cmd == MotorCommand::Interact) {
                if (result.message == "consumed_resource") ++resources;
                else if (result.message == "hit_hazard") ++hazards;
                else ++emptyInteracts;
            }
            
            const auto& body = world.getAgentBody();
            visited.insert({static_cast<int>(std::floor(body.x)),
                           static_cast<int>(std::floor(body.y))});
            
            if (step % 25 == 0) {
                std::cout << "  step " << step << ": " << agent.getThoughtTrace()
                          << " wmNeurons=" << (brain->getWorkingMemory() ? 
                              brain->getWorkingMemory()->getMemoryNeuronCount() : 0)
                          << " firing=" << brain->getFiringNeuronCount() << "\n";
            }
        }
        
        std::cout << "  sandbox: reward=" << totalReward
                  << " resources=" << resources << " hazards=" << hazards
                  << " emptyInteracts=" << emptyInteracts
                  << " visitedCells=" << visited.size()
                  << " avgRate=" << brain->getAverageFiringRate()
                  << " totalSpikes=" << brain->getTotalSpikeCount()
                  << " workingNeurons=" << (brain->getWorkingMemory() ? 
                      brain->getWorkingMemory()->getMemoryNeuronCount() : 0)
                  << " episodic=" << (brain->getEpisodicMemory() ? 
                      brain->getEpisodicMemory()->getEpisodeCount() : 0)
                  << " preference=" << agent.getLearnedPreference() << "\n";
        
        if (demoMode == "sandbox" || demoMode == "quick") {
            std::cout << "\nSandbox demo complete.\n";
            return 0;
        }
    }
    
    if (demoMode == "checkpoint" || demoMode == "full") {
        std::cout << "\n--- 3. Checkpoint round-trip ---\n";
        
        auto config = createConfig(neuronCount, seed, connectionProb, steps,
                                   enableCuriosity, enableRewardMod);
        auto brain1 = std::make_shared<Brain>(config);
        brain1->initialize();
        
        // Run some steps
        for (int i = 0; i < 100; ++i) {
            brain1->step(i, i * 0.001);
        }
        
        float avgFiring1 = brain1->getAverageFiringRate();
        size_t totalSpikes1 = brain1->getTotalSpikeCount();
        
        std::cout << "Brain1 avg firing: " << avgFiring1 << std::endl;
        std::cout << "Brain1 total spikes: " << totalSpikes1 << std::endl;
        
        // Save checkpoint
        std::string checkpointFile = checkpointPath.empty() ? "/tmp/nlm_phase5_demo.ckpt" : checkpointPath;
        bool saved = brain1->save(checkpointFile);
        auto brain2 = std::make_shared<Brain>(createConfig(neuronCount, seed, connectionProb, steps,
                                                             enableCuriosity, enableRewardMod));
        brain2->initialize();
        bool loaded = saved && brain2->load(checkpointFile);
        
        std::cout << "  checkpoint: saved=" << (saved ? "yes" : "NO")
                  << " loaded=" << (loaded ? "yes" : "NO");
        if (loaded) {
            // RNG continuity test
            uint32_t drawSaved = brain1->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(1000000));
            uint32_t drawReloaded = brain2->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(1000000));
            std::cout << " totalSpikes=" << brain2->getTotalSpikeCount()
                      << " workingNeurons=" << (brain2->getWorkingMemory() ? 
                          brain2->getWorkingMemory()->getMemoryNeuronCount() : 0)
                      << " episodic=" << (brain2->getEpisodicMemory() ? 
                          brain2->getEpisodicMemory()->getEpisodeCount() : 0)
                      << " preference=" << brain2->getLearnedPreference()
                      << " rngMatch=" << (drawSaved == drawReloaded ? "yes" : "NO");
            
            // Continue the reloaded brain
            AgentBrain agent2(brain2);
            agent2.initialize(world);
            agent2.enableCuriosity(false);
            const SensoryPercept& percept = world.getSensoryPercept();
            agent2.processSensoryInput(percept);
            for (int i = 0; i < 10; brain2->step(brainStep++)) {}
            std::cout << " continuedCmd=" << cmdName(agent2.decodeMotorCommand());
        }
        std::cout << "\n\nCheckpoint demo complete.\n";
    }
    
    std::cout << "Phase 5 demonstration complete.\n";
    return 0;
}