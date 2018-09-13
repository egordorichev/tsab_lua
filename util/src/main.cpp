#include <iostream>
#include <cstring>
#include <11Zip/elzip.hpp>
#include <experimental/filesystem>

#include "util.hpp"

namespace fs = std::experimental::filesystem;

static void print_help(int argc, char **argv, bool extra) {
	std::cout << argv[0] << ": A set of helper utils for distributing tsab projects\n";

	if (extra) {
		char *command = argv[2];
		std::cout << "TODO: help for " << command << "\n";
	}
}

static void pack(int argc, char **argv) {
	char *name = (char *) ".";
	char *out = (char *) "content";

	if (argc > 2) {
		name = argv[2];

		if (argc > 3) {
			out = argv[3];
		}
	}

	if (!fs::exists(std::string(name) + "/main.lua")) {
		std::cout << "Error: The directory doesn't seem to contain main.lua\n";
		return;
	}

	elz::zipFolder(name, out);
}

static void dist(int argc, char **argv) {
	char *name = (char *) "content.tsab";

	if (argc > 2) {
		name = argv[2];
	}

	if (!fs::exists(std::string(name))) {
		std::cout << "Error: " << name << " was not found, you can specify your file name after `dist`\n";
		return;
	}

	std::cout << "TODO: implement\n";
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		print_help(argc, argv, false);
	} else {
		char *command = argv[1];

		if (strcmp(command, "help") == 0) {
			print_help(argc, argv, argc > 2);
		} else if (strcmp(command, "pack") == 0) {
			pack(argc, argv);
		} else if (strcmp(command, "dist") == 0) {
			dist(argc, argv);
		} else {
			std::cout << "Unknown option " << command << ", use help for help\n";
		}
	}

	return 0;
}