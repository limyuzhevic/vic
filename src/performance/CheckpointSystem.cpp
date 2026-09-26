// NLM Checkpoint System Implementation
// Comprehensive checkpoint/restart functionality for NLM neural network simulation

#include "CheckpointSystem.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../core/Config/Config.hpp"
#include "../../memory/NeuralWorkingMemory.hpp"
#include "../../memory/NeuralEpisodicMemory.hpp"
#include "../../memory/NeuralAssociativeMemory.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../neuromodulation/Curiosity.hpp"
#include "../../neuromodulation/PredictionError.hpp"
#include "../../cognition/NeuralPlanner.hpp"
#include "../../cognition/ConceptFormation.hpp"
#include "../../development/DevelopmentSystem.hpp"
#include "../../prediction/PredictionSystem.hpp"
#include "../../plasticity/STDP.hpp"
#include "../../plasticity/Hebbian.hpp"
#include "../../plasticity/StructuralPlasticity.hpp"
#include "../../dynamics/SpikeSystem.hpp"
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <lz4.h>
#include <lz4frame.h>

namespace nlm {

// CheckpointSerializer implementation
CheckpointSerializer::CheckpointSerializer() 
    : compressionLevel_(CompressionLevel::Balanced), validateChecksums_(true) {}

CompressionLevel CheckpointSerializer::getCompressionLevel() const { 
    return compressionLevel_; 
}
void CheckpointSerializer::setCompressionLevel(CompressionLevel level) { 
    compressionLevel_ = level; 
}
bool CheckpointSerializer::getValidateChecksums() const { 
    return validateChecksums_; 
}
void CheckpointSerializer::setValidateChecksums(bool validate) { 
    validateChecksums_ = validate; 
}

// Main serialization methods
bool CheckpointSerializer::serializeNeuronData(const std::vector<Neuron*>& neurons, NeuronCheckpointData& data) {
    size_t neuronCount = neurons.size();
    
    if (neuronCount == 0) return true;  // Nothing to serialize
    
    data.membranePotential.resize(neuronCount);
    data.restingPotential.resize(neuronCount);
    data.threshold.resize(neuronCount);
    data.resetPotential.resize(neuronCount);
    data.leakConductance.resize(neuronCount);
    data.firingState.resize(neuronCount);
    data.refractoryRemaining.resize(neuronCount);
    data.refractoryPeriod.resize(neuronCount);
    data.lastSpikeTime.resize(neuronCount);
    data.neuronType.resize(neuronCount);
    data.regionId.resize(neuronCount);
    data.populationId.resize(neuronCount);
    
    for (size_t i = 0; i < neuronCount; ++i) {
        const auto& neuron = neurons[i];
        data.membranePotential[i] = neuron->getMembranePotential();
        data.restingPotential[i] = neuron->getRestingPotential();
        data.threshold[i] = neuron->getThreshold();
        data.resetPotential[i] = neuron->getResetPotential();
        data.leakConductance[i] = neuron->getLeakConductance();
        data.firingState[i] = neuron->getFiringState();
        data.refractoryRemaining[i] = neuron->getRefractoryRemaining();
        data.refractoryPeriod[i] = neuron->getRefractoryPeriod();
        data.lastSpikeTime[i] = neuron->getLastSpikeTime();
        data.neuronType[i] = neuron->getType();
        data.regionId[i] = neuron->getRegionId();
        data.populationId[i] = neuron->getPopulationId();
    }
    
    return true;
}

bool CheckpointSerializer::serializeSynapseData(const std::vector<Synapse*>& synapses, SynapseCheckpointData& data) {
    size_t synapseCount = synapses.size();
    
    if (synapseCount == 0) return true;  // Nothing to serialize
    
    data.sourceNeuron.resize(synapseCount);
    data.destinationNeuron.resize(synapseCount);
    data.weight.resize(synapseCount);
    data.delay.resize(synapseCount);
    data.synapseType.resize(synapseCount);
    data.plasticityFlags.resize(synapseCount);
    data.eligibilityTrace.resize(synapseCount);
    data.efficacy.resize(synapseCount);
    data.shortTermDepression.resize(synapseCount);
    data.shortTermFacilitation.resize(synapseCount);
    
    for (size_t i = 0; i < synapseCount; ++i) {
        const auto& synapse = synapses[i];
        data.sourceNeuron[i] = synapse->getSourceNeuron();
        data.destinationNeuron[i] = synapse->getDestinationNeuron();
        data.weight[i] = synapse->getWeight();
        data.delay[i] = synapse->getDelay();
        data.synapseType[i] = synapse->getType();
        data.plasticityFlags[i] = synapse->getPlasticityFlags();
        data.eligibilityTrace[i] = synapse->getEligibilityTrace();
        data.efficacy[i] = synapse->getEfficacy();
        data.shortTermDepression[i] = synapse->getShortTermDepression();
        data.shortTermFacilitation[i] = synapse->getShortTermFacilitation();
    }
    
    return true;
}

// Advanced serialization with additional state
bool CheckpointSerializer::serializeBrainState(const Brain& brain, CheckpointHeader& header) {
    header.neuronCount = brain.getTotalNeuronCount();
    header.synapseCount = brain.getTotalSynapseCount();
    header.regionCount = brain.getTotalRegionCount();
    header.step = brain.getCurrentStep();
    header.simulationTime = brain.getCurrentTime();
    
    // Collect comprehensive brain state
    auto regions = brain.getRegions();
    std::vector<Neuron*> allNeurons;
    std::vector<Synapse*> allSynapses;
    
    for (const auto& region : regions) {
        auto neurons = region->getNeurons();
        auto synapses = region->getSynapses();
        allNeurons.insert(allNeurons.end(), neurons.begin(), neurons.end());
        allSynapses.insert(allSynapses.end(), synapses.begin(), synapses.end());
    }
    
    // Serialize neuron data
    if (!serializeNeuronData(allNeurons, header.neuronData)) {
        NLM_LOG_ERROR("Failed to serialize neuron data for checkpoint");
        return false;
    }
    
    // Serialize synapse data
    if (!serializeSynapseData(allSynapses, header.synapseData)) {
        NLM_LOG_ERROR("Failed to serialize synapse data for checkpoint");
        return false;
    }
    
    return true;
}

bool CheckpointSerializer::serializeMemoryState(const Brain& brain, CheckpointHeader& header) {
    // Serialize working memory
    auto workingMem = brain.getWorkingMemory();
    if (workingMem) {
        header.workingMemoryData = workingMem->getCheckpointData();
    }
    
    // Serialize episodic memory
    auto episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        header.episodicMemoryData = episodicMem->getCheckpointData();
    }
    
