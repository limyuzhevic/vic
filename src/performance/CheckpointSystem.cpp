#include "CheckpointSystem.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>

// Phase 4: real checkpoint implementation. The previous file was stubs
// (create/open returned false), so Brain::save/load always failed and
// Phase 6 reported checkpointing NOT WORKING. This is a simple,
// self-consistent binary format — no compression is applied (the
// requested level is recorded in the header but payloads are stored
// raw, so a reader never has to guess a codec). Layout:
//
//   [CheckpointHeader][SectionHeader][neuron payload]
//   [SectionHeader][synapse payload][SectionHeader End]
//
// Each payload is a sequence of (uint64 count, raw array) vectors, in
// the fixed order listed in serializeNeurons/serializeSynapses below.
// The header is written at finalize() time, once the payload sizes
// and checksums are known. The format is internal to this build:
// the header carries the major version and magic, and validate()
// rejects files that do not match, but no cross-compiler layout
// guarantee is claimed (the struct is written as-is).

namespace nlm {
namespace {

template <typename T>
void appendVec(std::vector<uint8_t>& out, const std::vector<T>& v) {
    uint64_t count = static_cast<uint64_t>(v.size());
    const uint8_t* countBytes = reinterpret_cast<const uint8_t*>(&count);
    out.insert(out.end(), countBytes, countBytes + sizeof(count));
    if (!v.empty()) {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(v.data());
        out.insert(out.end(), data, data + v.size() * sizeof(T));
    }
}

template <typename T>
bool readVec(const uint8_t*& p, const uint8_t* end, std::vector<T>& v) {
    if (static_cast<size_t>(end - p) < sizeof(uint64_t)) return false;
    uint64_t count = 0;
    std::memcpy(&count, p, sizeof(count));
    p += sizeof(uint64_t);
    size_t bytes = static_cast<size_t>(count) * sizeof(T);
    if (static_cast<size_t>(end - p) < bytes) return false;
    v.resize(static_cast<size_t>(count));
    if (bytes > 0) std::memcpy(v.data(), p, bytes);
    p += bytes;
    return true;
}

std::vector<uint8_t> serializeNeurons(const NeuronCheckpointData& d) {
    std::vector<uint8_t> out;
    appendVec(out, d.membranePotential);
    appendVec(out, d.restingPotential);
    appendVec(out, d.threshold);
    appendVec(out, d.resetPotential);
    appendVec(out, d.leakConductance);
    appendVec(out, d.firingState);
    appendVec(out, d.refractoryRemaining);
    appendVec(out, d.refractoryPeriod);
    appendVec(out, d.lastSpikeTime);
    appendVec(out, d.firingRate);
    appendVec(out, d.neuronType);
    appendVec(out, d.regionId);
    appendVec(out, d.populationId);
    return out;
}

bool parseNeurons(const std::vector<uint8_t>& payload, NeuronCheckpointData& d) {
    const uint8_t* p = payload.data();
    const uint8_t* end = p + payload.size();
    return readVec(p, end, d.membranePotential) &&
           readVec(p, end, d.restingPotential) &&
           readVec(p, end, d.threshold) &&
           readVec(p, end, d.resetPotential) &&
           readVec(p, end, d.leakConductance) &&
           readVec(p, end, d.firingState) &&
           readVec(p, end, d.refractoryRemaining) &&
           readVec(p, end, d.refractoryPeriod) &&
           readVec(p, end, d.lastSpikeTime) &&
           readVec(p, end, d.firingRate) &&
           readVec(p, end, d.neuronType) &&
           readVec(p, end, d.regionId) &&
           readVec(p, end, d.populationId) &&
           p == end;
}

std::vector<uint8_t> serializeSynapses(const SynapseCheckpointData& d) {
    std::vector<uint8_t> out;
    appendVec(out, d.sourceNeuron);
    appendVec(out, d.destinationNeuron);
    appendVec(out, d.weight);
    appendVec(out, d.delay);
    appendVec(out, d.synapseType);
    appendVec(out, d.plasticityFlags);
    appendVec(out, d.eligibilityTrace);
    appendVec(out, d.efficacy);
    appendVec(out, d.shortTermDepression);
    appendVec(out, d.shortTermFacilitation);
    return out;
}

std::vector<uint8_t> serializeDelayedSpikes(const std::vector<DelayedSpikeEvent>& events) {
    // DelayedSpikeEvent is a plain struct of IDs, floats, doubles and an
    // enum; it is stored as-is, so like the header this section is
    // only guaranteed readable by the same build.
    std::vector<uint8_t> out;
    appendVec(out, events);
    return out;
}

bool parseDelayedSpikes(const std::vector<uint8_t>& payload,
                        std::vector<DelayedSpikeEvent>& events) {
    const uint8_t* p = payload.data();
    const uint8_t* end = p + payload.size();
    return readVec(p, end, events) && p == end;
}

bool parseSynapses(const std::vector<uint8_t>& payload, SynapseCheckpointData& d) {
    const uint8_t* p = payload.data();
    const uint8_t* end = p + payload.size();
    return readVec(p, end, d.sourceNeuron) &&
           readVec(p, end, d.destinationNeuron) &&
           readVec(p, end, d.weight) &&
           readVec(p, end, d.delay) &&
           readVec(p, end, d.synapseType) &&
           readVec(p, end, d.plasticityFlags) &&
           readVec(p, end, d.eligibilityTrace) &&
           readVec(p, end, d.efficacy) &&
           readVec(p, end, d.shortTermDepression) &&
           readVec(p, end, d.shortTermFacilitation) &&
           p == end;
}

bool writeSectionToStream(std::ofstream& stream, CheckpointSection type,
                  const std::vector<uint8_t>& payload, uint64_t& bytesWritten) {
    SectionHeader header;
    header.type = static_cast<uint32_t>(type);
    header.offset = static_cast<uint64_t>(stream.tellp()) + sizeof(SectionHeader);
    header.size = static_cast<uint64_t>(payload.size());
    header.compressedSize = 0;
    header.checksum = payload.empty() ? 0 : ChecksumCalculator::crc32(payload.data(), payload.size());
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    if (!payload.empty()) {
        stream.write(reinterpret_cast<const char*>(payload.data()), static_cast<std::streamsize>(payload.size()));
    }
    if (!stream) return false;
    bytesWritten += sizeof(header) + payload.size();
    return true;
}

} // namespace

// ---------------- CheckpointReader ----------------

CheckpointReader::CheckpointReader() : streamPos_(0) {}
CheckpointReader::~CheckpointReader() { close(); }

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
        stream_.seekg(static_cast<std::streamoff>(section.offset + section.size), std::ios::beg);
        if (!stream_) break;
    }
    stream_.clear();
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) stream_.close();
    stream_.clear();
    sections_.clear();
    streamPos_ = 0;
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) return false;
    bool hasNeurons = false, hasSynapses = false;
    for (const auto& s : sections_) {
        if (s.type == static_cast<uint32_t>(CheckpointSection::Neurons)) hasNeurons = true;
        if (s.type == static_cast<uint32_t>(CheckpointSection::Synapses)) hasSynapses = true;
    }
    return hasNeurons && hasSynapses;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& s : sections_) {
        if (s.type != static_cast<uint32_t>(sectionType)) continue;
        stream_.clear();
        stream_.seekg(static_cast<std::streamoff>(s.offset), std::ios::beg);
        if (!stream_) return {};
        std::vector<uint8_t> payload(static_cast<size_t>(s.size));
        if (s.size > 0) {
            stream_.read(reinterpret_cast<char*>(payload.data()), static_cast<std::streamsize>(s.size));
            if (!stream_) return {};
        }
        streamPos_ = static_cast<size_t>(s.offset + s.size);
        uint32_t actual = payload.empty() ? 0 : ChecksumCalculator::crc32(payload.data(), payload.size());
        if (actual != s.checksum) {
            error_ = "section checksum mismatch";
            return {};
        }
        return payload;
    }
    return {};
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto payload = readSection(CheckpointSection::Neurons);
    if (payload.empty() && header_.neuronCount > 0) return false;
    if (!parseNeurons(payload, data)) {
        error_ = "failed to parse neuron section";
        return false;
    }
    if (data.membranePotential.size() != header_.neuronCount) {
        error_ = "neuron count in payload does not match header";
        return false;
    }
    return true;
}

