#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <nfd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "organizer.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;

typedef enum {
    STATE_IDLE,
    STATE_CONFIRMATION,
    STATE_ORGANIZING,
    STATE_RESULTS
} AppState;

SDL_Color COLOR_BG_DARK = {18, 18, 24, 255};
SDL_Color COLOR_BG_LIGHT = {35, 35, 45, 255};
SDL_Color COLOR_CARD = {50, 50, 65, 255};
SDL_Color COLOR_CARD_HOVER = {60, 60, 80, 255};
SDL_Color COLOR_ACCENT_BLUE = {0, 150, 255, 255};
SDL_Color COLOR_ACCENT_BLUE_HOVER = {50, 170, 255, 255};
SDL_Color COLOR_SUCCESS = {76, 175, 80, 255};
SDL_Color COLOR_ERROR = {244, 67, 54, 255};
SDL_Color COLOR_WARNING = {255, 152, 0, 255};
SDL_Color COLOR_TEXT_PRIMARY = {245, 245, 250, 255};
SDL_Color COLOR_TEXT_SECONDARY = {180, 180, 200, 255};
SDL_Color COLOR_TEXT_TERTIARY = {130, 130, 150, 255};
SDL_Color COLOR_BORDER = {80, 80, 100, 255};

typedef struct {
    SDL_Rect rect;
    bool hovered;
    const char* text;
    SDL_Color color;
    SDL_Color color_hover;
} Button;

typedef struct {
    char title[256];
    char message[512];
    SDL_Color color;
    int display_ticks;
    bool visible;
} Toast;

typedef struct {
    char path[512];
    time_t timestamp;
} HistoryItem;

typedef struct {
    HistoryItem items[10];
    int count;
} History;

AppState app_state = STATE_IDLE;
char selected_path[512] = "";
OrganizeResult last_result = {0};
Toast current_toast = {{0}, {0}, {0, 0, 0, 0}, 0, false};
History history = {0};

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color, bool center) {
    if (!text || !font || strlen(text) == 0) return;
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) { SDL_FreeSurface(surface); return; }
    SDL_Rect dst = {x, y, surface->w, surface->h};
    if (center) { dst.x -= surface->w / 2; dst.y -= surface->h / 2; }
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void fill_rect_with_alpha(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &rect);
}

void draw_shadow(SDL_Renderer* renderer, SDL_Rect rect, int shadow_size) {
    SDL_Color shadow = {0, 0, 0, 40};
    for (int i = 0; i < shadow_size; i++) {
        shadow.a = (40 * (shadow_size - i)) / shadow_size;
        SDL_Rect shadow_rect = {rect.x + i, rect.y + i, rect.w, rect.h};
        fill_rect_with_alpha(renderer, shadow_rect, shadow);
    }
}

void draw_border(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color color, int thickness) {
    for (int i = 0; i < thickness; i++) {
        SDL_Rect border = {rect.x + i, rect.y + i, rect.w - 2*i, rect.h - 2*i};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawRect(renderer, &border);
    }
}

TTF_Font* load_font_safe(const char* font_name, int size) {
    TTF_Font* font = TTF_OpenFont(font_name, size);
    if (font) return font;
    char path[512];
    snprintf(path, sizeof(path), "../%s", font_name);
    font = TTF_OpenFont(path, size);
    if (font) return font;
    snprintf(path, sizeof(path), "assets/%s", font_name);
    font = TTF_OpenFont(path, size);
    if (font) return font;
    snprintf(path, sizeof(path), "../assets/%s", font_name);
    return TTF_OpenFont(path, size);
}

void show_toast(const char* title, const char* message, SDL_Color color, int duration_ms) {
    strncpy(current_toast.title, title, sizeof(current_toast.title) - 1);
    strncpy(current_toast.message, message, sizeof(current_toast.message) - 1);
    current_toast.color = color;
    current_toast.display_ticks = SDL_GetTicks() + duration_ms;
    current_toast.visible = true;
}