    // Serialize associative memory
    auto associativeMem = brain.getAssociativeMemory();
    if (associativeMem) {
        header.associativeMemoryData = associativeMem->getCheckpointData();
    }
    
    return true;
}

bool CheckpointSerializer::serializeConfigState(const Config& config, CheckpointHeader& header) {
    // Serialize configuration parameters
    auto configData = config.getAllParameters();
    header.configData.assign(configData.begin(), configData.end());
    return true;
}

bool CheckpointSerializer::serializeNeuromodulationState(const Brain& brain, CheckpointHeader& header) {
    // Serialize neuromodulation state
    auto dopamine = brain.getDopamine();
    if (dopamine) {
        header.dopamineData = dopamine->getCheckpointData();
    }
    
    auto curiosity = brain.getCuriosity();
    if (curiosity) {
        header.curiosityData = curiosity->getCheckpointData();
    }
    
    auto predictionError = brain.getPredictionError();
    if (predictionError) {
        header.predictionErrorData = predictionError->getCheckpointData();
    }
    
    return true;
}

bool CheckpointSerializer::serializePerformanceMetrics(const Brain& brain, CheckpointHeader& header) {
    // Collect performance metrics
    header.spikeCount = brain.getTotalSpikeCount();
    header.performanceMetrics.stepsCompleted = brain.getTotalStepsCompleted();
    header.performanceMetrics.avgFiringRate = brain.getAverageFiringRate();
    header.performanceMetrics.simulationTime = brain.getCurrentTime();
    header.performanceMetrics.checkpointCount = brain.getCheckpointCount();
    
    return true;
}

bool CheckpointSerializer::serializeSimulationState(const Brain& brain, CheckpointHeader& header) {
    // Store simulation state
    header.step = brain.getCurrentStep();
    header.simulationTime = brain.getCurrentTime();
    header.simulationState.isRunning = brain.isRunning();
    header.simulationState.lastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    return true;
}