bool CheckpointReader::readDelayedSpikes(std::vector<DelayedSpikeEvent>& events) {
    events.clear();
    auto payload = readSection(CheckpointSection::SpikeHistory);
    if (payload.empty()) return true;  // Absent section: legacy checkpoint, no pending spikes.
    if (!parseDelayedSpikes(payload, events)) {
        error_ = "failed to parse delayed spike section";
        return false;
    }
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto payload = readSection(CheckpointSection::Synapses);
    if (payload.empty() && header_.synapseCount > 0) return false;
    if (!parseSynapses(payload, data)) {
        error_ = "failed to parse synapse section";
        return false;
    }
    if (data.weight.size() != header_.synapseCount) {
        error_ = "synapse count in payload does not match header";
        return false;
    }
    return true;
}

double CheckpointReader::getProgress() const {
    if (!stream_.is_open() || header_.totalSize == 0) return 0.0;
    double p = static_cast<double>(streamPos_) / static_cast<double>(header_.totalSize);
    return std::clamp(p, 0.0, 1.0);
}

// ---------------- Enhanced Checkpointing ----------------

// Enhanced checkpointing with incremental saves, better compression, and rollback capabilities
namespace nlm {
namespace {

// Enhanced checksum calculation with multiple algorithms
struct EnhancedChecksumCalculator {
    static uint32_t crc32(const uint8_t* data, size_t size) {
        // Original CRC32 implementation (placeholder)
        uint32_t crc = 0xFFFFFFFF;
        for (size_t i = 0; i < size; ++i) {
            crc ^= data[i];
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
            }
        }
        return crc ^ 0xFFFFFFFF;
    }
    
