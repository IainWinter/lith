#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

/**
 * A type to store decimal (x, y) values
*/
#include "glm/vec2.hpp"
using namespace glm;

/**
 * A type to identify axes, you should replace with an enum
 */
using InputName = std::string;

/**
 * A type to identify masks, you should replace with an enum
 */
using InputMask = std::string;

// This is for the events to be able to pass a trivially copyable type
struct InputNameRef {
	const InputName* name;
	InputNameRef(const InputName& name) : name(&name) {}
	operator const InputName&() const { return *name; }
};

enum KeyboardInput
{
    // These values are copied directly from SDL_Scancode

    Key_A = 4,
    Key_B = 5,
    Key_C = 6,
    Key_D = 7,
    Key_E = 8,
    Key_F = 9,
    Key_G = 10,
    Key_H = 11,
    Key_I = 12,
    Key_J = 13,
    Key_K = 14,
    Key_L = 15,
    Key_M = 16,
    Key_N = 17,
    Key_O = 18,
    Key_P = 19,
    Key_Q = 20,
    Key_R = 21,
    Key_S = 22,
    Key_T = 23,
    Key_U = 24,
    Key_V = 25,
    Key_W = 26,
    Key_X = 27,
    Key_Y = 28,
    Key_Z = 29,
    Key_1 = 30,
    Key_2 = 31,
    Key_3 = 32,
    Key_4 = 33,
    Key_5 = 34,
    Key_6 = 35,
    Key_7 = 36,
    Key_8 = 37,
    Key_9 = 38,
    Key_0 = 39,
    Key_Return = 40,
    Key_Escape = 41,
    Key_Backspace = 42,
    Key_Tab = 43,
    Key_Space = 44,
    Key_Minus = 45,
    Key_Equals = 46,
    Key_Left_Bracket = 47,
    Key_Right_Bracket = 48,
    Key_Backslash = 49, 
    Key_Nonushash = 50,
    Key_Semicolon = 51,
    Key_Apostrophe = 52,
    Key_Grave = 53, 
    Key_Comma = 54,
    Key_Period = 55,
    Key_Slash = 56,
    Key_Lock_Caps = 57,
    Key_F1 = 58,
    Key_F2 = 59,
    Key_F3 = 60,
    Key_F4 = 61,
    Key_F5 = 62,
    Key_F6 = 63,
    Key_F7 = 64,
    Key_F8 = 65,
    Key_F9 = 66,
    Key_F10 = 67,
    Key_F11 = 68,
    Key_F12 = 69,
    Key_Printscreen = 70,
    Key_Lock_Scroll = 71,
    Key_Pause = 72,
    Key_Insert = 73, 
    Key_Home = 74,
    Key_Page_Up = 75,
    Key_Delete = 76,
    Key_End = 77,
    Key_Page_Down = 78,
    Key_Right = 79,
    Key_Left = 80,
    Key_Down = 81,
    Key_Up = 82,
    Key_Numlockclear = 83, 
    KeyPad_Divide = 84,
    KeyPad_Multiply = 85,
    KeyPad_Minus = 86,
    KeyPad_Plus = 87,
    KeyPad_Enter = 88,
    KeyPad_1 = 89,
    KeyPad_2 = 90,
    KeyPad_3 = 91,
    KeyPad_4 = 92,
    KeyPad_5 = 93,
    KeyPad_6 = 94,
    KeyPad_7 = 95,
    KeyPad_8 = 96,
    KeyPad_9 = 97,
    KeyPad_0 = 98,
    KeyPad_Period = 99,
    Key_Nonusbackslash = 100,
    Key_Application = 101, 
    Key_Power = 102, 
    KeyPad_Equals = 103,
    Key_F13 = 104,
    Key_F14 = 105,
    Key_F15 = 106,
    Key_F16 = 107,
    Key_F17 = 108,
    Key_F18 = 109,
    Key_F19 = 110,
    Key_F20 = 111,
    Key_F21 = 112,
    Key_F22 = 113,
    Key_F23 = 114,
    Key_F24 = 115,
    Key_Execute = 116,
    Key_Help = 117, 
    Key_Menu = 118, 
    Key_Select = 119,
    Key_Stop = 120,    
    Key_Again = 121,  
    Key_Undo = 122,    
    Key_Cut = 123,     
    Key_Copy = 124,    
    Key_Paste = 125,   
    Key_Find = 126,    
    Key_Mute = 127,
    Key_Volume_Up = 128,
    Key_Volume_Down = 129,
    KeyPad_Comma = 133,
    KeyPad_Equalsas400 = 134,
    Key_International1 = 135,
    Key_International2 = 136,
    Key_International3 = 137, 
    Key_International4 = 138,
    Key_International5 = 139,
    Key_International6 = 140,
    Key_International7 = 141,
    Key_International8 = 142,
    Key_International9 = 143,
    Key_Lang1 = 144,
    Key_Lang2 = 145,
    Key_Lang3 = 146,
    Key_Lang4 = 147,
    Key_Lang5 = 148,
    Key_Lang6 = 149,
    Key_Lang7 = 150,
    Key_Lang8 = 151, 
    Key_Lang9 = 152, 
    Key_Alterase = 153,
    Key_Sysreq = 154,
    Key_Cancel = 155, 
    Key_Clear = 156,
    Key_Prior = 157,
    Key_Return2 = 158,
    Key_Separator = 159,
    Key_Out = 160,
    Key_Oper = 161,
    Key_Clearagain = 162,
    Key_Crsel = 163,
    Key_Exsel = 164,
    KeyPad_00 = 176,
    KeyPad_000 = 177,
    Key_Thousandsseparator = 178,
    Key_Decimalseparator = 179,
    Key_Currencyunit = 180,
    Key_Currencysubunit = 181,
    KeyPad_Left_Paren = 182,
    KeyPad_Right_Paren = 183,
    KeyPad_Left_Brace = 184,
    KeyPad_Right_Brace = 185,
    KeyPad_Tab = 186,
    KeyPad_Backspace = 187,
    KeyPad_A = 188,
    KeyPad_B = 189,
    KeyPad_C = 190,
    KeyPad_D = 191,
    KeyPad_E = 192,
    KeyPad_F = 193,
    KeyPad_Xor = 194,
    KeyPad_Power = 195,
    KeyPad_Percent = 196,
    KeyPad_Less = 197,
    KeyPad_Greater = 198,
    KeyPad_Ampersand = 199,
    KeyPad_Dblampersand = 200,
    KeyPad_Verticalbar = 201,
    KeyPad_Dblverticalbar = 202,
    KeyPad_Colon = 203,
    KeyPad_Hash = 204,
    KeyPad_Space = 205,
    KeyPad_At = 206,
    KeyPad_Exclam = 207,
    KeyPad_Memstore = 208,
    KeyPad_Memrecall = 209,
    KeyPad_Memclear = 210,
    KeyPad_Memadd = 211,
    KeyPad_Memsubtract = 212,
    KeyPad_Memmultiply = 213,
    KeyPad_Memdivide = 214,
    KeyPad_Plusminus = 215,
    KeyPad_Clear = 216,
    KeyPad_Clearentry = 217,
    KeyPad_Binary = 218,
    KeyPad_Octal = 219,
    KeyPad_Decimal = 220,
    KeyPad_Hexadecimal = 221,
    Key_Left_Control = 224,
    Key_Left_Shift = 225,
    Key_Left_Alt = 226,
    Key_Left_GUI = 227,
    Key_Right_Control = 228,
    Key_Right_Shift = 229,
    Key_Right_Alt = 230,
    Key_Right_GUI = 231,
    Key_Mode = 257, 
    Key_Audion_Exit = 258,
    Key_Audio_Previous = 259,
    Key_Audio_Stop = 260,
    Key_Audio_Play = 261,
    Key_Audio_Mute = 262,
    Key_Media_Select = 263,
    Key_WWW = 264,        
    Key_Mail = 265,
    Key_Calculator = 266, 
    Key_Computer = 267,
    Key_AcSearch = 268,  
    Key_AcHome = 269, 
    Key_AcBack = 270,
    Key_AcForward = 271,
    Key_AcStop = 272,
    Key_AcRefresh = 273,
    Key_AcBookmarks = 274,
    Key_Brightness_Down = 275,
    Key_Brightness_Up = 276,
    Key_Display_Switch = 277, 
    Key_Kbdillum_Toggle = 278,
    Key_Kbdillum_Down = 279,
    Key_Kbdillum_Up = 280,
    Key_Eject = 281,
    Key_Sleep = 282,        
    Key_App1 = 283,
    Key_App2 = 284,
    Key_Audio_Rewind = 285,
    Key_Audio_Fastforward = 286,
    Key_Left_Soft = 287, 
    Key_Right_Soft = 288, 
    Key_Call = 289,
    Key_Call_End = 290,

