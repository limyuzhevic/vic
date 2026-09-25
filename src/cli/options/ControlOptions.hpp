#pragma once

#include <string>

namespace nlm {
namespace cli {
namespace options {

class ControlOptions {
public:
    ControlOptions();
    ~ControlOptions() = default;
    
    // Parse options from argument list
    void parse(const std::vector<std::string>& args);
    
    // Get parsed values
    int64_t getNeuronCount() const { return neuronCount; }
    double getSimulationTime() const { return simulationTime; }
    double getTimestep() const { return timestep; }
    int64_t getCheckpointInterval() const { return checkpointInterval; }
    bool getNoCheckpoint() const { return noCheckpoint; }
    
    // Set values
    void setNeuronCount(int64_t count) { neuronCount = count; }
    void setSimulationTime(double time) { simulationTime = time; }
    void setTimestep(double time) { timestep = time; }
    void setCheckpointInterval(int64_t interval) { checkpointInterval = interval; }
    void setNoCheckpoint(bool noCheckpoint) { this->noCheckpoint = noCheckpoint; }
    
private:
    int64_t neuronCount;
    double simulationTime;
    double timestep;
    int64_t checkpointInterval;
    bool noCheckpoint;
};

} // namespace options
} // namespace cli
} // namespace nlm
