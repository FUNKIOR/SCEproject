#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <../../lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *other;
    lv_obj_t *bar;
    lv_obj_t *obj0;
    lv_obj_t *rpm;
    lv_obj_t *power;
    lv_obj_t *percentage;
    lv_obj_t *scale;
    lv_obj_t *rpme;
    lv_obj_t *rpm_2;
    lv_obj_t *rpmd;
    lv_obj_t *rpmd_1;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_OTHER = 2,
};

void create_screen_main();
void tick_screen_main();

void create_screen_other();
void tick_screen_other();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/