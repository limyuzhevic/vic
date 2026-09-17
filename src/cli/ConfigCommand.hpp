#pragma once

#include "CLIContext.hpp"
#include <map>
#include <string>

namespace nlm {

// Configuration command implementations
class ConfigCommand {
public:
    static bool executeList(const CLIContext& context, const std::map<std::string, std::string>& options);
    static bool executeShow(const CLIContext& context, const std::string& key, const std::map<std::string, std::string>& options);
    static bool executeSet(CLIContext& context, const std::string& key, const std::string& value, const std::map<std::string, std::string>& options);
    static bool executeCreate(const CLIContext& context, const std::string& filename, const std::map<std::string, std::string>& options);
    static bool executeValidate(const CLIContext& context, const std::string& filename, const std::map<std::string, std::string>& options);
};

// Brain inspection command implementations
class BrainCommand {
public:
    static bool executeStatus(const CLIContext& context, const std::map<std::string, std::string>& options);
    static bool executeNeurons(CLIContext& context, const std::map<std::string, std::string>& options);
    static bool executeSynapses(CLIContext& context, const std::map<std::string, std::string>& options);
    static bool executeSave(const CLIContext& context, const std::string& filename, const std::map<std::string, std::string>& options);
    static bool executeLoad(CLIContext& context, const std::string& filename, const std::map<std::string, std::string>& options);
};

// Simulation control command implementations
class SimulationCommand {
public:
    static bool executeSimulate(CLIContext& context, const std::map<std::string, std::string>& options);
};

} // namespace nlm
