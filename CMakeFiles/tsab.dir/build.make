# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/egor/code/apps/clion-2018.2.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/egor/code/apps/clion-2018.2.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/egor/tsab

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/egor/tsab

# Include any dependencies generated for this target.
include CMakeFiles/tsab.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tsab.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tsab.dir/flags.make

CMakeFiles/tsab.dir/src/main.cpp.o: CMakeFiles/tsab.dir/flags.make
CMakeFiles/tsab.dir/src/main.cpp.o: src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/egor/tsab/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tsab.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tsab.dir/src/main.cpp.o -c /home/egor/tsab/src/main.cpp

CMakeFiles/tsab.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tsab.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/egor/tsab/src/main.cpp > CMakeFiles/tsab.dir/src/main.cpp.i

CMakeFiles/tsab.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tsab.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/egor/tsab/src/main.cpp -o CMakeFiles/tsab.dir/src/main.cpp.s

# Object files for target tsab
tsab_OBJECTS = \
"CMakeFiles/tsab.dir/src/main.cpp.o"

# External object files for target tsab
tsab_EXTERNAL_OBJECTS =

tsab: CMakeFiles/tsab.dir/src/main.cpp.o
tsab: CMakeFiles/tsab.dir/build.make
tsab: /usr/lib/x86_64-linux-gnu/libSDL2main.a
tsab: /usr/lib/x86_64-linux-gnu/libSDL2.so
tsab: libs/SDL_GPU/libSDL2_gpu.a
tsab: libs/SDL_GPU/libSDL2_gpu.so
tsab: /usr/lib/x86_64-linux-gnu/libluajit-5.1.so
tsab: /usr/lib/x86_64-linux-gnu/libm.so
tsab: /usr/lib/x86_64-linux-gnu/libGL.so
tsab: /usr/lib/x86_64-linux-gnu/libGLU.so
tsab: /usr/lib/x86_64-linux-gnu/libglut.so
tsab: /usr/lib/x86_64-linux-gnu/libXmu.so
tsab: /usr/lib/x86_64-linux-gnu/libXi.so
tsab: CMakeFiles/tsab.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/egor/tsab/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable tsab"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tsab.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tsab.dir/build: tsab

.PHONY : CMakeFiles/tsab.dir/build

CMakeFiles/tsab.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tsab.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tsab.dir/clean

CMakeFiles/tsab.dir/depend:
	cd /home/egor/tsab && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/egor/tsab /home/egor/tsab /home/egor/tsab /home/egor/tsab /home/egor/tsab/CMakeFiles/tsab.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tsab.dir/depend

