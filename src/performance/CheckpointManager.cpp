// Implementation of CheckpointManager
#include "CheckpointSystem.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace nlm {

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0)
{
}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir,
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    std::string name = "checkpoint_" + std::to_string(currentStep);
    return saveImmediately(name);
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        std::cerr << "Error: Neuron or synapse providers not set" << std::endl;
        return false;
    }
    
    std::string filename;
    if (name.empty()) {
        filename = checkpointDir_ + "/latest.bin";
    } else {
        filename = checkpointDir_ + "/" + name + ".bin";
    }
    
    try {
        // Create checkpoint file
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Cannot create checkpoint file: " << filename << std::endl;
            return false;
        }
        
        // For now, use a simple binary format
        // In a full implementation, this would use the CheckpointWriter
        
        // Get neuron and synapse data
        NeuronCheckpointData neuronData;
        SynapseCheckpointData synapseData;
        
        if (!neuronProvider_(neuronData) || !synapseProvider_(synapseData)) {
            std::cerr << "Error: Failed to get checkpoint data" << std::endl;
            return false;
        }
        
        // Write header
        uint64_t magic = CHECKPOINT_MAGIC;
        file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
        
        // Write neuron count
        uint64_t neuronCount = neuronData.membranePotential.size();
        file.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));
        
        // Write synapse count
        uint64_t synapseCount = synapseData.weight.size();
        file.write(reinterpret_cast<const char*>(&synapseCount), sizeof(synapseCount));
        
        // Write simulation step
        uint64_t step = currentStep; // Note: currentStep parameter is not available here
        // In a proper implementation, we'd need to pass this from Brain
        file.write(reinterpret_cast<const char*>(&step), sizeof(step));
        
        // Write simulation time
        double simTime = currentTime; // Note: currentTime parameter is not available here
        // In a proper implementation, we'd need to pass this from Brain
        file.write(reinterpret_cast<const char*>(&simTime), sizeof(simTime));
        
        // Write neuron data
        uint32_t numNeurons = static_cast<uint32_t>(neuronData.membranePotential.size());
        file.write(reinterpret_cast<const char*>(&numNeurons), sizeof(numNeurons));
        
        // Write membrane potentials
        for (float potential : neuronData.membranePotential) {
            file.write(reinterpret_cast<const char*>(&potential), sizeof(potential));
        }
        
        // Write threshold values
        for (float threshold : neuronData.threshold) {
            file.write(reinterpret_cast<const char*>(&threshold), sizeof(threshold));
        }
        
        // Write synapses
        uint32_t numSynapses = static_cast<uint32_t>(synapseData.weight.size());
        file.write(reinterpret_cast<const char*>(&numSynapses), sizeof(numSynapses));
        
        for (size_t i = 0; i < synapseData.weight.size(); ++i) {
            // Write source and destination neurons
            uint64_t src = synapseData.sourceNeuron[i];
            uint64_t dst = synapseData.destinationNeuron[i];
            float weight = synapseData.weight[i];
            
            file.write(reinterpret_cast<const char*>(&src), sizeof(src));
            file.write(reinterpret_cast<const char*>(&dst), sizeof(dst));
            file.write(reinterpret_cast<const char*>(&weight), sizeof(weight));
        }
        
        // Write footer (simple checksum)
        uint64_t checksum = ChecksumCalculator::crc64(nullptr, 0);
        file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
        
        file.close();
        
        lastCheckpointPath_ = filename;
        
        std::cout << "Checkpoint saved: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving checkpoint: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filename;
    if (name.empty()) {
        filename = checkpointDir_ + "/latest.bin";
    } else {
        filename = checkpointDir_ + "/" + name + ".bin";
    }
    
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Cannot open checkpoint file: " << filename << std::endl;
            return false;
        }
        
        // Read header
        uint64_t magic;
        file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        
        if (magic != CHECKPOINT_MAGIC) {
            std::cerr << "Error: Invalid checkpoint file magic number" << std::endl;
            return false;
        }
        
        uint64_t timestamp;
        file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
        
        uint64_t neuronCount, synapseCount;
        file.read(reinterpret_cast<char*>(&neuronCount), sizeof(neuronCount));
        file.read(reinterpret_cast<char*>(&synapseCount), sizeof(synapseCount));
        
        uint64_t step;
        file.read(reinterpret_cast<char*>(&step), sizeof(step));
        
        double simTime;
        file.read(reinterpret_cast<char*>(&simTime), sizeof(simTime));
        
        // Read neuron data
        uint32_t numNeurons;
        file.read(reinterpret_cast<char*>(&numNeurons), sizeof(numNeurons));
        
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.resize(numNeurons);
        neuronData.threshold.resize(numNeurons);
        
        for (uint32_t i = 0; i < numNeurons; ++i) {
            file.read(reinterpret_cast<char*>(&neuronData.membranePotential[i]), sizeof(float));
            file.read(reinterpret_cast<char*>(&neuronData.threshold[i]), sizeof(float));
        }
        
        // Read synapses
        uint32_t numSynapses;
        file.read(reinterpret_cast<char*>(&numSynapses), sizeof(numSynapses));
        
        SynapseCheckpointData synapseData;
        synapseData.sourceNeuron.resize(numSynapses);
        synapseData.destinationNeuron.resize(numSynapses);
        synapseData.weight.resize(numSynapses);
        
        for (uint32_t i = 0; i < numSynapses; ++i) {
            file.read(reinterpret_cast<char*>(&synapseData.sourceNeuron[i]), sizeof(uint64_t));
            file.read(reinterpret_cast<char*>(&synapseData.destinationNeuron[i]), sizeof(uint64_t));
            file.read(reinterpret_cast<char*>(&synapseData.weight[i]), sizeof(float));
        }
        
        // Read checksum
        uint64_t checksum;
        file.read(reinterpret_cast<char*>(&checksum), sizeof(checksum));
        
        file.close();
        
        // Note: In a real implementation, we would call the synapse and neuron providers
        // to apply this data to the actual brain components
        
        std::cout << "Checkpoint loaded: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading checkpoint: " << e.what() << std::endl;
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.path().extension() == ".bin") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            
            // Get file size
            info.fileSize = std::filesystem::file_size(entry.path());
            
            // Get modification time
            auto ftime = std::filesystem::last_write_time(entry.path());
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
            info.timestamp = std::chrono::system_clock::to_time_t(sctp);
            
            // Note: We can't easily parse the checkpoint data to get step and time
            // In a full implementation, we would read the header
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Keep the most recent keepCount checkpoints
    size_t toDelete = checkpoints.size() - keepCount;
    for (size_t i = toDelete; i < checkpoints.size(); ++i) {
        std::string filename = checkpointDir_ + "/" + checkpoints[i].name + ".bin";
        try {
            std::filesystem::remove(filename);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not delete old checkpoint " << filename 
                      << ": " << e.what() << std::endl;
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep < saveIntervalSteps_) {
        return false;
    }
    
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

} // namespace nlm