    KEY_INPUT_COUNT
};

enum MouseInput
{
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT,
	MOUSE_X1,
	MOUSE_X2,

	// these are in viewport space

	MOUSE_POS_X,
	MOUSE_POS_Y,

	MOUSE_VEL_X,
	MOUSE_VEL_Y,

	MOUSE_VEL_WHEEL_X,
	MOUSE_VEL_WHEEL_Y,

    // This is before the last 2 because they are not part of the linear rage of input codes
	MOUSE_INPUT_COUNT,

	// these are sent by the Window, EngineLoop handles breaking it into x,y/vx,vy
	MOUSE_VEL_POS,
	MOUSE_VEL_WHEEL,
};

enum ControllerInput
{
    BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_BACK,
    BUTTON_GUIDE,
    BUTTON_START,
    BUTTON_LEFTSTICK,
    BUTTON_RIGHTSTICK,
    BUTTON_LEFTSHOULDER,
    BUTTON_RIGHTSHOULDER,
    BUTTON_DPAD_UP,
    BUTTON_DPAD_DOWN,
    BUTTON_DPAD_LEFT,
    BUTTON_DPAD_RIGHT,
    BUTTON_MISC1,
    BUTTON_PADDLE1,
    BUTTON_PADDLE2,
    BUTTON_PADDLE3,
    BUTTON_PADDLE4,
    BUTTON_TOUCHPAD,
    