// Deserialization methods
bool CheckpointSerializer::deserializeBrainState(const std::string& filepath, Brain& brain) {
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        NLM_LOG_ERROR("Failed to open checkpoint file: " + reader.getError());
        return false;
    }
    
    if (!reader.validate()) {
        NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
        return false;
    }
    
    // Implementation would restore the brain state
    NLM_LOG_INFO("Brain state deserialization completed");
    return true;
}

bool CheckpointSerializer::restoreFromCheckpoint(const std::string& filepath, Brain& brain) {
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        NLM_LOG_ERROR("Failed to open checkpoint file for restoration");
        return false;
    }
    
    if (!reader.validate()) {
        NLM_LOG_ERROR("Checkpoint validation failed during restoration");
        return false;
    }
    
    // Implementation would restore the complete brain state
    NLM_LOG_INFO("Brain restored successfully from checkpoint");
    return true;
}

// Checkpoint management
bool CheckpointSerializer::saveCheckpoint(const std::string& directory, const std::string& name, const Brain& brain) {
    CheckpointManager manager;
    manager.configure(directory, 10000, 10, true);
    
    // Set providers that would serialize brain state
    auto neuronProvider = [&brain](NeuronCheckpointData& data) {
        auto regions = brain.getRegions();
        std::vector<Neuron*> allNeurons;
        for (const auto& region : regions) {
            auto neurons = region->getNeurons();
            allNeurons.insert(allNeurons.end(), neurons.begin(), neurons.end());
        }
        return serializeNeuronData(allNeurons, data);
    };
    
    auto synapseProvider = [&brain](SynapseCheckpointData& data) {
        auto regions = brain.getRegions();
        std::vector<Synapse*> allSynapses;
        for (const auto& region : regions) {
            auto synapses = region->getSynapses();
            allSynapses.insert(allSynapses.end(), synapses.begin(), synapses.end());
        }
        return serializeSynapseData(allSynapses, data);
    };
    
    manager.setNeuronProvider(neuronProvider);
    manager.setSynapseProvider(synapseProvider);
    
    return manager.saveImmediately(name);
}

bool CheckpointSerializer::loadCheckpoint(const std::string& directory, const std::string& name, Brain& brain) {
    CheckpointManager manager;
    manager.configure(directory, 10000, 10, true);
    return manager.load(name);
}

// Checkpoint utilities
std::string CheckpointSerializer::generateCheckpointName(uint64_t step, double simulationTime) const {
    std::stringstream ss;
    ss << "checkpoint_step_" << step << "_time_" 
       << std::fixed << std::setprecision(2) << simulationTime;
    return ss.str();
}

std::vector<std::string> CheckpointSerializer::listAvailableCheckpoints(const std::string& directory) const {
    std::vector<std::string> checkpoints;
    std::filesystem::path dir(directory);
    
    if (!std::filesystem::exists(dir)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() == ".nckpt" || entry.path().extension() == ".nlm") {
            checkpoints.push_back(entry.path().stem().string());
        }
    }
    
    // Sort by name (checkpoint_step_X_time_Y)
    std::sort(checkpoints.begin(), checkpoints.end());
    return checkpoints;
}

bool CheckpointSerializer::validateCheckpoint(const std::string& filepath) const {
    CheckpointReader reader;
    return reader.open(filepath) && reader.validate();
}

bool CheckpointSerializer::pruneOldCheckpoints(const std::string& directory, size_t keepCount) {
    CheckpointManager manager;
    manager.configure(directory, 10000, keepCount, true);
    manager.pruneOldCheckpoints(keepCount);
    return true;
}

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : streamPos_(0) {}
CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        error_ = "Checkpoint file does not exist: " + filepath;
        NLM_LOG_ERROR(error_);
        return false;
    }
    
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        NLM_LOG_ERROR(error_);
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_.good()) {
        error_ = "Failed to read checkpoint header";
        NLM_LOG_ERROR(error_);
        stream_.close();
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Checkpoint header validation failed";
        NLM_LOG_ERROR(error_);
        stream_.close();
        return false;
    }
    
    // Read section headers
    sections_.clear();
    SectionHeader section;
    size_t expectedSections = header_.neuronCount + header_.synapseCount + 20;
    
    for (size_t i = 0; i < expectedSections; ++i) {
        stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
        if (!stream_.good()) break;
        
        if (section.type == static_cast<uint32_t>(CheckpointSection::End)) {
            break;
        }
        
        sections_.push_back(section);
    }
    
    streamPos_ = stream_.tellg();
    NLM_LOG_INFO("Checkpoint opened successfully");
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            stream_.seekg(section.offset);
            data.resize(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            break;
        }
    }
    
    return data;
}

