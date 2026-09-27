// BrainStatistics.hpp - Statistics and logging integration interface
#pragma once

#include <string>

namespace nlm {

class Brain;

class BrainStatistics {
public:
    BrainStatistics();
    ~BrainStatistics();
    
    // Set reference to parent brain
    void setBrain(Brain* brain) { brain_ = brain; }
    
    // Get statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Generate status report
    void logStatus() const;
    
    // Get comprehensive status string
    std::string getStatusReport() const;
    
private:
    Brain* brain_;
};

} // namespace nlm