	BUTTON_MAX,

	AXIS_LEFTX,
	AXIS_LEFTY,
	AXIS_RIGHTX,
	AXIS_RIGHTY,
	AXIS_TRIGGERLEFT,
	AXIS_TRIGGERRIGHT,

    CONTROLLER_INPUT_COUNT
};

// Convert from an enum to a code that represents an index into a combined range containing all Keyboard/Mouse/Controller inputs
int GetInputCode(KeyboardInput scancode);
int GetInputCode(MouseInput input);
int GetInputCode(ControllerInput input);

// Return a string with the name of the input code
// nullptr is returned for invalid codes
const char* GetInputCodeName(int code);

/**
* An adapter for each input enum. Converts from the enum values to a linear integer range.
*/
struct InputCode
{
	int code;

	operator int() const {
		return code;
	}

	InputCode()                      : code(-1)                  {}
	InputCode(int code)              : code(code)                {}
	InputCode(KeyboardInput input)   : code(GetInputCode(input)) {}
	InputCode(MouseInput input)      : code(GetInputCode(input)) {}
	InputCode(ControllerInput input) : code(GetInputCode(input)) {}
};

namespace std {
	template <> struct hash<InputCode> {
		size_t operator()(const InputCode& code) const {
			return (int)code;
		}
	};
}

