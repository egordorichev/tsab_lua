#include "elzip.hpp"
#include "zipper.h"

#include <cstring>

namespace fs = std::experimental::filesystem;

namespace elz
{
    void extractZip(std::string zipname, std::string target)
    {
        ziputils::unzipper zipFile;
        zipFile.open(zipname.c_str());
        for (std::string filename : zipFile.getFilenames())
        {
            fs::path cDir(target + ((fs::path(filename).parent_path().string() == "") ? "" : "/") + fs::path(filename).parent_path().string());
            fs::path cFile(target + "/" + filename);

            fs::path fillPath;
            for (fs::path pathPart : cDir) {
                fillPath /= pathPart;
                if (!fs::exists(fillPath)) {
                    fs::create_directory(fillPath);
                }
            }
            zipFile.openEntry(filename.c_str());
            std::ofstream wFile;
            wFile.open(cFile.string());
            zipFile >> wFile;
            wFile.close();
        }
    }

	inline bool ends_with(std::string const & value, std::string const & ending) {
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

  void zipFolder(std::string folder, std::string out) {
    if (!std::experimental::filesystem::exists(folder)) {
      std::cout << "No such folder\n";
      return;
    }

	  if (ends_with(folder, "/")) {
	  	out.erase(out.length(), out.length());
	  }

    int len = folder.length();

    if (!ends_with(out, ".tsab")) {
    	out += ".tsab";
    }

    ziputils::zipper zipFile;
    zipFile.open(out.c_str(), false);

    for (auto &p: std::experimental::filesystem::recursive_directory_iterator(folder)) {
    	if (!std::experimental::filesystem::is_directory(p)) {
		    std::string str = p.path().relative_path().generic_string().erase(0, len);
		    char *pt = (char *) str.c_str();

		    //if (strstr(pt, ".zip") == NULL && strstr(pt, "..tsab") == NULL) {
			    std::ifstream file(p.path().relative_path().generic_string(), std::ios::in | std::ios::binary);

			    if (file.is_open()) {
				    std::cout << "Adding " << pt << "\n";

				    zipFile.addEntry(pt);
				    zipFile << file;
			    } else {
			    	std::cout << "Error adding " << pt << "\n";
			    }

			    file.close();
		    //}
	    }
    }

    zipFile.close();
  }
}
