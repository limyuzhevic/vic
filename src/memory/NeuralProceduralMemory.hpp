// NeuralProceduralMemory: Stores skills, habits, and motor patterns
// NOT a script - procedural knowledge is encoded as neural motor patterns
//
// Key mechanisms:
// - Skills stored as motor patterns with strength
// - Procedural learning through reinforcement
// - Hierarchical skill organization (composition of primitives)
// - Motor adaptation through error correction

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn a skill
    // Skill is encoded as neural motor pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern, float proficiency = 1.0f);

    // Retrieve skill
    Skill* getSkill(const std::string& name);

    // Get all skills
    std::vector<Skill*> getAllSkills();

    // Update skill proficiency
    void updateProficiency(const std::string& name, float delta);

    // Execute skill
    // Activates the neural pattern for the skill
    void executeSkill(const std::string& name, float intensity = 1.0f);

    // Check if skill is learned
    bool hasSkill(const std::string& name) const;

    // Get skill proficiency
    float getProficiency(const std::string& name) const;

    // Clear skill
    void clearSkill(const std::string& name);

    // Clear all skills
    void clear();

    // Get number of skills
    size_t getSkillCount() const;

    // Learn from trial and error
    // Refines skill based on reward/punishment
    void learnFromTrial(const std::string& skillName, float outcome, float learningRate = 0.1f);

    // Compose skills
    // Combines two skills to create a new composite skill
    void composeSkills(const std::string& primary, const std::string& secondary, const std::string& composite);

    // Decompose skill
    // Breaks down complex skill into primitives
    std::vector<std::string> decomposeSkill(const std::string& name) const;

    // Get skill similarity
    float getSkillSimilarity(const std::string& skill1, const std::string& skill2) const;

    // Consolidate skills
    void consolidate(float relevanceThreshold);

    // Get skills by type
    std::vector<Skill*> getSkillsByType(const std::string& type) const;

    // Get motor patterns for skill
    std::vector<float> getMotorPattern(const std::string& name) const;

    // Add motor adaptation
    void addMotorAdaptation(const std::string& skillName, const std::vector<float>& adaptation);

    // Get motor memory
    float getMotorMemory(const std::string& skillName) const;

    // Check if motor pattern is ready
    bool isMotorPatternReady(const std::string& name) const;

private:
    // Find or create skill neuron
    NeuronId findOrCreateSkillNeuron(const std::string& name);

    // Create motor pattern neuron
    NeuronId createMotorPattern(const std::string& name, const std::vector<NeuronId>& pattern);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::unordered_map<std::string, float> skillProficiency_;
    std::unordered_map<std::string, std::vector<NeuronId>> skillMotorPatterns_;
    std::unordered_map<std::string, std::vector<float>> skillAdaptations_;
};
