cmake_minimum_required(VERSION 3.10)
project(SimplePassInstrumentation)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
include(LLVMConfig)
include(AddLLVM)

add_definitions(-fno-rtti -fno-exceptions)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

set(_SRC_FILES
    SimplePassInstrument.cpp)
add_llvm_pass_plugin(SimplePassInstrument
  ${_SRC_FILES})
