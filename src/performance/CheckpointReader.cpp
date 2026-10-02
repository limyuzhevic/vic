#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cstring>

namespace nlm {

CheckpointReader::CheckpointReader()
    : streamPos_(0)
{
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    close();  // Close any existing file
    
    // Open file for reading
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        NLM_LOG_ERROR(error_);
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!stream_.good()) {
        error_ = "Failed to read checkpoint header";
        NLM_LOG_ERROR(error_);
        stream_.close();
        return false;
    }
    
    // Validate header
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header (wrong magic number or version)";
        NLM_LOG_ERROR(error_);
        stream_.close();
        return false;
    }
    
    // Read section headers
    size_t numSections = (header_.totalSize - sizeof(CheckpointHeader)) / sizeof(SectionHeader);
    sections_.resize(numSections);
    
    for (size_t i = 0; i < numSections; ++i) {
        uint64_t pos = sizeof(CheckpointHeader) + i * sizeof(SectionHeader);
        stream_.seekg(pos);
        stream_.read(reinterpret_cast<char*>(&sections_[i]), sizeof(SectionHeader));
        if (!stream_.good()) {
            error_ = "Failed to read section header at index " + std::to_string(i);
            NLM_LOG_ERROR(error_);
            stream_.close();
            return false;
        }
    }
    
    streamPos_ = stream_.tellg();
    NLM_LOG_INFO("Opened checkpoint file: " + filepath + " (version " + 
                 std::to_string(header_.majorVersion) + "." + 
                 std::to_string(header_.minorVersion) + "." + 
                 std::to_string(header_.patchVersion) + ")");
    
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
        stream_.clear();
    }
    
    sections_.clear();
    streamPos_ = 0;
    error_.clear();
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    return readSection(CheckpointSection::Neurons, data);
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    return readSection(CheckpointSection::Synapses, data);
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    // Find section
    auto it = std::find_if(sections_.begin(), sections_.end(),
                         [sectionType](const SectionHeader& header) {
                             return header.type == static_cast<uint32_t>(sectionType);
                         });
    
    if (it == sections_.end()) {
        error_ = "Section " + std::to_string(static_cast<uint32_t>(sectionType)) + " not found in checkpoint";
        NLM_LOG_ERROR(error_);
        return std::vector<uint8_t>();
    }
    
    const SectionHeader& section = *it;
    
    // Read section data
    std::vector<uint8_t> buffer(section.size);
    stream_.seekg(section.offset);
    stream_.read(reinterpret_cast<char*>(buffer.data()), section.size);
    
    if (!stream_.good()) {
        error_ = "Failed to read section data for " + std::to_string(static_cast<uint32_t>(sectionType));
        NLM_LOG_ERROR(error_);
        return std::vector<uint8_t>();
    }
    
    streamPos_ = stream_.tellg();
    return buffer;
}

bool CheckpointReader::validate() const {
    // Check if file is open
    if (!isOpen()) {
        error_ = "Checkpoint not open for validation";
        return false;
    }
    
    // Check if header is valid
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Check if total size matches file size
    stream_.seekg(0, std::ios::end);
    uint64_t fileSize = stream_.tellg();
    stream_.seekg(0);
    
    if (header_.totalSize != fileSize) {
        error_ = "Checkpoint total size mismatch (header says " + 
                 std::to_string(header_.totalSize) + ", file has " + 
                 std::to_string(fileSize) + " bytes)";
        return false;
    }
    
    // Check if all sections are within bounds
    uint64_t expectedEnd = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader);
    for (const auto& section : sections_) {
        if (section.offset < expectedEnd) {
            error_ = "Section offset before section headers";
            return false;
        }
        
        uint64_t sectionEnd = section.offset + section.size;
        if (sectionEnd > header_.totalSize) {
            error_ = "Section extends beyond checkpoint end (offset " + 
                     std::to_string(section.offset) + ", size " + 
                     std::to_string(section.size) + ", total " + 
                     std::to_string(header_.totalSize) + ")";
            return false;
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (!isOpen() || sections_.empty()) {
        return 0.0;
    }
    
    uint64_t currentPos = streamPos_;
    uint64_t startPos = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader);
    uint64_t endPos = header_.totalSize;
    
    if (endPos <= startPos) {
        return 1.0;
    }
    
    return static_cast<double>(currentPos - startPos) / static_cast<double>(endPos - startPos);
}

} // namespace nlm
