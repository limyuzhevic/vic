#include "ProceduralMemory.hpp"

namespace nlm {

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {}
Skill* ProceduralMemory::getSkill(const std::string& name) { return nullptr; }
std::vector<Skill*> ProceduralMemory::getAllSkills() { return std::vector<Skill*>(); }
void ProceduralMemory::updateProficiency(const std::string& name, float delta) {}
void ProceduralMemory::clear() {}

} // namespace nlm
