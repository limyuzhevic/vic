#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>

namespace nlm {

// Forward declarations for core classes
class Brain;
class Config;
class CheckpointManager;

// CLI help text
extern const std::string nlm_cli_help;

// Advanced CLI commands for power users
class NLMCli {
public:
    NLMCli();
    ~NLMCli();
    
    // Initialize CLI with configuration
    bool initialize(const std::string& configFile = "configs/default.cfg");
    
    // Parse and execute command line arguments
    int execute(int argc, char* argv[]);
    
    // Get help text for all commands
    static std::string getHelp() { return nlm_cli_help; }
    
    // Check if brain is initialized
    bool isBrainInitialized() const { return brainInitialized; }
    
    // Set global brain instance
    void setBrain(std::shared_ptr<Brain> brain) {
        globalBrain = brain;
        brainInitialized = (brain != nullptr);
    }
    
    // Set global config instance
    void setConfig(std::shared_ptr<Config> config) {
        globalConfig = config;
    }
    
    // Set global checkpoint manager instance
    void setCheckpointManager(std::shared_ptr<CheckpointManager> manager) {
        globalCheckpointManager = manager;
    }
    
private:
    // Command execution
    void parseArguments(int argc, char* argv[]);
    void executeCommand(const std::string& command, const std::vector<std::string>& args);
    void printHelp(const std::string& command = "");
    
    // Command handlers
    using CommandHandler = std::function<void(const std::vector<std::string>&)>;
    std::map<std::string, CommandHandler> commandHandlers;
    
    // State
    struct CLIState {
        bool verbose = false;
        bool quiet = false;
        std::string configFile = "configs/default.cfg";
        std::string currentWorkingDir;
        bool brainInitialized = false;
        
        CLIState() {
            currentWorkingDir = std::filesystem::current_path().string();
        }
    } state;
    
    // Global instances
    static std::shared_ptr<Brain> globalBrain;
    static std::shared_ptr<Config> globalConfig;
    static std::shared_ptr<CheckpointManager> globalCheckpointManager;
    static bool brainInitialized;
    
    // Private helper methods for individual commands
    void cmd_brain_snapshot(const std::vector<std::string>& args);
    void cmd_brain_save_snapshot(const std::vector<std::string>& args);
    void cmd_brain_load_snapshot(const std::vector<std::string>& args);
    void cmd_checkpoint_save(const std::vector<std::string>& args);
    void cmd_checkpoint_list(const std::vector<std::string>& args);
    void cmd_checkpoint_load(const std::vector<std::string>& args);
    void cmd_checkpoint_info(const std::vector<std::string>& args);
    void cmd_checkpoint_delete(const std::vector<std::string>& args);
    void cmd_checkpoint_restore(const std::vector<std::string>& args);
    void cmd_memory_working(const std::vector<std::string>& args);
    void cmd_memory_episodic(const std::vector<std::string>& args);
    void cmd_memory_semantic(const std::vector<std::string>& args);
    void cmd_memory_consolidate(const std::vector<std::string>& args);
    void cmd_memory_replay(const std::vector<std::string>& args);
    void cmd_memory_stats(const std::vector<std::string>& args);
    void cmd_memory_clear_working(const std::vector<std::string>& args);
    void cmd_memory_clear_episodic(const std::vector<std::string>& args);
    void cmd_memory_clear_semantic(const std::vector<std::string>& args);
    void cmd_neuromodulators(const std::vector<std::string>& args);
    void cmd_neuromodulator_set(const std::vector<std::string>& args);
    void cmd_neuromodulator_info(const std::vector<std::string>& args);
    void cmd_neuromodulators_calibrate(const std::vector<std::string>& args);
    void cmd_neuromodulators_stats(const std::vector<std::string>& args);
    void cmd_neuromodulators_reset(const std::vector<std::string>& args);
    void cmd_neuromodulators_snapshot(const std::vector<std::string>& args);
    void cmd_prediction_accuracy(const std::vector<std::string>& args);
    void cmd_prediction_errors(const std::vector<std::string>& args);
    void cmd_prediction_visualize(const std::vector<std::string>& args);
    void cmd_prediction_tune(const std::vector<std::string>& args);
    void cmd_prediction_reset(const std::vector<std::string>& args);
    void cmd_prediction_info(const std::vector<std::string>& args);
    void cmd_prediction_snapshot(const std::vector<std::string>& args);
    void cmd_config_export(const std::vector<std::string>& args);
    void cmd_config_import(const std::vector<std::string>& args);
    void cmd_config_summary(const std::vector<std::string>& args);
    void cmd_config_set(const std::vector<std::string>& args);
    void cmd_config_get(const std::vector<std::string>& args);
    void cmd_profile_start(const std::vector<std::string>& args);
    void cmd_profile_stop(const std::vector<std::string>& args);
    void cmd_profile_report(const std::vector<std::string>& args);
    void cmd_debug_neural_activity(const std::vector<std::string>& args);
    void cmd_debug_synapses(const std::vector<std::string>& args);
    void cmd_debug_plasticity(const std::vector<std::string>& args);
    void cmd_debug_events(const std::vector<std::string>& args);
    void cmd_debug_logs(const std::vector<std::string>& args);
    void cmd_debug_memory_usage(const std::vector<std::string>& args);
    void cmd_debug_performance(const std::vector<std::string>& args);
    void cmd_debug_connectivity(const std::vector<std::string>& args);
    void cmd_neural_inspect(const std::vector<std::string>& args);
    void cmd_synapse_inspect(const std::vector<std::string>& args);
    void cmd_plasticity_stats(const std::vector<std::string>& args);
    void cmd_event_log_enable(const std::vector<std::string>& args);
    void cmd_event_log_disable(const std::vector<std::string>& args);
};

} // namespace nlm
