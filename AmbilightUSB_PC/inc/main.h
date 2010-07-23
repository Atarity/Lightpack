#ifndef MAIN_H
#define MAIN_H

// Take colors from screen:
// - with Y-coordinates: 0..PIXELS_Y and (800-PIXELS_Y)..800
#define PIXELS_Y  400
// - with X-coordinates: 0..PIXELS_X and (1280-PIXELS_X)..1280
#define PIXELS_X  100

// Step to next pixel
#define Y_STEP  20
#define X_STEP  20

#define PIXELS_COUNT_FOR_EACH_LED  (PIXELS_Y / Y_STEP) * (PIXELS_X / X_STEP)

#endif // MAIN_H
