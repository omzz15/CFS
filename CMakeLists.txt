cmake_minimum_required(VERSION 4.0.0)

project(cfs)

file(GLOB CSources ./src/*.c)
file(GLOB LibSources ./libs/**/*.c)
add_executable(cfssim ${CSources} ${LibSources})

target_include_directories(cfssim PUBLIC include)
target_include_directories(cfssim PUBLIC libs)