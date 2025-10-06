
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

## Feature 4 Questions

### Compare the implementation complexity of the "down then across" (vertical) printing logic versus the "across" (horizontal) printing logic. Which one requires more pre-calculation and why?

The **"down then across" (vertical)** printing logic is significantly **more complex** and requires **more pre-calculation** than the "across" (horizontal) logic.

| Logic | Complexity | Required Pre-calculation |
| :--- | :--- | :--- |
| **Across (Horizontal)** | Lower | Only needs to track the current column position (or character count) and ensure a newline is printed when the terminal width is exceeded. |
| **Down then Across (Vertical)** | Higher | Requires calculating the **maximum filename length**, the **number of columns** that fit on the screen, and the **number of rows**. This pre-calculation is necessary to derive the non-linear index formula (i.e., $Index = Row + (j \times Rows)$) used to jump between columns within the loop structure. |

The vertical logic needs all item counts, maximum lengths, and screen size *before* printing can begin, whereas the horizontal logic can primarily make decisions on a per-item basis as it iterates through the list linearly.



### Describe the strategy you used in your code to manage the different display modes (`-l`, `-x`, and default). How did your program decide which function to call for printing?

The strategy to manage different display modes typically involves:

1.  **Command-Line Option Parsing:** Using a function (like `getopt()`) to parse command-line arguments and set corresponding **global flags** or an **enumeration variable** (e.g., `DISPLAY_MODE`).
    * If `-l` is present, set the mode to `LONG_FORMAT`.
    * If `-x` is present, set the mode to `ACROSS_FORMAT`.
    * If neither is present, default to `VERTICAL_FORMAT` (down then across).

2.  **Central Dispatch:** Using a **switch statement** or a series of **`if/else if`** checks on the `DISPLAY_MODE` variable after all input files/directories have been processed.

3.  **Specialized Functions:** Defining a specific printing function for each mode:
    * `print()` (for `-l`)
    * `print_horizontal_display()` (for `-x`)
    * `print_column_display()` (for default)

The program decides which function to call by using the central dispatch mechanism (the `switch` statement). Based on the value of the `DISPLAY_MODE` variable set during the initial argument parsing, execution branches to the appropriate, specialized printing function responsible for formatting and displaying the data for all files and directories.

---

## Feature-5  Questions/Answers

### 1. Why is it necessary to read all directory entries into memory before you can sort them? What are the potential drawbacks of this approach for directories containing millions of files?

It is necessary to read all directory entries into memory because sorting requires access to **all elements at once**. Functions like `qsort()` operate on arrays in memory, so without loading the entries into an array, you cannot rearrange them alphabetically.

**Potential drawbacks for very large directories:**
- High **memory usage**: Storing millions of filenames can exhaust RAM.
- **Performance issues**: Sorting millions of entries can be slow.
- Possible **allocation failures** if the system cannot allocate enough contiguous memory.

---

### 2. Explain the purpose and signature of the comparison function required by `qsort()`. How does it work, and why must it take `const void *` arguments?

**Purpose:**  
The comparison function tells `qsort()` how to order two elements. It returns:
- `< 0` if the first element should come **before** the second,
- `0` if they are **equal**,
- `> 0` if the first should come **after** the second.

**Signature example:**
```c
int cmpstring(const void *a, const void *b);

```

---

## Feature 6 Questions

### How do ANSI escape codes work to produce color in a standard Linux terminal? Show the specific code sequence for printing text in green.

ANSI escape codes are a standard mechanism used to control cursor movement, color, and other display options on terminal emulators. They are sequences of bytes that the terminal interprets as commands rather than characters to be displayed.

* **Structure:** An ANSI escape code always begins with the **Escape character** (ASCII 27 or `\033` in C strings), followed immediately by an open square bracket **`[`**. This combination is called the **Control Sequence Introducer (CSI)**: `\033[`.
* **Parameters:** Following the CSI are numerical parameters separated by semicolons (`;`), which specify the desired graphics rendition (e.g., foreground color, background color, bold).
* **Terminator:** The sequence ends with a letter (e.g., `m`), which is the command that applies the preceding parameters. The `m` command is the **Select Graphic Rendition (SGR)** command, which handles colors and text styles.

#### Code Sequence for Printing Text in Green

The specific SGR code for setting the **standard foreground green color** is $\mathbf{32}$.