void render_toast(SDL_Renderer* renderer, TTF_Font* font_medium, TTF_Font* font_small) {
    if (!current_toast.visible) return;
    int current_ticks = SDL_GetTicks();
    if (current_ticks > current_toast.display_ticks) { current_toast.visible = false; return; }
    int remaining = current_toast.display_ticks - current_ticks;
    int alpha = (remaining < 500) ? (remaining / 5) : 255;
    SDL_Rect toast_rect = {20, SCREEN_HEIGHT - 130, 360, 110};
    draw_shadow(renderer, toast_rect, 4);
    SDL_Color bg = current_toast.color;
    bg.a = alpha;
    fill_rect_with_alpha(renderer, toast_rect, bg);
    draw_border(renderer, toast_rect, COLOR_TEXT_PRIMARY, 1);
    SDL_Color text_color = COLOR_TEXT_PRIMARY;
    text_color.a = alpha;
    render_text(renderer, font_medium, current_toast.title, 40, SCREEN_HEIGHT - 110, text_color, false);
    render_text(renderer, font_small, current_toast.message, 40, SCREEN_HEIGHT - 75, COLOR_TEXT_SECONDARY, false);
}

void add_to_history(const char* path) {
    if (history.count < 10) {
        for (int i = history.count; i > 0; i--) history.items[i] = history.items[i-1];
    } else {
        for (int i = 9; i > 0; i--) history.items[i] = history.items[i-1];
        history.count = 10;
    }
    strncpy(history.items[0].path, path, sizeof(history.items[0].path) - 1);
    history.items[0].timestamp = time(NULL);
    if (history.count < 10) history.count++;
}

void render_confirmation_dialog(SDL_Renderer* renderer, TTF_Font* font_large, TTF_Font* font_medium, TTF_Font* font_small, Button* btn_confirm, Button* btn_cancel) {
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Color overlay_color = {0, 0, 0, 180};
    fill_rect_with_alpha(renderer, overlay, overlay_color);
    
    int dialog_width = 500, dialog_height = 300;
    SDL_Rect dialog = {(SCREEN_WIDTH - dialog_width) / 2, (SCREEN_HEIGHT - dialog_height) / 2, dialog_width, dialog_height};
    draw_shadow(renderer, dialog, 6);
    fill_rect_with_alpha(renderer, dialog, COLOR_CARD);
    draw_border(renderer, dialog, COLOR_ACCENT_BLUE, 2);
    
    render_text(renderer, font_large, "⚠️  Confirmation", SCREEN_WIDTH/2, dialog.y + 30, COLOR_TEXT_PRIMARY, true);
    render_text(renderer, font_medium, "Êtes-vous sûr de vouloir", SCREEN_WIDTH/2, dialog.y + 80, COLOR_TEXT_SECONDARY, true);
    render_text(renderer, font_medium, "organiser ce dossier ?", SCREEN_WIDTH/2, dialog.y + 110, COLOR_TEXT_SECONDARY, true);
    
    char path_display[100];
    if (strlen(selected_path) > 70) {
        strncpy(path_display, selected_path, 67);
        strcpy(path_display + 67, "...");
    } else {
        strcpy(path_display, selected_path);
    }
    render_text(renderer, font_small, path_display, SCREEN_WIDTH/2, dialog.y + 155, COLOR_TEXT_TERTIARY, true);
    
    SDL_Color btn_confirm_color = btn_confirm->hovered ? COLOR_SUCCESS : COLOR_ACCENT_BLUE;
    SDL_Color btn_cancel_color = btn_cancel->hovered ? COLOR_ERROR : COLOR_TEXT_SECONDARY;
    fill_rect_with_alpha(renderer, btn_confirm->rect, btn_confirm_color);
    draw_border(renderer, btn_confirm->rect, btn_confirm_color, 1);
    render_text(renderer, font_medium, "Organiser", btn_confirm->rect.x + btn_confirm->rect.w/2, btn_confirm->rect.y + btn_confirm->rect.h/2, COLOR_TEXT_PRIMARY, true);
    fill_rect_with_alpha(renderer, btn_cancel->rect, btn_cancel_color);
    draw_border(renderer, btn_cancel->rect, btn_cancel_color, 1);
    render_text(renderer, font_medium, "Annuler", btn_cancel->rect.x + btn_cancel->rect.w/2, btn_cancel->rect.y + btn_cancel->rect.h/2, COLOR_TEXT_PRIMARY, true);
}

