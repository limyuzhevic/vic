#pragma once

/**
 * Phase6TextDemo - Phase 6 text teaching and thought demonstration
 * 
 * This is NOT the old Phase 6 integration demo (nlm_phase6_demo, unchanged).
 * It proves the four Phase 6 claims with numbers:
 * 
 *   1. TEXT AS A SENSE. A teacher word is never given to the brain
 *      as a string: AgentBrain::encodeText turns it into a sparse
 *      pattern on dedicated sensory neurons. The probe below
 *      teaches "apple" by presenting it TOGETHER with a bright
 *      resource scene, then asks whether text alone now recalls
 *      that scene in the brain's own internal pattern — measured
 *      as cosine similarity between difference vectors (pattern
 *      minus the empty-scene baseline, so shared vision drive
 *      cannot fake an association). An untrained brain must show
 *      no association; a taught brain must; an untaught control
 *      word ("zebra") must stay unassociated. Nothing about
 *      "apple" is pre-taught in code: the only place the string
 *      appears is the encoder and this demo's teacher.
 *   2. PLANNER IN THE LOOP. A sandbox run reports planner
 *      overrides (planner bias actually changing the action) and
 *      planner agreement, not just an advisory trace.
 *   3. CONCEPTS, SELF-MODEL, SOCIAL LEARNING. Concepts are
 *      formed from experienced pattern/reward pairings and bias
 *      action; the self-model records every loop turn and reports
 *      confidence; a teacher demonstration is observed and then
 *      imitated in a matching state.
 *   4. PACING. The sandbox loop advances the world by one 0.1 s
 *      action interval per decision, so tooSoon must be 0 —
 *      every decoded action actually reaches the world.
 * 
 * This demo does NOT claim a human brain, language understanding,
 * or a real personality. Text here is a sensory channel, not
 * grammar, meaning, or a pretrained language model.
 */

namespace nlm {

/**
 * Result structure for Phase 6 text/demo demonstration
 */
struct Phase6TextDemoResult {
    bool textAssociationLearned;
    bool differentialAssociation;
    bool conceptsFormed;
    bool socialLearningObserved;
    float finalNoveltyLevel;
    float finalCuriosityLevel;
    float finalEnergy;
    float plannerOverrideCount;
    float plannerAgreementCount;
    float totalReward;
    float avgFiringRate;
    
    Phase6TextDemoResult()
        : textAssociationLearned(false)
        , differentialAssociation(false)
        , conceptsFormed(false)
        , socialLearningObserved(false)
        , finalNoveltyLevel(0.0f)
        , finalCuriosityLevel(0.0f)
        , finalEnergy(0.0f)
        , plannerOverrideCount(0.0f)
        , plannerAgreementCount(0.0f)
        , totalReward(0.0f)
        , avgFiringRate(0.0f) {}
};

/**
 * Phase6TextDemo - Main class for Phase 6 text teaching demonstration
 */
class Phase6TextDemo {
public:
    Phase6TextDemo();
    ~Phase6TextDemo();
    
    Phase6TextDemoResult run();
    
private:
    void runTextAssociationTest();
    void runConceptFormationTest();
    void runSocialLearningTest();
    void runSandboxTest();
    
    // Test results
    bool textLearned;
    bool specific;
    std::shared_ptr<Brain> demoBrain;
    std::shared_ptr<Config> demoConfig;
};

} // namespace nlm