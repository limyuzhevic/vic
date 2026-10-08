namespace nlm {

class MotorSystem {
public:
    MotorSystem(std::shared_ptr<Config> config) {}
    ~MotorSystem() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

class Action {
public:
    Action() {}
    ~Action() = default;
};

} // namespace nlm