cmake_minimum_required(VERSION 3.0.0)
project(utils VERSION 0.1.0 LANGUAGES C CXX)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_CXX_STANDARD 20)
#add_executable(executor-test executor-test.cc)
#add_executable(threadpool-test threadpool-test.cc)
#add_executable(shared-ptr-test shared-ptr-test.cc)
#add_executable(lockfree-queue-test lockfree-queue-test.cc)
add_executable(kmp kmp.cc)