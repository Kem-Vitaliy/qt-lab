# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\qtproject1_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\qtproject1_autogen.dir\\ParseCache.txt"
  "qtproject1_autogen"
  )
endif()
