// ParallelProcessing.cpp - Implementation of thread pool and parallel processing utilities

#include "ParallelProcessing.hpp"
#include <stdexcept>
#include <chrono>

namespace nlm {

// ThreadPool implementation

template<typename Func, typename... Args>
auto ThreadPool::submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type> {
    using ReturnType = typename std::invoke_result<Func, Args...>::type;
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );
    
    std::future<ReturnType> res = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    return res;
}

// ParallelFor implementation

// SIMDVectorization.cpp - Implementation of SIMD-optimized neural computation

#include "SIMDVectorization.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// VectorizedSTDP update implementation

template<typename WArray, typename PreArray, typename PostArray>
void VectorizedSTDP::updateWeights(WArray& weights, const PreArray& preSpikes,
                                   const PostArray& postSpikes, size_t numPre, size_t numPost) {
    // Implementation optimized for SIMD with special handling for various SIMD widths
#if defined(NLM_SIMD_AVX512)
    updateWeightsAVX512(weights, preSpikes, postSpikes, numPre, numPost);
#elif defined(NLM_SIMD_AVX2)
    updateWeightsAVX2(weights, preSpikes, postSpikes, numPre, numPost);
#elif defined(NLM_SIMD_SSE)
    updateWeightsSSE(weights, preSpikes, postSpikes, numPre, numPost);
#else
    updateWeightsScalar(weights, preSpikes, postSpikes, numPre, numPost);
#endif
}

// VectorizedLIF update implementation (already in header, but need to provide template definitions)

// CheckpointSystem.cpp - Implementation of checkpoint persistence

#include "CheckpointSystem.hpp"
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include <codecvt>
#include <iomanip>

namespace nlm {

// CheckpointReader implementation

CheckpointReader::CheckpointReader() : streamPos_(0) {
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    close();
    
    stream_ = std::ifstream(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_) {
        error_ = "Failed to read checkpoint header";
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Read section headers
    sections_.clear();
    size_t sectionCount = (header_.totalSize - sizeof(CheckpointHeader)) / sizeof(SectionHeader);
    
    for (size_t i = 0; i < sectionCount; ++i) {
        SectionHeader section;
        stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
        if (!stream_) {
            error_ = "Failed to read section header";
            return false;
        }
        sections_.push_back(section);
    }
    
    streamPos_ = stream_.tellg();
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
    error_.clear();
    streamPos_ = 0;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto section = getSection(CheckpointSection::Neurons);
    if (!section) {
        error_ = "Neurons section not found";
        return false;
    }
    
    // Move to section
    stream_.seekg(section->offset);
    
    // Determine neuron count from header or section size
    uint64_t neuronCount = header_.neuronCount;
    if (neuronCount == 0 && section->size > 0) {
        // Estimate from section size (rough approximation)
        neuronCount = section->size / sizeof(NeuronCheckpointData);
    }
    
    // Read neuron data
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
    
    stream_.read(reinterpret_cast<char*>(data.membranePotential.data()),
                 data.membranePotential.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.restingPotential.data()),
                 data.restingPotential.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.threshold.data()),
                 data.threshold.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.resetPotential.data()),
                 data.resetPotential.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.leakConductance.data()),
                 data.leakConductance.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.firingState.data()),
                 data.firingState.size() * sizeof(uint8_t));
    stream_.read(reinterpret_cast<char*>(data.refractoryRemaining.data()),
                 data.refractoryRemaining.size() * sizeof(uint32_t));
    stream_.read(reinterpret_cast<char*>(data.refractoryPeriod.data()),
                 data.refractoryPeriod.size() * sizeof(uint32_t));
    stream_.read(reinterpret_cast<char*>(data.lastSpikeTime.data()),
                 data.lastSpikeTime.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.neuronType.data()),
                 data.neuronType.size() * sizeof(uint64_t));
    stream_.read(reinterpret_cast<char*>(data.regionId.data()),
                 data.regionId.size() * sizeof(uint64_t));
    stream_.read(reinterpret_cast<char*>(data.populationId.data()),
                 data.populationId.size() * sizeof(uint64_t));
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto section = getSection(CheckpointSection::Synapses);
    if (!section) {
        error_ = "Synapses section not found";
        return false;
    }
    
    // Move to section
    stream_.seekg(section->offset);
    
    // Determine synapse count
    uint64_t synapseCount = header_.synapseCount;
    if (synapseCount == 0 && section->size > 0) {
        synapseCount = section->size / sizeof(SynapseCheckpointData);
    }
    
    // Read synapse data
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
    
    stream_.read(reinterpret_cast<char*>(data.sourceNeuron.data()),
                 data.sourceNeuron.size() * sizeof(uint64_t));
    stream_.read(reinterpret_cast<char*>(data.destinationNeuron.data()),
                 data.destinationNeuron.size() * sizeof(uint64_t));
    stream_.read(reinterpret_cast<char*>(data.weight.data()),
                 data.weight.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.delay.data()),
                 data.delay.size() * sizeof(uint32_t));
    stream_.read(reinterpret_cast<char*>(data.synapseType.data()),
                 data.synapseType.size() * sizeof(uint8_t));
    stream_.read(reinterpret_cast<char*>(data.plasticityFlags.data()),
                 data.plasticityFlags.size() * sizeof(uint8_t));
    stream_.read(reinterpret_cast<char*>(data.eligibilityTrace.data()),
                 data.eligibilityTrace.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.efficacy.data()),
                 data.efficacy.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.shortTermDepression.data()),
                 data.shortTermDepression.size() * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.shortTermFacilitation.data()),
                 data.shortTermFacilitation.size() * sizeof(float));
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    auto section = getSection(sectionType);
    if (!section) {
        return std::vector<uint8_t>();
    }
    
    stream_.seekg(section->offset);
    std::vector<uint8_t> data(section->size);
    stream_.read(reinterpret_cast<char*>(data.data()), data.size());
    
    return data;
}