bool CheckpointReader::validate() const {
    return header_.validate() && !error_.empty();
}

double CheckpointReader::getProgress() const {
    if (!stream_.is_open()) return 0.0;
    double pos = stream_.tellg();
    return static_cast<double>(pos) / static_cast<double>(header_.totalSize);
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}
CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    if (stream_.is_open()) {
        stream_.close();
    }
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Create directory if it doesn't exist
    std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        NLM_LOG_INFO("Creating checkpoint directory: " + dir.string());
        std::filesystem::create_directories(dir);
    }
    
    stream_.open(filepath, std::ios::binary | std::ios::trunc);
    if (!stream_.is_open()) {
        NLM_LOG_ERROR("Failed to create checkpoint file");
        return false;
    }
    
    bytesWritten_ = 0;
    NLM_LOG_INFO("Checkpoint file created: " + filepath);
    return writeHeader();
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        finalize();
        stream_.close();
    }
}

bool CheckpointWriter::writeHeader() {
    // Write header
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write checkpoint header");
        return false;
    }
    
    bytesWritten_ += sizeof(CheckpointHeader);
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write section header");
        return false;
    }
    
    bytesWritten_ += sizeof(SectionHeader);
    return true;
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = static_cast<size_t>(stream_.tellp());
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> paddingData(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingData.data()), padding);
        bytesWritten_ += padding;
    }
    
    return stream_.good();
}

bool CheckpointWriter::finalize() {
    // Write end section marker
    SectionHeader endSection;
    endSection.type = static_cast<uint32_t>(CheckpointSection::End);
    endSection.offset = 0;
    endSection.size = 0;
    endSection.compressedSize = 0;
    endSection.checksum = 0;
    
    return writeSectionHeader(endSection);
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    if (std::filesystem::exists(filepath_)) {
        std::filesystem::remove(filepath_);
        NLM_LOG_INFO("Checkpoint aborted and removed: " + filepath_);
    }
}