/**
*  Settings for each axis.
*  InputAxis and GroupAxis store this so these settings can be applied at both levels.
*/
struct InputAxisSettings
{
    /**
    * Limit the length of output to 1.
    */
	bool limitToUnit = false;

    /**
    * Normalize the length of the output to 1.
    */
	bool normalized = false;

    /**
    * States or axes which have not been used this frame should be read as 0.
    */
    bool useOnlyInputSetThisFrame = false;

    /**
    * When reading an axis, only consider the state with the latest frame.
    */
    bool useOnlyLatestInput = false;

    /**
    * Round to 0 if below this length.
    */
    float deadzone = 0.f;

    /**
    * Masks allow for storing multiple 'control schemes' at once, but only listen to one.
    * If mask is an empty string, it will be accepted by any active mask.
    */
	InputMask mask;

    /**
    * A list of user defined functions to transform after the default settings.
    */
    std::vector<std::function<vec2(vec2)>> userPipeline;

	bool operator==(const InputAxisSettings& other) const;
	bool operator!=(const InputAxisSettings& other) const;
};

/**
* An axis is defined as a collection of inputs and weights.
* Using this scheme, it is easy to define X/Y axes, and combo buttons.
* To create multi-bindings like Controller Left Stick / WASD / Arrow keys, use an AxisGroup.
*/
struct InputAxis
{
	std::unordered_map<int, vec2> components;
	InputAxisSettings settings;
                
	bool operator==(const InputAxis& other) const;
	bool operator!=(const InputAxis& other) const;
};

/**
* A summation of input axes. Useful to bind many inputs to the same axis.
* The main use for Group Axes is combining keyboard and analogue controller inputs as
* they need to have separate dead zones before summation. After summation
* the final value is processed again using the group axis' settings.
*/
struct AxisGroup
{
	std::vector<InputName> axes;
	InputAxisSettings settings;

	// only used for removing mappings, could just search Mapping
	std::vector<int> components;
        
	bool operator==(const AxisGroup& other) const;
	bool operator!=(const AxisGroup& other) const;
};

/**
* Allows for Input and Group axis to set their settings without code copy
*/
template<typename _a>
class InputAxisSettingsBuilder
{
public:
    InputAxisSettingsBuilder(InputAxisSettings* settings)
        : m_settings (settings)
    {}

    _a& LimitToUnit() {
        m_settings->limitToUnit = true;
        return *(_a*)this;
    }
        
    _a& Normalized() {
        m_settings->normalized = true;
        return *(_a*)this;
    }
        
    _a& UseOnlyInputSetThisFrame() {
        m_settings->useOnlyInputSetThisFrame = true;
        return *(_a*)this;
    }

    _a& UseOnlyLatestInput() {
        m_settings->useOnlyLatestInput = true;
        return *(_a*)this;
    }

    _a& Deadzone(float deadzone) {
        m_settings->deadzone = deadzone;
        return *(_a*)this;
    }

    _a& Mask(const InputMask& mask) {
        m_settings->mask = mask;
        return *(_a*)this;
    }
    
    _a& Process(std::function<vec2(vec2)> function) {
        m_settings->userPipeline.push_back(function);
        return *(_a*)this;
    }

private:
    InputAxisSettings* m_settings;
};

// need to forwards declare so builders can update Mapping

class InputMap;

/**
* Provides a fluent interface for configuring input axes
*/
class InputAxisBuilder : public InputAxisSettingsBuilder<InputAxisBuilder>
{
public:
    InputAxisBuilder(InputMap* map, InputAxis* axis, const InputName& name);
        
    InputAxisBuilder& Map(InputCode code, vec2 weight);
    InputAxisBuilder& MapButton(InputCode code, float weight = 1.f);
        
private:
    InputMap* m_map;
    InputAxis* m_axis;
    InputName m_name;
};