bool CheckpointReader::validate() const {
    if (!stream_.is_open()) {
        error_ = "Checkpoint file not open";
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Verify checksum
    uint64_t fileChecksum = calculateChecksum();
    if (fileChecksum != header_.checksum) {
        error_ = "Checksum validation failed";
        return false;
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (!stream_.is_open()) return 0.0;
    
    auto current = stream_.tellg();
    auto end = stream_.seekg(0, std::ios::end);
    stream_.seekg(current);
    
    return static_cast<double>(current) / end;
}

const CheckpointReader::SectionHeader* CheckpointReader::getSection(CheckpointSection sectionType) const {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            return &section;
        }
    }
    return nullptr;
}

uint64_t CheckpointReader::calculateChecksum() const {
    // Simple checksum implementation - could be enhanced
    stream_.seekg(sizeof(CheckpointHeader));
    stream_.seekg(0, std::ios::end);
    return 0; // Placeholder
}

// CheckpointWriter implementation

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {
}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    close();
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Create directory if it doesn't exist
    std::filesystem::path path(filepath_);
    std::filesystem::create_directories(path.parent_path());
    
    stream_ = std::ofstream(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to create checkpoint file: " + filepath_;
        return false;
    }
    
    // Write header (will be updated later)
    if (!writeHeader()) {
        return false;
    }
    
    return true;
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        finalize();
        stream_.close();
    }
    sections_.clear();
    error_.clear();
    bytesWritten_ = 0;
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                  uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    header_.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    // Serialize neuron data
    std::vector<uint8_t> buffer;
    buffer.reserve(data.membranePotential.size() * sizeof(float) * 15);  // Approximate
    
    // Write each array sequentially
    auto writeArray = [&buffer](const auto& arr) {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(arr.data());
        size_t size = arr.size() * sizeof(typename std::remove_reference<decltype(arr)>::type::value_type);
        buffer.insert(buffer.end(), ptr, ptr + size);
    };
    
    writeArray(data.membranePotential);
    writeArray(data.restingPotential);
    writeArray(data.threshold);
    writeArray(data.resetPotential);
    writeArray(data.leakConductance);
    writeArray(data.firingState);
    writeArray(data.refractoryRemaining);
    writeArray(data.refractoryPeriod);
    writeArray(data.lastSpikeTime);
    writeArray(data.neuronType);
    writeArray(data.regionId);
    writeArray(data.populationId);
    
    return writeSection(CheckpointSection::Neurons, buffer.data(), buffer.size());
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    std::vector<uint8_t> buffer;
    buffer.reserve(data.sourceNeuron.size() * sizeof(uint64_t) * 10);
    
    auto writeArray = [&buffer](const auto& arr) {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(arr.data());
        size_t size = arr.size() * sizeof(typename std::remove_reference<decltype(arr)>::type::value_type);
        buffer.insert(buffer.end(), ptr, ptr + size);
    };
    
    writeArray(data.sourceNeuron);
    writeArray(data.destinationNeuron);
    writeArray(data.weight);
    writeArray(data.delay);
    writeArray(data.synapseType);
    writeArray(data.plasticityFlags);
    writeArray(data.eligibilityTrace);
    writeArray(data.efficacy);
    writeArray(data.shortTermDepression);
    writeArray(data.shortTermFacilitation);
    
    return writeSection(CheckpointSection::Synapses, buffer.data(), buffer.size());
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open()) {
        error_ = "Checkpoint file not open";
        return false;
    }
    
    // Compress data if needed
    std::vector<uint8_t> compressedData;
    size_t compressedSize = size;
    
    if (compression_ != CompressionLevel::None) {
        // Placeholder for compression implementation
        compressedData.assign(static_cast<const uint8_t*>(data), 
                           static_cast<const uint8_t*>(data) + size);
        compressedSize = size;
    } else {
        compressedData.assign(static_cast<const uint8_t*>(data), 
                           static_cast<const uint8_t*>(data) + size);
    }
    
    // Write section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = static_cast<uint64_t>(stream_.tellp()) + sizeof(SectionHeader);
    section.size = compressedSize;
    section.compressedSize = (compression_ == CompressionLevel::None) ? 0 : compressedSize;
    section.checksum = calculateChecksum(compressedData.data(), compressedData.size());
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
    
    // Pad to alignment
    padToAlignment(64);
    
    sections_.push_back(section);
    bytesWritten_ += sizeof(SectionHeader) + compressedData.size();
    
    return stream_.good();
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) return false;
    
    // Update header with final checksum
    header_.totalSize = static_cast<uint64_t>(stream_.tellp());
    header_.checksum = calculateChecksum();
    
    // Seek back and write updated header
    stream_.seekp(0, std::ios::beg);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    
    return stream_.good();
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        // Remove partially written file
        std::filesystem::remove(filepath_);
    }
    sections_.clear();
    error_.clear();
    bytesWritten_ = 0;
}

