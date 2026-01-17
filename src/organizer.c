#include "organizer.h"
#include "../external/tinydir.h"
#include "../external/cwalk.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <unistd.h>
#endif

// Limite de sécurité pour les chemins
#define MAX_PATH_LENGTH 2048
#define MAX_EXTENSION_LENGTH 32

typedef struct {
    const char* extension;
    const char* category;
} ExtensionMap;

static ExtensionMap extension_map[] = {
    {".pdf", "Documents"}, {".docx", "Documents"}, {".txt", "Documents"}, 
    {".xlsx", "Documents"}, {".pptx", "Documents"}, {".doc", "Documents"},
    {".odt", "Documents"}, {".rtf", "Documents"},
    
    {".jpg", "Images"}, {".jpeg", "Images"}, {".png", "Images"}, 
    {".gif", "Images"}, {".bmp", "Images"}, {".svg", "Images"}, 
    {".webp", "Images"}, {".ico", "Images"},
    
    {".mp4", "Videos"}, {".mkv", "Videos"}, {".avi", "Videos"}, 
    {".mov", "Videos"}, {".wmv", "Videos"}, {".flv", "Videos"}, 
    {".webm", "Videos"}, {".m4v", "Videos"},
    
    {".mp3", "Music"}, {".wav", "Music"}, {".flac", "Music"}, 
    {".aac", "Music"}, {".ogg", "Music"}, {".wma", "Music"}, 
    {".m4a", "Music"}, {".opus", "Music"},
    
    {".zip", "Archives"}, {".tar", "Archives"}, {".gz", "Archives"}, 
    {".rar", "Archives"}, {".7z", "Archives"}, {".bz2", "Archives"}, 
    {".xz", "Archives"}, {".iso", "Archives"},
    
    {NULL, NULL}
};

// Valide qu'un chemin n'est pas trop long
static bool is_path_safe(const char* path) {
    if (!path) return false;
    return strlen(path) < MAX_PATH_LENGTH;
}

// Valide qu'une extension n'est pas trop longue
static bool is_extension_safe(const char* ext) {
    if (!ext) return false;
    return strlen(ext) < MAX_EXTENSION_LENGTH;
}

// Vérifie qu'un chemin est un dossier existant
static bool is_valid_directory(const char* path) {
    if (!path || !is_path_safe(path)) return false;
    
    struct stat info;
    if (stat(path, &info) != 0) {
        return false;
    }
    return S_ISDIR(info.st_mode);
}

const char* get_category(const char* ext) {
    if (!ext || !is_extension_safe(ext)) {
        return "Autres";
    }
    
    for (int i = 0; extension_map[i].extension != NULL; i++) {
        if (strcasecmp(ext, extension_map[i].extension) == 0) {
            return extension_map[i].category;
        }
    }
    return "Autres";
}

