#pragma once

#include "CLICommand.hpp"
#include "performance/StatusManager.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace nlm {

class StatusCommand : public SimpleCLICommand {
public:
    StatusCommand() : SimpleCLICommand(
        "status",
        "Show system and neural simulation status",
        "nlm status [--format json|text|csv] [--detailed] [--refresh] [--watch]",
        "Show current status of the neural simulation system.\n\n"
        "Options:\n"
        "  --format json|text|csv    Output format (default: text)\n"
        "  --detailed                Show detailed information\n"
        "  --refresh                 Refresh status information\n"
        "  --watch                   Watch status updates (continuous)\n"
        "  --interval <seconds>      Refresh interval for watch mode\n"
        "  --export <file>           Export status to file\n\n"
        "Examples:\n"
        "  nlm status\n"
        "  nlm status --detailed\n"
        "  nlm status --format json --refresh\n"
        "  nlm status --watch --interval 5"
    ) {
        options = {
            {"format", "", "Output format", OptionType::Choice, false, "text", "json|text|csv"},
            {"detailed", "", "Show detailed information", OptionType::Flag},
            {"refresh", "", "Refresh status information", OptionType::Flag},
            {"watch", "", "Watch status updates (continuous)", OptionType::Flag},
            {"interval", "", "Refresh interval for watch mode", OptionType::OptionalValue, false, "5"},
            {"export", "", "Export status to file", OptionType::OptionalValue}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get options
        std::string format = getOptionValue(args, "format");
        bool detailed = hasOption(args, "detailed");
        bool refresh = hasOption(args, "refresh");
        bool watch = hasOption(args, "watch");
        std::string intervalStr = getOptionValue(args, "interval");
        std::string exportFile = getOptionValue(args, "export");
        
        // Parse interval
        int interval = 5; // default
        if (!intervalStr.empty()) {
            try {
                interval = std::stoi(intervalStr);
            } catch (...) {
                errorHandler.addError("Invalid interval value: " + intervalStr);
                return errorHandler.exitWithError();
            }
        }
        
        // Get status manager
        auto& statusManager = StatusManager::getGlobalStatusManager();
        
        // Show status
        if (watch) {
            watchStatus(interval, detailed, format);
        } else {
            showStatus(statusManager, detailed, format, refresh);
        }
        
        // Export status if requested
        if (!exportFile.empty()) {
            exportStatus(statusManager, exportFile, format);
        }
        
        return errorHandler.isSuccess() ? 0 : 1;
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        // Basic validation of arguments
        for (const auto& arg : args) {
            if (arg.find("--interval") != std::string::npos && arg != "--interval") {
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
            } else if (arg.find("--export") != std::string::npos && arg != "--export") {
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
            }
        }
        return true;
    }
    
private:
    void showStatus(StatusManager& statusManager, bool detailed, const std::string& format, bool refresh) {
        if (refresh) {
            // Refresh status information
            statusManager.updateBrainStatus(statusManager.getBrainStatus());
            statusManager.updateSystemStatus(statusManager.getSystemStatus());
            statusManager.updateExperimentStatus(statusManager.getExperimentStatus());
        }
        
        // Get status
        BrainStatus brainStatus = statusManager.getBrainStatus();
        SystemStatus systemStatus = statusManager.getSystemStatus();
        ExperimentStatus experimentStatus = statusManager.getExperimentStatus();
        
        // Display status based on format
        if (format == "json") {
            displayJsonStatus(brainStatus, systemStatus, experimentStatus, detailed);
        } else if (format == "csv") {
            displayCsvStatus(brainStatus, systemStatus, experimentStatus, detailed);
        } else {
            displayTextStatus(brainStatus, systemStatus, experimentStatus, detailed);
        }
    }
    
    void watchStatus(int interval, bool detailed, const std::string& format) {
        CLIFormatters::printInfo("Watching status updates (Ctrl+C to stop)...");
        
        try {
            while (true) {
                // Clear screen (or at least separate entries)
                std::cout << "\n" << std::string(80, '=') << std::endl;
                
                // Show status
                showStatus(StatusManager::getGlobalStatusManager(), detailed, format, true);
                
                // Wait for next update
                std::cout << "\nWaiting " << interval << " seconds for next update...";
                std::this_thread::sleep_for(std::chrono::seconds(interval));
            }
        } catch (const std::exception& e) {
            CLIFormatters::printInfo("Watching stopped");
        }
    }
    
    void displayTextStatus(const BrainStatus& brainStatus, const SystemStatus& systemStatus,
                           const ExperimentStatus& experimentStatus, bool detailed) {
        std::cout << "Neural Simulation Status" << std::endl;
        std::cout << "======================" << std::endl << std::endl;
        
        // Brain status
        std::cout << "Brain Status:" << std::endl;
        std::cout << "  Neurons: " << brainStatus.neuronCount << std::endl;
        std::cout << "  Synapses: " << brainStatus.synapseCount << std::endl;
        std::cout << "  Active Neurons: " << brainStatus.activeNeuronCount << std::endl;
        std::cout << "  Firing Neurons: " << brainStatus.firingNeuronCount << std::endl;
        std::cout << "  Average Firing Rate: " << brainStatus.averageFiringRate << "%" << std::endl;
        std::cout << "  Total Spikes: " << brainStatus.totalSpikeCount << std::endl;
        std::cout << "  Current Step: " << brainStatus.currentStep << std::endl;
        std::cout << "  Current Time: " << brainStatus.currentTime << " s" << std::endl;
        
        if (detailed) {
            std::cout << "  Excitation/Inhibition Ratio: " << brainStatus.excitationInhibitionRatio << std::endl;
            std::cout << "  Pending Spike Count: " << brainStatus.pendingSpikeCount << std::endl;
        }
        
        // System status
        std::cout << "\nSystem Status:" << std::endl;
        std::cout << "  Uptime: " << systemStatus.uptime << " seconds" << std::endl;
        std::cout << "  CPU Cores: " << systemStatus.cpuCores << std::endl;
        std::cout << "  CPU Usage: " << systemStatus.cpuUsage << "%" << std::endl;
        std::cout << "  Memory Total: " << CLIFormatters::formatMemory(systemStatus.memoryTotal) << std::endl;
        std::cout << "  Memory Used: " << CLIFormatters::formatMemory(systemStatus.memoryUsed) << std::endl;
        std::cout << "  Memory Available: " << CLIFormatters::formatMemory(systemStatus.memoryAvailable) << std::endl;
        
        if (detailed) {
            std::cout << "  Memory Usage Percentage: " << systemStatus.getMemoryUsagePercentage() << "%" << std::endl;
            std::cout << "  Disk Usage: " << systemStatus.diskUsage << "%" << std::endl;
            std::cout << "  Checkpoint Count: " << systemStatus.checkpointCount << std::endl;
            std::cout << "  Performance Score: " << systemStatus.getPerformanceRating() << "/100" << std::endl;
        }
        
        // Experiment status
        std::cout << "\nExperiment Status:" << std::endl;
        std::cout << "  Name: " << experimentStatus.name << std::endl;
        std::cout << "  Running: " << (experimentStatus.isRunning ? "Yes" : "No") << std::endl;
        std::cout << "  Paused: " << (experimentStatus.isPaused ? "Yes" : "No") << std::endl;
        std::cout << "  Current Step: " << experimentStatus.currentStep << "/" << experimentStatus.maxSteps << std::endl;
        std::cout << "  Completion: " << experimentStatus.completionPercentage << "%" << std::endl;
        std::cout << "  Total Experiments: " << experimentStatus.totalExperiments << std::endl;
        std::cout << "  Completed Experiments: " << experimentStatus.completedExperiments << std::endl;
        
        if (detailed) {
            std::cout << "  Average Firing Rate: " << experimentStatus.averageFiringRate << "%" << std::endl;
            std::cout << "  Total Reward: " << experimentStatus.totalReward << std::endl;
            std::cout << "  Start Time: " << experimentStatus.startTime << " s" << std::endl;
            std::cout << "  End Time: " << experimentStatus.endTime << " s" << std::endl;
            std::cout << "  Last Update: " << experimentStatus.lastUpdateTime << " s" << std::endl;
        }
        
        // Status level
        std::cout << "\nOverall Status Level: " << static_cast<int>(StatusManager::getGlobalStatusManager().getOverallStatusLevel()) << std::endl;
        
        // Active alerts
        auto alerts = statusManager.getActiveAlerts();
        if (!alerts.empty()) {
            std::cout << "\nActive Alerts (" << alerts.size() << "):" << std::endl;
            for (const auto& alert : alerts) {
                std::cout << "  - " << alert << std::endl;
            }
        }
    }
    
    void displayJsonStatus(const BrainStatus& brainStatus, const SystemStatus& systemStatus,
                           const ExperimentStatus& experimentStatus, bool detailed) {
        std::cout << "{\n";
        std::cout << "  \"brainStatus\": {\n";
        std::cout << "    \"neuronCount\": " << brainStatus.neuronCount << ",\n";
        std::cout << "    \"synapseCount\": " << brainStatus.synapseCount << ",\n";
        std::cout << "    \"activeNeuronCount\": " << brainStatus.activeNeuronCount << ",\n";
        std::cout << "    \"firingNeuronCount\": " << brainStatus.firingNeuronCount << ",\n";
        std::cout << "    \"averageFiringRate\": " << brainStatus.averageFiringRate << ",\n";
        std::cout << "    \"totalSpikeCount\": " << brainStatus.totalSpikeCount << ",\n";
        std::cout << "    \"currentStep\": " << brainStatus.currentStep << ",\n";
        std::cout << "    \"currentTime\": " << brainStatus.currentTime << "\n";
        if (detailed) {
            std::cout << "    \"excitationInhibitionRatio\": " << brainStatus.excitationInhibitionRatio << ",\n";
            std::cout << "    \"pendingSpikeCount\": " << brainStatus.pendingSpikeCount << "\n";
        } else {
            std::cout << "    \"excitationInhibitionRatio\": 0.0,\n";
            std::cout << "    \"pendingSpikeCount\": 0\n";
        }
        std::cout << "  },\n";
        
        std::cout << "  \"systemStatus\": {\n";
        std::cout << "    \"uptime\": " << systemStatus.uptime << ",\n";
        std::cout << "    \"cpuCores\": " << systemStatus.cpuCores << ",\n";
        std::cout << "    \"cpuUsage\": " << systemStatus.cpuUsage << ",\n";
        std::cout << "    \"memoryTotal\": " << systemStatus.memoryTotal << ",\n";
        std::cout << "    \"memoryUsed\": " << systemStatus.memoryUsed << ",\n";
        std::cout << "    \"memoryAvailable\": " << systemStatus.memoryAvailable << "\n";
        if (detailed) {
            std::cout << "    \"memoryUsagePercentage\": " << systemStatus.getMemoryUsagePercentage() << ",\n";
            std::cout << "    \"diskUsage\": " << systemStatus.diskUsage << ",\n";
            std::cout << "    \"checkpointCount\": " << systemStatus.checkpointCount << ",\n";
            std::cout << "    \"performanceScore\": " << systemStatus.getPerformanceRating() << "\n";
        } else {
            std::cout << "    \"memoryUsagePercentage\": " << systemStatus.getMemoryUsagePercentage() << ",\n";
            std::cout << "    \"diskUsage\": 0.0,\n";
            std::cout << "    \"checkpointCount\": 0,\n";
            std::cout << "    \"performanceScore\": 0.0\n";
        }
        std::cout << "  },\n";
        
        std::cout << "  \"experimentStatus\": {\n";
        std::cout << "    \"name\": \"" << experimentStatus.name << "\",\n";
        std::cout << "    \"isRunning\": " << (experimentStatus.isRunning ? "true" : "false") << ",\n";
        std::cout << "    \"isPaused\": " << (experimentStatus.isPaused ? "true" : "false") << ",\n";
        std::cout << "    \"currentStep\": " << experimentStatus.currentStep << ",\n";
        std::cout << "    \"maxSteps\": " << experimentStatus.maxSteps << ",\n";
        std::cout << "    \"completionPercentage\": " << experimentStatus.completionPercentage << ",\n";
        std::cout << "    \"totalExperiments\": " << experimentStatus.totalExperiments << ",\n";
        std::cout << "    \"completedExperiments\": " << experimentStatus.completedExperiments << "\n";
        if (detailed) {
            std::cout << "    \"averageFiringRate\": " << experimentStatus.averageFiringRate << ",\n";
            std::cout << "    \"totalReward\": " << experimentStatus.totalReward << ",\n";
            std::cout << "    \"startTime\": " << experimentStatus.startTime << ",\n";
            std::cout << "    \"endTime\": " << experimentStatus.endTime << ",\n";
            std::cout << "    \"lastUpdateTime\": " << experimentStatus.lastUpdateTime << "\n";
        } else {
            std::cout << "    \"averageFiringRate\": 0.0,\n";
            std::cout << "    \"totalReward\": 0.0,\n";
            std::cout << "    \"startTime\": 0.0,\n";
            std::cout << "    \"endTime\": 0.0,\n";
            std::cout << "    \"lastUpdateTime\": 0.0\n";
        }
        std::cout << "  }\n";
        std::cout << "}\n";
    }
    
    void displayCsvStatus(const BrainStatus& brainStatus, const SystemStatus& systemStatus,
                          const ExperimentStatus& experimentStatus, bool detailed) {
        std::cout << "Section,Key,Value" << std::endl;
        std::cout << "Brain,neuronCount," << brainStatus.neuronCount << std::endl;
        std::cout << "Brain,synapseCount," << brainStatus.synapseCount << std::endl;
        std::cout << "Brain,activeNeuronCount," << brainStatus.activeNeuronCount << std::endl;
        std::cout << "Brain,firingNeuronCount," << brainStatus.firingNeuronCount << std::endl;
        std::cout << "Brain,averageFiringRate," << brainStatus.averageFiringRate << std::endl;
        std::cout << "Brain,totalSpikeCount," << brainStatus.totalSpikeCount << std::endl;
        std::cout << "Brain,currentStep," << brainStatus.currentStep << std::endl;
        std::cout << "Brain,currentTime," << brainStatus.currentTime << std::endl;
        
        if (detailed) {
            std::cout << "Brain,excitationInhibitionRatio," << brainStatus.excitationInhibitionRatio << std::endl;
            std::cout << "Brain,pendingSpikeCount," << brainStatus.pendingSpikeCount << std::endl;
        }
        
        std::cout << "System,uptime," << systemStatus.uptime << std::endl;
        std::cout << "System,cpuCores," << systemStatus.cpuCores << std::endl;
        std::cout << "System,cpuUsage," << systemStatus.cpuUsage << std::endl;
        std::cout << "System,memoryTotal," << systemStatus.memoryTotal << std::endl;
        std::cout << "System,memoryUsed," << systemStatus.memoryUsed << std::endl;
        std::cout << "System,memoryAvailable," << systemStatus.memoryAvailable << std::endl;
        
        if (detailed) {
            std::cout << "System,memoryUsagePercentage," << systemStatus.getMemoryUsagePercentage() << std::endl;
            std::cout << "System,diskUsage," << systemStatus.diskUsage << std::endl;
            std::cout << "System,checkpointCount," << systemStatus.checkpointCount << std::endl;
            std::cout << "System,performanceScore," << systemStatus.getPerformanceRating() << std::endl;
        }
        
        std::cout << "Experiment,name," << experimentStatus.name << std::endl;
        std::cout << "Experiment,isRunning," << (experimentStatus.isRunning ? "true" : "false") << std::endl;
        std::cout << "Experiment,isPaused," << (experimentStatus.isPaused ? "true" : "false") << std::endl;
        std::cout << "Experiment,currentStep," << experimentStatus.currentStep << std::endl;
        std::cout << "Experiment,maxSteps," << experimentStatus.maxSteps << std::endl;
        std::cout << "Experiment,completionPercentage," << experimentStatus.completionPercentage << std::endl;
        std::cout << "Experiment,totalExperiments," << experimentStatus.totalExperiments << std::endl;
        std::cout << "Experiment,completedExperiments," << experimentStatus.completedExperiments << std::endl;
        
        if (detailed) {
            std::cout << "Experiment,averageFiringRate," << experimentStatus.averageFiringRate << std::endl;
            std::cout << "Experiment,totalReward," << experimentStatus.totalReward << std::endl;
            std::cout << "Experiment,startTime," << experimentStatus.startTime << std::endl;
            std::cout << "Experiment,endTime," << experimentStatus.endTime << std::endl;
            std::cout << "Experiment,lastUpdateTime," << experimentStatus.lastUpdateTime << std::endl;
        }
    }
    
    void exportStatus(StatusManager& statusManager, const std::string& filename, const std::string& format) {
        std::ofstream file(filename, std::ios::out);
        if (!file.is_open()) {
            CLIFormatters::printError("Cannot write to file: " + filename);
            return;
        }
        
        if (format == "json") {
            // Export as JSON
            file << "{\n";
            file << "  \"brainStatus\": {\n";
            auto brainStatus = statusManager.getBrainStatus();
            file << "    \"neuronCount\": " << brainStatus.neuronCount << ",\n";
            file << "    \"synapseCount\": " << brainStatus.synapseCount << ",\n";
            file << "    \"activeNeuronCount\": " << brainStatus.activeNeuronCount << ",\n";
            file << "    \"firingNeuronCount\": " << brainStatus.firingNeuronCount << ",\n";
            file << "    \"averageFiringRate\": " << brainStatus.averageFiringRate << ",\n";
            file << "    \"totalSpikeCount\": " << brainStatus.totalSpikeCount << ",\n";
            file << "    \"currentStep\": " << brainStatus.currentStep << ",\n";
            file << "    \"currentTime\": " << brainStatus.currentTime << "\n";
            file << "  },\n";
            
            file << "  \"systemStatus\": {\n";
            auto systemStatus = statusManager.getSystemStatus();
            file << "    \"uptime\": " << systemStatus.uptime << ",\n";
            file << "    \"cpuCores\": " << systemStatus.cpuCores << ",\n";
            file << "    \"cpuUsage\": " << systemStatus.cpuUsage << ",\n";
            file << "    \"memoryTotal\": " << systemStatus.memoryTotal << ",\n";
            file << "    \"memoryUsed\": " << systemStatus.memoryUsed << ",\n";
            file << "    \"memoryAvailable\": " << systemStatus.memoryAvailable << "\n";
            file << "  },\n";
            
            file << "  \"experimentStatus\": {\n";
            auto experimentStatus = statusManager.getExperimentStatus();
            file << "    \"name\": \"" << experimentStatus.name << "\",\n";
            file << "    \"isRunning\": " << (experimentStatus.isRunning ? "true" : "false") << ",\n";
            file << "    \"isPaused\": " << (experimentStatus.isPaused ? "true" : "false") << ",\n";
            file << "    \"currentStep\": " << experimentStatus.currentStep << ",\n";
            file << "    \"maxSteps\": " << experimentStatus.maxSteps << ",\n";
            file << "    \"completionPercentage\": " << experimentStatus.completionPercentage << ",\n";
            file << "    \"totalExperiments\": " << experimentStatus.totalExperiments << ",\n";
            file << "    \"completedExperiments\": " << experimentStatus.completedExperiments << "\n";
            file << "  }\n";
            file << "}\n";
        } else {
            // Export as text
            auto brainStatus = statusManager.getBrainStatus();
            auto systemStatus = statusManager.getSystemStatus();
            auto experimentStatus = statusManager.getExperimentStatus();
            
            file << "Neural Simulation Status" << std::endl;
            file << "======================" << std::endl << std::endl;
            file << "Brain Status:" << std::endl;
            file << "  Neurons: " << brainStatus.neuronCount << std::endl;
            file << "  Synapses: " << brainStatus.synapseCount << std::endl;
            file << "  Active Neurons: " << brainStatus.activeNeuronCount << std::endl;
            file << "  Firing Neurons: " << brainStatus.firingNeuronCount << std::endl;
            file << "  Average Firing Rate: " << brainStatus.averageFiringRate << "%" << std::endl;
            file << "  Total Spikes: " << brainStatus.totalSpikeCount << std::endl;
            file << "  Current Step: " << brainStatus.currentStep << std::endl;
            file << "  Current Time: " << brainStatus.currentTime << " s" << std::endl;
            
            file << "\nSystem Status:" << std::endl;
            file << "  Uptime: " << systemStatus.uptime << " seconds" << std::endl;
            file << "  CPU Cores: " << systemStatus.cpuCores << std::endl;
            file << "  CPU Usage: " << systemStatus.cpuUsage << "%" << std::endl;
            file << "  Memory Total: " << CLIFormatters::formatMemory(systemStatus.memoryTotal) << std::endl;
            file << "  Memory Used: " << CLIFormatters::formatMemory(systemStatus.memoryUsed) << std::endl;
            file << "  Memory Available: " << CLIFormatters::formatMemory(systemStatus.memoryAvailable) << std::endl;
            
            file << "\nExperiment Status:" << std::endl;
            file << "  Name: " << experimentStatus.name << std::endl;
            file << "  Running: " << (experimentStatus.isRunning ? "Yes" : "No") << std::endl;
            file << "  Paused: " << (experimentStatus.isPaused ? "Yes" : "No") << std::endl;
            file << "  Current Step: " << experimentStatus.currentStep << "/" << experimentStatus.maxSteps << std::endl;
            file << "  Completion: " << experimentStatus.completionPercentage << "%" << std::endl;
        }
        
        file.close();
        CLIFormatters::printSuccess("Status exported to: " + filename);
    }
};

} // namespace nlm
