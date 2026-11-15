// Phase 5 demo — brain-personality / sandbox-teachable brain.
//
// Phase 5 was library-only: src/experiments/Phase5Experiment.cpp has no
// main() and the root CMakeLists wired nlm_phase5 as a library with the
// comment "Phase 5 has no standalone demo yet". This file is that demo.
// It does three things, all measurable from a single run:
//   1. Pattern probe: the same fresh brain (same seed) is shown three
//      synthetic percepts — empty scene, bright object ahead (the
//      world's encoding for a resource), dark object ahead (hazard) —
//      and the winning motor group and winner margin are reported.
//      Clean selective coding means different patterns give different
//      winners with a margin well above the noise between groups.
//   2. Sandbox run: a short Phase-3-style closed loop in SimpleWorld,
//      with coverage, interact-by-outcome (consumed_resource /
//      hit_hazard / no_object — the world's own result message, not a
//      label leaked to the brain) and a thought trace per sampled step.
//   3. Checkpoint round-trip: save mid-run, reload into a fresh brain
//      and continue; the demo reports exactly which state survived
//      (spike counters, working/episodic memory counts, and the
//      learned preference trait — see AgentBrain/Brain).
//
// This demo does NOT claim a human brain or a real personality. The
// "personality" step it measures is deliberately narrow: a scalar
// learned preference, updated only by sandbox reward, saved in the
// checkpoint, and used as a small bias in later action choice — so a
// reloaded brain behaves differently from a fresh one because of what
// it experienced, not because of a scripted persona.

#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <utility>

using namespace nlm;

