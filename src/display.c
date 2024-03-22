#include "display.h"

int window_width = 800;
int window_height = 600;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // get the maximum dimensions of the screen
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_height = display_mode.h;
    window_width = display_mode.w;

    // create an SDL window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        fprintf(stderr, "Error initializing SDL window.\n");
        return false;
    }

    // create an SDL renderer
    renderer = SDL_CreateRenderer(
        window,
        -1, // grab the default one
        0
    );

    if (!renderer) {
        fprintf(stderr, "Error initializing SDL renderer.\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void destroy_window(void) {
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_grid(void) {
    for (int r=0;r<window_height;r++) {
        for (int c=0; c<window_width; c++) {
            if (r % 10 == 0 || c % 10 == 0) {
                draw_pixel(c, r, 0xFF333333);
            }
        }
    }
}

inline void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
        color_buffer[window_width * y + x] = color;
    }
}

void draw_rect(
    int x, int y, int w, int h, uint32_t color
) {
    for (int r=0;r<h;r++) {
        for (int c=0; c<w; c++) {
            int curr_x = x + c;
            int curr_y = y + r;
            draw_pixel(curr_x, curr_y, color);
        }
    }
}

void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL, // render entire texture
        color_buffer, // the pixel values
        (int) (window_width * sizeof(uint32_t))
    );
    SDL_RenderCopy(
        renderer,
        color_buffer_texture,
        NULL, // render entire texture, no subdivisions
        NULL // render entire texture, no subdivisions
    );
}

void clear_color_buffer(uint32_t color) {
    for (int i=0;i<window_width*window_height;i++) {
        color_buffer[i] = color;
    }
}