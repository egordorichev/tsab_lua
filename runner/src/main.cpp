#include "tsab.hpp"

int main(int arg, char **argv) {
	int r = tsab_init(arg, argv);

	if (r != 0) {
		return r;
	}

	tsab_loop();
	tsab_quit();

	return 0;
}