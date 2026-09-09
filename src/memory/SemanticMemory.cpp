#pragma once

namespace nlm {

// SemanticMemory Implementation
class SemanticMemoryImpl {
public:
    std::vector<std::pair<std::string, std::string>> facts;
    
    SemanticMemoryImpl() {}
};

} // namespace nlm
