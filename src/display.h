#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

extern int window_width;
extern int window_height;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;

bool initialize_window(void);
void destroy_window(void);
void draw_grid(void);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);

#endif