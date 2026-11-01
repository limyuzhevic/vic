// Phase 6 demo — text teaching, planner in the loop, and the wired
// cognition systems.
//
// This is NOT the old Phase 6 integration demo (nlm_phase6_demo,
// unchanged). It proves the four Phase 6 claims with numbers:
//
//   1. TEXT AS A SENSE. A teacher word is never given to the brain
//      as a string: AgentBrain::encodeText turns it into a sparse
//      pattern on dedicated sensory neurons. The probe below
//      teaches "apple" by presenting it TOGETHER with a bright
//      resource scene, then asks whether text alone now recalls
//      that scene in the brain's own internal pattern — measured
//      as cosine similarity between difference vectors (pattern
//      minus the empty-scene baseline, so shared vision drive
//      cannot fake an association). An untrained brain must show
//      no association; a taught brain must; an untaught control
//      word ("zebra") must stay unassociated. Nothing about
//      "apple" is pre-taught in code: the only place the string
//      appears is the encoder and this demo's teacher.
//   2. PLANNER IN THE LOOP. A sandbox run reports planner
//      overrides (planner bias actually changing the action) and
//      planner agreement, not just an advisory trace.
//   3. CONCEPTS, SELF-MODEL, SOCIAL LEARNING. Concepts are
//      formed from experienced pattern/reward pairings and bias
//      action; the self-model records every loop turn and reports
//      confidence; a teacher demonstration is observed and then
//      imitated in a matching state.
//   4. PACING. The sandbox loop advances the world by one 0.1 s
//      action interval per decision, so tooSoon must be 0 —
//      every decoded action actually reaches the world.
//
// This demo does NOT claim a human brain, language understanding,
// or a real personality. Text here is a sensory channel, not
// grammar, meaning, or a pretrained language model.

#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace nlm {
namespace {

const char* cmdName(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        default: return "Wait";
    }
}

std::shared_ptr<Config> makeConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    return config;
}

SensoryPercept makePercept(const std::vector<float>& vision,
                           const std::vector<float>& touch,
                           const std::string& text = "") {
    SensoryPercept p;
    p.setVision(vision);
    p.setTouch(touch);
    p.setInternal({1.0f, 1.0f, 0.0f, 0.0f});
    p.setProprioception({0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f});
    p.setAudio({});
    p.setText(text);
    return p;
}

// Present a percept for `actions` loop turns (10 brain steps each),
// exactly as the closed loop does, and return internal firing rates.
std::vector<float> presentAndRead(AgentBrain& agent, const std::shared_ptr<Brain>& brain,
                                  const SensoryPercept& percept, int actions,
                                  uint64_t& step) {
    for (int a = 0; a < actions; ++a) {
        agent.processSensoryInput(percept);
        for (int i = 0; i < 10; ++i) brain->step(step++);
    }
    return agent.getInternalFiringRates();
}

std::vector<float> subtract(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> out;
    size_t n = std::min(a.size(), b.size());
    out.reserve(n);
    for (size_t i = 0; i < n; ++i) out.push_back(a[i] - b[i]);
    return out;
}

float cosine(const std::vector<float>& a, const std::vector<float>& b) {
    size_t n = std::min(a.size(), b.size());
    float dot = 0.0f, na = 0.0f, nb = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        dot += a[i] * b[i];
        na += a[i] * a[i];
        nb += b[i] * b[i];
    }
    if (na < 1e-9f || nb < 1e-9f) return 0.0f;
    return dot / (std::sqrt(na) * std::sqrt(nb));
}

}  // namespace
}  // namespace nlm

