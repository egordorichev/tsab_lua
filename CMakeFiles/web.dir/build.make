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
CMAKE_COMMAND = /home/egor/code/apps/CLion-2018.2.3/clion-2018.2.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/egor/code/apps/CLion-2018.2.3/clion-2018.2.3/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/egor/tsab

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/egor/tsab

# Include any dependencies generated for this target.
include CMakeFiles/web.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/web.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/web.dir/flags.make

CMakeFiles/web.dir/web/web.cpp.o: CMakeFiles/web.dir/flags.make
CMakeFiles/web.dir/web/web.cpp.o: web/web.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/egor/tsab/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/web.dir/web/web.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/web.dir/web/web.cpp.o -c /home/egor/tsab/web/web.cpp

CMakeFiles/web.dir/web/web.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/web.dir/web/web.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/egor/tsab/web/web.cpp > CMakeFiles/web.dir/web/web.cpp.i

CMakeFiles/web.dir/web/web.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/web.dir/web/web.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/egor/tsab/web/web.cpp -o CMakeFiles/web.dir/web/web.cpp.s

# Object files for target web
web_OBJECTS = \
"CMakeFiles/web.dir/web/web.cpp.o"

# External object files for target web
web_EXTERNAL_OBJECTS =

web.html: CMakeFiles/web.dir/web/web.cpp.o
web.html: CMakeFiles/web.dir/build.make
web.html: /usr/lib/x86_64-linux-gnu/libSDL2main.a
web.html: /usr/lib/x86_64-linux-gnu/libSDL2.so
web.html: /usr/lib/x86_64-linux-gnu/libSDL2_ttf.so
web.html: /usr/lib/x86_64-linux-gnu/libSDL2_image.so
web.html: /usr/lib/x86_64-linux-gnu/libSDL2_mixer.so
web.html: libs/SDL_GPU/libSDL2_gpu.a
web.html: libs/SDL_GPU/libSDL2_gpu.so
web.html: /usr/lib/x86_64-linux-gnu/libluajit-5.1.so
web.html: /usr/lib/x86_64-linux-gnu/libm.so
web.html: /usr/lib/x86_64-linux-gnu/libGL.so
web.html: /usr/lib/x86_64-linux-gnu/libGLU.so
web.html: /usr/lib/x86_64-linux-gnu/libglut.so
web.html: /usr/lib/x86_64-linux-gnu/libXmu.so
web.html: /usr/lib/x86_64-linux-gnu/libXi.so
web.html: CMakeFiles/web.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/egor/tsab/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable web.html"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/web.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/web.dir/build: web.html

.PHONY : CMakeFiles/web.dir/build

CMakeFiles/web.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/web.dir/cmake_clean.cmake
.PHONY : CMakeFiles/web.dir/clean

CMakeFiles/web.dir/depend:
	cd /home/egor/tsab && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/egor/tsab /home/egor/tsab /home/egor/tsab /home/egor/tsab /home/egor/tsab/CMakeFiles/web.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/web.dir/depend
