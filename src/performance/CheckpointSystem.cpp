#include "CheckpointSystem.hpp"

namespace nlm {

// Checkpoint support is declared in CheckpointSystem.hpp but had no
// implementation in the repo. These stubs keep save/load honest:
// create/open return false, so Brain::save/load correctly report failure
// instead of linking against missing symbols.

CheckpointReader::CheckpointReader() = default;
CheckpointReader::~CheckpointReader() = default;
bool CheckpointReader::open(const std::string&) { return false; }
void CheckpointReader::close() {}
bool CheckpointReader::readNeurons(NeuronCheckpointData&) { return false; }
bool CheckpointReader::readSynapses(SynapseCheckpointData&) { return false; }
std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection) { return {}; }
bool CheckpointReader::validate() const { return false; }
double CheckpointReader::getProgress() const { return 0.0; }

CheckpointWriter::CheckpointWriter() = default;
CheckpointWriter::~CheckpointWriter() = default;
bool CheckpointWriter::create(const std::string&, CompressionLevel) { return false; }
void CheckpointWriter::close() {}
void CheckpointWriter::setMetadata(uint64_t, uint64_t, uint64_t, uint64_t, double) {}
bool CheckpointWriter::writeNeurons(const NeuronCheckpointData&) { return false; }
bool CheckpointWriter::writeSynapses(const SynapseCheckpointData&) { return false; }
bool CheckpointWriter::writeSection(CheckpointSection, const void*, size_t) { return false; }
bool CheckpointWriter::finalize() { return false; }
void CheckpointWriter::abort() {}
bool CheckpointWriter::writeHeader() { return false; }
bool CheckpointWriter::writeSectionHeader(const SectionHeader&) { return false; }
bool CheckpointWriter::padToAlignment(size_t) { return false; }

CheckpointManager::CheckpointManager() = default;
CheckpointManager::~CheckpointManager() = default;
void CheckpointManager::configure(const std::string&, uint64_t, size_t, bool) {}
bool CheckpointManager::update(uint64_t, double) { return false; }
bool CheckpointManager::saveImmediately(const std::string&) { return false; }
bool CheckpointManager::load(const std::string&) { return false; }
std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const { return {}; }
void CheckpointManager::pruneOldCheckpoints(size_t) {}
std::string CheckpointManager::getLastCheckpointPath() const { return {}; }
bool CheckpointManager::shouldSave(uint64_t) const { return false; }

} // namespace nlm
