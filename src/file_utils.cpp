#include <sys/types.h>
#include <dirent.h>
#include <cerrno>
#include <cstring>
#include <fnmatch.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "file_utils.h"
#include "utils.h"

std::string uc_path_combine(const std::string &directory, const std::string &filename)
{
	return directory + '/' + filename;
}

std::string uc_path_basename(const std::string &name)
{
	auto idx = name.find_last_of('/');
	if (idx != std::string::npos) {
		return name.substr(idx + 1);
	}

	return name;
}

std::string uc_path_dirname(const std::string &name)
{
	auto idx = name.find_last_of('/');
	if (idx != std::string::npos) {
		return name.substr(0, idx);
	}

	return ""	;
}
std::string uc_path_extension(const std::string &name)
{
	std::string base = uc_path_basename(name);

	auto idx = base.find_last_of('.');
	if (idx != std::string::npos) {
		return base.substr(idx + 1);
	}

	return "";
}

std::string uc_path_name(const std::string &name)
{
	std::string base = uc_path_basename(name);

	auto idx = base.find_last_of('.');
	if (idx != std::string::npos) {
		return base.substr(0, idx);
	}

	return base;
}



static bool uc_list_directory_impl(const std::string &directory, const std::string &match_pattern, std::vector<std::string> &result, int level)
{
	if (level > 16) { //hardcoded for now
		return true;
	}
	DIR *dir = opendir(directory.c_str());
	if (dir == NULL) {
		return false;
	}

	dirent *dent;
	while ((dent = readdir(dir)) != NULL) {

		if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0 ) {
			continue;
		}

		std::string path = uc_path_combine(directory, dent->d_name);
		char *rpath = realpath(path.c_str(), NULL);
		if (rpath == NULL) {
			TRACEF("realpath on %s failed: %d\n", path.c_str(), errno);
			continue;
		}
		path = rpath;
		free(rpath);
		bool isdir = false;
		if (dent->d_type == DT_DIR) {
			isdir = true;
		} else if (dent->d_type == DT_UNKNOWN) {
			struct stat st;
			if (stat(path.c_str(), &st) == 0) {
				isdir = S_ISDIR(st.st_mode);
			}
		}
		if (isdir) {
			uc_list_directory_impl(path, match_pattern, result, level + 1);
			continue;
		}

 		if (fnmatch(match_pattern.c_str(), dent->d_name, FNM_PATHNAME) != 0) {
			continue;
		}

		result.push_back(std::move(path));
	}

	closedir(dir);

	return true;
}

bool uc_list_directory(const std::string &directory, const std::string &match_pattern, std::vector<std::string> &result)

{
	return uc_list_directory_impl(directory, match_pattern, result, 0);
}
