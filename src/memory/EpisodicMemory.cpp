#include "EpisodicMemory.hpp"

namespace nlm {

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {}
EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const { return EpisodicMemoryItem(); }
size_t EpisodicMemory::getEpisodeCount() const { return 0; }
std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const { return std::vector<EpisodicMemoryItem>(); }
void EpisodicMemory::clear() {}
void EpisodicMemory::consolidate(float relevanceThreshold) {}

} // namespace nlm
