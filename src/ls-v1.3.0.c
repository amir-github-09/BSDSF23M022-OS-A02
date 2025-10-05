

/*
* Programming Assignment 02: ls v1.3.0
* Base: v1.2.0 + Feature 4 (-x Horizontal Column Display)
*
* Usage:
*   $ ./bin/ls
*   $ ./bin/ls -l
*   $ ./bin/ls -x
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/types.h>

extern int errno;

void do_ls(const char *dir, int long_flag, int horizontal_flag);
void mode_to_str(mode_t mode, char *str);
void print_column_display(const char *dir);
void print_horizontal_display(const char *dir); // new helper for -x

// ──────────────────────────────────────────────
// main()
// ──────────────────────────────────────────────
int main(int argc, char const *argv[])
{
    int long_flag = 0;
    int horizontal_flag = 0;  // for -x option
    int opt;

    // Detect -l and -x options
    while ((opt = getopt(argc, (char * const *)argv, "lx")) != -1)
    {
        switch (opt)
        {
        case 'l':
            long_flag = 1;
            break;
        case 'x':
            horizontal_flag = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-l] [-x] [dir...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Remaining args after options
    if (optind == argc)
    {
        do_ls(".", long_flag, horizontal_flag);
    }
    else
    {
        for (int i = optind; i < argc; i++)
        {
            printf("Directory listing of %s:\n", argv[i]);
            do_ls(argv[i], long_flag, horizontal_flag);
            puts("");
        }
    }
    return 0;
}

// ──────────────────────────────────────────────
// Convert st_mode to permission string
// ──────────────────────────────────────────────
void mode_to_str(mode_t mode, char *str)
{
    str[0] = S_ISDIR(mode) ? 'd' :
             S_ISLNK(mode) ? 'l' :
             S_ISCHR(mode) ? 'c' :
             S_ISBLK(mode) ? 'b' :
             S_ISFIFO(mode)? 'p' :
             S_ISSOCK(mode)? 's' : '-';

    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
}

// ──────────────────────────────────────────────
// do_ls() — handles -l, default, and -x display
// ──────────────────────────────────────────────
void do_ls(const char *dir, int long_flag, int horizontal_flag)
{
    DIR *dp;
    struct dirent *entry;
    struct stat st;
    char path[PATH_MAX];
    long total_blocks = 0;

    dp = opendir(dir);
    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    errno = 0;

    // First pass — calculate total blocks
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.') continue;
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
        if (lstat(path, &st) == -1) continue;
        total_blocks += st.st_blocks;
    }

    // Display total (like ls)
    if (long_flag)
        printf("total %ld\n", total_blocks / 2);

    // Second pass — actual listing
    rewinddir(dp);
    errno = 0;

    if (long_flag)
    {
        while ((entry = readdir(dp)) != NULL)
        {
            if (entry->d_name[0] == '.') continue;

            snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
            if (lstat(path, &st) == -1)
            {
                perror("lstat");
                continue;
            }

            char perms[11];
            mode_to_str(st.st_mode, perms);

            struct passwd *pw = getpwuid(st.st_uid);
            struct group  *gr = getgrgid(st.st_gid);

            char timebuf[64];
            struct tm *tm = localtime(&st.st_mtime);
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);

            printf("%s %2lu %s %s %8lld %s %s\n",
                   perms,
                   (unsigned long)st.st_nlink,
                   pw ? pw->pw_name : "?",
                   gr ? gr->gr_name : "?",
                   (long long)st.st_size,
                   timebuf,
                   entry->d_name);
        }
    }
    else
    {
        // Default = vertical columns; -x = horizontal
        closedir(dp);
        if (horizontal_flag)
            print_horizontal_display(dir);
        else
            print_column_display(dir);
        return;
    }

    if (errno != 0)
        perror("readdir");

    closedir(dp);
}

// ──────────────────────────────────────────────
// Feature-3: Column Display ("down then across")
// ──────────────────────────────────────────────
void print_column_display(const char *dir)
{
    DIR *dp;
    struct dirent *entry;
    dp = opendir(dir);
    if (!dp)
    {
        perror(dir);
        return;
    }

    // Read all names
    char **names = NULL;
    size_t count = 0, maxlen = 0;
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.') continue;
        names = realloc(names, (count + 1) * sizeof(char *));
        names[count] = strdup(entry->d_name);
        size_t len = strlen(entry->d_name);
        if (len > maxlen) maxlen = len;
        count++;
    }
    closedir(dp);

    if (count == 0) { free(names); return; }

    struct winsize ws;
    int term_width = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        term_width = ws.ws_col;

    int padding = 2;
    int col_width = maxlen + padding;
    int num_cols = term_width / col_width;
    if (num_cols < 1) num_cols = 1;
    int num_rows = (count + num_cols - 1) / num_cols;

    // Down then across
    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            int index = r + c * num_rows;
            if (index >= (int)count) continue;
            if (c == num_cols - 1)
                printf("%s", names[index]);
            else
                printf("%-*s", col_width, names[index]);
        }
        printf("\n");
    }

    for (size_t i = 0; i < count; i++)
        free(names[i]);
    free(names);
}

// ──────────────────────────────────────────────
// Feature-4: Horizontal Column Display (-x)
// ──────────────────────────────────────────────
void print_horizontal_display(const char *dir)
{
    DIR *dp;
    struct dirent *entry;
    dp = opendir(dir);
    if (!dp)
    {
        perror(dir);
        return;
    }

    // Step 1: Read names
    char **names = NULL;
    size_t count = 0, maxlen = 0;
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.') continue;
        names = realloc(names, (count + 1) * sizeof(char *));
        names[count] = strdup(entry->d_name);
        size_t len = strlen(entry->d_name);
        if (len > maxlen) maxlen = len;
        count++;
    }
    closedir(dp);

    if (count == 0) { free(names); return; }

    // Step 2: Terminal width
    struct winsize ws;
    int term_width = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        term_width = ws.ws_col;

    int padding = 2;
    int col_width = maxlen + padding;
    int num_cols = term_width / col_width;
    if (num_cols < 1) num_cols = 1;
    int num_rows = (count + num_cols - 1) / num_cols;

    // Step 3: Across then down (row-major)
    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            int index = c + r * num_cols;
            if (index >= (int)count) continue;

            if (c == num_cols - 1)
                printf("%s", names[index]);
            else
                printf("%-*s", col_width, names[index]);
        }
        printf("\n");
    }

    for (size_t i = 0; i < count; i++)
        free(names[i]);
    free(names);
}