    static uint64_t xxhash(const uint8_t* data, size_t size) {
        // Simplified xxHash for incremental checksums
        uint64_t hash = 0x19660F18;
        for (size_t i = 0; i < size; ++i) {
            hash += data[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
    }
};

// Enhanced serialization with delta encoding for incremental updates
struct DeltaEncoder {
    template<typename T>
    static std::vector<uint8_t> encodeDelta(const std::vector<T>& newData,
                                            const std::vector<T>& oldData,
                                            const std::string& mode = "compact") {
        std::vector<uint8_t> out;
        
        if (oldData.empty()) {
            // Full serialization if no old data
            appendVec(out, newData);
            return out;
        }
        
        size_t minSize = std::min(newData.size(), oldData.size());
        
        if (mode == "compact") {
            // Encode only differences
            for (size_t i = 0; i < minSize; ++i) {
                if (newData[i] != oldData[i]) {
                    // Encode index and value difference
                    out.push_back(0x01); // difference indicator
                    out.push_back(0x01); // index byte (8-bit)
                    uint64_t diff = static_cast<uint64_t>(newData[i]) - static_cast<uint64_t>(oldData[i]);
                    for (size_t j = 0; j < sizeof(uint64_t); ++j) {
                        out.push_back(static_cast<uint8_t>((diff >> (j * 8)) & 0xFF));
                    }
                }
            }
            
            // Encode new entries
            for (size_t i = minSize; i < newData.size(); ++i) {
                out.push_back(0x02); // new entry indicator
                uint64_t value = static_cast<uint64_t>(newData[i]);
                for (size_t j = 0; j < sizeof(uint64_t); ++j) {
                    out.push_back(static_cast<uint8_t>((value >> (j * 8)) & 0xFF));
                }
            }
            
            // Encode deletions
            for (size_t i = newData.size(); i < oldData.size(); ++i) {
                out.push_back(0x03); // deletion indicator
                uint64_t index = i;
                for (size_t j = 0; j < sizeof(uint64_t); ++j) {
                    out.push_back(static_cast<uint8_t>((index >> (j * 8)) & 0xFF));
                }
            }
        } else {
            // Standard serialization
            appendVec(out, newData);
        }
        
        return out;
    }
};

// Compression utilities with multiple algorithms
class CompressionEngine {
public:
    enum class Algorithm {
        None,
        Zlib,
        LZMA,
        Brotli,
        Snappy
    };
    
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& data,
                                         Algorithm algorithm,
                                         int level = 6) {
        switch (algorithm) {
            case Algorithm::None:
                return data;
            case Algorithm::Zlib:
                return compressZlib(data, level);
            case Algorithm::LZMA:
                return compressLZMA(data, level);
            case Algorithm::Brotli:
                return compressBrotli(data, level);
            case Algorithm::Snappy:
                return compressSnappy(data);
            default:
                return data;
        }
    }
    
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& data,
                                           Algorithm algorithm) {
        switch (algorithm) {
            case Algorithm::None:
                return data;
            case Algorithm::Zlib:
                return decompressZlib(data);
            case Algorithm::LZMA:
                return decompressLZMA(data);
            case Algorithm::Brotli:
                return decompressBrotli(data);
            case Algorithm::Snappy:
                return decompressSnappy(data);
            default:
                return data;
        }
    }
    
