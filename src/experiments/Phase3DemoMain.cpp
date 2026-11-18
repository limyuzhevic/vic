// Phase 3 demo with improved CLI interface
// Shows: experience -> neural activity -> action -> consequence -> learning

#include "experiments/Phase3Demo.cpp"
#include "core/DemoProgram.hpp"

using namespace nlm;

class Phase3DemoProgram : public DemoProgram {
public:
    Phase3DemoProgram() : DemoProgram("nlm_phase3_demo") {
        parser.addArgument("episodes", "Number of episodes to run", false);
        parser.addArgument("steps", "Steps per episode", false);
        parser.addArgument("seed", "Random seed for reproducibility", false);
        parser.addArgument("instrument", "Enable instrumentation (debug mode)", false);
    }
    
    int execute() override {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Parse parameters
        int numEpisodes = parser.getIntValue("episodes", 5);
        int stepsPerEpisode = parser.getIntValue("steps", 200);
        uint64_t seed = parser.getBoolValue("seed", 42);
        bool instrument = parser.getBoolValue("instrument", false);
        
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Runtime);
        
        std::cout << "Configuration: " << numEpisodes << " episodes, " << stepsPerEpisode << " steps each" << std::endl;
        if (instrument) {
            std::cout << "Instrumentation mode enabled" << std::endl;
        }
        
        // Create and run the demo
        Phase3Demo demo;
        demo.run(numEpisodes, stepsPerEpisode);
        
        return 0;
    }
};

int main(int argc, char* argv[]) {
    Phase3DemoProgram demo;
    return demo.run(argc, argv);
}
