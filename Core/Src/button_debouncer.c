#include "button_debouncer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f 
#endif

// --- Private Functions ---

/**
 * @brief Calculates the EMA filter coefficient (alpha) based on time constants.
 * * @param sample_period_ms The update period in milliseconds.
 * @param cutoff_freq_hz The desired filter cutoff frequency in Hertz.
 * @return The calculated alpha value (0.0 to 1.0).
 */
static float calculate_alpha(float sample_period_ms, float cutoff_freq_hz) {
    // Convert sample period to seconds
    float Ts = sample_period_ms / 1000.0f; 
    
    // Calculate the time constant (Tau = RC) based on the desired cutoff frequency
    // Tau = 1 / (2 * PI * fc)
    float Tau = 1.0f / (2.0f * M_PI * cutoff_freq_hz);

    // Calculate alpha using the formula for discrete-time RC filter (exponential decay)
    // alpha = 1 - exp(-Ts / Tau)
    // M_PI is typically defined in math.h
    return 1.0f - expf(-Ts / Tau); 
}

// --- Public Functions Implementation ---

bool Button_Init(
    Button_t* btn,
    float sample_period_ms,
    float cutoff_freq_hz,
    float th_high,
    float th_low,
    uint8_t initial_raw_state
) {
    if (!btn || th_high <= th_low || th_high > 1.0f || th_low < 0.0f || sample_period_ms <= 0.0f || cutoff_freq_hz <= 0.0f) {
        // Validation check failed
        return false;
    }

    // Calculate alpha based on user-defined time base and cutoff frequency
    btn->alpha = calculate_alpha(sample_period_ms, cutoff_freq_hz);

    // Initialize state and thresholds
    btn->raw_state = initial_raw_state;
    btn->stable_state = initial_raw_state;
    
    // Set filtered value to match the initial state for stability
    btn->filtered_value = (float)initial_raw_state; 

    // Configure Hysteresis
    btn->threshold_high = th_high;
    btn->threshold_low = th_low;

    // Initialize Callbacks
    btn->on_rise_cb = NULL;
    btn->on_fall_cb = NULL;
    btn->cb_context = NULL;

    return true;
}

void Button_Process(Button_t* btn, uint8_t new_raw_state) {
    if (!btn) return;

    // 1. Update Raw State
    btn->raw_state = new_raw_state;

    // 2. Apply Low-Pass Filter (EMA)
    // The raw state (0 or 1) is the input to the filter
    float raw_float = (float)btn->raw_state;
    
    // EMA Formula: filtered = alpha * new_raw + (1 - alpha) * previous_filtered
    btn->filtered_value = btn->alpha * raw_float + (1.0f - btn->alpha) * btn->filtered_value;

    // 3. Apply Hysteresis Trigger and Check for State Change

    uint8_t previous_stable_state = btn->stable_state;

    if (btn->stable_state == 0) {
        // We are currently low/released. Look for a rise.
        if (btn->filtered_value >= btn->threshold_high) {
            btn->stable_state = 1; // Transition to high/pressed
        }
    } else { // stable_state == 1
        // We are currently high/pressed. Look for a fall.
        if (btn->filtered_value <= btn->threshold_low) {
            btn->stable_state = 0; // Transition to low/released
        }
    }

    // 4. Trigger Callbacks
    if (previous_stable_state == 0 && btn->stable_state == 1) {
        // Rising Edge Detected
        if (btn->on_rise_cb) {
            btn->on_rise_cb(btn->cb_context);
        }
    } else if (previous_stable_state == 1 && btn->stable_state == 0) {
        // Falling Edge Detected
        if (btn->on_fall_cb) {
            btn->on_fall_cb(btn->cb_context);
        }
    }
}

void Button_SetRiseCallback(Button_t* btn, Button_Callback_t callback) {
    if (btn) btn->on_rise_cb = callback;
}

void Button_SetFallCallback(Button_t* btn, Button_Callback_t callback) {
    if (btn) btn->on_fall_cb = callback;
}

void Button_SetContext(Button_t* btn, void* context) {
    if (btn) btn->cb_context = context;
}

uint8_t Button_GetStableState(const Button_t* btn) {
    return btn ? btn->stable_state : 0;
}

float Button_GetFilteredValue(const Button_t* btn) {
    return btn ? btn->filtered_value : 0.0f;
}