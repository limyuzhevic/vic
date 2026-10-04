
if(NOT "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp/cli11-populate-gitinfo.txt" IS_NEWER_THAN "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp/cli11-populate-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp/cli11-populate-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"  clone --no-checkout --config "advice.detachedHead=false" "https://github.com/CLIUtils/CLI11.git" "cli11-src"
    WORKING_DIRECTORY "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/CLIUtils/CLI11.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"  checkout v2.4.2 --
  WORKING_DIRECTORY "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'v2.4.2'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git"  submodule update --recursive --init 
    WORKING_DIRECTORY "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-src"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp/cli11-populate-gitinfo.txt"
    "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp/cli11-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_262d6285-160d-472c-804d-80b30a605e7b/build/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp/cli11-populate-gitclone-lastrun.txt'")
endif()

