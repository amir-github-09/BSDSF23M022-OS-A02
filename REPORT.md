
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

## Feature 2 Questions Version 1.1.0

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


## Feature 3 Questions Version 1.2.0

### Explain the general logic for printing items in a "down then across" columnar format. Why is a simple single loop through the list of filenames insufficient for this task?

The general logic for "down then across" (vertical) columnar printing is to **calculate the number of rows** required based on the total number of items and the number of columns that can fit on the screen.

1.  **Determine Parameters:** Find the maximum filename length, the terminal width, and the total count of files.
2.  **Calculate Columns:** Calculate the maximum number of columns that can fit based on the terminal width and the maximum filename length plus required spacing.
3.  **Calculate Rows:** Determine the number of rows: $Rows = \lceil \frac{Total\ Files}{Columns} \rceil$.
4.  **Nested Loop Iteration:** Use a structure that iterates through the list **row-by-row** (the outer loop) and then iterates **downward** for the items in that row (the inner loop).
    * The index for the $i$-th item in the $j$-th column is calculated as: $Index = Row + (j \times Rows)$.

A simple single loop through the list of filenames is insufficient because it prints items **"across then down"** (horizontal format). To achieve "down then across," you must first print the 1st, 2nd, 3rd... item of the **first column**, then jump back to print the 1st, 2nd, 3rd... item of the **second column**, and so on. A single loop naturally follows the list index linearly, which corresponds to the horizontal format. The vertical format requires a calculated, non-linear jump through the list indices.



### What is the purpose of the `ioctl` system call in this context? What would be the limitations of your program if you only used a fixed-width fallback (e.g., 80 columns) instead of detecting the terminal size?

The purpose of the **`ioctl`** (Input/Output Control) system call in this context is to **retrieve the current size of the terminal window (console)**. Specifically, it is used with the `TIOCGWINSZ` request, which fills a structure (`struct winsize`) with the terminal's dimensions in rows and columns. This information is crucial for calculating the optimal number of columns to use in the columnar display format.

If you only used a fixed-width fallback (e.g., 80 columns) instead of detecting the terminal size, the limitations of your program would be:

1.  **Inefficient Use of Space:** If the user has a wide terminal (e.g., 200 columns), the program would only use 80, wasting screen real estate and potentially requiring unnecessary vertical scrolling.
2.  **Poor Display on Narrow Terminals:** If the user has a very narrow terminal (e.g., 40 columns), the program would attempt to fit the display into 80 columns, leading to **mangled output** as lines would wrap or be truncated awkwardly, rendering the columnar display useless.
3.  **Lack of Adaptability:** The display would not adapt if the user resized the terminal window while the program was running, leading to an immediate mismatch between the expected layout and the actual screen size.


---
