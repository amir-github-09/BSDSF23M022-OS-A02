
## Feature 1: Project Setup & Initial Build

### Student
Roll No: BSDSF23M022
Repository: https://github.com/amir-github-09/BSDSF23M022-OS-A02

### Steps completed

1. Created GitHub repo and initialized with README.md.
2. Cloned the instructor starter `ls-v1.0.0` and copied `src/`.
3. Create the Makefile in root
4. Created directories: bin, obj, man.
5. Ran `make` and confirmed `./bin/ls` executes.
6. Committed and pushed initial project state.

### Commands used (exact)
- git clone https://github.com/amir-github-09/BSDSF23M022-OS-A02.git
- mkdir -p bin obj man
- touch bin/.gitkeep obj/.gitkeep man/.gitkeep
- touch REPORT.md
- make
- make clean
- ./bin/ls
- git add .
- git commit -m "Feature1: Initial project setup with starter code"
- git push origin main

### Verification
- `make` created the binary `bin/ls`.
- Running `./bin/ls` listed files correctly.

---

## Feature 2 Questions

### What is the crucial difference between the stat() and lstat() system calls? In the context of the ls command, when is it more appropriate to use lstat()?

The crucial difference between `stat()` and `lstat()` lies in how they handle **symbolic links** (symlinks).

* **`stat()`**: It follows symbolic links. If the path provided is a symlink, `stat()` returns information about the **target file** the link points to.
* **`lstat()`**: It does **not** follow symbolic links. If the path is a symlink, `lstat()` returns information about the **link itself** (the link file's properties), not the file it references.

In the context of the `ls` command, it is generally more appropriate to use **`lstat()`** when listing files. This allows the program to detect and report on symbolic links specifically, which is essential for the `-l` (long listing) format where symlinks are denoted with an `l` in the file type and their target is often displayed. Using `stat()` would hide the fact that the entry is a symbolic link, instead treating it as its target file type (e.g., a regular file or directory).


### The `st_mode` field in `struct stat` is an integer that contains both the file type (e.g., regular file, directory) and the permission bits. Explain how you can use bitwise operators (like `&`) and predefined macros (like `S_IFDIR` or `S_IRUSR`) to extract this information.

The `st_mode` field is a bitfield where various pieces of information are stored in specific bit positions.

1.  **Extracting File Type**:
    * To determine the file type, you use the **bitwise AND operator (`&`)** with the **`S_IFMT`** macro, which is a **mask** for the file type bits.
    * The result of `st_mode & S_IFMT` isolates the file type bits.
    * You then compare this result against specific file type macros, such as `S_IFREG` (regular file), **`S_IFDIR`** (directory), `S_IFLNK` (symbolic link), etc.
    * *Example:* `if ((statbuf.st_mode & S_IFMT) == S_IFDIR)` checks if the file is a directory.

2.  **Extracting Permission Bits**:
    * The permission bits are located in the lower 9 bits of `st_mode`.
    * You use the **bitwise AND operator (`&`)** with predefined permission macros to check if a specific permission bit is set.
    * For instance, **`S_IRUSR`** is a macro for the **Owner Read** permission bit.
    * *Example:* `if (statbuf.st_mode & S_IRUSR)` checks if the owner has read permission. Similarly, you can check `S_IWGRP` (Group Write), `S_IXOTH` (Others Execute), and so on.


---
