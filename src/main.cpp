#include <cstdio>
#include <cerrno>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include "cleaner.h"
#include "utils.h"

static void usage()
{
	puts("Usage: dir_cleaner [common_options] -s size directory\n");
	puts("Removes files in the given directory and its subdirectories to keep total size of all");
	puts(" files in that directory at a maximum size.");
	puts("\ncommon_options:");
	puts("\t-S ctime|mtime|filename|rfilename - remove the oldest files sorted based on the file ");
	puts("\t            ctime, mtime, filename or reverse sorted filenames");
	puts("\t            See man 2 stat for the description of mtime and ctime. Default is mtime");
	puts("\t-n          Dry run, print files that would be removed, but do not actually remove any files");
	puts("\t-N          Non-recursive. Don't loook in sub directories for files to remove.");
	

	puts("\nmandatory arguments:");
	puts("\t-s size -   Desired size of the directory. Files will be removed from the directory until its total size");
	puts("\t            is less than this given size. Unit is in bytes, or the size can be suffixed with");
	puts("\t            KB, MB, GB, TB for Kilobyte, Megabyte, Gigabyte or Terabyte.");
    puts("\t            -s 500MB means 500 Megabyte, -s 250GB means 250GB");
    puts("\tdirectory - Path to the directory to be cleaned. Only normal files in this directory are considered.");
    puts("\t            Subdirectories are not processed");


	printf("\nVersion: %s\n", VERSION_STR);
}

long long parse_size_spec(const char *size_spec)
{
	const struct {
		const char *const unit;
		long long multiplier;
	} units [] = {
			{"kB",  1000},
			{"KiB", 1024},
			{"MB",  1000 * 1000},
			{"MIB", 1024 * 1024},
			{"GB",  1000 * 1000 * 1000ll},
			{"GIB", 1024 * 1024 * 1024ll},
			{"TB",  1000 * 1000 * 1000ll * 1000ll},
			{"KiB", 1024 * 1024 * 1024ll * 1024ll}
	};

	long long multiplier = 1;
	char *endptr = NULL;
	errno = 0;

	long long val = strtoll(size_spec, &endptr, 10);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
	   || (errno != 0 && val == 0)) {

		return -1;
	}

	if (val < 0) {
		return -1;
	}

	if (endptr == size_spec) {
		return -1;
	}
	size_t i = 0;
	for (; i < ARRAY_SIZE(units); i++) {
		if (strcasecmp(endptr, units[i].unit) == 0) {
			multiplier = units[i].multiplier;
			break;
		}
	}

	if (*endptr && i == ARRAY_SIZE(units)) { //unknown prefix
		return -1;

	}
	//todo detect overflow
	return val * multiplier;
}

int main(int argc, char *argv[])
{
	const char *size_spec = NULL;
	bool dry_run = false;
	const char *sort_type = NULL;
	int c;
	bool recursive = true;

	while ((c = getopt(argc, argv, "Hns:S:N")) != -1) {
		switch (c) {
		case 'n':
			dry_run = 1;
			break;
		case 's':
			size_spec = optarg;
			break;
		case 'S':
			sort_type = optarg;
			break;
		case 'N':
			recursive = false;
			break;
		default:
			printf("Unknown option %c\n", c);
			// Fallthrough, no break
		case 'h':
			usage();
			return 1;
			break;
		}
	}

	if (size_spec == NULL) {
		puts("missing -s argument\n");
		usage();
		return 1;
	}

	if (optind >= argc) {
		puts("Missing directory name");
		usage();
		return 1;
	}

	long long total_size = parse_size_spec(size_spec);
	if (total_size < 0) {
		printf("Size specifier '%s' is invalid\n", size_spec);
		return 2;

	}

	CleanOptions options;

	char *dir = realpath(argv[optind], NULL);
	if (dir == NULL) {
		printf("Cannot resolve directory %s : %s\n", argv[optind], strerror(errno));
		return 1;
	}

	if (sort_type == NULL || strcmp(sort_type, "mtime") == 0) {
		options.sort_type = CleanOptions::SortType::MTIME;
	} else if (strcmp(sort_type, "ctime") == 0) {
		options.sort_type = CleanOptions::SortType::CTIME;
	} else if (strcmp(sort_type, "filename") == 0) {
		options.sort_type = CleanOptions::SortType::FILENAME_ASC;
	} else if (strcmp(sort_type, "rfilename") == 0) {
		options.sort_type = CleanOptions::SortType::FILENAME_DESC;
	} else {
		printf("Unknown sort type %s\n", sort_type);
		usage();
		return 1;
	}

	options.dry_run        = dry_run;
	options.total_size     = total_size;
	options.dir            = dir;
	options.recursive      = recursive;

	free(dir);

	if (options.dir == "/") {
		puts("Refusing to clean root filesystem");
		return 4;
	}

	printf("Cleaning %s to be size %lld\n", argv[optind], total_size);

	clean_dir(options);
}
