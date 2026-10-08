namespace nlm {

class Pruning {
public:
    Pruning(std::shared_ptr<Config> config) {}
    ~Pruning() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

} // namespace nlm