/**
* Provides a fluent interface for configuring axis groups
*/
class AxisGroupBuilder : public InputAxisSettingsBuilder<AxisGroupBuilder>
{
public:
    AxisGroupBuilder(InputMap* map, AxisGroup* axis, const InputName& name);
        
    AxisGroupBuilder& Map(const InputName& name);
        
private:
    InputMap* m_map;
    AxisGroup* m_axis;
    InputName m_name;
};

class InputMap
{
public:
    /**
    * Makes and returns a new input axis.
    * The returned axis is a reference with builder functions to bind inputs.
    */
    InputAxisBuilder CreateAxis(const InputName& name);

    /**
    * Makes and returns a new group axis.
    * The returned axis is a reference with builder functions to bind input axes by name.
    */
    AxisGroupBuilder CreateGroupAxis(const InputName& name);

	void RemoveAxis(const InputName& name);
	void RemoveGroupAxis(const InputName& name);

    /**
    * Return the state of an axis. If an input axis and axis group share names, the group will take precedence and be returned.
    */
    vec2 GetAxis(const InputName& axis);

    /**
    * Buttons are treated as axes with only an x component.
    * Shorthand for GetAxis(button).x
    */
    float GetButton(const InputName& button);

    /**
    * For single press buttons.
    * Once true is returned, false will be returned until the button is released and pressed again.
    */
    bool GetOnce(const InputName& button);

    /**
    * Return true if the last input set was a controller
    */
    bool IsUsingController() const;
    
    /**
    * Set the global active mask. See InputAxisSettings::mask
    */
    void SetActiveMask(const InputMask& mask);

    /**
    * If the view port doesn't take up the whole window, use this function to
    * map the screen to view port. This is not fullscreen at the OS level, it's
    * inside the window for split screen / editor windows.
    */
    void SetViewportBounds(vec2 screenMin, vec2 screenSize);

    /**
    * Map a screen coordinate to a view port coordinate. Only needed when view port
    * is a different size than the window
    */
    vec2 MapToViewport(float screenX, float screenY);

    /**
    * Update the states for falloff
    */
    void UpdateStates(float deltaTime);
    
    /**
    * Internal
    */

    void SetActiveFrame(int frame);
    void SetState(int code, float state);
    const std::unordered_set<InputName>& GetMapping(InputCode code);
    float GetRawState(InputCode code);

    InputMap();

private:
    bool _FailsMask(const InputMask& mask);
    bool _AxisExists(const InputName& axis);
    bool _AxisGroupExists(const InputName& axis);
    vec2 _GetAxisNoRecurse(const InputName& axisName, bool useOnlyInputSetThisFrameOverride);
    vec2 _ApplySettings(vec2 in, const InputAxisSettings& settings);

	void _RemoveAxisMapping(const InputName& axisName, const InputAxis& axis);

private:
    // mapping of name to Group axis
	// these are groups of other axes so each can have its own processing
	// then be combined
	std::unordered_map<InputName, AxisGroup> GroupAxes;

	// mapping of name to axis
	// all buttons can be represented as axes with a dead zone
	// multi-button combos can be thought as an axis with components ('A', .5) and ('B', .5) and a dead zone = 1
	std::unordered_map<InputName, InputAxis> Axes;

	// mapping of code to input name
	// allows us to skip a search through all components of each axis to find a mapping
	std::unordered_map<InputCode, std::unordered_set<InputName>> Mapping;

	// raw states. todo: this should be an array

    struct InputState {
        float value;
        int frameLastUpdated;
    };

	std::unordered_map<int, InputState> State;

	// once states
	std::unordered_set<InputName> OnceButtonIsDown;

	// for mapping mouse into view port
	vec2 ViewportMin;
	vec2 ViewportSize;

	InputMask activeMask;
    int activeFrame;
    
    bool lastInputWasController;
    
    // friend so these can access Mapping, seems fishy
    friend class InputAxisBuilder;
    friend class AxisGroupBuilder;
};
