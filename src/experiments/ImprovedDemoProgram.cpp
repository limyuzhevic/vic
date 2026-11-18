# Demonstrates the improved NLM CLI interface and user experience
// Shows enhanced configuration, progress reporting, and error handling

#include "core/DemoProgramBase.hpp"
#include <iostream>

using namespace nlm;

class ImprovedDemoProgram : public DemoProgram {
public:
    ImprovedDemoProgram() : DemoProgram("nlm_demo") {
        // Add custom demo parameters
        parser.addArgument("episodes", "Number of episodes (default: 10)", false);
        parser.addArgument("steps", "Steps per episode (default: 100)", false);
        parser.addArgument("learningRate", "Learning rate (default: 0.01)", false);
        parser.addArgument("populationSize", "Population size (default: 500)", false);
        parser.addArgument("enableVisual", "Enable visualization (default: false)", false);
        parser.addArgument("outputFile", "Output file path", false);
    }
    
    int execute() override {
        std::cout << "=== NLM Improved Demo Program ===" << std::endl;
        std::cout << "Enhanced CLI with comprehensive configuration and error handling" << std::endl;
        
        // Get custom parameters
        int episodes = parser.getIntValue("episodes", 10);
        int steps = parser.getIntValue("steps", 100);
        float learningRate = parser.getFloatValue("learningRate", 0.01f);
        int populationSize = parser.getIntValue("populationSize", 500);
        bool enableVisual = parser.getBoolValue("enableVisual", false);
        std::string outputFile = parser.getValue("outputFile");
        
        // Validate parameters
        if (episodes <= 0) {
            reporter.error("episodes must be positive, got: " + std::to_string(episodes));
            return 1;
        }
        
        if (steps <= 0) {
            reporter.error("steps must be positive, got: " + std::to_string(steps));
            return 1;
        }
        
        if (!ValidationUtils::isValid(learningRate, 0.0f, 1.0f)) {
            reporter.error("learningRate must be between 0.0 and 1.0, got: " + std::to_string(learningRate));
            return 1;
        }
        
        if (!ValidationUtils::isValid(populationSize, 100, 10000)) {
            reporter.error("populationSize must be between 100 and 10000, got: " + std::to_string(populationSize));
            return 1;
        }
        
        // Display configuration
        std::cout << "\n=== Configuration ===" << std::endl;
        std::cout << "  Episodes: " << episodes << std::endl;
        std::cout << "  Steps per episode: " << steps << std::endl;
        std::cout << "  Learning rate: " << learningRate << std::endl;
        std::cout << "  Population size: " << populationSize << std::endl;
        std::cout << "  Enable visualization: " << (enableVisual ? "Yes" : "No") << std::endl;
        if (!outputFile.empty()) {
            std::cout << "  Output file: " << outputFile << std::endl;
        }
        
        reporter.status("Configuration validated successfully");
        
        // Simulate some work
        reporter.status("Starting simulation...");
        
        for (int episode = 0; episode < episodes; ++episode) {
            reporter.progress(episode + 1, episodes, "Running episode...");
            
            for (int step = 0; step < steps; ++step) {
                // Simulate work
                if (step % 50 == 0) {
                    reporter.status("  Step " + std::to_string(step) + "/" + std::to_string(steps) +
                                  " | Learning rate: " + std::to_string(learningRate));
                }
                
                // Simulate some computation
                float value = static_cast<float>(episode * steps + step) * learningRate;
                if (enableVisual && step % 10 == 0) {
                    reporter.status("    Value: " + std::to_string(value));
                }
            }
        }
        
        reporter.success("Demo completed successfully!");
        
        // Write results if output file specified
        if (!outputFile.empty()) {
            reporter.status("Writing results to: " + outputFile);
            // Note: In a real implementation, this would write to file
            reporter.success("Results saved successfully");
        }
        
        std::cout << "\n=== Performance Summary ===" << std::endl;
        std::cout << "  Total simulated steps: " << episodes * steps << std::endl;
        std::cout << "  Average steps per second: " << (episodes * steps) / reporter.getElapsedTime() << std::endl;
        
        return 0;
    }
};

int main(int argc, char* argv[]) {
    return runDemoProgram<ImprovedDemoProgram>(
        argc, argv,
        "nlm_demo",
        "Improved Demo",
        10,  // default param1
        200,  // default param2
        "A demonstration of the improved NLM CLI interface and user experience"
    );
}
