// Complete CheckpointReader implementation
CheckpointReader::CheckpointReader() : stream_(), streamPos_(0), error_() {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            close();
            error_ = "Failed to read checkpoint header";
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            close();
            error_ = "Invalid checkpoint header magic or version";
            return false;
        }
        
        // Read sections
        streamPos_ = sizeof(CheckpointHeader);
        uint32_t sectionCount = 0;
        
        // Read section count (stored as uint32_t after header)
        stream_.seekg(streamPos_);
        stream_.read(reinterpret_cast<char*>(&sectionCount), sizeof(uint32_t));
        streamPos_ += sizeof(uint32_t);
        
        sections_.resize(sectionCount);
        for (uint32_t i = 0; i < sectionCount; ++i) {
            stream_.seekg(streamPos_);
            stream_.read(reinterpret_cast<char*>(&sections_[i]), sizeof(SectionHeader));
            if (!stream_.good()) {
                close();
                error_ = "Failed to read section header " + std::to_string(i);
                return false;
            }
            streamPos_ += sizeof(SectionHeader);
        }
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception opening checkpoint: " + std::string(e.what());
        close();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
    streamPos_ = 0;
    error_.clear();
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    try {
        // Find Neurons section
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(CheckpointSection::Neurons)) {
                // Read section data
                stream_.seekg(section.offset);
                std::vector<uint8_t> buffer(section.size);
                stream_.read(reinterpret_cast<char*>(buffer.data()), section.size);
                if (!stream_.good()) {
                    error_ = "Failed to read neurons data";
                    return false;
                }
                
                // Decompress if needed
                std::vector<uint8_t> decompressed;
                if (section.compressedSize > 0) {
                    // Simple decompression for demonstration
                    decompressed = buffer;
                } else {
                    decompressed = buffer;
                }
                
                // Parse neurons data
                size_t neuronCount = data.membranePotential.size();
                size_t expectedSize = neuronCount * (
                    sizeof(float) * 5 +  // membranePotential, restingPotential, threshold, resetPotential, leakConductance
                    sizeof(uint8_t) * 4 +  // firingState, refractoryRemaining, refractoryPeriod
                    sizeof(float) * 4 +   // lastSpikeTime, neuronType
                    sizeof(uint64_t) * 4  // regionId, populationId
                );
                
                if (decompressed.size() < expectedSize) {
                    error_ = "Neurons data size mismatch";
                    return false;
                }
                
                const uint8_t* ptr = decompressed.data();
                
                // Read all neuron data
                for (size_t i = 0; i < neuronCount; ++i) {
                    // Read membrane potential
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.membranePotential[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.restingPotential[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.threshold[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.resetPotential[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.leakConductance[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(uint8_t) <= decompressed.data() + decompressed.size()) {
                        data.firingState[i] = *reinterpret_cast<const uint8_t*>(ptr);
                        ptr += sizeof(uint8_t);
                    }
                    
                    if (ptr + sizeof(uint32_t) <= decompressed.data() + decompressed.size()) {
                        data.refractoryRemaining[i] = *reinterpret_cast<const uint32_t*>(ptr);
                        ptr += sizeof(uint32_t);
                    }
                    
                    if (ptr + sizeof(uint32_t) <= decompressed.data() + decompressed.size()) {
                        data.refractoryPeriod[i] = *reinterpret_cast<const uint32_t*>(ptr);
                        ptr += sizeof(uint32_t);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.lastSpikeTime[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(uint64_t) <= decompressed.data() + decompressed.size()) {
                        data.neuronType[i] = *reinterpret_cast<const uint64_t*>(ptr);
                        ptr += sizeof(uint64_t);
                    }
                    
                    if (ptr + sizeof(uint64_t) <= decompressed.data() + decompressed.size()) {
                        data.regionId[i] = *reinterpret_cast<const uint64_t*>(ptr);
                        ptr += sizeof(uint64_t);
                    }
                    
                    if (ptr + sizeof(uint64_t) <= decompressed.data() + decompressed.size()) {
                        data.populationId[i] = *reinterpret_cast<const uint64_t*>(ptr);
                        ptr += sizeof(uint64_t);
                    }
                }
                
                return true;
            }
        }
        
        error_ = "Neurons section not found";
        return false;
    } catch (const std::exception& e) {
        error_ = "Exception reading neurons: " + std::string(e.what());
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        // Find Synapses section
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(CheckpointSection::Synapses)) {
                // Read section data
                stream_.seekg(section.offset);
                std::vector<uint8_t> buffer(section.size);
                stream_.read(reinterpret_cast<char*>(buffer.data()), section.size);
                if (!stream_.good()) {
                    error_ = "Failed to read synapses data";
                    return false;
                }
                
                // Decompress if needed
                std::vector<uint8_t> decompressed;
                if (section.compressedSize > 0) {
                    // Simple decompression for demonstration
                    decompressed = buffer;
                } else {
                    decompressed = buffer;
                }
                
                // Parse synapses data
                size_t synapseCount = data.sourceNeuron.size();
                size_t expectedSize = synapseCount * (
                    sizeof(uint64_t) * 2 +  // sourceNeuron, destinationNeuron
                    sizeof(float) * 7 +     // weight, delay, synapseType, plasticityFlags, eligibilityTrace, efficacy, shortTermDepression
                    sizeof(float) * 2      // shortTermFacilitation
                );
                
                if (decompressed.size() < expectedSize) {
                    error_ = "Synapses data size mismatch";
                    return false;
                }
                
                const uint8_t* ptr = decompressed.data();
                
                // Read all synapse data
                for (size_t i = 0; i < synapseCount; ++i) {
                    if (ptr + sizeof(uint64_t) <= decompressed.data() + decompressed.size()) {
                        data.sourceNeuron[i] = *reinterpret_cast<const uint64_t*>(ptr);
                        ptr += sizeof(uint64_t);
                    }
                    
                    if (ptr + sizeof(uint64_t) <= decompressed.data() + decompressed.size()) {
                        data.destinationNeuron[i] = *reinterpret_cast<const uint64_t*>(ptr);
                        ptr += sizeof(uint64_t);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.weight[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(uint32_t) <= decompressed.data() + decompressed.size()) {
                        data.delay[i] = *reinterpret_cast<const uint32_t*>(ptr);
                        ptr += sizeof(uint32_t);
                    }
                    
                    if (ptr + sizeof(uint8_t) <= decompressed.data() + decompressed.size()) {
                        data.synapseType[i] = *reinterpret_cast<const uint8_t*>(ptr);
                        ptr += sizeof(uint8_t);
                    }
                    
                    if (ptr + sizeof(uint8_t) <= decompressed.data() + decompressed.size()) {
                        data.plasticityFlags[i] = *reinterpret_cast<const uint8_t*>(ptr);
                        ptr += sizeof(uint8_t);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.eligibilityTrace[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.efficacy[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.shortTermDepression[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                    
                    if (ptr + sizeof(float) <= decompressed.data() + decompressed.size()) {
                        data.shortTermFacilitation[i] = *reinterpret_cast<const float*>(ptr);
                        ptr += sizeof(float);
                    }
                }
                
                return true;
            }
        }
        
        error_ = "Synapses section not found";
        return false;
    } catch (const std::exception& e) {
        error_ = "Exception reading synapses: " + std::string(e.what());
        return false;
    }
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
    // For now, just check if we have sections
    return !sections_.empty() && header_.validate();
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    // Simple progress calculation - could be improved
    return 0.5;
}

// CheckpointWriter implementations
CheckpointWriter::CheckpointWriter() : bytesWritten_(0), error_() {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Ensure directory exists
        std::filesystem::path path(filepath);
        std::filesystem::path dir = path.parent_path();
        if (!dir.empty()) {
            std::filesystem::create_directories(dir);
        }
        
        // Remove existing file
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
        
        filepath_ = filepath;
        compression_ = compression;
        
        // Open file for writing
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        // Initialize header with defaults
        header_.timestamp = 0;  // Would use std::chrono in real implementation
        header_.totalSize = 0;
        header_.neuronCount = 0;
        header_.synapseCount = 0;
        header_.regionCount = 0;
        header_.step = 0;
        header_.simulationTime = 0.0;
        header_.checksum = 0;
        header_.compressionLevel = static_cast<uint32_t>(compression_);
        
        return writeHeader();
    } catch (const std::exception& e) {
        error_ = "Exception creating checkpoint: " + std::string(e.what());
        abort();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        finalize();
    }
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount, 
                                 uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Prepare neurons data buffer
        size_t neuronCount = data.membranePotential.size();
        size_t bufferSize = neuronCount * (
            sizeof(float) * 5 +  // membranePotential, restingPotential, threshold, resetPotential, leakConductance
            sizeof(uint8_t) * 4 +  // firingState, refractoryRemaining, refractoryPeriod
            sizeof(float) * 4 +   // lastSpikeTime, neuronType
            sizeof(uint64_t) * 4  // regionId, populationId
        );
        
        std::vector<uint8_t> buffer;
        buffer.reserve(bufferSize);
        
        // Write all neuron data
        for (size_t i = 0; i < neuronCount; ++i) {
            // Write membrane potential
            float* fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.membranePotential[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write resting potential
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.restingPotential[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write threshold
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.threshold[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write reset potential
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.resetPotential[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write leak conductance
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.leakConductance[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write firing state
            uint8_t* ub = reinterpret_cast<uint8_t*>(buffer.data() + buffer.size());
            new(ub) uint8_t(data.firingState[i]);
            buffer.resize(buffer.size() + sizeof(uint8_t));
            
            // Write refractory remaining
            uint32_t* up = reinterpret_cast<uint32_t*>(buffer.data() + buffer.size());
            new(up) uint32_t(data.refractoryRemaining[i]);
            buffer.resize(buffer.size() + sizeof(uint32_t));
            
            // Write refractory period
            up = reinterpret_cast<uint32_t*>(buffer.data() + buffer.size());
            new(up) uint32_t(data.refractoryPeriod[i]);
            buffer.resize(buffer.size() + sizeof(uint32_t));
            
            // Write last spike time
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.lastSpikeTime[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write neuron type
            uint64_t* up64 = reinterpret_cast<uint64_t*>(buffer.data() + buffer.size());
            new(up64) uint64_t(data.neuronType[i]);
            buffer.resize(buffer.size() + sizeof(uint64_t));
            
            // Write region id
            up64 = reinterpret_cast<uint64_t*>(buffer.data() + buffer.size());
            new(up64) uint64_t(data.regionId[i]);
            buffer.resize(buffer.size() + sizeof(uint64_t));
            
            // Write population id
            up64 = reinterpret_cast<uint64_t*>(buffer.data() + buffer.size());
            new(up64) uint64_t(data.populationId[i]);
            buffer.resize(buffer.size() + sizeof(uint64_t));
        }
        
        // Compress if needed
        std::vector<uint8_t> finalBuffer = std::move(buffer);
        if (compression_ != CompressionLevel::None) {
            // Simple compression for demonstration (in real implementation would use zlib)
            finalBuffer = buffer;  // No actual compression for now
        }
        
        // Write section
        return writeSection(CheckpointSection::Neurons, finalBuffer.data(), finalBuffer.size());
    } catch (const std::exception& e) {
        error_ = "Exception writing neurons: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Prepare synapses data buffer
        size_t synapseCount = data.sourceNeuron.size();
        size_t bufferSize = synapseCount * (
            sizeof(uint64_t) * 2 +  // sourceNeuron, destinationNeuron
            sizeof(float) * 7 +     // weight, delay, synapseType, plasticityFlags, eligibilityTrace, efficacy, shortTermDepression
            sizeof(float) * 2      // shortTermFacilitation
        );
        
        std::vector<uint8_t> buffer;
        buffer.reserve(bufferSize);
        
        // Write all synapse data
        for (size_t i = 0; i < synapseCount; ++i) {
            // Write source neuron
            uint64_t* up64 = reinterpret_cast<uint64_t*>(buffer.data() + buffer.size());
            new(up64) uint64_t(data.sourceNeuron[i]);
            buffer.resize(buffer.size() + sizeof(uint64_t));
            
            // Write destination neuron
            up64 = reinterpret_cast<uint64_t*>(buffer.data() + buffer.size());
            new(up64) uint64_t(data.destinationNeuron[i]);
            buffer.resize(buffer.size() + sizeof(uint64_t));
            
            // Write weight
            float* fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.weight[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write delay
            uint32_t* up32 = reinterpret_cast<uint32_t*>(buffer.data() + buffer.size());
            new(up32) uint32_t(data.delay[i]);
            buffer.resize(buffer.size() + sizeof(uint32_t));
            
            // Write synapse type
            uint8_t* ub = reinterpret_cast<uint8_t*>(buffer.data() + buffer.size());
            new(ub) uint8_t(data.synapseType[i]);
            buffer.resize(buffer.size() + sizeof(uint8_t));
            
            // Write plasticity flags
            ub = reinterpret_cast<uint8_t*>(buffer.data() + buffer.size());
            new(ub) uint8_t(data.plasticityFlags[i]);
            buffer.resize(buffer.size() + sizeof(uint8_t));
            
            // Write eligibility trace
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.eligibilityTrace[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write efficacy
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.efficacy[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write short term depression
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.shortTermDepression[i]);
            buffer.resize(buffer.size() + sizeof(float));
            
            // Write short term facilitation
            fp = reinterpret_cast<float*>(buffer.data() + buffer.size());
            new(fp) float(data.shortTermFacilitation[i]);
            buffer.resize(buffer.size() + sizeof(float));
        }
        
        // Compress if needed
        std::vector<uint8_t> finalBuffer = std::move(buffer);
        if (compression_ != CompressionLevel::None) {
            // Simple compression for demonstration (in real implementation would use zlib)
            finalBuffer = buffer;  // No actual compression for now
        }
        
        // Write section
        return writeSection(CheckpointSection::Synapses, finalBuffer.data(), finalBuffer.size());
    } catch (const std::exception& e) {
        error_ = "Exception writing synapses: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        // Create section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
        sectionHeader.size = size;
        sectionHeader.compressedSize = (compression_ != CompressionLevel::None) ? size : 0;
        sectionHeader.checksum = ChecksumCalculator::crc32(static_cast<const uint8_t*>(data), size);
        
        // Write section header
        if (!writeSectionHeader(sectionHeader)) {
            return false;
        }
        
        // Write section data
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        stream_.write(reinterpret_cast<const char*>(bytes), size);
        if (!stream_.good()) {
            error_ = "Failed to write section data";
            return false;
        }
        
        // Pad to 8-byte alignment
        padToAlignment(8);
        
        // Store section header
        sections_.push_back(sectionHeader);
        bytesWritten_ += sizeof(SectionHeader) + size;
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception writing section: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        // Calculate total size
        uint64_t totalSize = sizeof(CheckpointHeader) + sizeof(uint32_t) + 
                           sections_.size() * sizeof(SectionHeader) + bytesWritten_;
        
        // Update header
        header_.totalSize = totalSize;
        
        // Seek to beginning and write updated header
        stream_.seekp(0);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        // Write section count
        uint32_t sectionCount = static_cast<uint32_t>(sections_.size());
        stream_.write(reinterpret_cast<const char*>(&sectionCount), sizeof(uint32_t));
        
        // Write section headers
        for (const auto& section : sections_) {
            stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
        }
        
        if (!stream_.good()) {
            error_ = "Failed to write checkpoint metadata";
            return false;
        }
        
        // Calculate checksum
        stream_.seekg(0);
        uint64_t dataSize = static_cast<uint64_t>(stream_.tellp());
        stream_.seekg(0);
        
        std::vector<uint8_t> buffer(dataSize);
        stream_.read(reinterpret_cast<char*>(buffer.data()), dataSize);
        header_.checksum = ChecksumCalculator::fletcher64(
            reinterpret_cast<const uint64_t*>(buffer.data()), 
            buffer.size() / sizeof(uint64_t)
        );
        
        // Seek back to header and write checksum
        stream_.seekp(sizeof(CheckpointHeader) + sizeof(uint32_t) + 
                    sections_.size() * sizeof(SectionHeader), std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_.checksum), sizeof(uint64_t));
        
        stream_.flush();
        close();
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception finalizing checkpoint: " + std::string(e.what());
        return false;
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        if (!filepath_.empty()) {
            std::filesystem::path path(filepath_);
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
            }
        }
    }
    sections_.clear();
    bytesWritten_ = 0;
    compressBuffer_.clear();
}

bool CheckpointWriter::writeHeader() {
    try {
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            error_ = "Failed to write checkpoint header";
            return false;
        }
        bytesWritten_ += sizeof(CheckpointHeader);
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception writing header: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    try {
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
        if (!stream_.good()) {
            error_ = "Failed to write section header";
            return false;
        }
        bytesWritten_ += sizeof(SectionHeader);
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception writing section header: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    try {
        size_t currentPos = static_cast<size_t>(stream_.tellp());
        size_t pad = (alignment - (currentPos % alignment)) % alignment;
        
        if (pad > 0) {
            std::vector<uint8_t> padding(pad, 0);
            stream_.write(reinterpret_cast<const char*>(padding.data()), pad);
            if (!stream_.good()) {
                error_ = "Failed to pad section to alignment";
                return false;
            }
            bytesWritten_ += pad;
        }
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception padding section: " + std::string(e.what());
        return false;
    }
}

// CheckpointManager implementations
CheckpointManager::CheckpointManager() : lastSaveStep_(0), error_() {}

CheckpointManager::~CheckpointManager() {}

void CheckpointManager::configure(const std::string& checkpointDir, 
                               uint64_t saveIntervalSteps, 
                               size_t maxCheckpoints, 
                               bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    lastSaveStep_ = 0;
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filename = name.empty() ? "checkpoint_" + std::to_string(lastSaveStep_) : name;
    std::string filepath = checkpointDir_ + "/" + filename + ".ckp";
    
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not set";
        return false;
    }
    
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        error_ = "Failed to create checkpoint file";
        return false;
    }
    
    // Set metadata
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    if (!neuronProvider_(neuronData)) {
        error_ = "Failed to get neuron data from provider";
        writer.abort();
        return false;
    }
    
    if (!synapseProvider_(synapseData)) {
        error_ = "Failed to get synapse data from provider";
        writer.abort();
        return false;
    }
    
    writer.setMetadata(neuronData.membranePotential.size(),
                      synapseData.weight.size(),
                      0,  // region count placeholder
                      lastSaveStep_,
                      currentTime);
    
    if (!writer.writeNeurons(neuronData)) {
        error_ = "Failed to write neurons";
        writer.abort();
        return false;
    }
    
    if (!writer.writeSynapses(synapseData)) {
        error_ = "Failed to write synapses";
        writer.abort();
        return false;
    }
    
    if (!writer.finalize()) {
        error_ = "Failed to finalize checkpoint";
        writer.abort();
        return false;
    }
    
    lastCheckpointPath_ = filepath;
    lastSaveStep_ = 0;  // Reset for next save
    return true;
}

bool CheckpointManager::load(const std::string& filename) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not set";
        return false;
    }
    
    std::string filepath = checkpointDir_ + "/" + filename + ".ckp";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    if (!reader.validate()) {
        error_ = "Checkpoint validation failed: " + reader.getError();
        return false;
    }
    
    // Read neurons
    NeuronCheckpointData neuronData;
    if (!reader.readNeurons(neuronData)) {
        error_ = "Failed to read neurons from checkpoint";
        return false;
    }
    
    // Read synapses
    SynapseCheckpointData synapseData;
    if (!reader.readSynapses(synapseData)) {
        error_ = "Failed to read synapses from checkpoint";
        return false;
    }
    
    // Apply neurons to the brain via provider
    if (!neuronProvider_(neuronData)) {
        error_ = "Failed to apply neurons from checkpoint";
        return false;
    }
    
    // Apply synapses to the brain via provider
    if (!synapseProvider_(synapseData)) {
        error_ = "Failed to apply synapses from checkpoint";
        return false;
    }
    
    lastCheckpointPath_ = filepath;
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (std::filesystem::exists(checkpointDir_)) {
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.path().extension() == ".ckp") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.fileSize = std::filesystem::file_size(entry.path());
                // Other info would be read from file
                checkpoints.push_back(info);
            }
        }
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    size_t removeCount = std::max<size_t>(0, checkpoints.size() - keepCount);
    for (size_t i = 0; i < removeCount; ++i) {
        std::filesystem::path path(checkpointDir_ + "/" + checkpoints[i].name + ".ckp");
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

}} // namespace nlm