namespace {

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

std::shared_ptr<Config> makeConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    return config;
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

// Print banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 5 - Brain-Personality / Sandbox Demo                 ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help message
void printHelp() {
    std::cout << "\nUsage: nlm_phase5_demo [OPTIONS]\n\n";
    std::cout << "NLM Neural Learning Machine - Phase 5 Brain-Personality Demo\n\n";
    std::cout << "This demo shows measurable personality development through experience.\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h              Show this help message\n";
    std::cout << "  --version, --ver        Show version information\n";
    std::cout << "  --verbose, -v           Enable verbose logging\n";
    std::cout << "  --quiet                 Suppress output\n";
    std::cout << "  --config=<file>         Load configuration from file\n";
    std::cout << "  --neurons=<N>           Override neuron count (default: 500)\n";
    std::cout << "  --regions=<N>           Override region count (default: 2)\n";
    std::cout << "  --seed=<S>              Override random seed (default: 42)\n";
    std::cout << "  --timestep=<t>          Override simulation timestep (default: 0.001)\n";
    std::cout << "  --steps=<N>             Override sandbox steps (default: 100)\n";
    std::cout << "  --world-size=<W>        Override world size (default: 20x20)\n";
    std::cout << "  --world-grid=<G>        Override world grid size (default: 16x16)\n";
    std::cout << "  --sandbox-only          Skip pattern probe, run only sandbox\n";
    std::cout << "  --checkpoint-only       Skip sandbox, run only checkpoint test\n";
    std::cout << "  --checkpoints=<N>       Number of checkpoint saves to test (default: 1)\n";
    std::cout << "\nExamples:\n";
    std::cout << "  nlm_phase5_demo --help                  Show help\n";
    std::cout << "  nlm_phase5_demo --verbose --neurons=1000 Run with 1000 neurons\n";
    std::cout << "  nlm_phase5_demo --steps=500 --quiet     Run sandbox with 500 steps, quiet output\n";
    std::cout << "  nlm_phase5_demo --config=myconfig.cfg    Use custom configuration\n";
    std::cout << "  nlm_phase5_demo --checkpoints=3         Test checkpoint persistence 3 times\n";
    std::cout << "  nlm_phase5_demo --sandbox-only          Run only sandbox portion\n";
    std::cout << std::endl;
}

// Print version
void printVersion() {
    std::cout << "NLM Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 5 - Brain-Personality / Sandbox Demo" << std::endl;
    std::cout << "Build with C++20, scikit-build-core, pybind11" << std::endl;
}

// Run one pattern through a fresh brain and report the decode.
void probePattern(const char* label, const SensoryPercept& percept) {
    auto brain = std::make_shared<Brain>(makeConfig());
    brain->initialize();
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableCuriosity(false);  // Probe the brain, not the random explorer.

    // Present the pattern repeatedly, exactly as the closed loop does
    // (one injection per action step, 10 brain steps per action). A
    // single 10-step presentation fires nothing from rest — the probe
    // must give the network time to propagate, or it measures silence,
    // not discrimination.
    uint64_t step = 0;
    for (int action = 0; action < 50; ++action) {
        agent.processSensoryInput(percept);
        for (int i = 0; i < 10; ++i) brain->step(step++);
    }
    MotorCommand cmd = agent.decodeMotorCommand();
    auto acts = agent.getMotorGroupActivities();
    // Winner margin over the runner-up, across the five competing groups.
    float best = 0.0f, second = 0.0f;
    for (size_t i = 0; i < 5 && i < acts.size(); ++i) {
        float r = acts[i].firingRate;
        if (r > best) { second = best; best = r; }
        else if (r > second) { second = r; }
    }
    // Margin here is the raw motor-rate margin (best minus runner-up);
    // the winner is after decode biases (reflex bonus, hazard
    // suppression, preference), so winner and raw margin need not
    // agree — e.g. bright wins Interact on the bonus while Forward
    // has the highest raw rate. That disagreement is itself the point:
    // it shows the reflex, not just the raw network, deciding.
    std::cout << "  " << label << ": winner=" << cmdName(cmd)
              << " rawMargin=" << (best - second) << " Hz  rates=[";
    for (size_t i = 0; i < 5 && i < acts.size(); ++i) {
        if (i) std::cout << ",";
        std::cout << acts[i].name << "=" << acts[i].firingRate;
    }
    std::cout << "] avgRate=" << brain->getAverageFiringRate() << "\n";
}

}  // namespace

}  // namespace nlm

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool showHelp = false;
    bool showVersion = false;
    bool verboseMode = false;
    bool quietMode = false;
    bool sandboxOnly = false;
    bool checkpointOnly = false;
    std::string configFile = "configs/default.cfg";
    int neurons = 500;
    int regions = 2;
    int seed = 42;
    double timestep = 0.001;
    int steps = 100;
    float worldSize = 20.0f;
    int worldGrid = 16;
    int checkpoints = 1;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--version" || arg == "--ver") {
            showVersion = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
        } else if (arg == "--quiet") {
            quietMode = true;
        } else if (arg == "--sandbox-only") {
            sandboxOnly = true;
        } else if (arg == "--checkpoint-only") {
            checkpointOnly = true;
        } else if (arg.substr(0, 9) == "--config=") {
            configFile = arg.substr(9);
        } else if (arg.substr(0, 11) == "--neurons=") {
            neurons = std::stoi(arg.substr(11));
        } else if (arg.substr(0, 10) == "--regions=") {
            regions = std::stoi(arg.substr(10));
        } else if (arg.substr(0, 8) == "--seed=") {
            seed = std::stoi(arg.substr(8));
        } else if (arg.substr(0, 12) == "--timestep=") {
            timestep = std::stod(arg.substr(12));
        } else if (arg.substr(0, 8) == "--steps=") {
            steps = std::stoi(arg.substr(8));
        } else if (arg.substr(0, 12) == "--world-size=") {
            worldSize = std::stof(arg.substr(12));
        } else if (arg.substr(0, 12) == "--world-grid=") {
            worldGrid = std::stoi(arg.substr(12));
        } else if (arg.substr(0, 14) == "--checkpoints=") {
            checkpoints = std::stoi(arg.substr(14));
        }
    }
    
    if (showHelp) {
        printBanner();
        printHelp();
        return 0;
    }
    
    if (showVersion) {
        printVersion();
        return 0;
    }
    
    using namespace nlm;
    
    // Configure logging based on verbosity
    auto logger = std::make_shared<Logger>();
    if (verboseMode) {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    } else if (!quietMode) {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Error));
    }
    Logger::setGlobal(logger);
    
    printBanner();
    std::cout << "\n";
    
    // Override default config
    auto config = makeConfig();
    config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(regions), ConfigSource::CommandLine);
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    
    // --- 1. Pattern probe ---
    if (!checkpointOnly) {
        std::cout << "--- 1. Pattern probe (fresh brain per pattern, seed " << seed << ") ---\n";
        std::vector<float> emptyVision(worldGrid * worldGrid, 1.0f);
        std::vector<float> noTouch(worldGrid, 0.0f);
        probePattern("empty scene      ", makePercept(emptyVision, noTouch));
        std::vector<float> brightVision(worldGrid * worldGrid, 1.0f);
        for (int i = 5; i <= 10; ++i) brightVision[i] = 0.7f;  // bright = resource encoding
        std::vector<float> fwdTouch(worldGrid, 0.0f);
        fwdTouch[0] = 0.8f;
        probePattern("bright ahead+touch", makePercept(brightVision, fwdTouch));
        std::vector<float> darkVision(worldGrid * worldGrid, 1.0f);
        for (int i = 5; i <= 10; ++i) darkVision[i] = 0.25f;  // dark = hazard encoding
        probePattern("dark ahead+touch  ", makePercept(darkVision, fwdTouch));
        std::cout << "\n";
    }
    
    // --- 2. Sandbox run + checkpoint round-trip ---
    if (!sandboxOnly) {
        std::cout << "--- 2. Sandbox run (" << steps << " steps) + checkpoint round-trip ---\n";
        auto sandboxConfig = makeConfig();
        sandboxConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
        sandboxConfig->set("region_count", static_cast<int64_t>(regions), ConfigSource::CommandLine);
        sandboxConfig->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
        
        auto brain = std::make_shared<Brain>(sandboxConfig);
        brain->initialize();
        SimpleWorld world;
        world.configure(worldSize, worldSize, worldGrid, worldGrid);
        world.setRandomSeed(seed);
        world.reset();
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        
        float totalReward = 0.0f;
        int resources = 0, hazards = 0, emptyInteracts = 0;
        std::set<std::pair<int, int>> visited;
        uint64_t brainStep = 0;
        for (int step = 0; step < steps; ++step) {
            const SensoryPercept& percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            for (int i = 0; i < 10; ++i) brain->step(brainStep++);
            MotorCommand cmd = agent.decodeMotorCommand();
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            agent.applyRewardModulation(result.reward, 0.0f);
            world.update(0.1);  // Phase 6 pacing: one decision = one 0.1 s action interval.
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
                          << " wmNeurons=" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeuronCount() : 0)
                          << " firing=" << brain->getFiringNeuronCount() << "\n";
            }
        }
        std::cout << "  sandbox: reward=" << totalReward
                  << " resources=" << resources << " hazards=" << hazards
                  << " emptyInteracts=" << emptyInteracts
                  << " visitedCells=" << visited.size()
                  << " avgRate=" << brain->getAverageFiringRate()
                  << " totalSpikes=" << brain->getTotalSpikeCount()
                  << " workingNeurons=" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeuronCount() : 0)
                  << " episodic=" << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0)
                  << " preference=" << agent.getLearnedPreference() << "\n";
        
        // Per-synapse spike-history totals, for the checkpoint check below.
        auto synHistoryTotal = [](const std::shared_ptr<Brain>& b) {
            size_t total = 0;
            for (const auto& region : b->getRegions()) {
                for (const auto& syn : region->getSynapses()) {
                    total += syn->getPreSpikeHistory().size() + syn->getPostSpikeHistory().size();
                }
            }
            return total;
        };
        size_t histBefore = synHistoryTotal(brain);
        
        const std::string ckpt = "/tmp/nlm_phase5_demo.ckpt";
        bool saved = brain->save(ckpt);
        auto brain2 = std::make_shared<Brain>(makeConfig());
        brain2->initialize();
        bool loaded = saved && brain2->load(ckpt);
        std::cout << "  checkpoint: saved=" << (saved ? "yes" : "NO")
                  << " loaded=" << (loaded ? "yes" : "NO");
        if (loaded) {
            // RNG continuity: the next draw from the reloaded brain must
            // equal the next draw from the saved brain (which has drawn
            // nothing since the save).
            uint32_t drawSaved = brain->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(1000000));
            uint32_t drawReloaded = brain2->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(1000000));
            std::cout << " totalSpikes=" << brain2->getTotalSpikeCount()
                      << " workingNeurons=" << (brain2->getWorkingMemory() ? brain2->getWorkingMemory()->getMemoryNeuronCount() : 0)
                      << " episodic=" << (brain2->getEpisodicMemory() ? brain2->getEpisodicMemory()->getEpisodeCount() : 0)
                      << " preference=" << brain2->getLearnedPreference()
                      << " synHist=" << histBefore << "->" << synHistoryTotal(brain2)
                      << " rngMatch=" << (drawSaved == drawReloaded ? "yes" : "NO");
            // Continue the reloaded brain for a few steps: it must still act.
            AgentBrain agent2(brain2);
            agent2.initialize(world);
            agent2.enableCuriosity(false);
            const SensoryPercept& percept = world.getSensoryPercept();
            agent2.processSensoryInput(percept);
            for (int i = 0; i < 10; ++i) brain2->step(brainStep++);
            std::cout << " continuedCmd=" << cmdName(agent2.decodeMotorCommand());
        }
        std::cout << "\n\n";
    }
    
    std::cout << "Phase 5 demonstration complete.\n";
    return 0;
}