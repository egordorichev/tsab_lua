#include "unzipper.h"
#include <experimental/filesystem>
#include <iostream>
#include <fstream>

namespace elz {
  void extractZip(std::string zipname, std::string target = ".");
	void zipFolder(std::string folder, std::string out);
}
