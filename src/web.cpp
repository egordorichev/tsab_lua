#include "tsab.hpp"
#include <emscripten.h>
#include <iostream>

void loop_function() {
	std::cout << "call\n";
	if (tsab_loop_step()) {
		emscripten_cancel_main_loop();
	}
}

int main(int arg, char **argv) {
	std::cout << "HELLLO WORLD\n";

	int r = tsab_init(arg, argv);

	if (r != 0) {
		std::cout << "main error: " << r << "\n";
		return r;
	}

	emscripten_set_main_loop(loop_function, 60, 1);
	std::cout << "DONE\n";

	tsab_quit();
	std::cout << "QUIT\n";

	return 0;
}