#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_PATH 4096  // Increased buffer size for safety

typedef struct s_list {
    char *filepath;
    struct s_list *next;
} t_list;

void free_list(t_list *head) {
    while (head) {
        t_list *tmp = head;
        head = head->next;
        free(tmp->filepath);
        free(tmp);
    }
}

void list_append(t_list **head, const char *path) {
    t_list *new_node = malloc(sizeof(t_list));
    if (!new_node) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_node->filepath = strdup(path);
    if (!new_node->filepath) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    new_node->next = NULL;

    if (!*head) {
        *head = new_node;
    } else {
        t_list *cur = *head;
        while (cur->next)
            cur = cur->next;
        cur->next = new_node;
    }
}

int is_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1)
        return 0;
    return S_ISDIR(st.st_mode);
}

int has_extension(const char *filename, const char *ext) {
    size_t len_filename = strlen(filename);
    size_t len_ext = strlen(ext);
    if (len_filename < len_ext)
        return 0;
    return strcmp(filename + len_filename - len_ext, ext) == 0;
}

void collect_files(const char *basepath, const char *relpath, t_list **c_files, t_list **h_files) {
    char path[MAX_PATH];
    DIR *dir;
    struct dirent *entry;

    // Construct current directory path
    int n;
    if (strlen(relpath) == 0) {
        n = snprintf(path, sizeof(path), "%s", basepath);
    } else {
        n = snprintf(path, sizeof(path), "%s/%s", basepath, relpath);
    }
    if (n < 0 || n >= (int)sizeof(path)) {
        fprintf(stderr, "Path too long: '%s/%s'\n", basepath, relpath);
        return;
    }

    dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "Error opening directory '%s': %s\n", path, strerror(errno));
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char new_relpath[MAX_PATH];
        if (strlen(relpath) == 0) {
            n = snprintf(new_relpath, sizeof(new_relpath), "%s", entry->d_name);
        } else {
            n = snprintf(new_relpath, sizeof(new_relpath), "%s/%s", relpath, entry->d_name);
        }
        if (n < 0 || n >= (int)sizeof(new_relpath)) {
            fprintf(stderr, "Relative path too long: '%s/%s'\n", relpath, entry->d_name);
            continue;
        }

        char full_path[MAX_PATH];
        n = snprintf(full_path, sizeof(full_path), "%s/%s", basepath, new_relpath);
        if (n < 0 || n >= (int)sizeof(full_path)) {
            fprintf(stderr, "Full path too long: '%s/%s'\n", basepath, new_relpath);
            continue;
        }

        if (is_dir(full_path)) {
            collect_files(basepath, new_relpath, c_files, h_files);
        } else {
            if (has_extension(entry->d_name, ".c")) {
                list_append(c_files, new_relpath);
            } else if (has_extension(entry->d_name, ".h")) {
                list_append(h_files, new_relpath);
            }
        }
    }
    closedir(dir);
}

// The rest of your code unchanged


void source_to_object(char *dest, const char *src) {
    strcpy(dest, src);
    size_t len = strlen(dest);
    if (len >= 2 && strcmp(dest + len - 2, ".c") == 0) {
        dest[len - 2] = '\0';
        strcat(dest, ".o");
    }
}


// ... includes, structs, helpers as before ...

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <executable_name>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *exe_name = argv[1];
    t_list *c_files = NULL;
    t_list *h_files = NULL;

    collect_files(".", "", &c_files, &h_files);

    if (!c_files) {
        fprintf(stderr, "No .c files found.\n");
        return EXIT_FAILURE;
    }

    FILE *mk = fopen("Makefile", "w");
    if (!mk) {
        perror("fopen");
        free_list(c_files);
        free_list(h_files);
        return EXIT_FAILURE;
    }

    fprintf(mk, "NAME = %s\n", exe_name);
    fprintf(mk, "CC = cc\n");
    fprintf(mk, "CFLAGS = -Wall -Wextra -Werror\n\n");

    fprintf(mk, "SRCS =");
    for (t_list *cur = c_files; cur != NULL; cur = cur->next) {
        fprintf(mk, " %s", cur->filepath);
    }
    fprintf(mk, "\n");

    fprintf(mk, "OBJS = $(SRCS:.c=.o)\n\n");

    fprintf(mk, "HEADER =");
    for (t_list *cur = h_files; cur != NULL; cur = cur->next) {
        fprintf(mk, " %s", cur->filepath);
    }
    fprintf(mk, "\n\n");

    fprintf(mk, "all: $(NAME)\n\n");

    fprintf(mk, "$(NAME): $(OBJS) $(HEADER)\n");
    fprintf(mk, "\t$(CC) $(CFLAGS) -o $(NAME) $(OBJS)\n\n");

    fprintf(mk, "%%.o: %%.c $(HEADER)\n");
    fprintf(mk, "\t$(CC) $(CFLAGS) -c $< -o $@\n\n");

    fprintf(mk, "clean:\n");
    fprintf(mk, "\trm -f $(OBJS)\n\n");

    fprintf(mk, "fclean: clean\n");
    fprintf(mk, "\trm -f $(NAME)\n\n");

    fprintf(mk, "re: fclean all\n");

    fclose(mk);
    free_list(c_files);
    free_list(h_files);

    printf("Makefile generated successfully for '%s'.\n", exe_name);
    return EXIT_SUCCESS;
}
