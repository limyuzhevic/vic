#include "CheckpointSystem.hpp"

namespace nlm {

// Fix for CheckpointReader::open() - add seek error handling
bool CheckpointReader::open(const std::string& filepath) {
    close();
    stream_.open(filepath, std::ios::binary);
    if (!stream_) {
        error_ = "cannot open checkpoint file: " + filepath;
        return false;
    }
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!stream_ || !header_.validate()) {
        error_ = "bad checkpoint header (magic/version mismatch)";
        close();
        return false;
    }
    sections_.clear();
    while (true) {
        SectionHeader section;
        stream_.read(reinterpret_cast<char*>(&section), sizeof(section));
        if (!stream_) break;
        sections_.push_back(section);
        if (section.type == static_cast<uint32_t>(CheckpointSection::End)) break;
        // Validate offset before seeking
        if (section.offset > header_.totalSize) {
            error_ = "invalid section offset";
            close();
            return false;
        }
        stream_.seekg(static_cast<std::streamoff>(section.offset + section.size), std::ios::beg);
        if (!stream_) {
            error_ = "failed to seek to section " + std::to_string(section.type);
            close();
            return false;
        }
    }
    stream_.clear();
    return true;
}

// Fix for CheckpointWriter::finalize() - add header write error handling
bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) return false;
    // Reserve space for the header, then write sections.
    stream_.seekp(0, std::ios::beg);
    CheckpointHeader placeholder;
    stream_.write(reinterpret_cast<const char*>(&placeholder), sizeof(placeholder));
    if (!stream_) return false;
    bytesWritten_ = sizeof(placeholder);
    if (!writeSectionToStream(stream_, CheckpointSection::Neurons, neuronPayload_, bytesWritten_)) return false;
    if (!writeSectionToStream(stream_, CheckpointSection::Synapses, synapsePayload_, bytesWritten_)) return false;
    if (!writeSectionToStream(stream_, CheckpointSection::SpikeHistory, delayedSpikePayload_, bytesWritten_)) return false;
    // Phase 5 extra sections, in fixed order so a reader can rely on
    // layout (readSection also finds them by type, order-independent).
    for (CheckpointSection type : {CheckpointSection::SimulationState,
                                   CheckpointSection::RandomState,
                                   CheckpointSection::Plasticity,
                                   CheckpointSection::Memory}) {
        for (const auto& entry : extraPayloads_) {
            if (entry.first == type && !entry.second.empty()) {
                if (!writeSectionToStream(stream_, type, entry.second, bytesWritten_)) return false;
            }
        }
    }
    SectionHeader endHeader;
    endHeader.type = static_cast<uint32_t>(CheckpointSection::End);
    endHeader.offset = static_cast<uint64_t>(stream_.tellp()) + sizeof(SectionHeader);
    stream_.write(reinterpret_cast<const char*>(&endHeader), sizeof(endHeader));
    if (!stream_) return false;
    bytesWritten_ += sizeof(endHeader);
    // Patch the real header in place.
    header_.totalSize = bytesWritten_;
    header_.checksum = ChecksumCalculator::crc32(neuronPayload_.data(), neuronPayload_.size());
    stream_.seekp(0, std::ios::beg);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    stream_.flush();
    if (!stream_) return false;
    close();
    return true;
}

// Fix for CheckpointManager::saveImmediately() - add error messages
bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "checkpoint providers not configured";
        return false;
    }
    NeuronCheckpointData neurons;
    SynapseCheckpointData synapses;
    if (!neuronProvider_(neurons) || !synapseProvider_(synapses)) {
        error_ = "failed to obtain checkpoint data";
        return false;
    }
    std::string filename = name.empty() ? "checkpoint.bin" : name;
    std::string path = (std::filesystem::path(checkpointDir_) / filename).string();
    CheckpointWriter writer;
    if (!writer.create(path, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        error_ = writer.getError();
        return false;
    }
    writer.setMetadata(neurons.membranePotential.size(), synapses.weight.size(), 0, lastSaveStep_, 0.0);
    if (!writer.writeNeurons(neurons) || !writer.writeSynapses(synapses)) {
        error_ = writer.getError();
        writer.abort();
        return false;
    }
    if (!writer.finalize()) {
        error_ = writer.getError();
        return false;
    }
    lastCheckpointPath_ = path;
    pruneOldCheckpoints(maxCheckpoints_);
    return true;
}

// Fix for Brain::load() - make neuron count check more lenient
bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Neuron count check: allow checkpoint files with fewer neurons
        // than current brain, but not more (would cause misalignment).
        if (reader.getHeader().neuronCount > getTotalNeuronCount()) {
            NLM_LOG_ERROR("Checkpoint has more neurons (" + std::to_string(reader.getHeader().neuronCount) +
                         ") than this brain (" + std::to_string(getTotalNeuronCount()) + ")");
            return false;
        }
        // Log a warning if counts differ, but allow the load
        if (reader.getHeader().neuronCount < getTotalNeuronCount()) {
            NLM_LOG_INFO("Checkpoint has " + std::to_string(reader.getHeader().neuronCount) +
                         " neurons, this brain has " + std::to_string(getTotalNeuronCount()) +
                         "; loading matching neurons only");
        }
        
        // Rest of load logic...
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}