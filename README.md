## About

dir_cleaner can recursively delete files in a directory and its subdirectories
to keep to total size of files below a certain size.

The decision of which files to remove are determined by oldest files according
to the ctime, mtime or filename of the files.

## Building

Run:
make

## Using
The built binary after running 'make' is the executable in src/build/dir_cleaner

Run src/build/dir_cleaner -h to see the usage.
