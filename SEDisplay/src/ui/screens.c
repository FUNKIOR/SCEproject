#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_bar(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_arc_get_value(ta);
            set_var_power(value);
        }
    }
}

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 320);
            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff8f1f1), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xff676767), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // rpm
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rpm = obj;
                    lv_obj_set_pos(obj, 140, 66);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd22e2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // Power
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.power = obj;
                    lv_obj_set_pos(obj, 54, 170);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd70f0f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_color(obj, lv_color_hex(0xffb916ce), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffde1ce4), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // percentage
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.percentage = obj;
                            lv_obj_set_pos(obj, 69, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "%");
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffd70f0f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_outline_color(obj, lv_color_hex(0xffb916ce), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xffde1ce4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // Scale
                    lv_obj_t *obj = lv_scale_create(parent_obj);
                    objects.scale = obj;
                    lv_obj_set_pos(obj, 25, 130);
                    lv_obj_set_size(obj, 161, 175);
                    lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_OUTER);
                    lv_scale_set_range(obj, 0, 100);
                    lv_scale_set_total_tick_count(obj, 31);
                    lv_scale_set_major_tick_every(obj, 3);
                    lv_scale_set_label_show(obj, true);
                    lv_obj_set_style_length(obj, 10, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0xffea952c), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffa50ae3), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_color(obj, lv_color_hex(0xffeb0101), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_line_color(obj, lv_color_hex(0xff19ff00), LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff07ccce), LV_PART_ITEMS | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // Bar
                            lv_obj_t *obj = lv_arc_create(parent_obj);
                            objects.bar = obj;
                            lv_obj_set_pos(obj, 5, 4);
                            lv_obj_set_size(obj, 151, 150);
                            lv_arc_set_bg_start_angle(obj, 135);
                            lv_arc_set_bg_end_angle(obj, 45);
                            lv_obj_add_event_cb(obj, event_handler_cb_main_bar, LV_EVENT_ALL, 0);
                            lv_obj_set_style_arc_width(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_rounded(obj, false, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            lv_obj_set_style_arc_width(obj, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(0xffd12ded), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_img_create(parent_obj);
                                    lv_obj_set_pos(obj, -40, 36);
                                    lv_obj_set_size(obj, 231, 148);
                                    lv_img_set_src(obj, &img_nian);
                                }
                            }
                        }
                    }
                }
                {
                    // rpme
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rpme = obj;
                    lv_obj_set_pos(obj, 129, 39);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "RPME:");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd22e2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // rpm_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rpm_2 = obj;
                    lv_obj_set_pos(obj, 2, 66);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd22e2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // rpmd
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rpmd = obj;
                    lv_obj_set_pos(obj, -11, 39);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "RPMD:");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd22e2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // rpmd_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rpmd_1 = obj;
                    lv_obj_set_pos(obj, -9, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd22e2e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_main() {
    {
        const char *new_val = get_var_rpm();
        const char *cur_val = lv_label_get_text(objects.rpm);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.rpm;
            lv_label_set_text(objects.rpm, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_power();
        const char *cur_val = lv_label_get_text(objects.power);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.power;
            lv_label_set_text(objects.power, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_power();
        int32_t cur_val = lv_arc_get_value(objects.bar);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.bar;
            lv_arc_set_value(objects.bar, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_rpm();
        const char *cur_val = lv_label_get_text(objects.rpm_2);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.rpm_2;
            lv_label_set_text(objects.rpm_2, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_other() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.other = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
}

void tick_screen_other() {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_other();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_other,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
