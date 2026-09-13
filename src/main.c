#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <eadk.h>

__attribute__((used, visibility("default"), section(".rodata.eadk_app_name"))) const char eadk_app_name[] = "SysInfo";
__attribute__((used, visibility("default"), section(".rodata.eadk_api_level"))) const uint32_t eadk_api_level = 0;

// Palette RGB565 (Dark Navy & Cyber Cyan/White)
const eadk_color_t C_BG      = 0x0841; // Deep Navy
const eadk_color_t C_CARD    = 0x18E3; // Dark Slate Card
const eadk_color_t C_HEADER  = 0x001F; // Primary Blue
const eadk_color_t C_TEXT    = 0xFFFF; // White
const eadk_color_t C_MUTED   = 0x8410; // Gray
const eadk_color_t C_CYAN    = 0x07FF; // Cyan label / accent

static uint32_t stack_base_ref = 0;

static uint32_t get_stack_delta(void) {
    uint8_t dummy;
    uint32_t current_sp = (uint32_t)&dummy;
    if (stack_base_ref == 0) stack_base_ref = current_sp + 128;
    return (stack_base_ref > current_sp) ? (stack_base_ref - current_sp) : 0;
}

void draw_text(const char * text, uint16_t x, uint16_t y, eadk_color_t fg, eadk_color_t bg) {
    eadk_point_t pos = {x, y};
    eadk_display_draw_string(text, pos, false, fg, bg);
}

int main(int argc, char * argv[]) {
    char buf[58];
    uint32_t frames = 0;
    uint64_t start_ms = eadk_timing_millis();
    uint64_t last_fps_ms = start_ms;
    uint32_t fps = 0;
    uint32_t frame_count = 0;

    { uint8_t dummy; stack_base_ref = (uint32_t)&dummy; }

    // --- DÉCOR STATIQUE ---
    eadk_display_push_rect_uniform((eadk_rect_t){0, 0, 320, 20}, C_HEADER);
    draw_text("SYSINFO V1.1  |  Official NumWorks OS", 8, 2, C_TEXT, C_HEADER);
    eadk_display_push_rect_uniform((eadk_rect_t){0, 20, 320, 220}, C_BG);

    // Conteneurs Cartes triées par catégories
    eadk_display_push_rect_uniform((eadk_rect_t){8, 24,  304, 66}, C_CARD); // Cadre 1: Système & Uptime
    eadk_display_push_rect_uniform((eadk_rect_t){8, 92,  304, 54}, C_CARD); // Cadre 2: Architecture & RAM
    eadk_display_push_rect_uniform((eadk_rect_t){8, 148, 304, 67}, C_CARD); // Cadre 3: Conformité & Modèle
    draw_text("[BACK] Quitter l'application", 8, 220, C_MUTED, C_BG);

    // --- BOUCLE D'AFFICHAGE DYNAMIQUE ---
    while (1) {
        frames++;
        frame_count++;
        uint64_t now = eadk_timing_millis();
        if (now - last_fps_ms >= 500) {
            fps = (frame_count * 1000) / (uint32_t)(now - last_fps_ms);
            frame_count = 0;
            last_fps_ms = now;
        }

        uint32_t total_sec = (uint32_t)(now / 1000);
        uint32_t hours = total_sec / 3600;
        uint32_t mins  = (total_sec % 3600) / 60;
        uint32_t secs  = total_sec % 60;
        uint32_t stack_used = get_stack_delta();

        eadk_keyboard_state_t kb = eadk_keyboard_scan();
        if ((kb & ((uint64_t)1 << eadk_key_back)) != 0) {
            break;
        }

        // --- CADRE 1 : SYSTEME & UPTIME BOOT ---
        draw_text("[SYSTEME]", 14, 28, C_CYAN, C_CARD);
        snprintf(buf, sizeof(buf), "Allume depuis : %02lu:%02lu:%02lu", (unsigned long)hours, (unsigned long)mins, (unsigned long)secs);
        draw_text(buf, 14, 45, C_TEXT, C_CARD);
        snprintf(buf, sizeof(buf), "FPS     : ~%lu", (unsigned long)fps);
        draw_text(buf, 14, 60, C_TEXT, C_CARD);
        snprintf(buf, sizeof(buf), "Boucles : ~%lu", (unsigned long)frames);
        draw_text(buf, 14, 75, C_TEXT, C_CARD);

        // --- CADRE 2 : ARCHITECTURE & RAM ---
        draw_text("[ARCH & MEMOIRE]", 14, 94, C_CYAN, C_CARD);
        draw_text("CPU     : ARM Cortex-M7 (STM32F730)", 14, 110, C_TEXT, C_CARD);
        snprintf(buf, sizeof(buf), "RAM     : ~%lu Bytes (Local Frame)", (unsigned long)stack_used);
        draw_text(buf, 14, 126, C_TEXT, C_CARD);

        // --- CADRE 3 : CONFORMITE & MODELE ---
        draw_text("[CONFORMITE / MODELE]", 14, 150, C_CYAN, C_CARD);
        draw_text("Model   : N0120   Normes :  CE | X-BIN", 14, 166, C_TEXT, C_CARD);
        draw_text("Assemble en Chine | Ne pas jeter dans la", 14, 182, C_MUTED, C_CARD);
        draw_text("poubelle ordinaire", 14, 195, C_MUTED, C_CARD);

        eadk_timing_msleep(30);
    }

    return 0;
}
