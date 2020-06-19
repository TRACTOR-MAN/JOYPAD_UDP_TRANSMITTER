# define the minimum cmake verion
cmake_minimum_required(VERSION 3.13)

# set the project name
project(joypad_udp_transmitter LANGUAGES C CXX)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

# Set the linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

# set all of the source files
add_executable( joypad_udp_transmitter "" )

# Set the desired C and CXX standards
set_property(TARGET joypad_udp_transmitter PROPERTY CXX_STANDARD 11)
set_property(TARGET joypad_udp_transmitter PROPERTY CXX_STANDARD_REQUIRED ON)

# set all of the source files below
target_sources(joypad_udp_transmitter
PRIVATE
#  ___  ___   ___      _  ___  ___  _____  
# | _ \| _ \ / _ \  _ | || __|/ __||_   _| 
# |  _/|   /| (_) || || || _|| (__   | |   
# |_|__|_|_\_\___/  \__/_|___|\___|__|_|   
#  / __| / _ \ | | | || _ \ / __|| __|     
#  \__ \| (_) || |_| ||   /| (__ | _|      
#  |___/ \___/  \___/ |_|_\ \___||___|  

# ../src/
../src/main.cpp
../src/joypad_if.cpp
)

# Set all of the include directories below
target_include_directories(joypad_udp_transmitter
PRIVATE
../src/inc
)