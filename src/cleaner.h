#ifndef DIR_CLEANER_SRC_CLEANER_H_
#define DIR_CLEANER_SRC_CLEANER_H_
#include <string>

struct CleanOptions {
	enum SortType {
		MTIME,
		CTIME,
		FILENAME_ASC,
		FILENAME_DESC, //descending
	};
	std::string dir;
	SortType sort_type;
	long long total_size;
	bool dry_run;
};


void clean_dir(const CleanOptions &options);


#endif /* DIR_CLEANER_SRC_CLEANER_H_ */