int main() {
    using namespace nlm;
    Logger::getGlobal().setLevel(LogLevel::Error);
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 6 - TEXT TEACHING / THOUGHT DEMO\n";
    std::cout << "===========================================\n\n";

    std::vector<float> emptyVision(16, 1.0f);
    std::vector<float> noTouch(8, 0.0f);
    std::vector<float> brightVision(16, 1.0f);
    for (int i = 5; i <= 10; ++i) brightVision[i] = 0.7f;
    std::vector<float> fwdTouch(8, 0.0f);
    fwdTouch[0] = 0.8f;

    // --- 1. Text association probe ---
    std::cout << "--- 1. Text association: \"apple\" taught with a resource scene ---\n";
    auto brain = std::make_shared<Brain>(makeConfig());
    brain->initialize();
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableCuriosity(false);
    agent.enableRewardModulation(false);  // Pure STDP association, no reward teacher.
    uint64_t step = 0;

    // Probe design, corrected after a first failed attempt: text
    // alone must NOT be presented with empty vision (all rays
    // 1.0). Empty vision is itself strong drive, and 10 of its
    // 16 neurons are also active in the paired scene, so after
    // teaching ANY word presented over empty vision recalled
    // the scene through the shared vision neurons — the control
    // word rose as much as the taught word (0.88 vs 0.86), and
    // the probe measured vision, not the word. Text is therefore
    // presented "in the dark" (vision 0, no visual drive), so
    // only the word's own neurons can evoke anything, and the
    // baseline subtracted is dark silence, not empty vision.
    std::vector<float> darkVision(16, 0.0f);
    auto emptyPat = presentAndRead(agent, brain, makePercept(emptyVision, noTouch), 50, step);
    auto darkPat = presentAndRead(agent, brain, makePercept(darkVision, noTouch), 50, step);
    auto visualBefore = presentAndRead(agent, brain, makePercept(brightVision, fwdTouch), 50, step);
    auto textBefore = presentAndRead(agent, brain, makePercept(darkVision, noTouch, "apple"), 50, step);
    auto controlBefore = presentAndRead(agent, brain, makePercept(darkVision, noTouch, "zebra"), 50, step);
    float simTextBefore = cosine(subtract(textBefore, darkPat), subtract(visualBefore, emptyPat));
    float simControlBefore = cosine(subtract(controlBefore, darkPat), subtract(visualBefore, emptyPat));
    std::cout << "  before teaching: text~visual=" << simTextBefore
              << " control~visual=" << simControlBefore << "\n";
    std::cout << "  encoder: apple ->";
    for (size_t idx : AgentBrain::encodeText("apple")) std::cout << " " << idx;
    std::cout << " ; zebra ->";
    for (size_t idx : AgentBrain::encodeText("zebra")) std::cout << " " << idx;
    std::cout << " (same word always gives the same pattern; different words differ)\n";

    // Paired teaching: the word and the scene, together, repeatedly.
    // No reward, no label, no string matching — only co-presentation.
    for (int a = 0; a < 300; ++a) {
        agent.processSensoryInput(makePercept(brightVision, fwdTouch, "apple"));
        for (int i = 0; i < 10; ++i) brain->step(step++);
    }
    auto visualAfter = presentAndRead(agent, brain, makePercept(brightVision, fwdTouch), 50, step);
    auto textAfter = presentAndRead(agent, brain, makePercept(darkVision, noTouch, "apple"), 50, step);
    auto controlAfter = presentAndRead(agent, brain, makePercept(darkVision, noTouch, "zebra"), 50, step);
    float simTextAfter = cosine(subtract(textAfter, darkPat), subtract(visualAfter, emptyPat));
    float simControlAfter = cosine(subtract(controlAfter, darkPat), subtract(visualAfter, emptyPat));
    std::cout << "  after 300 paired presentations: text~visual=" << simTextAfter
              << " control~visual=" << simControlAfter << "\n";
    bool textLearned = (simTextAfter > simTextBefore + 0.05f) &&
                       (simTextAfter > simControlAfter + 0.05f);
    std::cout << "  text association learned from experience: " << (textLearned ? "YES" : "NO")
              << " (needs text gain > 0.05 and text > control by > 0.05)\n";
    // Differential probe: does a word recall ITS scene, not just
    // any scene? Teach apple->resource and zebra->hazard
    // interleaved on a fresh brain, then ask each word in the
    // dark which scene it resembles more. A generic attractor
    // that completes any scene from any word fails this even
    // when the single-word probe above shows a large gain.
    std::cout << "  --- differential probe (apple->resource, zebra->hazard, fresh brain) ---\n";
    {
        auto bDiff = std::make_shared<Brain>(makeConfig());
        bDiff->initialize();
        AgentBrain aDiff(bDiff);
        aDiff.initialize(world);
        aDiff.enableCuriosity(false);
        aDiff.enableRewardModulation(false);
        uint64_t ds = 0;
        std::vector<float> darkVision2(16, 0.0f);
        std::vector<float> hazardVision(16, 1.0f);
        for (int i = 5; i <= 10; ++i) hazardVision[i] = 0.25f;
        auto emptyDiff = presentAndRead(aDiff, bDiff, makePercept(emptyVision, noTouch), 50, ds);
        for (int round = 0; round < 300; ++round) {
            aDiff.processSensoryInput(makePercept(brightVision, fwdTouch, "apple"));
            for (int i = 0; i < 10; ++i) bDiff->step(ds++);
            aDiff.processSensoryInput(makePercept(hazardVision, fwdTouch, "zebra"));
            for (int i = 0; i < 10; ++i) bDiff->step(ds++);
        }
        auto resPat = presentAndRead(aDiff, bDiff, makePercept(brightVision, fwdTouch), 50, ds);
        auto hazPat = presentAndRead(aDiff, bDiff, makePercept(hazardVision, fwdTouch), 50, ds);
        auto applePat = presentAndRead(aDiff, bDiff, makePercept(darkVision2, noTouch, "apple"), 50, ds);
        auto zebraPat = presentAndRead(aDiff, bDiff, makePercept(darkVision2, noTouch, "zebra"), 50, ds);
        float aR = cosine(subtract(applePat, emptyDiff), subtract(resPat, emptyDiff));
        float aH = cosine(subtract(applePat, emptyDiff), subtract(hazPat, emptyDiff));
        float zH = cosine(subtract(zebraPat, emptyDiff), subtract(hazPat, emptyDiff));
        float zR = cosine(subtract(zebraPat, emptyDiff), subtract(resPat, emptyDiff));
        std::cout << "  apple~resource=" << aR << " apple~hazard=" << aH
                  << " ; zebra~hazard=" << zH << " zebra~resource=" << zR << "\n";
        bool specific = (aR > aH + 0.05f) && (zH > zR + 0.05f);
        std::cout << "  differential (specific) association: " << (specific ? "YES" : "NO") << "\n";
        // Behavioural readout: after differential teaching, do the
        // two words decode to different actions? The resource
        // scene's probe action is Interact, the hazard scene's is
        // TurnLeft — if the words inherited their scenes' actions,
        // that is specificity at the level that matters for
        // behaviour, even if internal patterns overlap.
        {
            AgentBrain decAgent(bDiff);
            decAgent.initialize(world);
            decAgent.enableCuriosity(false);
            MotorCommand appleCmd = MotorCommand::Wait, zebraCmd = MotorCommand::Wait;
            for (int i = 0; i < 50; ++i) {
                decAgent.processSensoryInput(makePercept(darkVision2, noTouch, "apple"));
                for (int j = 0; j < 10; ++j) bDiff->step(ds++);
            }
            appleCmd = decAgent.decodeMotorCommand();
            for (int i = 0; i < 50; ++i) {
                decAgent.processSensoryInput(makePercept(darkVision2, noTouch, "zebra"));
                for (int j = 0; j < 10; ++j) bDiff->step(ds++);
            }
            zebraCmd = decAgent.decodeMotorCommand();
            std::cout << "  behavioural: apple decodes to " << cmdName(appleCmd)
                      << ", zebra decodes to " << cmdName(zebraCmd)
                      << " (resource scene -> Interact, hazard scene -> TurnLeft)\n";
        }
        textLearned = specific;
    }
    // Motor consequence, as an observation not a claim: what does
    // text alone decode to now, vs before teaching?
    {
        AgentBrain probeAgent(brain);
        probeAgent.initialize(world);
        probeAgent.enableCuriosity(false);
        // Note: a second AgentBrain over the same brain shares its
        // neurons; this only reads the decode for the taught pattern.
        for (int a = 0; a < 50; ++a) {
            probeAgent.processSensoryInput(makePercept(darkVision, noTouch, "apple"));
            for (int i = 0; i < 10; ++i) brain->step(step++);
        }
        std::cout << "  text-alone (in the dark) decode after teaching: " << cmdName(probeAgent.decodeMotorCommand()) << "\n";
    }
    std::cout << "\n";

    // --- 2. Concept formation from experience ---
    std::cout << "--- 2. Concepts from pattern/reward pairings ---\n";
    {
        ConceptFormation concepts;
        concepts.initialize(brain.get());
        std::vector<float> resourcePat(24, 0.0f);
        for (int i = 5; i <= 10; ++i) resourcePat[i] = 0.3f;  // closeness of bright scene
        resourcePat[16] = 0.8f;
        std::vector<float> hazardPat(24, 0.0f);
        for (int i = 5; i <= 10; ++i) hazardPat[i] = 0.75f;  // closeness of dark scene
        hazardPat[16] = 0.8f;
        std::vector<float> feats(8, 0.0f);
        for (int i = 0; i < 10; ++i) {
            concepts.presentExperience(resourcePat, feats, 10.0f, i);
            concepts.presentExperience(hazardPat, feats, -15.0f, i);
        }
        size_t ca = concepts.getMatchingConcept(resourcePat);
        size_t cb = concepts.getMatchingConcept(hazardPat);
        const auto* pa = ca ? concepts.getConcept(ca) : nullptr;
        const auto* pb = cb ? concepts.getConcept(cb) : nullptr;
        std::cout << "  concepts formed: " << concepts.getConceptCount()
                  << " ; resource scene -> concept " << ca
                  << (pa ? " (hint=" + pa->categoryHint + ")" : "")
                  << " ; hazard scene -> concept " << cb
                  << (pb ? " (hint=" + pb->categoryHint + ")" : "")
                  << " ; distinct: " << ((ca != 0 && cb != 0 && ca != cb) ? "YES" : "NO") << "\n";
    }
    std::cout << "\n";

    // --- 3. Social learning: observe a teacher, then imitate ---
    std::cout << "--- 3. Social learning: teacher demonstration ---\n";
    {
        SocialLearning social;
        social.initialize(brain.get());
        auto [actBefore, simBefore] = social.getImitationActionAndSimilarity(brightVision);
        for (int i = 0; i < 5; ++i) {
            social.observeAgentAction(ActionType::Interact, brightVision, brightVision);
        }
        auto [actAfter, simAfter] = social.getImitationActionAndSimilarity(brightVision);
        std::cout << "  before observation: imitation=" << static_cast<int>(actBefore)
                  << " similarity=" << simBefore
                  << " ; after 5 teacher Interacts in this state: imitation="
                  << static_cast<int>(actAfter) << " similarity=" << simAfter
                  << " (Interact=" << static_cast<int>(ActionType::Interact) << ")\n";
    }
    std::cout << "\n";

    // --- 4. Sandbox run: teacher labels what is touched, planner in loop ---
    std::cout << "--- 4. Sandbox run (100 steps, teacher labels touched things \"apple\") ---\n";
    {
        auto config = makeConfig();
        auto b2 = std::make_shared<Brain>(config);
        b2->initialize();
        SimpleWorld w2;
        w2.configure(20.0f, 20.0f, 16, 16);
        w2.setRandomSeed(42);
        w2.reset();
        AgentBrain a2(b2);
        a2.initialize(w2);
        a2.enableRewardModulation(true);
        a2.enableCuriosity(true);
        float totalReward = 0.0f;
        int resources = 0, hazards = 0, emptyInteracts = 0, tooSoon = 0, teacherLabels = 0;
        std::set<std::pair<int, int>> visited;
        uint64_t bs = 0;
        for (int s = 0; s < 100; ++s) {
            SensoryPercept percept = w2.getSensoryPercept();
            float maxTouch = 0.0f;
            for (float t : percept.getTouch()) maxTouch = std::max(maxTouch, t);
            // The teacher names whatever is being touched. The
            // teacher does NOT know (and the brain is not told)
            // whether it is a resource or a hazard — that is what
            // reward experience must teach apart.
            if (maxTouch > 0.05f) { percept.setText("apple"); ++teacherLabels; }
            a2.processSensoryInput(percept);
            for (int i = 0; i < 10; ++i) b2->step(bs++);
            MotorCommand cmd = a2.decodeMotorCommand();
            ActionResult result = w2.applyMotorCommand(cmd, w2.getSimulationTime());
            a2.applyRewardModulation(result.reward, 0.0f);
            w2.update(0.1);
            totalReward += result.reward;
            if (result.message == "too soon") ++tooSoon;
            if (cmd == MotorCommand::Interact) {
                if (result.message == "consumed_resource") ++resources;
                else if (result.message == "hit_hazard") ++hazards;
                else ++emptyInteracts;
            }
            const auto& body = w2.getAgentBody();
            visited.insert({static_cast<int>(std::floor(body.x)),
                            static_cast<int>(std::floor(body.y))});
            if (s % 25 == 0) {
                std::cout << "  step " << s << ": " << a2.getThoughtTrace()
                          << " wmTraces=" << (b2->getWorkingMemory() ? b2->getWorkingMemory()->getActiveTraces() : 0)
                          << " firing=" << b2->getFiringNeuronCount() << "\n";
            }
        }
        std::cout << "  sandbox: reward=" << totalReward << " resources=" << resources
                  << " hazards=" << hazards << " emptyInteracts=" << emptyInteracts
                  << " visitedCells=" << visited.size() << " avgRate=" << b2->getAverageFiringRate()
                  << " tooSoon=" << tooSoon << " teacherLabels=" << teacherLabels << "\n";
        std::cout << "  planner: overrides=" << a2.getPlannerOverrideCount()
                  << " agreement=" << a2.getPlannerAgreementCount() << "/"
                  << a2.getPlannerDecisionCount()
                  << " conceptBias=" << a2.getConceptBiasCount()
                  << " selfBias=" << a2.getSelfBiasCount()
                  << " socialBias=" << a2.getSocialBiasCount()
                  << " concepts=" << a2.getConceptCount()
                  << " selfConf=" << a2.getSelfModelConfidence()
                  << " preference=" << a2.getLearnedPreference() << "\n";
        // Checkpoint: preference and state survive a reload, and
        // the reloaded brain still acts on the same pattern.
        const std::string ckpt = "/tmp/nlm_phase6_text_demo.ckpt";
        bool saved = b2->save(ckpt);
        auto b3 = std::make_shared<Brain>(makeConfig());
        b3->initialize();
        bool loaded = saved && b3->load(ckpt);
        std::cout << "  checkpoint: saved=" << (saved ? "yes" : "NO")
                  << " loaded=" << (loaded ? "yes" : "NO");
        if (loaded) {
            std::cout << " totalSpikes=" << b3->getTotalSpikeCount()
                      << " workingNeurons=" << (b3->getWorkingMemory() ? b3->getWorkingMemory()->getMemoryNeuronCount() : 0)
                      << " wmTraces=" << (b3->getWorkingMemory() ? b3->getWorkingMemory()->getActiveTraces() : 0)
                      << " episodic=" << (b3->getEpisodicMemory() ? b3->getEpisodicMemory()->getEpisodeCount() : 0)
                      << " preference=" << b3->getLearnedPreference();
            AgentBrain a3(b3);
            a3.initialize(w2);
            a3.enableCuriosity(false);
            for (int i = 0; i < 50; ++i) {
                a3.processSensoryInput(makePercept(brightVision, fwdTouch));
                for (int j = 0; j < 10; ++j) b3->step(bs++);
            }
            std::cout << " continuedCmd=" << cmdName(a3.decodeMotorCommand());
        }
        std::cout << "\n";
    }

    std::cout << "Phase 6 text/thought demonstration complete.\n";
    // Exit 0 regardless of the specificity verdict: this demo
    // reports the probe honestly (including its failure) rather
    // than hiding it behind a non-zero exit that would look like
    // a build/demo break. The verdict is in the output above.
    (void)textLearned;
    return 0;
}
