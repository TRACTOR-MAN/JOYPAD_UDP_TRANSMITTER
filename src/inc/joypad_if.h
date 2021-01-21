#ifndef JOYPAD_IF_H
#define JOYPAD_IF_H

// For mutex
#include <mutex>

// C modules
extern "C"
{
    #include <linux/joystick.h>

    // Joystick event struct, typedefined for compatibility with c++.
    typedef js_event joystick_event;
}
/**************************************************************************
                         ___  _  _  _   _  __  __  ___ 
                        | __|| \| || | | ||  \/  |/ __|
                        | _| | .` || |_| || |\/| |\__ \
                        |___||_|\_| \___/ |_|  |_||___/
**************************************************************************/
// button mappings
enum button_mappings_et
{
    BUTTON_A,
    BUTTON_B,
    BUTTON_Y,
    BUTTON_X,
    BUTTON_TL,
    BUTTON_TR,
    BUTTON_BL,
    BUTTON_BR,
    BUTTON_SELECT,
    BUTTON_START,
    BUTTON_MODE,
    BUTTON_THUMBL,
    BUTTON_THUMBR,
    NUM_BUTTONS
};

// button mappings
enum ana_ctl_mappings_et
{
    ANA_LT,
    ANA_RT,
    ANA_DPAD_X,
    ANA_DPAD_Y,
    ANA_LSTICK_X,
    ANA_LSTICK_Y,
    ANA_RSTICK_X,
    ANA_RSTICK_Y,
    NUM_ANA_CTL
};

/**************************************************************************
                 ___  _____  ___  _   _   ___  _____  ___ 
                / __||_   _|| _ \| | | | / __||_   _|/ __|
                \__ \  | |  |   /| |_| || (__   | |  \__ \
                |___/  |_|  |_|_\ \___/  \___|  |_|  |___/
**************************************************************************/
// Virtual joystick struct
struct virjoy_st
{
    //buttons
    unsigned char VIRJOY_BTN_A         :1;
    unsigned char VIRJOY_BTN_B         :1;
    unsigned char VIRJOY_BTN_X         :1;
    unsigned char VIRJOY_BTN_Y         :1;
    unsigned char VIRJOY_BTN_TL        :1;
    unsigned char VIRJOY_BTN_TR        :1;
    unsigned char VIRJOY_BTN_THUMBL    :1;
    unsigned char VIRJOY_BTN_THUMBR    :1;
   
    unsigned char VIRJOY_BTN_DPAD_UP       :1;
    unsigned char VIRJOY_BTN_DPAD_DOWN     :1;
    unsigned char VIRJOY_BTN_DPAD_LEFT     :1;
    unsigned char VIRJOY_BTN_DPAD_RIGHT    :1;
    unsigned char VIRJOY_BTN_START         :1;
    unsigned char VIRJOY_BTN_SELECT        :1;
    unsigned char VIRJOY_BTN_MODE          :1;
    unsigned char VIRJOY_PADDING_1         :1;

    //analogue (signed)
    short VIRJOY_ABS_LX;
    short VIRJOY_ABS_LY;
    short VIRJOY_ABS_RX;
    short VIRJOY_ABS_RY;
    short VIRJOY_ABS_LB;
    short VIRJOY_ABS_RB;
    short VIRJOY_ABS_HAT0X;
    short VIRJOY_ABS_HAT0Y;


    unsigned char VIRJOY_CHECKSUM;
};

struct axis_state 
{
    short x, y;
};

/**************************************************************************
                   _   _  _  _  ___  ___   _  _  ___ 
                  | | | || \| ||_ _|/ _ \ | \| |/ __|
                  | |_| || .` | | || (_) || .` |\__ \
                   \___/ |_|\_||___|\___/ |_|\_||___/
**************************************************************************/
// Virtual joystick union
union virjoy_un
{
    virjoy_st virtualjoydata;
    unsigned char raw[sizeof(virjoy_st)];
};

/**************************************************************************
                  ___  _       _    ___  ___  ___  ___ 
                 / __|| |     /_\  / __|/ __|| __|/ __|
                | (__ | |__  / _ \ \__ \\__ \| _| \__ \
                 \___||____|/_/ \_\|___/|___/|___||___/
**************************************************************************/
/*! 
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      15/06/2020
 * 
 *  \par       Description:
 *             This class contains the interface to the linux kernel joypad module
 */
class joypad_interface_c
{
public:
    joypad_interface_c ( );
    ~joypad_interface_c ( );
   
    //parse external button config file
    bool parse_buttoncfg ( void );
    // Member function for reading a joystick event
    bool read_event ( void );
    // Member function for returning the number of joystick axes
    size_t get_axis_count ( void );
    // Member function for returning the number of buttons on the controller
    size_t get_button_count ( void );
    // Member function for returning the axis state of the joystick axes
    size_t get_axis_state ( void );
    // Member function for returning the axis state of the joystick dpad
    void get_dpad_state ( void );
    // Member function for mapping the digital buttons to the controller data structure
    void map_digital_button ( void );
    // map analog joystick
    void map_analog_joystick ( void );
    // function for polling for joypad data
    void poll_joypad_data ( void );
    // Member function for exiting any active while loops
    void exit_while_loops ( void );
    // Member function for locking the joypad data mutex
    void lock_joypad_mutex ( void );
    // Member function for unlocking the joypad data mutex
    void unlock_joypad_mutex ( void );
    // Exit program flag
    bool exit_program_b;
    // The mutex ptr for joypad union data locks
    std::mutex *joy_mutex;
    // The joypad data union
    virjoy_un virjoy_u;
    // Array to store read button map
    int buttoncfg[NUM_BUTTONS];
    // Array to store read analog map
    int anacfg[NUM_ANA_CTL];

protected:
    char device[255];
    FILE *file;
    joystick_event joystick_event_s;
    bool exit_while_loop_b;

private:
    const bool extended_debug_b;

};


#endif