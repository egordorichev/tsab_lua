#include "tsab.hpp"
#include <emscripten.h>

void loop_function(void *arg) {
	if (tsab_loop_step()) {
		emscripten_cancel_main_loop();
	}
}

int main(int arg, char **argv) {
	int r = tsab_init(arg, argv);

	if (r != 0) {
		return r;
	}

	emscripten_set_main_loop_arg(loop_function, 0, 60, 1);

	tsab_quit();

	return 0;
}