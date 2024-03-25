#include "triangle.h"
#include "display.h"

void int_swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/*
      (x0,y0)
     /     \
    /       \
   /         \
(x1,y1) ---- (x2,y2)
      
*/
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // find two inverted slopes for two triangle legs
    // because our y value increases by 1 consistently, 
    // so we are interested in the amount of change it causes in x values
    float inv_slope1 = (float)(x1 - x0) / (y1 - y0); // x_start slope
    float inv_slope2 = (float)(x2 - x0) / (y2 - y0); // x_end slope

    // start x_start and x_end from the top vertex (x0, y0)
    float x_start = x0;
    float x_end = x0;

    // loop all the scanlines from top to bottom
    for (int y= y0; y <= y2; y++) {
        // TODO: probably we do not need to use draw_line which recalculates stuff
        // TODO: just should fill the array here without a function call
        draw_line(x_start, y, x_end, y, color); 

        x_start += inv_slope1;
        x_end += inv_slope2;
    }
}
/*
(x0,y0) ------ (x1,y1)
   \          /
    \        /
     \      /
      (x2,y2)
*/
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // find two inverted slopes for two triangle legs
    // because our y value increases by 1 consistently, 
    // so we are interested in the amount of change it causes in x values
    float inv_slope1 = (float)(x2 - x0) / (y2 - y0); // x_start slope
    float inv_slope2 = (float)(x2 - x1) / (y2 - y1); // x_end slope

    // start x_start and x_end from the bottom vertex (x2, y2)
    float x_start = x2;
    float x_end = x2;

    // loop all the scanlines from top to bottom
    for (int y= y2; y >= y0; y--) {
        // TODO: probably we do not need to use draw_line which recalculates stuff
        // TODO: just should fill the array here without a function call
        draw_line(x_start, y, x_end, y, color); 

        x_start -= inv_slope1;
        x_end -= inv_slope2;
    }
}

// this function draws using flat-top/flat-bottom method
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // sort vertices by y-coordinate (ascending) -> y0 < y1 < y2

    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }

    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 == y2) {
        // if the triangle is already in the flat bottom shape, we do not need to draw flat top
        fill_flat_bottom_triangle(
            x0, y0, x1, y1, x2, y2, color
        );
    } else if (y0 == y1) {
        // if the triangle is already in the flat top shape, we do not need to draw the flat bottom
        fill_flat_top_triangle(
            x0, y0, x1, y1, x2, y2, color
        );
    } else {
        // calculate the midpoint vertex
        int mx = ((float)((x2 - x0)*(y1 - y0)) / (float)(y2 - y0)) + x0;
        int my = y1;

        // draw flat bottom triangle
        fill_flat_bottom_triangle(
            x0, y0, x1, y1, mx, my, color
        );

        // draw flat top triangle
        fill_flat_top_triangle(
            x1, y1, mx, my, x2, y2, color
        );
    }


}