// NeuronGroupManager.hpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "NeuronGroup.hpp"
#include "MotorNeuronGroup.hpp"
#include "SensoryNeuronGroup.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class NeuronGroupManager {
public:
    NeuronGroupManager(std::shared_ptr<Config> config);
    ~NeuronGroupManager() = default;
    
    // Initialize the manager with configuration
    void initialize();
    
    // Add a neuron group
    void addGroup(std::unique_ptr<NeuronGroup> group);
    
    // Get a group by name
    NeuronGroup* getGroup(const std::string& name);
    
    // Get a group by name (const version)
    const NeuronGroup* getGroup(const std::string& name) const;
    
    // Get all motor groups
    std::vector<MotorNeuronGroup*> getMotorGroups() const;
    
    // Get all sensory groups
    std::vector<SensoryNeuronGroup*> getSensoryGroups() const;
    
    // Get all groups
    const std::vector<std::unique_ptr<NeuronGroup>>& getAllGroups() const { return groups_; }
    
    // Remove a group
    bool removeGroup(const std::string& name);
    
    // Check if group exists
    bool hasGroup(const std::string& name) const;
    
    // Get distribution algorithm
    std::string getDistributionAlgorithm() const { return distributionAlgorithm_; }
    
    // Set distribution algorithm
    void setDistributionAlgorithm(const std::string& algorithm) { distributionAlgorithm_ = algorithm; }
    
    // Create motor neuron groups based on configuration
    void createMotorGroups();
    
    // Create sensory neuron groups based on configuration
    void createSensoryGroups();
    
    // Create text neurons
    void createTextNeurons(size_t count);
    
    // Add neuron to appropriate group based on type and distribution
    void addNeuronToGroup(Neuron* neuron, NeuronType type);
    
    // Get motor group count from config
    size_t getMotorGroupCount() const;
    
    // Get sensory group count from config
    size_t getSensoryGroupCount() const;
    
    // Get text neuron count from config
    size_t getTextNeuronCount() const;
    
    // Get group by type
    NeuronGroup* getGroupByType(NeuronType type);
    
    // Get group by type (const version)
    const NeuronGroup* getGroupByType(NeuronType type) const;
    
    // Update all groups with latest neuron data
    void updateGroups();
    
    // Clear all groups
    void clear();
    
    // Get total number of neurons across all groups
    size_t getTotalNeuronCount() const;
    
    // Get distribution statistics
    std::string getDistributionStats() const;
    
private:
    std::shared_ptr<Config> config_;
    std::vector<std::unique_ptr<NeuronGroup>> groups_;
    std::unordered_map<std::string, NeuronGroup*> groupsByName_;
    std::string distributionAlgorithm_ = "MODULO";
    
    // Helper methods
    std::string generateGroupName(const std::string& type, size_t index);
    void distributeNeuron(Neuron* neuron, NeuronType type, size_t index);
};

} // namespace nlm
