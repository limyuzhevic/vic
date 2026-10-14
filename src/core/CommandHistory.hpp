#pragma once

#include "Command.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace nlm {

// CommandHistoryEntry: Represents a single command execution in history
struct CommandHistoryEntry {
    std::unique_ptr<Command> command;
    CommandResult result;
    CommandContext context;
    SimulationStep step;
    Timestamp time;
    std::string timestamp;
    
    CommandHistoryEntry(std::unique_ptr<Command> cmd, const CommandResult& res, 
                       const CommandContext& ctx, SimulationStep step, Timestamp time)
        : command(std::move(cmd)), result(res), context(ctx), step(step), time(time) {
        // Generate timestamp
        auto timePoint = std::chrono::high_resolution_clock::now();
        auto timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
        timestamp = std::to_string(timeSinceEpoch.count());
    }
};

// CommandHistory: Manages command history for undo/redo and analysis
class CommandHistory {
public:
    CommandHistory();
    ~CommandHistory() = default;
    
    // Add a command execution to history
    void addEntry(std::unique_ptr<CommandHistoryEntry> entry);
    
    // Get command entries for a specific time range
    std::vector<CommandHistoryEntry*> getEntries(SimulationStep startStep, SimulationStep endStep);
    
    // Get command entries by step range
    std::vector<CommandHistoryEntry*> getEntriesByStepRange(SimulationStep startStep, SimulationStep endStep);
    
    // Get command entries by time range
    std::vector<CommandHistoryEntry*> getEntriesByTimeRange(Timestamp startTime, Timestamp endTime);
    
    // Get command entries matching specific criteria
    std::vector<CommandHistoryEntry*> getEntriesByCriteria(const std::unordered_map<std::string, std::string>& criteria);
    
    // Find entries by command name
    std::vector<CommandHistoryEntry*> findByCommandName(const std::string& commandName);
    
    // Find entries by step
    CommandHistoryEntry* findByStep(SimulationStep step);
    
    // Get total history size
    size_t getSize() const { return entries.size(); }
    
    // Clear all entries
    void clear() { entries.clear(); }
    
    // Get all entries
    const std::vector<std::unique_ptr<CommandHistoryEntry>>& getAllEntries() const { return entries; }
    
    // Get successful commands
    std::vector<CommandHistoryEntry*> getSuccessfulCommands();
    
    // Get failed commands
    std::vector<CommandHistoryEntry*> getFailedCommands();
    
    // Get commands by category
    std::vector<CommandHistoryEntry*> getCommandsByCategory(const std::string& category);
    
    // Get command statistics
    struct Statistics {
        size_t totalCommands = 0;
        size_t successfulCommands = 0;
        size_t failedCommands = 0;
        std::string mostUsedCommand;
        std::string mostUsedCategory;
        SimulationStep earliestStep = 0;
        SimulationStep latestStep = 0;
    };
    
    Statistics getStatistics() const;
    
    // Export history to file
    bool exportToFile(const std::string& filepath) const;
    
    // Import history from file
    bool importFromFile(const std::string& filepath);
    
    // Filter history by conditions
    std::vector<CommandHistoryEntry*> filter(
        std::function<bool(const CommandHistoryEntry&)> predicate);
    
private:
    // Max history size
    static constexpr size_t MAX_HISTORY_SIZE = 50000;
    
    std::vector<std::unique_ptr<CommandHistoryEntry>> entries;
};

} // namespace nlm