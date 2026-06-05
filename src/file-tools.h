#ifndef FILE_TOOLS_H

#define FILE_TOOLS_H

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>

int remove_recursive(const char *path) {
    struct stat st;

    if (lstat(path, &st) != 0)
        return -1;

    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir)
            return -1;

        struct dirent *ent;
        char buf[PATH_MAX];

        while ((ent = readdir(dir)) != NULL) {
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                continue;

            snprintf(buf, sizeof(buf), "%s/%s", path, ent->d_name);
            remove_recursive(buf);
        }

        closedir(dir);
        return rmdir(path);
    }

    return unlink(path);
}

int remove_list(const char *listfile) {
    FILE *fp = fopen(listfile, "r");
    if (!fp)
        return -1;

    char path[PATH_MAX];

    while (fgets(path, sizeof(path), fp)) {
        path[strcspn(path, "\r\n")] = '\0';

        if (path[0] == '\0')
            continue;

        struct stat st;
        if (lstat(path, &st) != 0)
            continue;

        if (S_ISDIR(st.st_mode)) {
            remove_recursive(path);
        }
    }

    fclose(fp);
    return 0;
}

int copy_symlink(const char *src, const char *dst) {
    char buf[4096];

    ssize_t len = readlink(src, buf, sizeof(buf) - 1);
    if (len < 0) return -1;

    buf[len] = '\0';

    unlink(dst);

    return symlink(buf, dst);
}

int copy_file(const char *src, const char *dst, const struct stat *st) {
    int in = open(src, O_RDONLY);
    if (in < 0) return -1;

    int out = open(dst,
                   O_WRONLY | O_CREAT | O_TRUNC,
                   st->st_mode & 07777);

    if (out < 0) {
        close(in);
        return -1;
    }

    char buf[8192];
    ssize_t n;

    while ((n = read(in, buf, sizeof(buf))) > 0) {
        if (write(out, buf, n) != n) {
            close(in);
            close(out);
            return -1;
        }
    }

    close(in);
    close(out);

    chown(dst, st->st_uid, st->st_gid);

    chmod(dst, st->st_mode & 07777);

    struct timespec times[2] = {
        st->st_atim,
        st->st_mtim
    };

    utimensat(0, dst, times, 0);

    return 0;
}

int copy_dir(const char *src, const char *dst_root) {
    DIR *dir = opendir(src);
    if (!dir) return -1;

    struct dirent *ent;
    struct stat st;

    while ((ent = readdir(dir))) {
        if (!strcmp(ent->d_name, ".") ||
            !strcmp(ent->d_name, ".."))
            continue;

        char src_path[4096];
        char dst_path[4096];

        snprintf(src_path, sizeof(src_path), "%s/%s", src, ent->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_root, ent->d_name);

        if (lstat(src_path, &st) < 0)
            continue;

        if (S_ISDIR(st.st_mode)) {
            mkdir(dst_path, st.st_mode & 07777);
            chown(dst_path, st.st_uid, st.st_gid);
            chmod(dst_path, st.st_mode & 07777);
        
            copy_dir(src_path, dst_path);
        }
        else if (S_ISLNK(st.st_mode)) {
            copy_symlink(src_path, dst_path);
        }
        else if (S_ISREG(st.st_mode)) {
            copy_file(src_path, dst_path, &st);
        }
    }

    closedir(dir);
    return 0;
}

#endif
