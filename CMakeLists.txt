cmake_minimum_required(VERSION 3.16)

project(BuddySystem)

set(CMAKE_CXX_STANDARD 17)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(BuddySystem)
add_subdirectory(UnitTest)
add_subdirectory(ThirdParty/googletest EXCLUDE_FROM_ALL)

