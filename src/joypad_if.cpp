/**************************************************************************
             ___  _  _   ___  _    _   _  ___   ___  ___ 
            |_ _|| \| | / __|| |  | | | ||   \ | __|/ __|
             | | | .` || (__ | |__| |_| || |) || _| \__ \
            |___||_|\_| \___||____|\___/ |___/ |___||___/
**************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "joypad_if.h"
#include <unistd.h>


/**************************************************************************
                        _    _     ___   ___  
                       /_\  | |   / __| / _ \ 
                      / _ \ | |__| (_ || (_) |
                     /_/ \_\|____|\___| \___/
**************************************************************************/
/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      15/06/2020
 *
 *  \par       Description:
 *             Constructor for the joypad_interface_c class
 */
joypad_interface_c::joypad_interface_c( ) :
    joystick_event_s( { } ),
    device( "/dev/input/js0" ),
    file( fopen(device, "rb+") ),
    virjoy_u( { } ),
    joystick_event_b( false ),
    extended_debug_b( true )

{
    // Initialise certain virjoy elems
    virjoy_u.virtualjoydata.VIRJOY_ABS_RB = -32767;
    virjoy_u.virtualjoydata.VIRJOY_ABS_LB = -32767;

    // Check to see whether the file was successfully opened
    if( file == nullptr )
    {
        std::cout << "Could not access joystick, check it is connected" << std::endl;
    }
    else
    {
        // Sit in a while loop, (blocking mode) and pull out all joypad events
        while( read_event( ) != false ) 
        {
            switch (joystick_event_s.type)
            {
            case JS_EVENT_BUTTON:
                joystick_event_b = true;
                // Map the button press to the data structure
                map_digital_button( );

                break;
            case JS_EVENT_AXIS:
                size_t axis;
                axis = get_axis_state( );

                // Must be an analog axis
                if ( axis < 3 )
                {
                    joystick_event_b = true;
                    map_analog_joystick( );
                }
                // Must be the dpad
                else
                if( axis == 3 )
                {
                    get_dpad_state( );
                }
                else
                {
                    // Do nothing
                }

                break;

            default:

                /* Ignore init events. */
                break;
            }
        }
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      16/06/2020
 *
 *  \par       Description:
 *             Member function for reading a joystick event
 */
bool joypad_interface_c::read_event ( void )
{
    ssize_t objects;

    // Read the joystick event data structure from the joystick input file
    objects = fread( &joystick_event_s, sizeof(joystick_event), 1, file );

    if ( objects == 1 )
        return true;
    else
        // Error, could not read full event. 
        return false;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      16/06/2020
 *
 *  \par       Description:
 *             Member function for returning the number of joystick axes
 */
size_t joypad_interface_c::get_axis_count ( void )
{
    __u8 axes;

    if (ioctl( fileno( file ), JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      16/06/2020
 *
 *  \par       Description:
 *             Member function for returning the number of buttons on the controller
 */
size_t joypad_interface_c::get_button_count ( void )
{
    __u8 buttons;

    if (ioctl( fileno( file ), JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      16/06/2020
 *
 *  \par       Description:
 *             Member function for returning the axis state of the joystick axes
 */
size_t joypad_interface_c::get_axis_state ( void )
{
    size_t axis = joystick_event_s.number / 2;

    return axis;
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      18/06/2020
 *
 *  \par       Description:
 *             Member function for returning the axis state of the joystick dpad
 */
void joypad_interface_c::get_dpad_state ( void )
{
    size_t axis = joystick_event_s.number / 2;

    if (axis == 3)
    {
        if( joystick_event_s.number == 7 )
        {
            if( joystick_event_s.value == -32767 )
            {
                joystick_event_b = true;
                if( extended_debug_b )
                    std::cout << "DPAD UP" << std::endl;
            }
            else 
            if( joystick_event_s.value == 32767 )
            {
                joystick_event_b = true;
                if( extended_debug_b )
                    std::cout << "DPAD DOWN" << std::endl;
            }
        }
        else
        if( joystick_event_s.number == 6 )
        {
            if( joystick_event_s.value == -32767 )
            {
                joystick_event_b = true;
                if( extended_debug_b )
                    std::cout << "DPAD_LEFT" << std::endl;
            }
            else 
            if( joystick_event_s.value == 32767 )
            {
                joystick_event_b = true;
                if( extended_debug_b )
                    std::cout << "DPAD_RIGHT" << std::endl;
            }
        }
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      18/06/2020
 *
 *  \par       Description:
 *             Member function for returning the axis state of the joystick dpad
 */
void joypad_interface_c::map_digital_button ( void )
{
    switch( joystick_event_s.number )
    {
    case BUTTON_A:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_A" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_A = true : virjoy_u.virtualjoydata.VIRJOY_BTN_A = false);
        break;

    case BUTTON_B:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_B" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_B = true : virjoy_u.virtualjoydata.VIRJOY_BTN_B = false);
        break;

    case BUTTON_Y:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_Y" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_Y = true : virjoy_u.virtualjoydata.VIRJOY_BTN_Y = false);
        
        break;

    case BUTTON_X:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_X" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_X = true : virjoy_u.virtualjoydata.VIRJOY_BTN_X = false);
        break;

    case BUTTON_TL:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_TL" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_TL = true : virjoy_u.virtualjoydata.VIRJOY_BTN_TL = false);
        break;

    case BUTTON_TR:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_TR" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_TR = true : virjoy_u.virtualjoydata.VIRJOY_BTN_TR = false);
        break;

    case BUTTON_BL:
        // LB pressed, this is analog
        break;

    case BUTTON_BR:
        // LB pressed, this is analog
        break;

    case BUTTON_SELECT:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_SELECT" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_SELECT = true : virjoy_u.virtualjoydata.VIRJOY_BTN_SELECT = false);
        break;

    case BUTTON_START:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_START" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_START = true : virjoy_u.virtualjoydata.VIRJOY_BTN_START = false);
        break;

    case BUTTON_MODE:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_MODE" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_MODE = true : virjoy_u.virtualjoydata.VIRJOY_BTN_MODE = false);
        break;

    case BUTTON_THUMBL:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_THUMBL" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_THUMBL = true : virjoy_u.virtualjoydata.VIRJOY_BTN_THUMBL = false);
        break;

    case BUTTON_THUMBR:
        if( extended_debug_b )
            std::cout << " VIRJOY_BTN_THUMBR" << " " << ((joystick_event_s.value != 0) ? "pressed" : "released") << std::endl;
        ((joystick_event_s.value != 0) ? virjoy_u.virtualjoydata.VIRJOY_BTN_THUMBR = true : virjoy_u.virtualjoydata.VIRJOY_BTN_THUMBR = false);
        break;

    default:
        break;
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      18/06/2020
 *
 *  \par       Description:
 *             Member function for mapping the analog joystick values to the data structure
 */
void joypad_interface_c::map_analog_joystick ( void )
{
    if ( joystick_event_s.number % 2 == 0 )
    {
        switch( joystick_event_s.number )
        {
        case 0:
            virjoy_u.virtualjoydata.VIRJOY_ABS_LX = joystick_event_s.value;
            if( extended_debug_b )
                std::cout << " VIRJOY_ABS_LX" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_LX << std::endl;
            break;
        case 2:
            virjoy_u.virtualjoydata.VIRJOY_ABS_LB = joystick_event_s.value;
            if( extended_debug_b )
                std::cout << " VIRJOY_ABS_LB" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_LB << std::endl;
            break;
        case 4:
            virjoy_u.virtualjoydata.VIRJOY_ABS_RX = joystick_event_s.value;
            if( extended_debug_b )
                std::cout << " VIRJOY_ABS_RX" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_RX << std::endl;
            break;
        }
    }
    else
    {
        switch( joystick_event_s.number )
        {
        case 1:
            virjoy_u.virtualjoydata.VIRJOY_ABS_LY = joystick_event_s.value;
            if( extended_debug_b )
                std::cout << " VIRJOY_ABS_LY" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_LY << std::endl;
            break;
        case 5:
            virjoy_u.virtualjoydata.VIRJOY_ABS_RB = joystick_event_s.value;
            if( extended_debug_b )
                std::cout << " VIRJOY_ABS_RB" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_RB << std::endl;
            break;
        case 3:
            virjoy_u.virtualjoydata.VIRJOY_ABS_RY = joystick_event_s.value;
            if( extended_debug_b )
                std::cout << " VIRJOY_ABS_RY" << " " << virjoy_u.virtualjoydata.VIRJOY_ABS_RY << std::endl;
            break;
        }
    }
}

/*!
 *  \author    Thomas Sutton
 *  \version   1.0
 *  \date      19/06/2020
 *
 *  \par       Description:
 *             Destructor for the joypad_interface_c class
 */
joypad_interface_c::~joypad_interface_c( ) 
{
    // Close the file stream
    fclose( file );    
}