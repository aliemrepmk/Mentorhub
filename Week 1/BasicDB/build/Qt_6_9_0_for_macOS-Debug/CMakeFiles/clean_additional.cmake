# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "BasicDB_autogen"
  "CMakeFiles/BasicDB_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/BasicDB_autogen.dir/ParseCache.txt"
  )
endif()
