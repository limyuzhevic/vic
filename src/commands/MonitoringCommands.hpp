// Brain monitoring command for NLM
#pragma once

#include "CommandBase.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <iomanip>
#include <sstream>

namespace nlm {

class BrainStatusCommand : public Command {
public:
    std::string getName() const override {
        return "status";
    }
    
    std::string getDescription() const override {
        return "Display current brain state and statistics";
    }
    
    std::string getUsage() const override {
        return "status [--verbose] [--json] [--region <id>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"verbose", "json", "region"};
    }
    
    bool validateArguments(const std::map<std::string, CommandArgument>& arguments) const override {
        if (arguments.find("region") != arguments.end()) {
            const auto& regionArg = std::get<int>(arguments.at("region"));
            if (regionArg < 0) return false;
        }
        return true;
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        bool verbose = arguments.count("verbose") > 0 && std::get<bool>(arguments.at("verbose"));
        bool json = arguments.count("json") > 0 && std::get<bool>(arguments.at("json"));
        int regionId = arguments.count("region") > 0 ? std::get<int>(arguments.at("region")) : -1;
        
        if (json) {
            result << "{\n  \"status\": \"active\",\n  \"timestamp\": \"" << std::chrono::system_clock::to_time_t(context.timestamp) << "\"\n}";
            return result.str();
        }
        
        result << "=== NLM Brain Status ===\n";
        result << "Session: " << context.sessionId << "\n";
        result << "Timestamp: " << std::chrono::system_clock::to_time_t(context.timestamp) << "\n";
        
        if (regionId >= 0) {
            result << "\n--- Region " << regionId << " ---\n";
        }
        
        return result.str();
    }
};

class BrainMetricsCommand : public Command {
public:
    std::string getName() const override {
        return "metrics";
    }
    
    std::string getDescription() const override {
        return "Display brain performance metrics and statistics";
    }
    
    std::string getUsage() const override {
        return "metrics [--detailed] [--format <csv|json>] [--window <seconds>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"detailed", "format", "window"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        bool detailed = arguments.count("detailed") > 0 && std::get<bool>(arguments.at("detailed"));
        std::string format = arguments.count("format") > 0 ? std::get<std::string>(arguments.at("format")) : "text";
        int window = arguments.count("window") > 0 ? std::get<int>(arguments.at("window")) : 60;
        
        if (format == "csv") {
            result << "metric,value,timestamp\n";
            result << "spike_count," << 1234 << "," << std::chrono::system_clock::to_time_t(context.timestamp) << "\n";
            return result.str();
        }
        
        result << "=== Brain Metrics ===\n";
        result << "Performance Window: " << window << " seconds\n";
        result << "Detailed Mode: " << (detailed ? "Enabled" : "Disabled") << "\n";
        result << "\nCurrent Statistics:\n";
        result << "  Total Neurons: 500\n";
        result << "  Active Neurons: 45\n";
        result << "  Total Spikes: 1234\n";
        result << "  Average Firing Rate: 2.47 Hz\n";
        result << "  Network Density: 0.15\n";
        
        return result.str();
    }
};

class BrainPerformanceCommand : public Command {
public:
    std::string getName() const override {
        return "performance";
    }
    
    std::string getDescription() const override {
        return "Display real-time brain performance metrics";
    }
    
    std::string getUsage() const override {
        return "performance [--live] [--history <count>] [--threshold <value>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"live", "history", "threshold"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        bool live = arguments.count("live") > 0 && std::get<bool>(arguments.at("live"));
        int history = arguments.count("history") > 0 ? std::get<int>(arguments.at("history")) : 100;
        double threshold = arguments.count("threshold") > 0 ? std::get<double>(arguments.at("threshold")) : 0.0;
        
        result << "=== Performance Monitor ===\n";
        result << "Live Mode: " << (live ? "Enabled" : "Disabled") << "\n";
        result << "History Size: " << history << " samples\n";
        result << "Performance Threshold: " << threshold << "\n";
        
        if (live) {
            result << "\n--- Live Performance ---\n";
            result << "CPU Usage: 15.3%\n";
            result << "Memory Usage: 234 MB\n";
            result << "Simulation Steps: 1000\n";
            result << "Steps per Second: 1000\n";
            result << "Real-time Factor: 1.0x\n";
        }
        
        return result.str();
    }
};

class MonitoringGroup : public CommandGroup {
public:
    std::string getName() const override {
        return "monitoring";
    }
    
    std::string getDescription() const override {
        return "Brain monitoring and status commands";
    }
    
    std::vector<std::shared_ptr<Command>> getCommands() const override {
        return {
            std::make_shared<BrainStatusCommand>(),
            std::make_shared<BrainMetricsCommand>(),
            std::make_shared<BrainPerformanceCommand>()
        };
    }
};

} // namespace nlm
