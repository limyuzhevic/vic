// Working memory management commands

#pragma once

#include "AdvancedCLI.hpp"
#include <sstream>

namespace nlm {
namespace cli {

class WorkingMemoryCommand : public Command {
public:
    std::string getName() const override { return "working_memory"; }
    std::string getDescription() const override { return "Working memory analysis and management"; }
    std::string getUsage() const override {
        return "working_memory <subcommand> [options]"; 
    }
    
    void execute(const CommandContext& context) override {
        std::string subcommand = context.options["subcommand"];
        
        if (subcommand == "info" || subcommand.empty()) {
            executeInfo(context);
        } else if (subcommand == "store") {
            executeStore(context);
        } else if (subcommand == "retrieve") {
            executeRetrieve(context);
        } else if (subcommand == "clear") {
            executeClear(context);
        } else if (subcommand == "stats") {
            executeStats(context);
        } else {
            throw std::runtime_error("Unknown subcommand: " + subcommand);
        }
    }
    
private:
    void executeInfo(const CommandContext& context) {
        auto workingMemory = context.brain->getWorkingMemory();
        if (!workingMemory) {
            context.output << "Working memory: Not available (placeholder implementation)" << std::endl;
            return;
        }
        
        context.output << "Working Memory Status:" << std::endl;
        context.output << "  Capacity: " << workingMemory->getCapacity() << std::endl;
        context.output << "  Current size: " << workingMemory->getCurrentSize() << std::endl;
        
        // Note: This is a placeholder implementation
        context.output << "  Note: Working memory integration is in progress (Phase 2)" << std::endl;
    }
    
    void executeStore(const CommandContext& context) {
        if (context.options.find("neuron") == context.options.end()) {
            throw std::runtime_error("Missing --neuron parameter");
        }
        
        if (context.options.find("value") == context.options.end()) {
            throw std::runtime_error("Missing --value parameter");
        }
        
        NeuronId neuron = static_cast<NeuronId>(std::stoul(context.options.at("neuron")));
        float value = std::stof(context.options.at("value"));
        
        auto workingMemory = context.brain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->store(neuron, value);
            context.output << "Stored value " << value << " for neuron " << neuron << std::endl;
        } else {
            context.output << "Warning: Working memory not available, but stored in context" << std::endl;
        }
    }
    
    void executeRetrieve(const CommandContext& context) {
        if (context.options.find("neuron") == context.options.end()) {
            throw std::runtime_error("Missing --neuron parameter");
        }
        
        NeuronId neuron = static_cast<NeuronId>(std::stoul(context.options.at("neuron")));
        
        auto workingMemory = context.brain->getWorkingMemory();
        if (workingMemory) {
            float value = workingMemory->retrieve(neuron);
            context.output << "Retrieved value: " << value << " for neuron " << neuron << std::endl;
        } else {
            context.output << "Warning: Working memory not available" << std::endl;
        }
    }
    
    void executeClear(const CommandContext& context) {
        auto workingMemory = context.brain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->clear();
            context.output << "Working memory cleared" << std::endl;
        } else {
            context.output << "Warning: Working memory not available" << std::endl;
        }
    }
    
    void executeStats(const CommandContext& context) {
        auto workingMemory = context.brain->getWorkingMemory();
        if (!workingMemory) {
            context.output << "Working memory: Not available (placeholder implementation)" << std::endl;
            return;
        }
        
        context.output << "Working Memory Statistics:" << std::endl;
        context.output << "  Current size: " << workingMemory->getCurrentSize() << std::endl;
        context.output << "  Capacity: " << workingMemory->getCapacity() << std::endl;
        context.output << "  Utilization: " << 
            (workingMemory->getCapacity() > 0 ? 
             workingMemory->getCurrentSize() * 100.0f / workingMemory->getCapacity() : 0.0f) << "%" << std::endl;
    }
};

class EpisodicMemoryCommand : public Command {
public:
    std::string getName() const override { return "episodic_memory"; }
    std::string getDescription() const override { return "Episodic memory queries and management"; }
    std::string getUsage() const override {
        return "episodic_memory <subcommand> [options]"; 
    }
    
    void execute(const CommandContext& context) override {
        std::string subcommand = context.options["subcommand"];
        
        if (subcommand == "info" || subcommand.empty()) {
            executeInfo(context);
        } else if (subcommand == "store") {
            executeStore(context);
        } else if (subcommand == "retrieve") {
            executeRetrieve(context);
        } else if (subcommand == "recent") {
            executeRecent(context);
        } else if (subcommand == "consolidate") {
            executeConsolidate(context);
        } else {
            throw std::runtime_error("Unknown subcommand: " + subcommand);
        }
    }
    
private:
    void executeInfo(const CommandContext& context) {
        auto episodicMemory = context.brain->getEpisodicMemory();
        if (!episodicMemory) {
            context.output << "Episodic memory: Not available (placeholder implementation)" << std::endl;
            return;
        }
        
        context.output << "Episodic Memory Status:" << std::endl;
        context.output << "  Episode count: " << episodicMemory->getEpisodeCount() << std::endl;
        
        // Note: This is a placeholder implementation
        context.output << "  Note: Episodic memory integration is in progress (Phase 2)" << std::endl;
    }
    
