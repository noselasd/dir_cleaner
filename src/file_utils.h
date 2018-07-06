#ifndef LIBCOMMON_INCLUDE_FILE_UTILS_H_
#define LIBCOMMON_INCLUDE_FILE_UTILS_H_
#include <string>
#include <vector>

std::string uc_path_combine(const std::string &directory, const std::string &filename);
//list files in the given directory, fnmatch() the match pattern.
//result is realpath() of the files. return false if an error occurs (and errno should be set)
bool uc_list_directory(const std::string &directory, const std::string &match_pattern, std::vector<std::string> &result);

//file path without directtory
std::string uc_path_basename(const std::string &name);
//directory name, without the last /
std::string uc_path_dirname(const std::string &name);

//returns the file extension, everything after the last '.', or an empty string
std::string uc_path_extension(const std::string &name);

//returns the file name without the exension(and directory), everything up to the last '.'
std::string uc_path_name(const std::string &name);


#endif /* LIBCOMMON_INCLUDE_FILE_UTILS_H_ */