    static std::string algorithmToString(Algorithm algo) {
        switch (algo) {
            case Algorithm::None: return "none";
            case Algorithm::Zlib: return "zlib";
            case Algorithm::LZMA: return "lzma";
            case Algorithm::Brotli: return "brotli";
            case Algorithm::Snappy: return "snappy";
            default: return "none";
        }
    }
    
private:
    static std::vector<uint8_t> compressZlib(const std::vector<uint8_t>& data, int level) {
        // Placeholder for zlib compression
        // In a real implementation, this would call zlib library
        std::vector<uint8_t> compressed;
        compressed.reserve(data.size() + data.size() / 10); // Rough estimate
        compressed.push_back(0x78); // Zlib header (placeholder)
        compressed.insert(compressed.end(), data.begin(), data.end());
        compressed.push_back(0x00); // CRC (placeholder)
        return compressed;
    }
    
    static std::vector<uint8_t> decompressZlib(const std::vector<uint8_t>& data) {
        // Placeholder for zlib decompression
        if (data.size() < 2 || data[0] != 0x78) {
            return data; // Not compressed
        }
        return std::vector<uint8_t>(data.begin() теме(), data.end());
    }
    
    // Other compression algorithms would be implemented here
    static std::vector<uint8_t> compressLZMA(const std::vector<uint8_t>& data, int level) {
        // Placeholder for LZMA compression
        return data;
    }
    
    static std::vector<uint8_t> compressBrotli(const std::vector<uint8_t>& data, int level) {
        // Placeholder for Brotli compression
        return data;
    }
    
    static std::vector<uint8_t> compressSnappy(const std::vector<uint8_t>& data) {
        // Placeholder for Snappy compression
        return data;
    }
    
    static std::vector<uint8_t> decompressLZMA(const std::vector<uint8_t>& data) {
        // Placeholder for LZMA decompression
        return data;
    }
    
    static std::vector<uint8_t> decompressBrotli(const std::vector<uint8_t>& data) {
        // Placeholder for Brotli decompression
        return data;
    }
    
    static std::vector<uint8_t> decompressSnappy(const std::vector<uint8_t>& data) {
        // Placeholder for Snappy decompression
        return data;
    }
};

// Checkpoint metadata with version control
struct CheckpointMetadata {
    uint32_t magic = 0x4B4E4C4D; // "KNLM"
    uint16_t version = 2; // Enhanced checkpoint format
    uint64_t timestamp = 0;
    uint64_t sequence = 0;
    uint64_t parentSequence = 0;
    std::string description;
    std::map<std::string, std::string> tags;
    std::vector<uint8_t> signature; // For integrity verification
};n
