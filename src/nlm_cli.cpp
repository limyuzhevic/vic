#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "development/DevelopmentSystem.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface(std::shared_ptr<Config> config) : config(config) {
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void run() {
        printWelcome();
        
        while (true) {
            printPrompt();
            std::string input;
            std::getline(std::cin, input);
            
            if (input.empty()) {
                continue;
            }
            
            std::vector<std::string> tokens = split(input, ' ');
            std::string command = tokens[0];
            
            if (command == "exit" || command == "quit") {
                handleExit();
                break;
            } else if (command == "help") {
                showHelp(tokens.size() > 1 ? tokens[1] : "");
            } else if (command == "brain") {
                handleBrainCommand(tokens);
            } else if (command == "neuro") {
                handleNeuromodulationCommand(tokens);
            } else if (command == "memory") {
                handleMemoryCommand(tokens);
            } else if (command == "prediction") {
                handlePredictionCommand(tokens);
            } else if (command == "cognition") {
                handleCognitionCommand(tokens);
            } else if (command == "development") {
                handleDevelopmentCommand(tokens);
            } else if (command == "config") {
                handleConfigCommand(tokens);
            } else if (command == "experiment") {
                handleExperimentCommand(tokens);
            } else if (command == "profile") {
                handleProfileCommand(tokens);
            } else {
                std::cout << "Unknown command: " << command << std::endl;
                std::cout << "Type 'help' for available commands." << std::endl;
            }
        }
    }
    
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    std::unique_ptr<Brain> brain;
    std::chrono::steady_clock::time_point startTime;
    
    void printWelcome() {
        std::cout << "=== NLM Advanced Command Line Interface ===" << std::endl;
        std::cout << "Phase 6: Integrated Artificial Brain" << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl << std::endl;
    }
    
    void printPrompt() {
        std::cout << "nlm> ";
        std::cout.flush();
    }
    
    void showHelp(const std::string& topic = "") {
        if (topic.empty()) {
            std::cout << "=== Available Commands ===" << std::endl;
            std::cout << "brain     - Brain control and monitoring" << std::endl;
            std::cout << "neuro     - Neuromodulation control" << std::endl;
            std::cout << "memory    - Memory system control" << std::endl;
            std::cout << "prediction - Prediction system control" << std::endl;
            std::cout << "cognition - Cognitive system control" << std::endl;
            std::cout << "development - Development system control" << std::endl;
            std::cout << "config    - Configuration management" << std::endl;
            std::cout << "experiment - Run experiments" << std::endl;
            std::cout << "profile   - Performance profiling" << std::endl;
            std::cout << "help      - Show help for a specific topic" << std::endl;
            std::cout << "exit/quit - Exit the program" << std::endl;
        } else {
            showTopicHelp(topic);
        }
    }
    
    void showTopicHelp(const std::string& topic) {
        if (topic == "brain") {
            std::cout << "=== Brain Commands ===" << std::endl;
            std::cout << "brain status    - Show brain status" << std::endl;
            std::cout << "brain step N    - Run N simulation steps" << std::endl;
            std::cout << "brain reset     - Reset brain state" << std::endl;
            std.cpp << "brain inject [region] [type] [current] - Inject current into neurons" << std::endl;
            std::cout << "brain save [file] - Save brain state" << std::endl;
            std::cout << "brain load [file] - Load brain state" << std::endl;
        } else if (topic == "neuro") {
            std::cout << "=== Neuromodulation Commands ===" << std::endl;
            std::cout << "neuro status    - Show neuromodulation status" << std::endl;
            std::cout << "neuro dopamine [level] - Set dopamine level" << std::endl;
            std::cout << "neuro acetylcholine [level] - Set acetylcholine level" << std::endl;
            std::cout << "neuro norepinephrine [level] - Set norepinephrine level" << std::endl;
            std::cout << "neuro serotonin [level] - Set serotonin level" << std::endl;
            std::cout << "neuro signal reward [amount] - Signal reward" << std::endl;
            std::cout << "neuro signal novelty [amount] - Signal novelty" << std::endl;
            std::cout << "neuro signal error [amount] - Signal prediction error" << std::endl;
        } else if (topic == "memory") {
            std::cout << "=== Memory Commands ===" << std::endl;
            std::cout << "memory status    - Show memory status" << std::endl;
            std::cout << "memory working   - Show working memory content" << std::endl;
            std::cout << "memory episodic  - Show episodic memory content" << std::endl;
            std::cout << "memory associative - Show associative memory content" << std::endl;
            std::cout << "memory replay     - Trigger memory replay" << std::endl;
            std::cout << "memory consolidate - Trigger memory consolidation" << std::endl;
        } else if (topic == "prediction") {
            std::cout << "=== Prediction Commands ===" << std::endl;
            std::cout << "prediction status - Show prediction system status" << std::endl;
            std::cout << "prediction predict [input] - Make prediction" << std::endl;
            std::cout << "prediction learn [prediction] [outcome] - Learn from outcome" << std::endl;
        } else if (topic == "cognition") {
            std::cout << "=== Cognition Commands ===" << std::endl;
            std::cout << "cognition status - Show cognition system status" << std::endl;
            std::cout << "cognition plan [goal] - Generate action plan" << std::endl;
            std::cout << "cognition attend [input] - Attend to input" << std::endl;
            std::cout << "cognition concept [input] - Form concept from input" << std::endl;
        } else if (topic == "development") {
            std::cout << "=== Development Commands ===" << std::endl;
            std::cout << "development status - Show development system status" << std::endl;
            std::cout << "development stage [stage] - Set developmental stage" << std::endl;
            std::cout << "development evolve - Advance to next development stage" << std::endl;
        } else if (topic == "config") {
            std::cout << "=== Config Commands ===" << std::endl;
            std::cout << "config show     - Show current configuration" << std::endl;
            std::cout << "config set key value - Set configuration value" << std::endl;
            std::cout << "config reset    - Reset to default configuration" << std::endl;
            std::cout << "config save [file] - Save configuration to file" << std::endl;
            std::cout << "config load [file] - Load configuration from file" << std::endl;
        } else {
            std::cout << "Unknown help topic: " << topic << std::endl;
        }
    }
    
    void handleBrainCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Brain command requires subcommand." << std::endl;
            showTopicHelp("brain");
            return;
        }
        
        std::string subcommand = tokens[1];
        
        if (subcommand == "status") {
            handleBrainStatus();
        } else if (subcommand == "step") {
            if (tokens.size() < 3) {
                std::cout << "Please specify number of steps." << std::endl;
                return;
            }
            int steps = std::stoi(tokens[2]);
            handleBrainStep(steps);
        } else if (subcommand == "reset") {
            handleBrainReset();
        } else if (subcommand == "inject") {
            handleBrainInject(tokens);
        } else if (subcommand == "save") {
            std::string filename = tokens.size() > 2 ? tokens[2] : "brain_state.bin";
            handleBrainSave(filename);
        } else if (subcommand == "load") {
            std::string filename = tokens.size() > 2 ? tokens[2] : "brain_state.bin";
            handleBrainLoad(filename);
        } else {
            std::cout << "Unknown brain subcommand: " << subcommand << std::endl;
            showTopicHelp("brain");
        }
    }
    
    void handleNeuromodulationCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Neuromodulation command requires subcommand." << std::endl;
            showTopicHelp("neuro");
            return;
        }
        
        std::string subcommand = tokens[1];
        
        if (subcommand == "status") {
            handleNeuroStatus();
        } else if (subcommand == "dopamine") {
            float level = tokens.size() > 2 ? std::stof(tokens[2]) : 1.0f;
            handleNeuroDopamine(level);
        } else if (subcommand == "acetylcholine") {
            float level = tokens.size() > 2 ? std::stof(tokens[2]) : 1.0f;
            handleNeuroAcetylcholine(level);
        } else if (subcommand == "norepinephrine") {
            float level = tokens.size() > 2 ? std::stof(tokens[2]) : 1.0f;
            handleNeuroNorepinephrine(level);
        } else if (subcommand == "serotonin") {
            float level = tokens.size() > 2 ? std::stof(tokens[2]) : 1.0f;
            handleNeuroSerotonin(level);
        } else if (subcommand == "signal") {
            handleNeuroSignal(tokens);
        } else {
            std::cout << "Unknown neuromodulation subcommand: " << subcommand << std::endl;
            showTopicHelp("neuro");
        }
    }
    
    void handleBrainStatus() {
        if (!brain) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "=== Brain Status ===" << std::endl;
        brain->logStatus();
        
        // Show neuromodulation status
        auto dopamine = brain->getDopamine();
        auto acetylcholine = brain->getAcetylcholine();
        auto norepinephrine = brain->getNorepinephrine();
        auto serotonin = brain->getSerotonin();
        
        std::cout << "Neuromodulation Levels:" << std::endl;
        std::cout << "  Dopamine: " << (dopamine ? dopamine->getLevel() : 0.0f) << std::endl;
        std::cout << "  Acetylcholine: " << (acetylcholine ? acetylcholine->getLevel() : 0.0f) << std::endl;
        std::cout << "  Norepinephrine: " << (norepinephrine ? norepinephrine->getLevel() : 0.0f) << std::endl;
        std::cout << "  Serotonin: " << (serotonin ? serotonin->getLevel() : 0.0f) << std::endl;
        
        // Show plasticity factors
        std::cout << "Plasticity Factors:" << std::endl;
        std::cout << "  Dopamine plasticity: " << (dopamine ? dopamine->getPlasticityFactor() : 1.0f) << std::endl;
        std::cout << "  Acetylcholine plasticity: " << (acetylcholine ? acetylcholine->getPlasticityFactor() : 1.0f) << std::endl;
        std::cout << "  Norepinephrine plasticity: " << (norepinephrine ? norepinephrine->getPlasticityFactor() : 1.0f) << std::endl;
        std::cout << "  Serotonin plasticity: " << (serotonin ? serotonin->getPlasticityFactor() : 1.0f) << std::endl;
    }
    
    void handleBrainStep(int steps) {
        if (!brain) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "Running " << steps << " simulation steps..." << std::endl;
        for (int i = 0; i < steps; ++i) {
            brain->step(i, 0.001);
        }
        std::cout << "Completed " << steps << " steps." << std::endl;
    }
    
    void handleBrainReset() {
        if (brain) {
            brain->reset();
            brain->initialize();
            std::cout << "Brain reset and reinitialized." << std::endl;
        } else {
            std::cout << "Brain not initialized." << std::endl;
        }
    }
    
    void handleBrainInject(const std::vector<std::string>& tokens) {
        if (tokens.size() < 4) {
            std::cout << "Brain inject requires: [region] [type] [current]" << std::endl;
            return;
        }
        
        if (!brain) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        // Simple implementation for demonstration
        std::cout << "Injecting current into brain neurons..." << std::endl;
        // This would be more sophisticated in a real implementation
    }
    
    void handleBrainSave(const std::string& filename) {
        if (brain) {
            std::cout << "Brain save not yet implemented." << std::endl;
        } else {
            std::cout << "Brain not initialized." << std::endl;
        }
    }
    
    void handleBrainLoad(const std::string& filename) {
        std::cout << "Brain load not yet implemented." << std::endl;
    }
    
    void handleNeuroStatus() {
        if (!brain) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "=== Neuromodulation Status ===" << std::endl;
        
        auto dopamine = brain->getDopamine();
        auto acetylcholine = brain->getAcetylcholine();
        auto norepinephrine = brain->getNorepinephrine();
        auto serotonin = brain->getSerotonin();
        auto curiosity = brain->getCuriosity();
        auto novelty = brain->getNovelty();
        auto predictionError = brain->getPredictionErrorSignal();
        
        std::cout << "System Status:" << std::endl;
        std::cout << "  Dopamine: Level=" << (dopamine ? dopamine->getLevel() : 0.0f) 
                  << ", Plasticity=" << (dopamine ? dopamine->getPlasticityFactor() : 1.0f) << std::endl;
        std::cout << "  Acetylcholine: Level=" << (acetylcholine ? acetylcholine->getLevel() : 0.0f) 
                  << ", Plasticity=" << (acetylcholine ? acetylcholine->getPlasticityFactor() : 1.0f) << std::endl;
        std::cout << "  Norepinephrine: Level=" << (norepinephrine ? norepinephrine->getLevel() : 0.0f) 
                  << ", Plasticity=" << (norepinephrine ? norepinephrine->getPlasticityFactor() : 1.0f) << std::endl;
        std::cout << "  Serotonin: Level=" << (serotonin ? serotonin->getLevel() : 0.0f) 
                  << ", Plasticity=" << (serotonin ? serotonin->getPlasticityFactor() : 1.0f) << std::endl;
        
        if (curiosity) {
            std::cout << "  Curiosity: Level=" << curiosity->getLevel() 
                      << ", ExplorationDrive=" << curiosity->getExplorationDrive() << std::endl;
        }
        
        if (novelty) {
            std::cout << "  Novelty: Level=" << novelty->getLevel() << std::endl;
        }
        
        if (predictionError) {
            std::cout << "  Prediction Error: Error=" << predictionError->getError() 
                      << ", Magnitude=" << predictionError->getMagnitude() << std::endl;
        }
    }
    
    void handleNeuroDopamine(float level) {
        if (brain && brain->getDopamine()) {
            brain->getDopamine()->setLevel(level);
            std::cout << "Dopamine level set to: " << level << std::endl;
        } else {
            std::cout << "Brain not initialized or dopamine system not available." << std::endl;
        }
    }
    
    void handleNeuroAcetylcholine(float level) {
        if (brain && brain->getAcetylcholine()) {
            brain->getAcetylcholine()->setLevel(level);
            std::cout << "Acetylcholine level set to: " << level << std::endl;
        } else {
            std::cout << "Brain not initialized or acetylcholine system not available." << std::endl;
        }
    }
    
    void handleNeuroNorepinephrine(float level) {
        if (brain && brain->getNorepinephrine()) {
            brain->getNorepinephrine()->setLevel(level);
            std::cout << "Norepinephrine level set to: " << level << std::endl;
        } else {
            std::cout << "Brain not initialized or norepinephrine system not available." << std::endl;
        }
    }
    
    void handleNeuroSerotonin(float level) {
        if (brain && brain->getSerotonin()) {
            brain->getSerotonin()->setLevel(level);
            std::cout << "Serotonin level set to: " << level << std::endl;
        } else {
            std::cout << "Brain not initialized or serotonin system not available." << std::endl;
        }
    }
    
    void handleNeuroSignal(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            std::cout << "Neuro signal requires: [type] [value]" << std::endl;
            return;
        }
        
        std::string signalType = tokens[2];
        float value = tokens.size() > 3 ? std::stof(tokens[3]) : 1.0f;
        
        if (!brain) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        if (signalType == "reward") {
            if (brain->getDopamine()) {
                brain->getDopamine()->signalReward(value);
                std::cout << "Reward signal sent: " << value << std::endl;
            } else {
                std::cout << "Dopamine system not available." << std::endl;
            }
        } else if (signalType == "novelty") {
            if (brain->getNovelty()) {
                brain->getNovelty()->setLevel(value);
                std::cout << "Novelty signal sent: " << value << std::endl;
            } else {
                std::cout << "Novelty system not available." << std::endl;
            }
        } else if (signalType == "error") {
            if (brain->getPredictionErrorSignal()) {
                brain->getPredictionErrorSignal()->computeError(0.0f, value);
                std::cout << "Prediction error signal sent: " << value << std::endl;
            } else {
                std::cout << "Prediction error system not available." << std::endl;
            }
        } else {
            std::cout << "Unknown signal type: " << signalType << std::endl;
        }
    }
    
    void handleExit() {
        std::cout << "Shutting down NLM Advanced CLI..." << std::endl;
    }
    
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        for (char c : str) {
            if (c == delimiter) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Advanced Command Line Interface ===" << std::endl;
    std::cout << "Phase 6: Integrated Artificial Brain" << std::endl;
    std::cout << "Note: Full CLI features available in NLM CLI executable." << std::endl;
    std::cout << std::endl;
    std::cout << "This demo shows the enhanced neuromodulation systems." << std::endl;
    std::cout << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    config->loadFromFile("configs/default.cfg");
    
    // Create and initialize brain
    auto brain = std::make_unique<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully." << std::endl;
    std::cout << std::endl;
    
    // Start the command line interface
    CommandLineInterface cli(config);
    cli.run();
    
    return 0;
}
