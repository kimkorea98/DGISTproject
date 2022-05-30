
program goal
make program for zip and unzip by using fastlzilb

You can find the original source code here:
https://github.com/xroche/fastlzlib

makefile
make comp.exe, decomp.exe


# Usage
$ ./comp compressed-filename source-file1 [source-file2 source-file3 ...]
This program compresses multiple source files into a single file named with
“compressed-filename”.

# Usage
$ ./decomp compressed-filename output-di
This program decompresses the compressed-filename into a directory, output-dir.