OrganizeResult organize_directory(const char* path) {
    OrganizeResult result = {0, 0, "", 0, 0};
    
    // Initialiser les catégories
    for (int i = 0; i < MAX_CATEGORIES; i++) {
        result.categories[i].name[0] = '\0';
        result.categories[i].count = 0;
    }
    result.category_count = 0;
    
    // Validation du chemin
    if (!path || !is_path_safe(path)) {
        result.errors++;
        snprintf(result.last_error, sizeof(result.last_error), 
                "Erreur: Chemin invalide ou trop long");
        return result;
    }
    
    if (!is_valid_directory(path)) {
        result.errors++;
        snprintf(result.last_error, sizeof(result.last_error), 
                "Erreur: Le chemin n'existe pas ou n'est pas un dossier");
        return result;
    }

    tinydir_dir dir;

    if (tinydir_open(&dir, path) == -1) {
        result.errors++;
        snprintf(result.last_error, sizeof(result.last_error), 
                "Impossible d'ouvrir le dossier: %s", strerror(errno));
        return result;
    }

    while (dir.has_next) {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        // Ignore les dossiers, les fichiers cachés et les fichiers système
        if (!file.is_dir && file.name[0] != '.' && strlen(file.name) > 0) {
            result.total_files_scanned++;
            const char* ext = NULL;
            size_t ext_len = 0;
            
            // Extraction sécurisée de l'extension
            cwk_path_get_extension(file.name, &ext, &ext_len);

            // Validation de l'extension
            if (ext != NULL && ext_len > 0 && is_extension_safe(ext)) {
                char ext_str[MAX_EXTENSION_LENGTH];
                strncpy(ext_str, ext, ext_len);
                ext_str[ext_len] = '\0';
                
                const char* category = get_category(ext_str);
                char dest_dir[MAX_PATH_LENGTH];
                char dest_path[MAX_PATH_LENGTH];

                // Construction sécurisée des chemins
                int len = snprintf(dest_dir, sizeof(dest_dir), "%s/%s", path, category);
                if (len >= sizeof(dest_dir)) {
                    result.errors++;
                    snprintf(result.last_error, sizeof(result.last_error), 
                            "Erreur: Chemin destination trop long pour %s", file.name);
                    continue;
                }
                
                // Créer le dossier s'il n'existe pas
                if (mkdir(dest_dir, 0755) != 0 && errno != EEXIST) {
                    result.errors++;
                    snprintf(result.last_error, sizeof(result.last_error), 
                            "Impossible de créer le dossier: %s", strerror(errno));
                    continue;
                }

                len = snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, file.name);
                if (len >= sizeof(dest_path)) {
                    result.errors++;
                    snprintf(result.last_error, sizeof(result.last_error), 
                            "Erreur: Chemin destination trop long");
                    continue;
                }

                // Vérifier que source et destination sont différents
                if (strcmp(file.path, dest_path) == 0) {
                    continue; // Le fichier est déjà au bon endroit
                }

                // Essayer le déplacement
                if (rename(file.path, dest_path) == 0) {
                    result.files_moved++;
                    
                    // Tracker par catégorie
                    int cat_index = -1;
                    for (int i = 0; i < result.category_count; i++) {
                        if (strcmp(result.categories[i].name, category) == 0) {
                            cat_index = i;
                            break;
                        }
                    }
                    if (cat_index == -1) {
                        if (result.category_count < MAX_CATEGORIES) {
                            cat_index = result.category_count;
                            strncpy(result.categories[cat_index].name, category, sizeof(result.categories[cat_index].name) - 1);
                            result.categories[cat_index].count = 0;
                            result.category_count++;
                        }
                    }
                    if (cat_index >= 0) {
                        result.categories[cat_index].count++;
                    }
                } else {
                    result.errors++;
                    snprintf(result.last_error, sizeof(result.last_error), 
                            "Erreur déplacement %s: %s", file.name, strerror(errno));
                }
            } else if (ext == NULL) {
                // Fichier sans extension - placer dans "Autres"
                result.files_without_extension++;
                char dest_dir[MAX_PATH_LENGTH];
                char dest_path[MAX_PATH_LENGTH];
                
                snprintf(dest_dir, sizeof(dest_dir), "%s/Autres", path);
                mkdir(dest_dir, 0755);
                
                snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, file.name);
                
                if (strcmp(file.path, dest_path) != 0 && rename(file.path, dest_path) == 0) {
                    result.files_moved++;
                    
                    // Tracker catégorie "Autres"
                    int cat_index = -1;
                    for (int i = 0; i < result.category_count; i++) {
                        if (strcmp(result.categories[i].name, "Autres") == 0) {
                            cat_index = i;
                            break;
                        }
                    }
                    if (cat_index == -1) {
                        if (result.category_count < MAX_CATEGORIES) {
                            cat_index = result.category_count;
                            strncpy(result.categories[cat_index].name, "Autres", sizeof(result.categories[cat_index].name) - 1);
                            result.categories[cat_index].count = 0;
                            result.category_count++;
                        }
                    }
                    if (cat_index >= 0) {
                        result.categories[cat_index].count++;
                    }
                } else {
                    result.errors++;
                }
            }
        }

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
    return result;
}
