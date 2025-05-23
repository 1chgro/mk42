#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_PATH 512

int ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr) return 0;
    return strcmp(str + lenstr - lensuffix, suffix) == 0;
}

void collect_files_with_extension(const char *ext, char *buffer, size_t size) {
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATH];
    char *buf_ptr = buffer;
    size_t remaining = size;

    dir = opendir(".");
    if (!dir) {
        perror("opendir failed");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (entry->d_type == DT_DIR) {
            DIR *subdir = opendir(entry->d_name);
            if (!subdir) continue;

            struct dirent *subentry;
            while ((subentry = readdir(subdir)) != NULL) {
                if (ends_with(subentry->d_name, ext)) {
                    snprintf(path, sizeof(path), "%s/%s", entry->d_name, subentry->d_name);
                    int written = snprintf(buf_ptr, remaining, "%s ", path);
                    if (written < 0 || (size_t)written >= remaining) break;
                    buf_ptr += written;
                    remaining -= written;
                }
            }
            closedir(subdir);
        } else if (ends_with(entry->d_name, ext)) {
            int written = snprintf(buf_ptr, remaining, "%s ", entry->d_name);
            if (written < 0 || (size_t)written >= remaining) break;
            buf_ptr += written;
            remaining -= written;
        }
    }
    closedir(dir);
}

void write_makefile(const char *project_name)
{
    FILE *fp = fopen("Makefile", "w");
    if (!fp) {
        perror("Error creating Makefile");
        exit(EXIT_FAILURE);
    }

    char src_files[8192] = {0};
    char header_files[8192] = {0};
    collect_files_with_extension(".c", src_files, sizeof(src_files));
    collect_files_with_extension(".h", header_files, sizeof(header_files));

    fprintf(fp, "NAME = %s\n", project_name);
    fprintf(fp, "CC = cc\n");
    fprintf(fp, "CFLAGS = -Wall -Wextra -Werror\n\n");

    fprintf(fp, "SRC = %s\n", src_files);
    fprintf(fp, "OBJ = $(SRC:.c=.o)\n");
    fprintf(fp, "HEADER = %s\n\n", header_files);

    fprintf(fp, "all: $(NAME)\n\n");

    fprintf(fp, "$(NAME): $(OBJ) $(HEADER)\n");
    fprintf(fp, "\t$(CC) $(CFLAGS) -o $(NAME) $(OBJ)\n\n");

    fprintf(fp, "%%.o: %%.c $(HEADER)\n");
    fprintf(fp, "\t$(CC) $(CFLAGS) -c $< -o $@\n\n");

    fprintf(fp, "clean:\n");
    fprintf(fp, "\trm -f $(OBJ)\n\n");

    fprintf(fp, "fclean: clean\n");
    fprintf(fp, "\trm -f $(NAME)\n\n");

    fprintf(fp, "re: fclean all\n\n");

    fprintf(fp, ".PHONY: all clean fclean re\n");

    fclose(fp);
    printf("Makefile generated for project: %s\n", project_name);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <project_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    write_makefile(argv[1]);
    return EXIT_SUCCESS;
}
