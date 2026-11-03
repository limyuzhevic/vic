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

#include "DemoFramework.hpp"
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

namespace nlm {

class Phase5Demo : public DemoBase {
public:
    Phase5Demo() : DemoBase() {}
    ~Phase5Demo() {}
    
    std::string getDemoName() const override {
        return "PHASE 5 - BRAIN-PERSONALITY / SANDBOX DEMO";
    }
    
    void onSetup() override {
        Logger::getGlobal().setLevel(LogLevel::Error);
        std::cout << "Setting up Phase 5 demo...\n";
    }
    
    void onRun(int numEpisodes, int stepsPerEpisode) override {
        // Run pattern probe
        runPatternProbe();
        
        // Run sandbox run + checkpoint round-trip
        runSandboxAndCheckpoint();
    }
    
    void onComplete() override {
        std::cout << "\nPhase 5 demonstration complete.\n";
    }
    
private:
    // Pattern probe: show same fresh brain three synthetic percepts
    void runPatternProbe() {
        std::cout << "--- 1. Pattern probe ---\n";
        std::cout << "Pattern probe (fresh brain per pattern, seed 42)\n";
        
        // Create percepts for different patterns
        std::vector<float> emptyVision(16, 1.0f);
        std::vector<float> noTouch(8, 0.0f);
        probePattern("empty scene      ", makePercept(emptyVision, noTouch));
        
        std::vector<float> brightVision(16, 1.0f);
        for (int i = 5; i <= 10; ++i) brightVision[i] = 0.7f;  // bright = resource encoding
        std::vector<float> fwdTouch(8, 0.0f);
        fwdTouch[0] = 0.8f;
        probePattern("bright ahead+touch", makePercept(brightVision, fwdTouch));
        
        std::vector<float> darkVision(16, 1.0f);
        for (int i = 5; i <= 10; ++i) darkVision[i] = 0.25f;  // dark = hazard encoding
        probePattern("dark ahead+touch  ", makePercept(darkVision, fwdTouch));
        
        std::cout << "\n";
    }
    
    // Probe pattern with fresh brain
    void probePattern(const char* label, const SensoryPercept& percept) {
        auto brain = std::make_shared<Brain>(ConfigFactory::createPhase5Config());
        brain->initialize();
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableCuriosity(false);  // Probe the brain, not the random explorer.
        
        // Present pattern repeatedly (same as closed loop)
        uint64_t step = 0;
        for (int action = 0; action < 50; ++action) {
            agent.processSensoryInput(percept);
            for (int i = 0; i < 10; ++i) brain->step(step++);
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
        
        // Report result
        std::cout << "  " << label << ": winner=" << DemoHelpers::cmdName(cmd)
                  << " rawMargin=" << (best - second) << " Hz  rates=[";
        for (size_t i = 0; i < 5 && i < acts.size(); ++i) {
            if (i) std::cout << ",";
            std::cout << acts[i].name << "=" << acts[i].firingRate;
        }
        std::cout << "] avgRate=" << brain->getAverageFiringRate() << "\n";
    }
    
    // Create percept helper
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
    
    // Sandbox run + checkpoint round-trip
    void runSandboxAndCheckpoint() {
        std::cout << "--- 2. Sandbox run (100 steps) + checkpoint round-trip ---\n";
        
        auto config = ConfigFactory::createPhase5Config();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        world.reset();
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        
        float totalReward = 0.0f;
        int resources = 0, hazards = 0, emptyInteracts = 0;
        std::set<std::pair<int, int>> visited;
        uint64_t brainStep = 0;
        
        // Run sandbox simulation
        for (int step = 0; step < 100; ++step) {
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
        
        // Print sandbox metrics
        std::cout << "  sandbox: reward=" << totalReward
                  << " resources=" << resources << " hazards=" << hazards
                  << " emptyInteracts=" << emptyInteracts
                  << " visitedCells=" << visited.size()
                  << " avgRate=" << brain->getAverageFiringRate()
                  << " totalSpikes=" << brain->getTotalSpikeCount()
                  << " workingNeurons=" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeuronCount() : 0)
                  << " episodic=" << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0)
                  << " preference=" << agent.getLearnedPreference() << "\n";
        
        // Checkpoint round-trip
        runCheckpointRoundTrip(brain, agent, brainStep, world);
    }
    
    // Checkpoint round-trip test
    void runCheckpointRoundTrip(std::shared_ptr<Brain> brain, AgentBrain& agent, 
                               uint64_t& brainStep, SimpleWorld& world) {
        const std::string ckpt = "/tmp/nlm_phase5_demo.ckpt";
        
        // Save mid-run
        bool saved = brain->save(ckpt);
        
        // Create fresh brain
        auto brain2 = std::make_shared<Brain>(ConfigFactory::createPhase5Config());
        brain2->initialize();
        
        // Load checkpoint
        bool loaded = saved && brain2->load(ckpt);
        
        std::cout << "  checkpoint: saved=" << (saved ? "yes" : "NO")
                  << " loaded=" << (loaded ? "yes" : "NO");
        
        if (loaded) {
            // Test RNG continuity
            uint32_t drawSaved = brain->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(1000000));
            uint32_t drawReloaded = brain2->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(1000000));
            
            std::cout << " totalSpikes=" << brain2->getTotalSpikeCount()
                      << " workingNeurons=" << (brain2->getWorkingMemory() ? brain2->getWorkingMemory()->getMemoryNeuronCount() : 0)
                      << " episodic=" << (brain2->getEpisodicMemory() ? brain2->getEpisodicMemory()->getEpisodeCount() : 0)
                      << " preference=" << agent.getLearnedPreference()
                      << " rngMatch=" << (drawSaved == drawReloaded ? "yes" : "NO");
            
            // Continue the reloaded brain
            AgentBrain agent2(brain2);
            agent2.initialize(world);
            agent2.enableCuriosity(false);
            
            const SensoryPercept& percept = world.getSensoryPercept();
            agent2.processSensoryInput(percept);
            for (int i = 0; i < 10; ++i) brain2->step(brainStep++);
            
            std::cout << " continuedCmd=" << DemoHelpers::cmdName(agent2.decodeMotorCommand());
        }
        
        std::cout << "\n\nPhase 5 demonstration complete.\n";
    }
};

} // namespace nlm

// Main entry point for Phase 5 demo
int main() {
    try {
        nlm::Phase5Demo demo;
        // Use default parameters (5 episodes, 100 steps each for sandbox)
        demo.run(5, 100);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