void render_results_panel(SDL_Renderer* renderer, TTF_Font* font_large, TTF_Font* font_medium, TTF_Font* font_small, Button* btn_finish) {
    SDL_Rect panel = {30, 150, SCREEN_WIDTH - 60, 450};
    draw_shadow(renderer, panel, 6);
    fill_rect_with_alpha(renderer, panel, COLOR_CARD);
    draw_border(renderer, panel, COLOR_BORDER, 1);
    
    render_text(renderer, font_large, "✨ Résultats de l'organisation", panel.x + 30, panel.y + 30, COLOR_SUCCESS, false);
    
    int y_offset = 80;
    char stats[256];
    snprintf(stats, sizeof(stats), "📊 Fichiers traités: %d / %d", last_result.files_moved, last_result.total_files_scanned);
    render_text(renderer, font_medium, stats, panel.x + 30, panel.y + y_offset, COLOR_TEXT_PRIMARY, false);
    
    if (last_result.errors > 0) {
        snprintf(stats, sizeof(stats), "⚠️  Erreurs: %d", last_result.errors);
        render_text(renderer, font_small, stats, panel.x + 30, panel.y + y_offset + 35, COLOR_ERROR, false);
    }
    
    y_offset += 80;
    render_text(renderer, font_medium, "📋 Par catégorie:", panel.x + 30, panel.y + y_offset, COLOR_ACCENT_BLUE, false);
    y_offset += 40;
    
    for (int i = 0; i < last_result.category_count; i++) {
        if (i >= 8) { render_text(renderer, font_small, "...", panel.x + 30, panel.y + y_offset, COLOR_TEXT_TERTIARY, false); break; }
        snprintf(stats, sizeof(stats), "  • %s: %d fichiers", last_result.categories[i].name, last_result.categories[i].count);
        SDL_Color cat_color = (i % 2 == 0) ? COLOR_TEXT_PRIMARY : COLOR_TEXT_SECONDARY;
        render_text(renderer, font_small, stats, panel.x + 50, panel.y + y_offset, cat_color, false);
        y_offset += 32;
    }
    
    SDL_Color btn_color = btn_finish->hovered ? COLOR_ACCENT_BLUE_HOVER : COLOR_ACCENT_BLUE;
    fill_rect_with_alpha(renderer, btn_finish->rect, btn_color);
    draw_border(renderer, btn_finish->rect, btn_color, 1);
    render_text(renderer, font_medium, "✓ Terminer", btn_finish->rect.x + btn_finish->rect.w/2, btn_finish->rect.y + btn_finish->rect.h/2, COLOR_TEXT_PRIMARY, true);
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0 || NFD_Init() != NFD_OKAY) return 1;
    
    SDL_Window* window = SDL_CreateWindow("🗂️  Organisateur de Fichiers - Pro Edition", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!window || !renderer) return 1;
    
    TTF_Font* font_xlarge = load_font_safe("font.ttf", 40);
    TTF_Font* font_large = load_font_safe("font.ttf", 32);
    TTF_Font* font_medium = load_font_safe("font.ttf", 20);
    TTF_Font* font_small = load_font_safe("font.ttf", 15);
    TTF_Font* font_tiny = load_font_safe("font.ttf", 12);
    if (!font_large || !font_medium || !font_small) return 1;
    
    Button btn_select = {{SCREEN_WIDTH/2 - 140, 250, 280, 60}, false, "📁 Choisir un dossier", COLOR_ACCENT_BLUE, COLOR_ACCENT_BLUE_HOVER};
    Button btn_reset = {{SCREEN_WIDTH/2 - 140, 330, 280, 60}, false, "🔄 Réinitialiser", COLOR_TEXT_SECONDARY, COLOR_TEXT_PRIMARY};
    Button btn_history = {{SCREEN_WIDTH/2 - 140, 410, 280, 60}, false, "⏱️  Historique", COLOR_CARD, COLOR_CARD_HOVER};
    Button btn_confirm = {{SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 + 80, 120, 50}, false, "Confirmer", COLOR_SUCCESS, COLOR_SUCCESS};
    Button btn_cancel = {{SCREEN_WIDTH/2 + 20, SCREEN_HEIGHT/2 + 80, 120, 50}, false, "Annuler", COLOR_ERROR, COLOR_ERROR};
    Button btn_finish = {{SCREEN_WIDTH/2 - 100, 570, 200, 50}, false, "Terminer", COLOR_ACCENT_BLUE, COLOR_ACCENT_BLUE_HOVER};
    
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        
        btn_select.hovered = (mouse_x >= btn_select.rect.x && mouse_x <= btn_select.rect.x + btn_select.rect.w && mouse_y >= btn_select.rect.y && mouse_y <= btn_select.rect.y + btn_select.rect.h);
        btn_reset.hovered = (mouse_x >= btn_reset.rect.x && mouse_x <= btn_reset.rect.x + btn_reset.rect.w && mouse_y >= btn_reset.rect.y && mouse_y <= btn_reset.rect.y + btn_reset.rect.h);
        btn_history.hovered = (mouse_x >= btn_history.rect.x && mouse_x <= btn_history.rect.x + btn_history.rect.w && mouse_y >= btn_history.rect.y && mouse_y <= btn_history.rect.y + btn_history.rect.h);
        
        if (app_state == STATE_CONFIRMATION) {
            btn_confirm.hovered = (mouse_x >= btn_confirm.rect.x && mouse_x <= btn_confirm.rect.x + btn_confirm.rect.w && mouse_y >= btn_confirm.rect.y && mouse_y <= btn_confirm.rect.y + btn_confirm.rect.h);
            btn_cancel.hovered = (mouse_x >= btn_cancel.rect.x && mouse_x <= btn_cancel.rect.x + btn_cancel.rect.w && mouse_y >= btn_cancel.rect.y && mouse_y <= btn_cancel.rect.y + btn_cancel.rect.h);
        }
        
        if (app_state == STATE_RESULTS) {
            btn_finish.hovered = (mouse_x >= btn_finish.rect.x && mouse_x <= btn_finish.rect.x + btn_finish.rect.w && mouse_y >= btn_finish.rect.y && mouse_y <= btn_finish.rect.y + btn_finish.rect.h);
        }

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                if (app_state == STATE_CONFIRMATION || app_state == STATE_RESULTS) app_state = STATE_IDLE;
            }
            
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (app_state == STATE_IDLE) {
                    if (btn_select.hovered) {
                        nfdchar_t *outPath = NULL;
                        if (NFD_PickFolder(&outPath, NULL) == NFD_OKAY && outPath) {
                            strncpy(selected_path, outPath, sizeof(selected_path) - 1);
                            app_state = STATE_CONFIRMATION;
                            NFD_FreePath(outPath);
                        }
                    } else if (btn_reset.hovered && strlen(selected_path) > 0) {
                        selected_path[0] = '\0';
                        memset(&last_result, 0, sizeof(last_result));
                    } else if (btn_history.hovered && history.count > 0) {
                        strncpy(selected_path, history.items[0].path, sizeof(selected_path) - 1);
                        app_state = STATE_CONFIRMATION;
                    }
                } else if (app_state == STATE_CONFIRMATION) {
                    if (btn_confirm.hovered) {
                        app_state = STATE_ORGANIZING;
                        last_result = organize_directory(selected_path);
                        add_to_history(selected_path);
                        app_state = STATE_RESULTS;
                        show_toast("✅ Succès", "Organisation terminée !", COLOR_SUCCESS, 3000);
                    } else if (btn_cancel.hovered) {
                        app_state = STATE_IDLE;
                    }
                } else if (app_state == STATE_RESULTS) {
                    if (btn_finish.hovered) {
                        app_state = STATE_IDLE;
                        selected_path[0] = '\0';
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, COLOR_BG_DARK.r, COLOR_BG_DARK.g, COLOR_BG_DARK.b, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            int intensity = 16 + (i * 35) / SCREEN_HEIGHT;
            SDL_SetRenderDrawColor(renderer, intensity, intensity, intensity + 12, 255);
            SDL_RenderDrawLine(renderer, 0, i, SCREEN_WIDTH, i);
        }

        if (app_state == STATE_IDLE || app_state == STATE_CONFIRMATION) {
            SDL_Rect header_rect = {30, 30, SCREEN_WIDTH - 60, 90};
            draw_shadow(renderer, header_rect, 3);
            fill_rect_with_alpha(renderer, header_rect, COLOR_CARD);
            draw_border(renderer, header_rect, COLOR_BORDER, 1);
            
            render_text(renderer, font_xlarge, "🗂️  Organisateur de Fichiers", SCREEN_WIDTH/2, 50, COLOR_ACCENT_BLUE, true);
            render_text(renderer, font_tiny, "Organisez automatiquement vos fichiers par catégorie", SCREEN_WIDTH/2, 90, COLOR_TEXT_TERTIARY, true);

            if (strlen(selected_path) > 0 && app_state == STATE_IDLE) {
                SDL_Rect path_rect = {30, 140, SCREEN_WIDTH - 60, 80};
                fill_rect_with_alpha(renderer, path_rect, COLOR_BG_LIGHT);
                render_text(renderer, font_medium, "📂 Dossier actuel:", 50, 155, COLOR_ACCENT_BLUE, false);
                char path_display[100];
                if (strlen(selected_path) > 90) {
                    strncpy(path_display, selected_path, 87);
                    strcpy(path_display + 87, "...");
                } else {
                    strcpy(path_display, selected_path);
                }
                render_text(renderer, font_small, path_display, 50, 185, COLOR_TEXT_PRIMARY, false);
            }

            if (app_state == STATE_IDLE) {
                SDL_Color btn_select_color = btn_select.hovered ? COLOR_ACCENT_BLUE_HOVER : COLOR_ACCENT_BLUE;
                fill_rect_with_alpha(renderer, btn_select.rect, btn_select_color);
                draw_border(renderer, btn_select.rect, btn_select_color, 1);
                render_text(renderer, font_medium, btn_select.text, btn_select.rect.x + btn_select.rect.w/2, btn_select.rect.y + btn_select.rect.h/2, COLOR_TEXT_PRIMARY, true);
                
                if (strlen(selected_path) > 0) {
                    SDL_Color btn_reset_color = btn_reset.hovered ? COLOR_ERROR : COLOR_TEXT_SECONDARY;
                    fill_rect_with_alpha(renderer, btn_reset.rect, btn_reset_color);
                    draw_border(renderer, btn_reset.rect, btn_reset_color, 1);
                    render_text(renderer, font_medium, btn_reset.text, btn_reset.rect.x + btn_reset.rect.w/2, btn_reset.rect.y + btn_reset.rect.h/2, COLOR_TEXT_PRIMARY, true);
                }
                
                if (history.count > 0) {
                    SDL_Color btn_history_color = btn_history.hovered ? COLOR_ACCENT_BLUE : COLOR_CARD;
                    fill_rect_with_alpha(renderer, btn_history.rect, btn_history_color);
                    draw_border(renderer, btn_history.rect, COLOR_BORDER, 1);
                    render_text(renderer, font_medium, btn_history.text, btn_history.rect.x + btn_history.rect.w/2, btn_history.rect.y + btn_history.rect.h/2, COLOR_TEXT_PRIMARY, true);
                }

                SDL_Rect info_rect = {30, 500, SCREEN_WIDTH - 60, 150};
                fill_rect_with_alpha(renderer, info_rect, COLOR_BG_LIGHT);
                draw_border(renderer, info_rect, COLOR_BORDER, 1);
                
                render_text(renderer, font_medium, "📋 Catégories disponibles:", 50, 520, COLOR_ACCENT_BLUE, false);
                render_text(renderer, font_small, "📄 Documents  •  🖼️ Images  •  🎬 Vidéos  •  🎵 Musique  •  📦 Archives  •  ⚙️ Autres", 50, 555, COLOR_TEXT_PRIMARY, false);
                render_text(renderer, font_tiny, "Les fichiers seront organisés dans des sous-dossiers selon leur type", 50, 585, COLOR_TEXT_TERTIARY, false);
            }

            if (app_state == STATE_CONFIRMATION) {
                render_confirmation_dialog(renderer, font_large, font_medium, font_small, &btn_confirm, &btn_cancel);
            }
        }
        else if (app_state == STATE_RESULTS) {
            render_results_panel(renderer, font_large, font_medium, font_small, &btn_finish);
        }

        render_toast(renderer, font_medium, font_small);
        render_text(renderer, font_tiny, "🖥️  Windows • Linux • macOS  |  v2.0 - Enhanced Edition", SCREEN_WIDTH/2, SCREEN_HEIGHT - 15, COLOR_TEXT_TERTIARY, true);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font_xlarge);
    TTF_CloseFont(font_large);
    TTF_CloseFont(font_medium);
    TTF_CloseFont(font_small);
    TTF_CloseFont(font_tiny);
    TTF_Quit();
    NFD_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
