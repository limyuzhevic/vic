// Practical demonstration of NLM improvements
// Shows the real-world benefits of the enhanced CLI, error handling, and user experience

#include "core/DemoProgramBase.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace nlm;

class PracticalDemoProgram : public DemoProgram {
public:
    PracticalDemoProgram() : DemoProgram("nlm_practical_demo") {
        // Practical CLI arguments for real-world usage
        parser.addArgument("experiment", "Experiment type (evolution, reinforcement, supervised)", false);
        parser.addArgument("population", "Population size for evolutionary experiments", false);
        parser.addArgument("generations", "Number of generations for evolutionary experiments", false);
        parser.addArgument("learningRate", "Learning rate for gradient-based methods", false);
        parser.addArgument("batchSize", "Batch size for training", false);
        parser.addArgument("output", "Output file for results", false);
        parser.addArgument("validation", "Enable validation data", false);
        parser.addArgument("checkpoint", "Enable checkpoint saving", false);
    }
    
    int execute() override {
        std::cout << "=== NLM Practical Demo Program ===" << std::endl;
        std::cout << "Demonstrates real-world benefits of improved CLI and error handling" << std::endl;
        
        // Get practical parameters
        std::string experiment = parser.getValue("experiment", "reinforcement");
        int population = parser.getIntValue("population", 100);
        int generations = parser.getIntValue("generations", 50);
        float learningRate = parser.getFloatValue("learningRate", 0.01f);
        int batchSize = parser.getIntValue("batchSize", 32);
        std::string outputFile = parser.getValue("output", "results.json");
        bool validation = parser.getBoolValue("validation", true);
        bool checkpoint = parser.getBoolValue("checkpoint", false);
        
        // Validate practical parameters
        reporter.status("Validating practical parameters...");
        
        if (population <= 0 || population > 10000) {
            reporter.error("Population size must be between 1 and 10000, got: " + std::to_string(population));
            return 1;
        }
        
        if (generations <= 0 || generations > 1000) {
            reporter.error("Generat�กons must be between 1 and 1000, got: " + std::to_string(generations));
            return 1;
        }
        
        if (!ValidationUtils::isValid(learningRate, 0.0001f, 1.0f)) {
            reporter.error("Learning rate must be between 0.0001 and 1.0, got: " + std::to_string(learningRate));
            return 1;
        }
        
        if (batchSize <= 0 || batchSize > population) {
            reporter.error("Batch size must be between 1 and population size, got: " + std::to_string(batchSize));
            return 1;
        }
        
        // Display practical configuration
        std::cout << "\n=== Practical Configuration ===" << std::endl;
        std::cout << "Experiment Type: " << experiment << std::endl;
        std::cout << "Population Size: " << population << std::endl;
        std::cout << "Generations: " << generations << std::endl;
        std::cout << "Learning Rate: " << learningRate << std::endl;
        std::cout << "Batch Size: " << batchSize << std::endl;
        std::cout << "Validation Data: " << (validation ? "Yes" : "No") << std::endl;
        std::cout << "Checkpointing: " << (checkpoint ? "Yes" : "No") << std::endl;
        std::cout << "Output File: " << outputFile << std::endl;
        
        reporter.success("Configuration validated successfully");
        
        // Simulate practical experiment
        std::cout << "\n=== Simulating " << experiment << " Experiment ===" << std::endl;
        
        double startTime = reporter.getElapsedTime();
        
        // Simulate evolutionary experiment
        if (experiment == "evolution") {
            reporter.status("Running evolutionary experiment...");
            simulateEvolution(population, generations, learningRate);
        }
        // Simulate reinforcement learning
        else if (experiment == "reinforcement") {
            reporter.status("Running reinforcement learning experiment...");
            simulateReinforcementLearning(population, generations, learningRate, batchSize);
        }
        // Simulate supervised learning
        else if (experiment == "supervised") {
            reporter.status("Running supervised learning experiment...");
            simulateSupervisedLearning(population, generations, learningRate, batchSize, validation);
        }
        else {
            reporter.error("Unknown experiment type: " + experiment);
            return 1;
        }
        
        double elapsedTime = reporter.getElapsedTime() - startTime;
        
        // Save results
        if (!outputFile.empty()) {
            reporter.status("Saving results to: " + outputFile);
            saveResults(outputFile, experiment, population, generations, learningRate, batchSize, validation, checkpoint, elapsedTime);
        }
        
        std::cout << "\n=== Practical Demo Results ===" << std::endl;
        std::cout << "Experiment Type: " << experiment << std::endl;
        std::cout << "Population: " << population << std::endl;
        std::cout << "Generations: " << generations << std::endl;
        std::cout << "Learning Rate: " << learningRate << std::endl;
        std::cout << "Batch Size: " << batchSize << std::endl;
        std::cout << "Validation: " << (validation ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Checkpointing: " << (checkpoint ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Total Time: " << elapsedTime << " seconds" << std::endl;
        std::cout << "Output File: " << outputFile << std::endl;
        
        reporter.success("Practical demo completed successfully");
        
        return 0;
    }
    
private:
    void simulateEvolution(int population, int generations, float learningRate) {
        reporter.status("Starting population evolution...");
        
        for (int gen = 0; gen < generations; ++gen) {
            reporter.progress(gen + 1, generations, "Evolving population...");
            
            // Simulate evolution step
            float fitness = calculateFitness(population, learningRate);
            reporter.status("  Generation " + std::to_string(gen) + ": fitness = " + std::to_string(fitness));
            
            // Mutation and selection
            reporter.status("  Applying mutations and natural selection...");
        }
        
        reporter.success("Evolution completed");
    }
    
    void simulateReinforcementLearning(int population, int generations, float learningRate, int batchSize) {
        reporter.status("Starting reinforcement learning...");
        
        for (int gen = 0; gen < generations; ++gen) {
            reporter.progress(gen + 1, generations, "Learning from environment...");
            
            // Simulate RL step
            float reward = simulateRLStep(population, learningRate, batchSize);
            reporter.status("  Generation " + std::to_string(gen) + ": reward = " + std::to_string(reward));
        }
        
        reporter.success("Reinforcement learning completed");
    }
    
    void simulateSupervisedLearning(int population, int generations, float learningRate, int batchSize, bool validation) {
        reporter.status("Starting supervised learning with validation...");
        
        for (int gen = 0; gen < generations; ++gen) {
            reporter.progress(gen + 1, generations, "Training on labeled data...");
            
            // Simulate supervised learning step
            float accuracy = simulateSLStep(population, learningRate, batchSize, validation);
            reporter.status("  Generation " + std::to_string(gen)Lp + ": accuracy = " + std::to_string(accuracy));
        }
        
        reporter.success("Supervised learning completed");
    }
    
    void saveResults(std::string filename, std::string experiment, int population, int generations, float learningRate, int batchSize, bool validation, bool checkpoint, double time) {
        // In a real implementation, this would save to file
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "{\n                "experiment": \"" << experiment << "\",
                "population": " << population << ",
                "generations": " << generations << ",
                "learningRate": " << learningRate << ",
                "batchSize": " << batchSize << ",
                "validation": " << (validation ? "true" : "false") << ",
                "checkpoint": " << (checkpoint ? "true" : "false") << ",
                "time": " << time << ",
                "timestamp": \"" << getCurrentTimestamp() << "\"
            }";
            file.close();
        }
    }
    
    float calculateFitness(int population, float learningRate) {
        // Simulate fitness calculation
        float baseFitness = 100.0f;
        float learningBonus = learningRate * 50.0f;
        float diversityBonus = static_cast<float>(rand()) / RAND_MAX * 30.0f;
        
        return baseFitness + learningBonus + diversityBonus;
    }
    
    float simulateRLStep(int population, float learningRate, int batchSize) {
        // Simulate RL step with exploration/exploitation
        float explorationRate = 0.1f * (1.0f - learningRate);
        float reward = static_cast<float>(rand()) / RAND_MAX * 100.0f;
        
        return reward * (1.0f - explorationRate) + static_cast<float>(rand()) / RAND_MAX * 20.0f;
    }
    
    float simulateSLStep(int population, float learningRate, int batchSize, bool validation) {
        // Simulate supervised learning step
        float baseAccuracy = 85.0f;
        float learningImprovement = learningRate * 15.0f;
        float validationBonus = validation ? 5.0f : 0.0f;
        
        return std::min(100.0f, baseAccuracy + learningImprovement + validationBonus);
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

int main(int argc, char* argv[]) {
    return runDemoProgram<PracticalDemoProgram>(
        argc, argv,
        "nlm_practical_demo",
        "Practical Demo",
        100,  // default population
        50,   // default generations
        "A practical demonstration of NLM improvements for real-world usage"
    );
}