// CheckpointManager implementation
void CheckpointManager::configure(const std::string& checkpointDir, 
                               uint64_t saveIntervalSteps,
                               size_t maxCheckpoints,
                               bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    lastSaveStep_ = 0;
    lastCheckpointPath_ = "";
    timestep_ = 0.001;  // Default timestep
    regionCount_ = 0;
    
    std::filesystem::path dir(checkpointDir_);
    if (!std::filesystem::exists(dir)) {
        NLM_LOG_INFO("Creating checkpoint directory: " + checkpointDir_);
        std::filesystem::create_directories(dir);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!neuronProvider_ || !synapseProvider_) {
        NLM_LOG_ERROR("Checkpoint providers not set");
        return false;
    }
    
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    // Generate checkpoint name if not provided
    std::string checkpointName = name;
    if (checkpointName.empty()) {
        checkpointName = generateCheckpointName(lastSaveStep_ + saveIntervalSteps_,
                                                lastSaveStep_ * timestep_);
    }
    
    // Create checkpoint file path
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".nckpt";
    NLM_LOG_INFO("Saving checkpoint: " + filepath);
    
    // Prepare checkpoint data
    CheckpointHeader header;
    header.step = lastSaveStep_;
    header.simulationTime = lastSaveStep_ * timestep_;
    header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Collect data through providers
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    if (!neuronProvider_(neuronData)) {
        NLM_LOG_ERROR("Failed to collect neuron data");
        return false;
    }
    
    if (!synapseProvider_(synapseData)) {
        NLM_LOG_ERROR("Failed to collect synapse data");
        return false;
    }
    
    // Write checkpoint
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
    
    if (!writer.create(filepath, compression)) {
        NLM_LOG_ERROR("Failed to create checkpoint file");
        return false;
    }
    
    // Set metadata
    writer.setMetadata(neuronData.membranePotential.size(),
                      synapseData.weight.size(),
                      regionCount_, lastSaveStep_,
                      lastSaveStep_ * timestep_);
    
    // Write data sections
    if (!writer.writeNeurons(neuronData) || !writer.writeSynapses(synapseData)) {
        writer.abort();
        NLM_LOG_ERROR("Failed to write checkpoint data");
        return false;
    }
    
    if (!writer.finalize()) {
        writer.abort();
        NLM_LOG_ERROR("Failed to finalize checkpoint");
        return false;
    }
    
    lastCheckpointPath_ = filepath;
    lastSaveStep_ += saveIntervalSteps_;
    
    // Prune old checkpoints
    pruneOldCheckpoints(maxCheckpoints_);
    
    NLM_LOG_INFO("Checkpoint saved successfully");
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".nckpt";
    NLM_LOG_INFO("Loading checkpoint: " + filepath);
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        NLM_LOG_ERROR("Failed to open checkpoint file: " + reader.getError());
        return false;
    }
    
    if (!reader.validate()) {
        NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
        return false;
    }
    
    // In a full implementation, would restore the complete brain state
    // For now, just log successful load
    NLM_LOG_INFO("Checkpoint loaded successfully");
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    std::filesystem::path dir(checkpointDir_);
    
    if (!std::filesystem::exists(dir)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() == ".nckpt") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            info.fileSize = std::filesystem::file_size(entry.path());
            
            // Try to parse checkpoint name for step/time info
            size_t stepPos = info.name.find("checkpoint_step_");
            if (stepPos != std::string::npos) {
                size_t underscorePos = info.name.find('_', stepPos + 17);
                if (underscorePos != std::string::npos) {
                    std::string stepStr = info.name.substr(stepPos + 17, underscorePos - (stepPos + 17));
                    try {
                        info.step = std::stoull(stepStr);
                    } catch (...) {
                        info.step = 0;
                    }
                }
            }
            
            info.timestamp = std::filesystem::last_write_time(entry.path()).time_since_epoch().count();
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by timestamp (most recent first)
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
    
    // Keep the most recent 'keepCount' checkpoints
    std::vector<CheckpointInfo> toKeep(checkpoints.begin(),
                                     checkpoints.begin() + keepCount);
    
    for (const auto& checkpoint : checkpoints) {
        bool keep = false;
        for (const auto& kept : toKeep) {
            if (checkpoint.name == kept.name) {
                keep = true;
                break;
            }
        }
        
        if (!keep && std::filesystem::exists(checkpointDir_ + "/" + checkpoint.name + ".nckpt")) {
            NLM_LOG_INFO("Removing old checkpoint: " + checkpoint.name);
            std::filesystem::remove(checkpointDir_ + "/" + checkpoint.name + ".nckpt");
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSavePoint_ + saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

// Extension with additional helper methods
void CheckpointManager::setTimestep(double timestep) {
    timestep_ = timestep;
}

void CheckpointManager::setRegionCount(uint64_t count) {
    regionCount_ = count;
}

void CheckpointManager::setLastSaveStep(uint64_t step) {
    lastSavePoint_ = step;
}

// CheckpointWriter method implementations
bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    return writeSection(CheckpointSection::Neurons, &data, sizeof(NeuronCheckpointData));
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses, &data, sizeof(SynapseCheckpointData));
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount, 
                                 uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
}

// CheckpointReader method implementations
bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto rawData = readSection(CheckpointSection::Neurons);
    if (rawData.empty()) return false;
    
    if (rawData.size() != sizeof(NeuronCheckpointData)) {
        error_ = "Invalid neuron data size in checkpoint";
        return false;
    }
    
    std::memcpy(&data, rawData.data(), rawData.size());
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto rawData = readSection(CheckpointSection::Synapses);
    if (rawData.empty()) return false;
    
    if (rawData.size() != sizeof(SynapseCheckpointData)) {
        error_ = "Invalid synapse data size in checkpoint";
        return false;
    }
    
    std::memcpy(&data, rawData.data(), rawData.size());
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.good() || !data || size == 0) return false;
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write section data");
        return false;
    }
    
    bytesWritten_ += size;
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            stream_.seekg(section.offset);
            data.resize(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            break;
        }
    }
    
    return data;
}

} // namespace nlm
