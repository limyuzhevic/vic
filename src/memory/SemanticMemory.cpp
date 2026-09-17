#include "SemanticMemory.hpp"

namespace nlm {

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {}
std::string SemanticMemory::retrieveFact(const std::string& key) const { return ""; }
bool SemanticMemory::hasFact(const std::string& key) const { return false; }
std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const { return std::vector<std::pair<std::string, std::string>>(); }
void SemanticMemory::clear() {}

} // namespace nlm
