#include "cgit.hpp"

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <memory.h>
#include <iostream>
#include <sstream>

#define DEFAULT_GIT_FILE "/usr/bin/git"
#define DEFAULT_GITHUB_URL "https://github.com/"
// slow
// #define DEFAULT_MIRROR_URL "https://github.com.cnpmjs.org/"
#define DEFAULT_MIRROR_URL "https://hub.fastgit.org/"

namespace cgit {
    using namespace std;

    static string create_cmd(std::vector<string> &args) {
        std::ostringstream ss;
        for (const auto &s:args) {
#ifdef WIN32
            ss << s << " ";
#else
            ss << "\"" << s << "\" ";
#endif
        }
        return ss.str();
    }

#ifdef WIN32
    const char *get_git_file() {
        char *file;
        size_t len;
        errno_t err = _dupenv_s(&file, &len, "GIT");
        return err ?  DEFAULT_GIT_FILE : file ;
    }
#else
    const char *get_git_file() {
        char *file = getenv("GIT");
        return file ? file : DEFAULT_GIT_FILE;
    }
#endif
    int cgit_clone(vector<string> &args) {
        // 1.git clone https://github.com.cnpmjs.org/killf/cgit.git
        string old_url, new_url, folder_name;
        for (size_t i = 2; i < args.size(); i++) {
            if (args[i].startsWith(DEFAULT_GITHUB_URL)) {
                old_url = args[i];
                new_url = args[i].replace(0, strlen(DEFAULT_GITHUB_URL), DEFAULT_MIRROR_URL);

                auto index = old_url.find_last_of('/');
                if (index != std::string::npos) {
                    folder_name = old_url.substr(index + 1);
                    if (folder_name.endswith(".git")) {
                        folder_name = folder_name.substr(0, folder_name.size() - 4);
                    }
                }
            }
        }
        args[0] = get_git_file();

        auto cmd1 = create_cmd(args);
        auto err = system(cmd1.c_str());
        if (err != 0 || new_url.empty() || folder_name.empty())return err;

        // 2.cd cgit
#ifdef WIN32
        err = _chdir(folder_name.c_str());
#else
        err = chdir(folder_name.c_str());
#endif
        if (err != 0)return err;

        // 3.git remote set-url origin https://github.com/killf/cgit.git
        auto cmd3 = "git remote set-url origin \"" + old_url + "\"";
        return system(cmd3.c_str());
    }

    int cgit_pull(vector<string> &args) {
        return 0;
    }
}
