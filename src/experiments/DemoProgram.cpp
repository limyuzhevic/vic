#include "core/DemoProgramBase.hpp"
#include <iostream>

using namespace nlm;

class DemoProgram : public DemoProgram {
public:
    DemoProgram() : DemoProgram("nlm_demo") {
        parser.addArgument("param1", "First parameter for demo", false);
        parser.addArgument("param2", "Second parameter for demo", false);
        parser.addArgument("episodes", "Number of episodes (default: 10)", false);
        parser.addArgument("steps", "Steps per episode (default: 100)", false);
    }
    
    int execute() override {
        std::cout << "=== NLM Demo Program ===" << std::endl;
        std::cout << "Description: Improved CLI with configurable parameters" << std::endl;
        
        // Get parameters
        int param1 = 10;
        int param2 = 200;
        
        if (parser.hasValue("param1")) {
            param1 = std::atoi(parser.getValue("param1").c_str());
        }
        if (parser.hasValue("param2")) {
            param2 = std::atoi(parser.getValue("param2").c_str());
        }
        
        int episodes = parser.getIntValue("episodes", 10);
        int steps = parser.getIntValue("steps", 100);
        
        std::cout << "Parameters:" << std::endl;
        std::cout << "  param1: " << param1 << std::endl;
        std::cout << "  param2: " << param2 << std::endl;
        std::cout << "  episodes: " << episodes << std::endl;
        std::cout << "  steps per episode: " << steps << std::endl;
        
        reporter.status("Demo program running successfully!");
        
        // Simulate some work
        for (int i = 0; i < episodes; ++i) {
            reporter.progress(i + 1, episodes, "Running demo...");
            
            for (int j = 0; j < steps; ++j) {
                // Simulate work
                if (j % 10 == 0) {
                    reporter.status("Step " + std::to_string(j) + "/" + std::to_string(steps));
                }
            }
        }
        
        reporter.success("Demo completed successfully!");
        
        std::cout << "\n=== Demo Results ===" << std::endl;
        std::cout << "Total simulated steps: " << episodes * steps << std::endl;
        std::cout << "Average steps per second: " << (episodes * steps) / reporter.getElapsedTime() << std::endl;
        
        return 0;
    }
};

int main(int argc, char* argv[]) {
    return runDemoProgram<DemoProgram>(
        argc, argv,
        "nlm_demo",
        "Demo",
        10,  // default param1
        200,  // default param2
        "A simple demonstration of improved CLI interface"
    );
}
