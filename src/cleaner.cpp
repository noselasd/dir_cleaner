#include "cleaner.h"
#include <vector>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "file_utils.h"
#include "utils.h"

struct FileCleanEntry {
	std::string filename;
	struct stat st;
};

static void stat_files(const std::vector<std::string> &filenames, std::vector<FileCleanEntry> &entries)
{
	for (auto &f : filenames) {
		FileCleanEntry entry;
		if (stat(f.c_str(), &entry.st) == 0) {
			if (!S_ISDIR(entry.st.st_mode)) {
				entry.filename = f;
				entries.push_back(std::move(entry));
			}
		} else {
			if (errno != EEXIST) { //could be removed/renamed in the meantime, don't care
				printf("Error getting info on file %s : %s\n", f.c_str(), strerror(errno));
			}
		}
	}
}


static bool get_files(const std::string &dir, std::vector<FileCleanEntry> &entries)
{
	std::vector<std::string> filenames;

	if (!uc_list_directory(dir, "*", filenames)) {
		printf("Cannot list files in %s : %s\n", dir.c_str(), strerror(errno));
		return false;
	}

	stat_files(filenames, entries);

	return true;
}

static void sort_files(std::vector<FileCleanEntry> &entries, CleanOptions::SortType sort_type)
{
	if (sort_type == CleanOptions::SortType::MTIME) {
		std::sort(entries.begin(), entries.end(),
				[](const FileCleanEntry &a, const FileCleanEntry &b) {return a.st.st_mtim.tv_sec < b.st.st_mtim.tv_sec;});
	} else if (sort_type == CleanOptions::SortType::CTIME) {
		std::sort(entries.begin(), entries.end(),
				[](const FileCleanEntry &a, const FileCleanEntry &b) {return a.st.st_ctim.tv_sec < b.st.st_ctim.tv_sec;});
	} else if (sort_type == CleanOptions::SortType::FILENAME_ASC) {
			std::sort(entries.begin(), entries.end(),
					[](const FileCleanEntry &a, const FileCleanEntry &b) {return uc_path_basename(a.filename) < uc_path_basename(b.filename);});
	} else if (sort_type == CleanOptions::SortType::FILENAME_DESC) {
			std::sort(entries.begin(), entries.end(),
					[](const FileCleanEntry &a, const FileCleanEntry &b) {return uc_path_basename(a.filename) > uc_path_basename(b.filename);});
	} else {
		abort();
	}
}

static long long get_total_size(const std::vector<FileCleanEntry>& entries)
{
	long long total_size = 0;

	for (auto& e : entries)
	{
		total_size += e.st.st_size;
	}
	return total_size;
}

void clean_dir(const CleanOptions &options)
{

	std::vector<FileCleanEntry> entries;

	if (!get_files(options.dir, entries)) {
		return;
	}

	sort_files(entries, options.sort_type);

	long long total_size = get_total_size(entries);

	TRACEF("Total dir size: %lld\n", total_size);

	long long remaining_size = total_size;
	std::vector<FileCleanEntry> to_remove;

	for (const auto &e : entries) {
		if (remaining_size <= options.total_size) {
			break;
		}
		remaining_size -= e.st.st_size;
		to_remove.push_back(e);
	}

	size_t files_removed = 0;
	for (const auto &e : to_remove) {
		if (options.dry_run) {
			printf("Would remove file %s of size %lld\n", e.filename.c_str(), (long long)e.st.st_size);
		} else {
			if (remove(e.filename.c_str()) == 0) {
				printf("Removed file %s with size %lld\n", e.filename.c_str(), (long long)e.st.st_size);
				files_removed++;
			} else {
				if (errno != EEXIST) {
					printf("Cannot remove file %s : %s\n", e.filename.c_str(), strerror(errno));
				}
			}
		}
	}

	printf("Removed %zu files out of %zu total\n", files_removed, entries.size());
	printf("Total size %lld\nRemaining size %lld\nRemoved size: %lld\n", total_size, remaining_size, total_size - remaining_size);
}
