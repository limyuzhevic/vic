// Neuromodulation system bindings

#include "bindings.h"

namespace nlm {

void bind_neuromodulation_module(py::module_& m) {
    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(
        Abstract base class for neuromodulatory signals.
        
        Neuromodulators provide:
        - Global influence over neural plasticity
        - Modulation of learning rates
        - Regulation of attention and motivation
        - Integration with reward and prediction systems
        
        Types of neuromodulators:
        - Dopamine: Reward prediction error, reinforcement learning
        - Acetylcholine: Attention, memory consolidation
        - Norepinephrine: Arousal, vigilance, stress response
        - Serotonin: Mood, impulsivity, social behavior
        
        Each neuromodulator affects:
        - Plasticity factors (learning rate modifiers)
        - Neural excitability
        - Synaptic strength dynamics
    )pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor (learning rate modifier)")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    // Dopamine class
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(
        Dopamine neuromodulator for reward and reinforcement learning.
        
        Dopamine provides:
        - Reward prediction error signaling
        - Reinforcement learning modulation
        - Action selection bias
        - Motivation and pleasure signals
        
        Key functions:
        - Signal actual rewards
        - Signal reward prediction errors
        - Modulate synaptic plasticity for reward-associated behaviors
        - Learn reward associations
    )pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal actual reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error");

    // Acetylcholine class
    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(
        Acetylcholine neuromodulator for attention and memory consolidation.
        
        Acetylcholine provides:
        - Attention modulation
        - Memory consolidation
        - Cortical arousal
        - Sensory processing enhancement
        
        Key functions:
        - Enhance attention to relevant stimuli
        - Promote memory encoding
        - Prepare cortex for new learning
        - Filter irrelevant information
    )pbdoc")
        .def(py::init<>());

    // Norepinephrine class
    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(
        Norepinephrine neuromodulator for arousal and vigilance.
        
        Norepinephrine provides:
        - Arousal and alertness
        - Vigilance and attention
        - Stress response
        - Fight-or-flight preparation
        
        Key functions:
        - Increase overall brain arousal
        - Enhance vigilance for novel/important stimuli
        - Prepare for action
        - Modulate emotional responses
    )pbdoc")
        .def(py::init<>());

    // Serotonin class
    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(
        Serotonin neuromodulator for mood and social behavior.
        
        Serotonin provides:
        - Mood regulation
        - Social behavior modulation
        - Impulse control
        - Sleep regulation
        
        Key functions:
        - Stabilize mood
        - Inhibit impulsive actions
        - Promote social behavior
        - Regulate sleep-wake cycles
    )pbdoc")
        .def(py::init<>());

    // Reward class
    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(
        Reward neuromodulator for direct reward signaling.
        
        Reward provides:
        - Direct reward signal
        - Positive reinforcement
        - Motivation enhancement
        - Approach behavior activation
        
        Key functions:
        - Signal immediate rewards
        - Enhance approach behaviors
        - Activate reward pathways
        - Learn reward associations
    )pbdoc")
        .def(py::init<>());

    // PredictionError class
    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(
        Prediction error neuromodulator for learning from surprises.
        
        Prediction error provides:
        - Novelty detection
        - Surprise signaling
        - Learning trigger
        - Adaptation to unexpected events
        
        Key functions:
        - Detect unexpected events
        - Signal prediction errors
        - Trigger learning updates
        - Adapt to environmental changes
    )pbdoc")
        .def(py::init<>());

    // Novelty class
    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(
        Novelty neuromodulator for exploration and curiosity.
        
        Novelty provides:
        - Exploration drive
        - Curiosity motivation
        - Novelty detection
        - Behavioral flexibility
        
        Key functions:
        - Detect novel stimuli
        - Drive exploration
        - Promote curiosity
        - Enable behavioral adaptation
    )pbdoc")
        .def(py::init<>());

    // Curiosity class
    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(
        Curiosity neuromodulator for exploration and learning.
        
        Curiosity provides:
        - Exploration motivation
        - Information seeking
        - Learning drive
        - Environmental exploration
        
        Key functions:
        - Drive information seeking
        - Promote exploration
        - Enhance learning
        - Discover new environments
    )pbdoc")
        .def(py::init<>());
}

} // namespace nlm