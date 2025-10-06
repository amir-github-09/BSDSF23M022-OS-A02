

/*
 * Programming Assignment 02: ls v1.6.0
 * Base: v1.5.0 + Feature 7 (Recursive Listing -R)
 *
 * Usage:
 *   $ ./bin/ls
 *   $ ./bin/ls -l
 *   $ ./bin/ls -x
 *   $ ./bin/ls -R
 *   $ ./bin/ls -lR /path
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

// Function declarations
void do_ls(const char *dir, int long_flag, int horizontal_flag, int recursive_flag);
void mode_to_str(mode_t mode, char *str);
char **read_names(const char *dir, size_t *out_count);
void print_column_display_array(char **names, size_t count, const char *dir);
void print_horizontal_display_array(char **names, size_t count, const char *dir);
const char* get_color_for_file(const char *path);
int namecmp(const void *a, const void *b);  // helper for qsort()

// =============================================================
// main()
// =============================================================
int main(int argc, char const *argv[])
{
    int long_flag = 0;
    int horizontal_flag = 0;
    int recursive_flag = 0;
    int opt;

    while ((opt = getopt(argc, (char * const *) argv, "lxR")) != -1)
    {
        switch (opt)
        {
        case 'l': long_flag = 1; break;
        case 'x': horizontal_flag = 1; break;
        case 'R': recursive_flag = 1; break;
        default:
            fprintf(stderr, "Usage: %s [-l] [-x] [-R] [dir...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc)
        do_ls(".", long_flag, horizontal_flag, recursive_flag);
    else
    {
        for (int i = optind; i < argc; i++)
        {
            if (argc - optind > 1)
                printf("%s:\n", argv[i]);
            do_ls(argv[i], long_flag, horizontal_flag, recursive_flag);
            if (i < argc - 1) puts("");
        }
    }

    return 0;
}

// =============================================================
// Convert st_mode to permission string
// =============================================================
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

// =============================================================
// qsort helper: alphabetical comparison
// =============================================================
int namecmp(const void *a, const void *b)
{
    const char *s1 = *(const char **)a;
    const char *s2 = *(const char **)b;
    return strcmp(s1, s2);
}

// =============================================================
// Read all names into dynamic array (sorted alphabetically)
// =============================================================
char **read_names(const char *dir, size_t *out_count)
{
    DIR *dp = opendir(dir);
    if (!dp)
    {
        perror(dir);
        *out_count = 0;
        return NULL;
    }

    struct dirent *entry;
    char **names = NULL;
    size_t count = 0;

    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.') continue; // skip hidden files
        names = realloc(names, (count + 1) * sizeof(char *));
        names[count] = strdup(entry->d_name);
        count++;
    }

    closedir(dp);

    if (count > 0)
        qsort(names, count, sizeof(char *), namecmp);

    *out_count = count;
    return names;
}

// =============================================================
// Color logic (same as v1.5.0)
// =============================================================
const char* get_color_for_file(const char *path)
{
    struct stat st;
    if (lstat(path, &st) == -1) return "\033[0m";

    if (S_ISDIR(st.st_mode)) return "\033[1;34m"; // Blue
    if (S_ISLNK(st.st_mode)) return "\033[1;35m"; // Magenta
    if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) return "\033[1;32m"; // Green
    if (S_ISREG(st.st_mode))
    {
        const char *ext = strrchr(path, '.');
        if (ext && (strcmp(ext, ".tar") == 0 || strcmp(ext, ".gz") == 0 || strcmp(ext, ".zip") == 0))
            return "\033[1;31m"; // Red
    }
    if (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode) || S_ISFIFO(st.st_mode) || S_ISSOCK(st.st_mode))
        return "\033[7m"; // Reverse video

    return "\033[0m";
}

// =============================================================
// Column display (down then across)
// =============================================================
void print_column_display_array(char **names, size_t count, const char *dir)
{
    if (count == 0) return;

    size_t maxlen = 0;
    for (size_t i = 0; i < count; i++)
        if (strlen(names[i]) > maxlen) maxlen = strlen(names[i]);

    struct winsize ws;
    int term_width = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        term_width = ws.ws_col;

    int padding = 2;
    int col_width = maxlen + padding;
    int num_cols = term_width / col_width;
    if (num_cols < 1) num_cols = 1;
    int num_rows = (count + num_cols - 1) / num_cols;

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            int index = r + c * num_rows;
            if (index >= (int)count) continue;

            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir, names[index]);
            const char *color = get_color_for_file(path);
            printf("%s%-*s\033[0m", color, col_width, names[index]);
        }
        printf("\n");
    }
}

// =============================================================
// Horizontal display (-x)
// =============================================================
void print_horizontal_display_array(char **names, size_t count, const char *dir)
{
    if (count == 0) return;

    size_t maxlen = 0;
    for (size_t i = 0; i < count; i++)
        if (strlen(names[i]) > maxlen) maxlen = strlen(names[i]);

    struct winsize ws;
    int term_width = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        term_width = ws.ws_col;

    int padding = 2;
    int col_width = maxlen + padding;
    int num_cols = term_width / col_width;
    if (num_cols < 1) num_cols = 1;
    int num_rows = (count + num_cols - 1) / num_cols;

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            int index = c + r * num_cols;
            if (index >= (int)count) continue;

            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir, names[index]);
            const char *color = get_color_for_file(path);
            printf("%s%-*s\033[0m", color, col_width, names[index]);
        }
        printf("\n");
    }
}

// =============================================================
// Recursive do_ls() — Feature 7 (-R)
// =============================================================
void do_ls(const char *dir, int long_flag, int horizontal_flag, int recursive_flag)
{
    size_t count;
    char **names = read_names(dir, &count);
    if (!names || count == 0) return;

   // if (recursive_flag){
    //	printf("%s:\n", dir);  // print directory header (for -R)
    //}

    if (long_flag)
    {
        struct stat st;
        long total_blocks = 0;

        for (size_t i = 0; i < count; i++)
        {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir, names[i]);
            if (lstat(path, &st) == -1) continue;
            total_blocks += st.st_blocks;
        }

        printf("total %ld\n", total_blocks / 2);

        for (size_t i = 0; i < count; i++)
        {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir, names[i]);
            if (lstat(path, &st) == -1) continue;

            char perms[11];
            mode_to_str(st.st_mode, perms);

            struct passwd *pw = getpwuid(st.st_uid);
            struct group  *gr = getgrgid(st.st_gid);

            char timebuf[64];
            struct tm *tm = localtime(&st.st_mtime);
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);

            const char *color = get_color_for_file(path);
            printf("%s %2lu %s %s %8lld %s %s%s\033[0m\n",
                perms,
                (unsigned long)st.st_nlink,
                pw ? pw->pw_name : "?",
                gr ? gr->gr_name : "?",
                (long long)st.st_size,
                timebuf,
                color,
                names[i]);
        }
    }
    else
    {
        if (horizontal_flag)
            print_horizontal_display_array(names, count, dir);
        else
            print_column_display_array(names, count, dir);
    }

    // Recursively list subdirectories
    if (recursive_flag)
    {
        for (size_t i = 0; i < count; i++)
        {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir, names[i]);

            struct stat st;
            if (lstat(path, &st) == -1) continue;

            if (S_ISDIR(st.st_mode))
            {
                if (strcmp(names[i], ".") == 0 || strcmp(names[i], "..") == 0)
                    continue;

                printf("\n");
		printf("%s:\n",path);
                do_ls(path, long_flag, horizontal_flag, recursive_flag);
            }
        }
    }

    for (size_t i = 0; i < count; i++) free(names[i]);
    free(names);
}
