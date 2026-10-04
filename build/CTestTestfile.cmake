# CMake generated Testfile for 
# Source directory: /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b
# Build directory: /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ci_validation "./nlm" "--version")
set_tests_properties(ci_validation PROPERTIES  _BACKTRACE_TRIPLES "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;354;add_test;/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;0;")
add_test(unit_tests "./nlm_test")
set_tests_properties(unit_tests PROPERTIES  _BACKTRACE_TRIPLES "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;355;add_test;/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;0;")
add_test(validation_tests "./validation_test")
set_tests_properties(validation_tests PROPERTIES  _BACKTRACE_TRIPLES "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;356;add_test;/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;0;")
add_test(phase_3_tests "./nlm_phase3_demo" "--run-phase3")
set_tests_properties(phase_3_tests PROPERTIES  _BACKTRACE_TRIPLES "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;359;add_test;/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;0;")
add_test(phase_4_tests "./nlm_phase4_demo" "--run-phase4")
set_tests_properties(phase_4_tests PROPERTIES  _BACKTRACE_TRIPLES "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;360;add_test;/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;0;")
add_test(phase_5_tests "./nlm_phase5_demo" "--run-phase5")
set_tests_properties(phase_5_tests PROPERTIES  _BACKTRACE_TRIPLES "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;361;add_test;/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;0;")
add_test(phase_6_tests "./nlm_phase6_demo" "--run-phase6")
set_tests_properties(phase_6_tests PROPERTIES  _BACKTRACE_TRIPLES "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;362;add_test;/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/CMakeLists.txt;0;")
subdirs("_deps/cli11-build")
subdirs("tests")
