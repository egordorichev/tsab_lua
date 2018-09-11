#include "elzip.hpp"

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
}
