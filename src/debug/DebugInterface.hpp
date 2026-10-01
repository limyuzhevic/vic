#pragma once

#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class NeuronId;
class RegionId;
class NeuronType;
class ActionType;
class PlasticityRule;

// Forward declarations for types
using Timestamp = double;
using TimestepDuration = double;
using SynapticWeight = float;

// Debug interface for monitoring and analyzing NLM brain systems
class DebugInterface {
public:
    DebugInterface();
    ~DebugInterface();
    
    // Connection management
    void setBrain(Brain* brain);
    void setVerbose(bool verbose);
    
    // System status reporting
    std::string getSystemStatus() const;
    std::string getConnectivityReport() const;
    std::string getActivityReport() const;
    std::string getPlasticityReport() const;
    
    // Detailed debug information
    std::string getDebugInfo() const;
    
    // Report saving
    void saveDebugReport(const std::string& filename) const;
    
private:
    // Helper methods
    std::string getCurrentTimestamp() const;
    
    Brain* brain_;
    bool verbose_;
};

} // namespace nlm