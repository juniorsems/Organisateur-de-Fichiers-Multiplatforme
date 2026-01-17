#ifndef ORGANIZER_H
#define ORGANIZER_H

#define MAX_CATEGORIES 10

typedef struct {
    char name[64];
    int count;
} CategoryStats;

typedef struct {
    int files_moved;
    int errors;
    char last_error[256];
    int total_files_scanned;
    int files_without_extension;
    CategoryStats categories[MAX_CATEGORIES];
    int category_count;
} OrganizeResult;

OrganizeResult organize_directory(const char* path);

#endif
