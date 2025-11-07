#ifndef BUTTON_DEBOUNCER_H
#define BUTTON_DEBOUNCER_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h> // Required for 'exp' in the .c file

// --- Type Definitions ---

/**
 * @brief Function pointer type for button state change callbacks.
 * @param context A user-defined pointer, often pointing back to the Button_t instance itself.
 */
typedef void (*Button_Callback_t)(void* context);

/**
 * @brief Structure defining a single push button instance and its state.
 */
typedef struct Button_Debouncer {
    // Filter State
    float filtered_value;    ///< Current filtered value (0.0 to 1.0)
    float alpha;             ///< Low-pass filter coefficient

    // Hysteresis Configuration
    float threshold_high;    ///< Value threshold for rising edge (e.g., 0.8)
    float threshold_low;     ///< Value threshold for falling edge (e.g., 0.2)
    
    // State Tracking
    uint8_t raw_state;       ///< The instantaneous digital state (0 or 1)
    uint8_t stable_state;    ///< The stable state after hysteresis (0 or 1)

    // Callbacks
    Button_Callback_t on_rise_cb;   ///< Function called when stable state changes from 0 to 1
    Button_Callback_t on_fall_cb;   ///< Function called when stable state changes from 1 to 0
    void* cb_context;              ///< Context pointer passed to callbacks
} Button_t;

// --- Function Prototypes ---

/**
 * @brief Initializes a Button_t instance with filter parameters and thresholds.
 * * @param btn Pointer to the Button_t instance to initialize.
 * @param sample_period_ms The time base (update interval) of the filter, in milliseconds.
 * @param cutoff_freq_hz The desired low-pass filter cutoff frequency, in Hertz.
 * @param th_high The high threshold for the hysteresis trigger (0.0 to 1.0).
 * @param th_low The low threshold for the hysteresis trigger (0.0 to 1.0).
 * @param initial_raw_state The initial reading of the pin (0 or 1).
 * @return true if initialization was successful, false otherwise.
 */
bool Button_Init(
    Button_t* btn,
    float sample_period_ms,
    float cutoff_freq_hz,
    float th_high,
    float th_low,
    uint8_t initial_raw_state
);

/**
 * @brief The main processing function. Must be called periodically by the system timer.
 * * @param btn Pointer to the Button_t instance to process.
 * @param new_raw_state The current instantaneous digital state read from the GPIO pin (0 or 1).
 */
void Button_Process(Button_t* btn, uint8_t new_raw_state);

/**
 * @brief Sets the callback function for a rising edge (released -> pressed).
 * * @param btn Pointer to the Button_t instance.
 * @param callback The function to call on a rise event. Use NULL to disable.
 */
void Button_SetRiseCallback(Button_t* btn, Button_Callback_t callback);

/**
 * @brief Sets the callback function for a falling edge (pressed -> released).
 * * @param btn Pointer to the Button_t instance.
 * @param callback The function to call on a fall event. Use NULL to disable.
 */
void Button_SetFallCallback(Button_t* btn, Button_Callback_t callback);

/**
 * @brief Sets the context pointer passed to the callbacks.
 * * @param btn Pointer to the Button_t instance.
 * @param context The pointer to pass to the callbacks.
 */
void Button_SetContext(Button_t* btn, void* context);

/**
 * @brief Gets the current stable state of the button after debouncing.
 * * @param btn Pointer to the Button_t instance.
 * @return 1 (pressed) or 0 (released).
 */
uint8_t Button_GetStableState(const Button_t* btn);

/**
 * @brief Gets the current filtered value (0.0 to 1.0).
 * * @param btn Pointer to the Button_t instance.
 * @return The filtered value as a float.
 */
float Button_GetFilteredValue(const Button_t* btn);


#endif // BUTTON_DEBOUNCER_H