bool CheckpointWriter::writeHeader() {
    header_.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    header_.compressionLevel = static_cast<uint32_t>(compression_);
    
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    return stream_.good();
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    return stream_.good();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t current = static_cast<size_t>(stream_.tellp());
    size_t padding = (alignment - (current % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> paddingData(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingData.data()), padding);
    }
    
    return stream_.good();
}

uint64_t CheckpointWriter::calculateChecksum() const {
    // Calculate checksum from entire file (excluding header checksum field)
    // For simplicity, return 0 (implementation should be improved)
    return 0;
}

uint64_t CheckpointWriter::calculateChecksum(const void* data, size_t size) const {
    // Simple checksum implementation
    uint64_t checksum = 0;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        checksum = (checksum + bytes[i]) ^ 0x9E3779B97F4A7C15ULL;
        checksum = (checksum << 31) | (checksum >> 33);  // Rotate
    }
    
    return checksum;
}

// CheckpointManager implementation

CheckpointManager::CheckpointManager() : lastSaveStep_(0) {
}

CheckpointManager::~CheckpointManager() {
    // Cleanup files in directory
    pruneOldCheckpoints(0);  // Delete all checkpoints
}

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
    if (currentStep >= lastSaveStep_ + saveIntervalSteps_) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filename = name;
    if (filename.empty()) {
        filename = "checkpoint_" + std::to_string(lastSaveStep_) + ".bin";
    }
    
    std::string filepath = checkpointDir_ + "/" + filename;
    
    // Create checkpoint writer
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Set metadata
    writer.setMetadata(0, 0, 0, lastSaveStep_, currentTime);  // TODO: Get actual counts from brain
    
    // Write data (TODO: Integrate with actual brain checkpointing)
    // For now, just create an empty checkpoint
    writer.finalize();
    
    lastSaveStep_ = lastSaveStep_ + saveIntervalSteps_;
    lastCheckpointPath_ = filepath;
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name;
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        return false;
    }
    
    // TODO: Implement actual loading from checkpoint
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            CheckpointInfo info;
            info.name = entry.path().filename().string();
            info.fileSize = entry.file_size();
            
            // Try to extract metadata from filename or parse file
            // This is a placeholder implementation
            info.step = 0;
            info.simulationTime = 0.0;
            info.neuronCount = 0;
            info.synapseCount = 0;
            info.timestamp = std::chrono::system_clock::now();
            
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
    
    // Delete old checkpoints
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::filesystem::remove(checkpointDir_ + "/" + checkpoints[i].name);
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_ + saveIntervalSteps_;
}

// OptimizedBrain.cpp - Implementation of high-performance brain with SIMD and parallel processing

#include "OptimizedBrain.hpp"
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>
#include <numeric>