The full code sequence to print text in green is:
* `\033[`: Control Sequence Introducer (CSI)
* `32`: Parameter for **Foreground Color Green**
* `m`: SGR (Select Graphic Rendition) command

To print the word "Hello" in green and then **reset** the color (crucial to prevent subsequent text from also being green), you would use:
```c
printf("\033[32mHello\033[0m\n");

```

### To color an executable file, you need to check its permission bits. Explain which bits in the `st_mode` field you need to check to determine if a file is executable by the owner, group, or others.

To determine if a file is executable by the owner, group, or others, you need to check the following specific bits in the **`st_mode`** field of the `struct stat`, using the **bitwise AND operator (`&`)** with the corresponding predefined macros:

| Executable By | `st_mode` Bit (Octal) | Predefined Macro | Condition Check |
| :--- | :--- | :--- | :--- |
| **Owner (User)** | $9^{th}$ bit (0100) | **`S_IXUSR`** | `(statbuf.st_mode & S_IXUSR)` |
| **Group** | $6^{th}$ bit (0010) | **`S_IXGRP`** | `(statbuf.st_mode & S_IXGRP)` |
| **Others** | $3^{rd}$ bit (0001) | **`S_IXOTH`** | `(statbuf.st_mode & S_IXOTH)` |

A file is considered executable for coloring purposes if **any** of these three bits is set. Therefore, the overall check often looks like:

```c
if (statbuf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
    // The file is executable by at least one entity
}

---

## Feature 7 Questions

### In a recursive function, what is a "base case"? In the context of your recursive ls, what is the base case that stops the recursion from continuing forever?

#### What is a "Base Case"?

In a recursive function, the **base case** is the **non-recursive condition** that **stops the function from calling itself further**. It is the point at which the recursion unwinds and returns a result. Without a base case, or with an improperly defined one, a recursive function will lead to an infinite loop, eventually causing a **stack overflow**.

#### Base Case in Recursive `ls`

In the context of a recursive `ls` (implementing the `-R` option), the primary base case that stops the recursion from continuing forever is the condition where the function encounters an entry that **is not a directory**.

The recursive `do_ls()` function will call itself only if the current entry it processes is a directory (and meets other criteria like not being "." or "..").

Therefore, the base cases (the points where recursion stops) are when `do_ls()` processes an entry that is:

* A **regular file**, symbolic link, block device, etc. (i.e., **not a directory**). The function simply prints its details and returns.
* The **current directory** (`.`) or **parent directory** (`..`). These are explicitly skipped to prevent infinite or redundant loops.
* A **directory for which the user lacks read permission** (an error condition).

The most fundamental base case is encountering a **non-directory file type**.


### Explain why it is essential to construct a full path (e.g., `"parent_dir/subdir"`) before making a recursive call. What would happen if you simply called `do_ls("subdir")` from within the `do_ls("parent_dir")` function call?

#### Why Construct a Full Path?

It is essential to construct a **full, absolute, or relative path** (e.g., `"parent_dir/subdir"`) before making a recursive call because system calls like `stat()`, `lstat()`, and `opendir()` typically resolve paths **relative to the process's current working directory (CWD)**, *not* relative to the directory currently being processed by the function.

When `do_ls()` is called, it might open a directory like `"parent_dir"`. Inside this call, it finds an entry named `"subdir"`. The process's CWD often remains unchanged (e.g., the directory where the user initially ran the `ls -R` command).

* If you construct the full path (`"parent_dir/subdir"`), the next recursive call, `do_ls("parent_dir/subdir")`, correctly points to the intended directory regardless of the CWD.

#### What Would Happen If You Called `do_ls("subdir")`?

If you simply called `do_ls("subdir")` from within `do_ls("parent_dir")`, the program would attempt to open a directory named `"subdir"` **relative to the process's CWD**.

This could lead to two critical errors:

1.  **Failure:** If a directory named `"subdir"` does not exist in the CWD, the call to `opendir("subdir")` will fail, and the recursion chain will be broken for that branch.
2.  **Incorrect Recursion/Infinite Loop:** If a directory named `"subdir"` *does* happen to exist in the CWD, the program will process that **wrong directory**. In a worst-case scenario, if the CWD contains a directory with the same name as one of the recursive subdirectories, it could lead to the program jumping to a completely different part of the filesystem or even triggering an **infinite loop** if the path structure is circular.

By constructing the full path, you ensure that the recursive call is operating on the correct, fully qualified location in the file system, independent of the process's CWD.
