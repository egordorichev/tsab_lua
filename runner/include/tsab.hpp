#ifndef TSAB_HPP
#define TSAB_HPP

#define DEFAULT_CONTENT_ZIP "content.tsab"

int tsab_init(int arg, char **argv);
void tsab_loop();
bool tsab_loop_step();
void tsab_quit();

#endif