namespace nlm {

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config) : 
    Brain(config),
    eventDrivenEnabled_(true),
    multithreadingEnabled_(true),
    simdEnabled_(true),
    sparseEnabled_(true),
    threadCount_(DEFAULT_NUM_THREADS),
    activeNeuronCount_(0),
    totalSteps_(0) {
    // Initialize performance-optimized structures
    neuralState_.initialize(config->getOr<int64_t>("neuron_count", 1000));
    synapseState_.initialize(config->getOr<int64_t>("neuron_count", 1000) * 
                           config->getOr<float>("connection_probability", 0.1) * 10);
}

OptimizedBrain::~OptimizedBrain() = default;

bool OptimizedBrain::initialize() {
    if (!Brain::initialize()) {
        return false;
    }
    
    // Initialize connectivity
    initializeSparseConnectivity();
    
    // Setup parallel processing
    if (multithreadingEnabled_) {
        parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    }
    
    // Initialize LIF vectorizer
    if (simdEnabled_) {
        // Configure vectorizer based on available SIMD
        // For now, use default parameters
    }
    
    return true;
}

void OptimizedBrain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep * 0.001));  // Default timestep
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    std::shared_lock<std::shared_mutex> lock(stepMutex_);
    
    // Update performance statistics
    lastStepTime_ = std::chrono::high_resolution_clock::now();
    
    // Advance step counter
    totalSteps_++;
    
    // Main simulation loop with optimizations
    if (eventDrivenEnabled_) {
        processActiveNeurons(currentStep, currentTime);
    } else {
        // Full synchronous update
        processFiringNeurons(currentStep, currentTime);
    }
    
    // Update development system
    if (getDevelopmentSystem() && currentStep % 10 == 0) {
        getDevelopmentSystem()->update(currentTime);
    }
    
    // Update statistics
    updateStatistics();
}

void OptimizedBrain::initializeSparseConnectivity() {
    // Generate sparse connectivity based on configuration
    float connectionProb = config_->getOr<float>("connection_probability", 0.1f);
    
    // Create connectivity manager
    connectivity_ = SparseConnectivity(
        neuralState_.membranePotential.size(),
        10,  // Max connections per neuron
        connectionProb,
        random_  // TODO: Pass random generator
    );
}

void OptimizedBrain::processActiveNeurons(uint64_t currentStep, double currentTime) {
    // Identify active neurons
    activeNeurons_.clear();
    activeNeuronCount_ = 0;
    
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        if (neuralState_.synapticInput[i] > 0.0f) {
            neuralState_.activeNeuronIndices.push_back(static_cast<uint32_t>(i));
            activeNeurons_.push_back(i);
            activeNeuronCount_++;
        }
    }
    
    // Process active neurons in parallel
    if (multithreadingEnabled_ && parallelProcessor_) {
        auto processFunc = [&](size_t neuronIdx, ParallelNeuralProcessor::ThreadLocalState& state) {
            size_t localIdx = neuronIdx;
            
            // Process neuron
            if (neuralState_.refractoryRemaining[localIdx] <= 0) {
                // Update membrane potential using SIMD if enabled
                if (simdEnabled_) {
                    // Use SIMD vectorized update
                    updateNeuronSIMD(localIdx, currentStep, currentTime);
                } else {
                    updateNeuron(localIdx, currentStep, currentTime);
                }
                
                // Check for spike
                if (neuralState_.membranePotential[localIdx] >= neuralState_.threshold[localIdx]) {
                    neuralState_.firingNeuronIndices.push_back(static_cast<uint32_t>(localIdx));
                    neuralState_.membranePotential[localIdx] = neuralState_.resetPotential[localIdx];
                    neuralState_.refractoryRemaining[localIdx] = neuralState_.refractoryPeriod[localIdx];
                    
                    state.localSpikeCount++;
                    state.localFiringCount++;
                }
            }
        };
        
        parallelProcessor_->processNeurons(neuralState_.membranePotential.size(), 
                                          processFunc);
    } else {
        // Sequential processing
        for (size_t neuronIdx : activeNeurons_) {
            if (neuralState_.refractoryRemaining[neuronIdx] <= 0) {
                updateNeuron(neuronIdx, currentStep, currentTime);
                
                if (neuralState_.membranePotential[neuronIdx] >= neuralState_.threshold[neuronIdx]) {
                    neuralState_.firingNeuronIndices.push_back(static_cast<uint32_t>(neuronIdx));
                    neuralState_.membranePotential[neuronIdx] = neuralState_.resetPotential[neuronIdx];
                    neuralState_.refractoryRemaining[neuronIdx] = neuralState_.refractoryPeriod[neuronIdx];
                }
            }
        }
    }
    
    // Process spikes and propagate through synapses
    processSpikes(currentStep, currentTime);
}