    void executeStore(const CommandContext& context) {
        // This would require storing an episode
        context.output << "Episodic memory store: Requires episode data" << std::endl;
        context.output << "  Note: Episodic memory integration is in progress (Phase 2)" << std::endl;
    }
    
    void executeRetrieve(const CommandContext& context) {
        if (context.options.find("index") == context.options.end()) {
            throw std::runtime_error("Missing --index parameter");
        }
        
        size_t index = static_cast<size_t>(std::stoul(context.options.at("index")));
        
        auto episodicMemory = context.brain->getEpisodicMemory();
        if (episodicMemory) {
            auto episode = episodicMemory->retrieveEpisode(index);
            context.output << "Episode " << index << ":" << std::endl;
            context.output << "  Timestamp: " << episode.timestamp << std::endl;
            context.output << "  Neurons: " << episode.neurons.size() << std::endl;
            context.output << "  Values: " << episode.values.size() << std::endl;
            context.output << "  Metadata: " << episode.metadata << std::endl;
        } else {
            context.output << "Warning: Episodic memory not available" << std::endl;
        }
    }
    
    void executeRecent(const CommandContext& context) {
        if (context.options.find("count") == context.options.end()) {
            throw std::runtime_error("Missing --count parameter");
        }
        
        size_t count = static_cast<size_t>(std::stoul(context.options.at("count")));
        
        auto episodicMemory = context.brain->getEpisodicMemory();
        if (episodicMemory) {
            auto episodes = episodicMemory->getRecentEpisodes(count);
            context.output << "Recent Episodes (last " << episodes.size() << "):" << std::endl;
            for (size_t i = 0; i < episodes.size(); ++i) {
                context.output << "  " << i << ": timestamp=" << episodes[i].timestamp 
                              << ", neurons=" << episodes[i].neurons.size() 
                              << ", metadata='" << episodes[i].metadata << "'" << std::endl;
            }
        } else {
            context.output << "Warning: Episodic memory not available" << std::endl;
        }
    }
    
    void executeConsolidate(const CommandContext& context) {
        if (context.options.find("threshold") == context.options.end()) {
            throw std::runtime_error("Missing --threshold parameter");
        }
        
        float threshold = std::stof(context.options.at("threshold"));
        
        auto episodicMemory = context.brain->getEpisodicMemory();
        if (episodicMemory) {
            episodicMemory->consolidate(threshold);
            context.output << "Consolidated episodic memory with threshold: " << threshold << std::endl;
        } else {
            context.output << "Warning: Episodic memory not available" << std::endl;
        }
    }
};

class PredictionAnalysisCommand : public Command {
public:
    std::string getName() const override { return "prediction_analysis"; }
    std::string getDescription() const override { return "Prediction confidence and error analysis"; }
    std::string getUsage() const override {
        return "prediction_analysis <subcommand> [options]"; 
    }
    
    void execute(const CommandContext& context) override {
        std::string subcommand = context.options["subcommand"];
        
        if (subcommand == "info" || subcommand.empty()) {
            executeInfo(context);
        } else if (subcommand == "confidence") {
            executeConfidence(context);
        } else if (subcommand == "error_analysis") {
            executeErrorAnalysis(context);
        } else if (subcommand == "system_info") {
            executeSystemInfo(context);
        } else {
            throw std::runtime_error("Unknown subcommand: " + subcommand);
        }
    }
    
private:
    void executeInfo(const CommandContext& context) {
        auto predictionSystem = context.brain->getPredictionSystem();
        if (!predictionSystem) {
            context.output << "Prediction system: Not available (placeholder implementation)" << std::endl;
            return;
        }
        
        context.output << "Prediction System Status:" << std::endl;
        
        // Note: This is a placeholder implementation
        context.output << "  Note: Prediction system integration is in progress (Phase 2)" << std::endl;
        context.output << "  Prediction error integration available" << std::endl;
    }
    
    void executeConfidence(const CommandContext& context) {
        // Placeholder for confidence analysis
        context.output << "Prediction confidence analysis:" << std::endl;
        context.output << "  Note: Prediction confidence metrics available in integrated system" << std::endl;
    }
    
    void executeErrorAnalysis(const CommandContext& context) {
        // Placeholder for error analysis
        context.output << "Prediction error analysis:" << std::endl;
        context.output << "  Note: Prediction error analysis available in integrated system" << std::endl;
        
        // Show current prediction error if available
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(context.brain);
        if (agentBrain) {
            float error = agentBrain->getPredictionError();
            context.output << "  Current prediction error: " << error << std::endl;
        }
    }
    
    void executeSystemInfo(const CommandContext& context) {
        context.output << "Prediction System Integration:" << std::endl;
        context.output << "  Prediction system available: " 
                      << (context.brain->getPredictionSystem() ? "Yes" : "No") << std::endl;
        context.output << "  Working memory available: " 
                      << (context.brain->getWorkingMemory() ? "Yes" : "No") << std::endl;
        context.output << "  Episodic memory available: " 
                      << (context.brain->getEpisodicMemory() ? "Yes" : "No") << std::endl;
    }
};

} // namespace cli
} // namespace nlm
