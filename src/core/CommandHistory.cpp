#include "CommandHistory.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>

namespace nlm {

CommandHistory::CommandHistory() {}

void CommandHistory::addEntry(std::unique_ptr<CommandHistoryEntry> entry) {
    if (!entry) return;
    
    entries.push_back(std::move(entry));
    
    if (entries.size() > MAX_HISTORY_SIZE) {
        entries.erase(entries.begin());
    }
}

std::vector<CommandHistoryEntry*> CommandHistory::getEntries(SimulationStep startStep, SimulationStep endStep) {
    std::vector<CommandHistoryEntry*> result;
    for (auto& entry : entries) {
        if (entry->step >= startStep && entry->step <= endStep) {
            result.push_back(entry.get());
        }
    }
    return result;
}

std::vector<CommandHistoryEntry*> CommandHistory::getEntriesByStepRange(SimulationStep startStep, SimulationStep endStep) {
    return getEntries(startStep, endStep);
}

std::vector<CommandHistoryEntry*> CommandHistory::getEntriesByTimeRange(Timestamp startTime, Timestamp endTime) {
    std::vector<CommandHistoryEntry*> result;
    for (auto& entry : entries) {
        if (entry->time >= startTime && entry->time <= endTime) {
            result.push_back(entry.get());
        }
    }
    return result;
}

std::vector<CommandHistoryEntry*> CommandHistory::getEntriesByCriteria(const std::unordered_map<std::string, std::string>& criteria) {
    std::vector<CommandHistoryEntry*> result;
    for (auto& entry : entries) {
        bool matches = true;
        for (const auto& [key, value] : criteria) {
            if (key == "commandName") {
                if (entry->command && entry->command->getName() != value) {
                    matches = false;
                    break;
                }
            } else if (key == "category") {
                if (entry->command && entry->command->getCategory() != value) {
                    matches = false;
                    break;
                }
            } else if (key == "success" && value == "true") {
                if (!entry->result.success) {
                    matches = false;
                    break;
                }
            }
        }
        if (matches) {
            result.push_back(entry.get());
        }
    }
    return result;
}

std::vector<CommandHistoryEntry*> CommandHistory::findByCommandName(const std::string& commandName) {
    std::vector<CommandHistoryEntry*> result;
    for (auto& entry : entries) {
        if (entry->command && entry->command->getName() == commandName) {
            result.push_back(entry.get());
        }
    }
    return result;
}

CommandHistoryEntry* CommandHistory::findByStep(SimulationStep step) {
    for (auto& entry : entries) {
        if (entry->step == step) {
            return entry.get();
        }
    }
    return nullptr;
}

std::vector<CommandHistoryEntry*> CommandHistory::getSuccessfulCommands() {
    std::vector<CommandHistoryEntry*> result;
    for (auto& entry : entries) {
        if (entry->result.success) {
            result.push_back(entry.get());
        }
    }
    return result;
}

std::vector<CommandHistoryEntry*> CommandHistory::getFailedCommands() {
    std::vector<CommandHistoryEntry*> result;
    for (auto& entry : entries) {
        if (!entry->result.success) {
            result.push_back(entry.get());
        }
    }
    return result;
}

std::vector<CommandHistoryEntry*> CommandHistory::getCommandsByCategory(const std::string& category) {
    std::vector<CommandHistoryEntry*> result;
    for (auto& entry : entries) {
        if (entry->command && entry->command->getCategory() == category) {
            result.push_back(entry.get());
        }
    }
    return result;
}

CommandHistory::Statistics CommandHistory::getStatistics() const {
    Statistics stats;
    std::unordered_map<std::string, size_t> commandCounts;
    std::unordered_map<std::string, size_t> categoryCounts;
    
    for (const auto& entry : entries) {
        stats.totalCommands++;
        if (entry->result.success) {
            stats.successfulCommands++;
        } else {
            stats.failedCommands++;
        }
        
        if (entry->command) {
            commandCounts[entry->command->getName()]++;
            categoryCounts[entry->command->getCategory()]++;
        }
        
        if (entry->step < stats.earliestStep || stats.earliestStep == 0) {
            stats.earliestStep = entry->step;
        }
        if (entry->step > stats.latestStep) {
            stats.latestStep = entry->step;
        }
    }
    
    // Find most used command
    if (!commandCounts.empty()) {
        auto maxIt = std::max_element(commandCounts.begin(), commandCounts.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        if (maxIt != commandCounts.end()) {
            stats.mostUsedCommand = maxIt->first;
        }
    }
    
    // Find most used category
    if (!categoryCounts.empty()) {
        auto maxIt = std::max_element(categoryCounts.begin(), categoryCounts.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        if (maxIt != categoryCounts.end()) {
            stats.mostUsedCategory = maxIt->first;
        }
    }
    
    return stats;
}

bool CommandHistory::exportToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "CommandHistory Export" << std::endl;
    file << "=====================" << std::endl << std::endl;
    
    for (const auto& entry : entries) {
        file << "Step: " << entry->step << " Time: " << entry->time << std::endl;
        file << "Command: " << (entry->command ? entry->command->getName() : "None") << std::endl;
        file << "Success: " << (entry->result.success ? "Yes" : "No") << std::endl;
        file << "Output: " << entry->result.output << std::endl;
        if (!entry->result.errorMessage.empty()) {
            file << "Error: " << entry->result.errorMessage << std::endl;
        }
        file << "---" << std::endl;
    }
    
    return true;
}

bool CommandHistory::importFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Note: This is a simplified import - would need full implementation
    // for actual command history restoration
    std::string line;
    while (std::getline(file, line)) {
        // Parse import data (simplified)
        if (line.find("Step:") != std::string::npos) {
            // Extract step number (simplified parsing)
        }
    }
    
    return true;
}

std::vector<CommandHistoryEntry*> CommandHistory::filter(
    std::function<bool(const CommandHistoryEntry&)> predicate) {
    std::vector<CommandHistoryEntry*> result;
    for (auto& entry : entries) {
        if (predicate(*entry)) {
            result.push_back(entry.get());
        }
    }
    return result;
}

} // namespace nlm