void OptimizedBrain::processFiringNeurons(uint64_t currentStep, double currentTime) {
    // Synchronous update of all neurons
    // This is less efficient but provides deterministic behavior
    
    // Update all neurons
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        if (neuralState_.refractoryRemaining[i] <= 0) {
            updateNeuron(i, currentStep, currentTime);
            
            if (neuralState_.membranePotential[i] >= neuralState_.threshold[i]) {
                neuralState_.firingNeuronIndices.push_back(static_cast<uint32_t>(i));
                neuralState_.membranePotential[i] = neuralState_.resetPotential[i];
                neuralState_.refractoryRemaining[i] = neuralState_.refractoryPeriod[i];
            }
        }
    }
    
    // Process spikes synchronously
    processSpikes(currentStep, currentTime);
}

void OptimizedBrain::updateNeuron(size_t neuronIdx, uint64_t currentStep, double currentTime) {
    // LIF dynamics
    float V = neuralState_.membranePotential[neuronIdx];
    float V_rest = neuralState_.restingPotential[neuronIdx];
    float V_reset = neuralState_.resetPotential[neuronIdx];
    float V_thresh = neuralState_.threshold[neuronIdx];
    float tau = neuralState_.leakConductance[neuronIdx];
    float C = 1.0f;  // Membrane capacitance
    
    // Membrane potential update
    float dV = (V_rest - V) / tau + neuralState_.synapticInput[neuronIdx] / C;
    float dt = 0.001f;  // Simulation timestep
    
    V += dt * dV * 1000.0f;  // Scale for ms
    
    // Apply refractory if needed
    if (neuralState_.refractoryRemaining[neuronIdx] > 0) {
        V = V_rest;
        neuralState_.refractoryRemaining[neuronIdx]--;
    }
    
    neuralState_.membranePotential[neuronIdx] = V;
}

void OptimizedBrain::updateNeuronSIMD(size_t neuronIdx, uint64_t currentStep, double currentTime) {
    // SIMD-optimized neuron update - use vectorized LIF implementation
    // For now, call regular update
    updateNeuron(neuronIdx, currentStep, currentTime);
}

void OptimizedBrain::processSpikes(uint64_t currentStep, double currentTime) {
    // Process firing neurons to generate spike events
    spikeQueue_.clear();  // Clear old spikes
    
    for (uint32_t localIdx : neuralState_.firingNeuronIndices) {
        // Convert local index to global neuron ID
        // This is a simplified implementation
        size_t globalIdx = localIdx;
        
        // Generate spike event
        QueuedSpikeEvent spike(globalIdx, globalIdx, 0, 1.0f, true);  // placeholder synapse ID
        
        // Add to spike queue
        spikeQueue_.push(spike);
    }
    
    // Process delayed spikes
    auto dueSpikes = delayedSpikeQueue_.getDueSpikes(currentStep);
    
    // Reset firing neuron indices for next step
    neuralState_.firingNeuronIndices.clear();
    
    // TODO: Process spikes and update synaptic inputs
}

void OptimizedBrain::applyPlasticity() {
    // Apply STDP, Hebbian, and structural plasticity
    // This would integrate with the plasticity systems
    
    // Simplified implementation
    if (getDevelopmentSystem()) {
        getDevelopmentSystem()->applyPlasticity();
    }
}

void OptimizedBrain::updateStatistics() {
    // Update performance statistics
    perfStats_.stepTimeMs = std::chrono::duration<double, std::milli>(
        std::chrono::high_resolution_clock::now() - lastStepTime_).count();
    
    // Calculate other statistics
    perfStats_.activeNeurons = activeNeuronCount_;
    perfStats_.firingNeurons = neuralState_.firingNeuronIndices.size();
    perfStats_.pendingSpikes = spikeQueue_.size();
    perfStats_.pendingDelayedSpikes = delayedSpikeQueue_.pendingCount();
    
    // Calculate derived metrics
    if (totalSteps_ > 0) {
        perfStats_.neuronsPerSecond = neuralState_.membranePotential.size() / perfStats_.stepTimeMs * 1000.0;
        perfStats_.spikesPerSecond = perfStats_.pendingSpikes / perfStats_.stepTimeMs * 1000.0;
        perfStats_.synapticEventsPerSecond = perfStats_.pendingDelayedSpikes / perfStats_.stepTimeMs * 1000.0;
    }
}

OptimizedBrain::PerformanceStats OptimizedBrain::getPerformanceStats() const {
    return perfStats_;
}

} // namespace nlm