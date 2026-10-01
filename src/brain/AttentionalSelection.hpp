// Simple placeholder implementation for AttentionalSelection
// This is a minimal implementation to make integration tests compile

namespace nlm {

class AttentionalSelection {
public:
    AttentionalSelection() {}
    ~AttentionalSelection() {}
    void initialize(Brain* brain) {}
    size_t selectFocus(const std::vector<size_t>& candidates) {
        if (candidates.empty()) return 0;
        return candidates[0];
    }
};

} // namespace nlm