#include "lith/input.h"
#include "lith/log.h"
#include "lith/math.h"

// do this in the window
//ControllerInput MapSDLGameControllerButton(SDL_GameControllerButton button) {
//	if (button == SDL_CONTROLLER_BUTTON_INVALID)
//		return ControllerInput::cBUTTON_INVALID;
//
//	return (ControllerInput)button;
//}
//
//ControllerInput MapSDLGameControllerAxis(SDL_GameControllerAxis axis) {
//	if (axis == SDL_CONTROLLER_AXIS_INVALID)
//		return ControllerInput::cAXIS_INVALID;
//
//	return (ControllerInput)(1 + axis + ControllerInput::cBUTTON_MAX);
//}

static std::unordered_map<int, const char*> s_inputNames = {
    { GetInputCode(Key_A), "Key_A" },
    { GetInputCode(Key_B), "Key_B" },
    { GetInputCode(Key_C), "Key_C" },
    { GetInputCode(Key_D), "Key_D" },
    { GetInputCode(Key_E), "Key_E" },
    { GetInputCode(Key_F), "Key_F" },
    { GetInputCode(Key_G), "Key_G" },
    { GetInputCode(Key_H), "Key_H" },
    { GetInputCode(Key_I), "Key_I" },
    { GetInputCode(Key_J), "Key_J" },
    { GetInputCode(Key_K), "Key_K" },
    { GetInputCode(Key_L), "Key_L" },
    { GetInputCode(Key_M), "Key_M" },
    { GetInputCode(Key_N), "Key_N" },
    { GetInputCode(Key_O), "Key_O" },
    { GetInputCode(Key_P), "Key_P" },
    { GetInputCode(Key_Q), "Key_Q" },
    { GetInputCode(Key_R), "Key_R" },
    { GetInputCode(Key_S), "Key_S" },
    { GetInputCode(Key_T), "Key_T" },
    { GetInputCode(Key_U), "Key_U" },
    { GetInputCode(Key_V), "Key_V" },
    { GetInputCode(Key_W), "Key_W" },
    { GetInputCode(Key_X), "Key_X" },
    { GetInputCode(Key_Y), "Key_Y" },
    { GetInputCode(Key_Z), "Key_Z" },
    { GetInputCode(Key_1), "Key_1" },
    { GetInputCode(Key_2), "Key_2" },
    { GetInputCode(Key_3), "Key_3" },
    { GetInputCode(Key_4), "Key_4" },
    { GetInputCode(Key_5), "Key_5" },
    { GetInputCode(Key_6), "Key_6" },
    { GetInputCode(Key_7), "Key_7" },
    { GetInputCode(Key_8), "Key_8" },
    { GetInputCode(Key_9), "Key_9" },
    { GetInputCode(Key_0), "Key_0" },
    { GetInputCode(Key_Return), "Key_Return" },
    { GetInputCode(Key_Escape), "Key_Escape" },
    { GetInputCode(Key_Backspace), "Key_Backspace" },
    { GetInputCode(Key_Tab), "Key_Tab" },
    { GetInputCode(Key_Space), "Key_Space" },
    { GetInputCode(Key_Minus), "Key_Minus" },
    { GetInputCode(Key_Equals), "Key_Equals" },
    { GetInputCode(Key_Left_Bracket), "Key_Left_Bracket" },
    { GetInputCode(Key_Right_Bracket), "Key_Right_Bracket" },
    { GetInputCode(Key_Backslash), "Key_Backslash" },
    { GetInputCode(Key_Nonushash), "Key_Nonushash" },
    { GetInputCode(Key_Semicolon), "Key_Semicolon" },
    { GetInputCode(Key_Apostrophe), "Key_Apostrophe" },
    { GetInputCode(Key_Grave), "Key_Grave" },
    { GetInputCode(Key_Comma), "Key_Comma" },
    { GetInputCode(Key_Period), "Key_Period" },
    { GetInputCode(Key_Slash), "Key_Slash" },
    { GetInputCode(Key_Lock_Caps), "Key_Lock_Caps" },
    { GetInputCode(Key_F1), "Key_F1" },
    { GetInputCode(Key_F2), "Key_F2" },
    { GetInputCode(Key_F3), "Key_F3" },
    { GetInputCode(Key_F4), "Key_F4" },
    { GetInputCode(Key_F5), "Key_F5" },
    { GetInputCode(Key_F6), "Key_F6" },
    { GetInputCode(Key_F7), "Key_F7" },
    { GetInputCode(Key_F8), "Key_F8" },
    { GetInputCode(Key_F9), "Key_F9" },
    { GetInputCode(Key_F10), "Key_F10" },
    { GetInputCode(Key_F11), "Key_F11" },
    { GetInputCode(Key_F12), "Key_F12" },
    { GetInputCode(Key_Printscreen), "Key_Printscreen" },
    { GetInputCode(Key_Lock_Scroll), "Key_Lock_Scroll" },
    { GetInputCode(Key_Pause), "Key_Pause" },
    { GetInputCode(Key_Insert), "Key_Insert" },
    { GetInputCode(Key_Home), "Key_Home" },
    { GetInputCode(Key_Page_Up), "Key_Page_Up" },
    { GetInputCode(Key_Delete), "Key_Delete" },
    { GetInputCode(Key_End), "Key_End" },
    { GetInputCode(Key_Page_Down), "Key_Page_Down" },
    { GetInputCode(Key_Right), "Key_Right" },
    { GetInputCode(Key_Left), "Key_Left" },
    { GetInputCode(Key_Down), "Key_Down" },
    { GetInputCode(Key_Up), "Key_Up" },
    { GetInputCode(Key_Numlockclear), "Key_Numlockclear" },
    { GetInputCode(KeyPad_Divide), "KeyPad_Divide" },
    { GetInputCode(KeyPad_Multiply), "KeyPad_Multiply" },
    { GetInputCode(KeyPad_Minus), "KeyPad_Minus" },
    { GetInputCode(KeyPad_Plus), "KeyPad_Plus" },
    { GetInputCode(KeyPad_Enter), "KeyPad_Enter" },
    { GetInputCode(KeyPad_1), "KeyPad_1" },
    { GetInputCode(KeyPad_2), "KeyPad_2" },
    { GetInputCode(KeyPad_3), "KeyPad_3" },
    { GetInputCode(KeyPad_4), "KeyPad_4" },
    { GetInputCode(KeyPad_5), "KeyPad_5" },
    { GetInputCode(KeyPad_6), "KeyPad_6" },
    { GetInputCode(KeyPad_7), "KeyPad_7" },
    { GetInputCode(KeyPad_8), "KeyPad_8" },
    { GetInputCode(KeyPad_9), "KeyPad_9" },
    { GetInputCode(KeyPad_0), "KeyPad_0" },
    { GetInputCode(KeyPad_Period), "KeyPad_Period" },
    { GetInputCode(Key_Nonusbackslash), "Key_Nonusbackslash" },
    { GetInputCode(Key_Application), "Key_Application" },
    { GetInputCode(Key_Power), "Key_Power" },
    { GetInputCode(KeyPad_Equals), "KeyPad_Equals" },
    { GetInputCode(Key_F13), "Key_F13" },
    { GetInputCode(Key_F14), "Key_F14" },
    { GetInputCode(Key_F15), "Key_F15" },
    { GetInputCode(Key_F16), "Key_F16" },
    { GetInputCode(Key_F17), "Key_F17" },
    { GetInputCode(Key_F18), "Key_F18" },
    { GetInputCode(Key_F19), "Key_F19" },
    { GetInputCode(Key_F20), "Key_F20" },
    { GetInputCode(Key_F21), "Key_F21" },
    { GetInputCode(Key_F22), "Key_F22" },
    { GetInputCode(Key_F23), "Key_F23" },
    { GetInputCode(Key_F24), "Key_F24" },
    { GetInputCode(Key_Execute), "Key_Execute" },
    { GetInputCode(Key_Help), "Key_Help" },
    { GetInputCode(Key_Menu), "Key_Menu" },
    { GetInputCode(Key_Select), "Key_Select" },
    { GetInputCode(Key_Stop), "Key_Stop" },
    { GetInputCode(Key_Again), "Key_Again" },
    { GetInputCode(Key_Undo), "Key_Undo" },
    { GetInputCode(Key_Cut), "Key_Cut" },
    { GetInputCode(Key_Copy), "Key_Copy" },
    { GetInputCode(Key_Paste), "Key_Paste" },
    { GetInputCode(Key_Find), "Key_Find" },
    { GetInputCode(Key_Mute), "Key_Mute" },
    { GetInputCode(Key_Volume_Up), "Key_Volume_Up" },
    { GetInputCode(Key_Volume_Down), "Key_Volume_Down" },
    { GetInputCode(KeyPad_Comma), "KeyPad_Comma" },
    { GetInputCode(KeyPad_Equalsas400), "KeyPad_Equalsas400" },
    { GetInputCode(Key_International1), "Key_International1" },
    { GetInputCode(Key_International2), "Key_International2" },
    { GetInputCode(Key_International3), "Key_International3" },
    { GetInputCode(Key_International4), "Key_International4" },
    { GetInputCode(Key_International5), "Key_International5" },
    { GetInputCode(Key_International6), "Key_International6" },
    { GetInputCode(Key_International7), "Key_International7" },
    { GetInputCode(Key_International8), "Key_International8" },
    { GetInputCode(Key_International9), "Key_International9" },
    { GetInputCode(Key_Lang1), "Key_Lang1" },
    { GetInputCode(Key_Lang2), "Key_Lang2" },
    { GetInputCode(Key_Lang3), "Key_Lang3" },
    { GetInputCode(Key_Lang4), "Key_Lang4" },
    { GetInputCode(Key_Lang5), "Key_Lang5" },
    { GetInputCode(Key_Lang6), "Key_Lang6" },
    { GetInputCode(Key_Lang7), "Key_Lang7" },
    { GetInputCode(Key_Lang8), "Key_Lang8" },
    { GetInputCode(Key_Lang9), "Key_Lang9" },
    { GetInputCode(Key_Alterase), "Key_Alterase" },
    { GetInputCode(Key_Sysreq), "Key_Sysreq" },
    { GetInputCode(Key_Cancel), "Key_Cancel" },
    { GetInputCode(Key_Clear), "Key_Clear" },
    { GetInputCode(Key_Prior), "Key_Prior" },
    { GetInputCode(Key_Return2), "Key_Return2" },
    { GetInputCode(Key_Separator), "Key_Separator" },
    { GetInputCode(Key_Out), "Key_Out" },
    { GetInputCode(Key_Oper), "Key_Oper" },
    { GetInputCode(Key_Clearagain), "Key_Clearagain" },
    { GetInputCode(Key_Crsel), "Key_Crsel" },
    { GetInputCode(Key_Exsel), "Key_Exsel" },
    { GetInputCode(KeyPad_00), "KeyPad_00" },
    { GetInputCode(KeyPad_000), "KeyPad_000" },
    { GetInputCode(Key_Thousandsseparator), "Key_Thousandsseparator" },
    { GetInputCode(Key_Decimalseparator), "Key_Decimalseparator" },
    { GetInputCode(Key_Currencyunit), "Key_Currencyunit" },
    { GetInputCode(Key_Currencysubunit), "Key_Currencysubunit" },
    { GetInputCode(KeyPad_Left_Paren), "KeyPad_Left_Paren" },
    { GetInputCode(KeyPad_Right_Paren), "KeyPad_Right_Paren" },
    { GetInputCode(KeyPad_Left_Brace), "KeyPad_Left_Brace" },
    { GetInputCode(KeyPad_Right_Brace), "KeyPad_Right_Brace" },
    { GetInputCode(KeyPad_Tab), "KeyPad_Tab" },
    { GetInputCode(KeyPad_Backspace), "KeyPad_Backspace" },
    { GetInputCode(KeyPad_A), "KeyPad_A" },
    { GetInputCode(KeyPad_B), "KeyPad_B" },
    { GetInputCode(KeyPad_C), "KeyPad_C" },
    { GetInputCode(KeyPad_D), "KeyPad_D" },
    { GetInputCode(KeyPad_E), "KeyPad_E" },
    { GetInputCode(KeyPad_F), "KeyPad_F" },
    { GetInputCode(KeyPad_Xor), "KeyPad_Xor" },
    { GetInputCode(KeyPad_Power), "KeyPad_Power" },
    { GetInputCode(KeyPad_Percent), "KeyPad_Percent" },
    { GetInputCode(KeyPad_Less), "KeyPad_Less" },
    { GetInputCode(KeyPad_Greater), "KeyPad_Greater" },
    { GetInputCode(KeyPad_Ampersand), "KeyPad_Ampersand" },
    { GetInputCode(KeyPad_Dblampersand), "KeyPad_Dblampersand" },
    { GetInputCode(KeyPad_Verticalbar), "KeyPad_Verticalbar" },
    { GetInputCode(KeyPad_Dblverticalbar), "KeyPad_Dblverticalbar" },
    { GetInputCode(KeyPad_Colon), "KeyPad_Colon" },
    { GetInputCode(KeyPad_Hash), "KeyPad_Hash" },
    { GetInputCode(KeyPad_Space), "KeyPad_Space" },
    { GetInputCode(KeyPad_At), "KeyPad_At" },
    { GetInputCode(KeyPad_Exclam), "KeyPad_Exclam" },
    { GetInputCode(KeyPad_Memstore), "KeyPad_Memstore" },
    { GetInputCode(KeyPad_Memrecall), "KeyPad_Memrecall" },
    { GetInputCode(KeyPad_Memclear), "KeyPad_Memclear" },
    { GetInputCode(KeyPad_Memadd), "KeyPad_Memadd" },
    { GetInputCode(KeyPad_Memsubtract), "KeyPad_Memsubtract" },
    { GetInputCode(KeyPad_Memmultiply), "KeyPad_Memmultiply" },
    { GetInputCode(KeyPad_Memdivide), "KeyPad_Memdivide" },
    { GetInputCode(KeyPad_Plusminus), "KeyPad_Plusminus" },
    { GetInputCode(KeyPad_Clear), "KeyPad_Clear" },
    { GetInputCode(KeyPad_Clearentry), "KeyPad_Clearentry" },
    { GetInputCode(KeyPad_Binary), "KeyPad_Binary" },
    { GetInputCode(KeyPad_Octal), "KeyPad_Octal" },
    { GetInputCode(KeyPad_Decimal), "KeyPad_Decimal" },
    { GetInputCode(KeyPad_Hexadecimal), "KeyPad_Hexadecimal" },
    { GetInputCode(Key_Left_Control), "Key_Left_Control" },
    { GetInputCode(Key_Left_Shift), "Key_Left_Shift" },
    { GetInputCode(Key_Left_Alt), "Key_Left_Alt" },
    { GetInputCode(Key_Left_GUI), "Key_Left_GUI" },
    { GetInputCode(Key_Right_Control), "Key_Right_Control" },
    { GetInputCode(Key_Right_Shift), "Key_Right_Shift" },
    { GetInputCode(Key_Right_Alt), "Key_Right_Alt" },
    { GetInputCode(Key_Right_GUI), "Key_Right_GUI" },
    { GetInputCode(Key_Mode), "Key_Mode" },
    { GetInputCode(Key_Audion_Exit), "Key_Audion_Exit" },
    { GetInputCode(Key_Audio_Previous), "Key_Audio_Previous" },
    { GetInputCode(Key_Audio_Stop), "Key_Audio_Stop" },
    { GetInputCode(Key_Audio_Play), "Key_Audio_Play" },
    { GetInputCode(Key_Audio_Mute), "Key_Audio_Mute" },
    { GetInputCode(Key_Media_Select), "Key_Media_Select" },
    { GetInputCode(Key_WWW), "Key_WWW" },
    { GetInputCode(Key_Mail), "Key_Mail" },
    { GetInputCode(Key_Calculator), "Key_Calculator" },
    { GetInputCode(Key_Computer), "Key_Computer" },
    { GetInputCode(Key_AcSearch), "Key_AcSearch" },
    { GetInputCode(Key_AcHome), "Key_AcHome" },
    { GetInputCode(Key_AcBack), "Key_AcBack" },
    { GetInputCode(Key_AcForward), "Key_AcForward" },
    { GetInputCode(Key_AcStop), "Key_AcStop" },
    { GetInputCode(Key_AcRefresh), "Key_AcRefresh" },
    { GetInputCode(Key_AcBookmarks), "Key_AcBookmarks" },
    { GetInputCode(Key_Brightness_Down), "Key_Brightness_Down" },
    { GetInputCode(Key_Brightness_Up), "Key_Brightness_Up" },
    { GetInputCode(Key_Display_Switch), "Key_Display_Switch" },
    { GetInputCode(Key_Kbdillum_Toggle), "Key_Kbdillum_Toggle" },
    { GetInputCode(Key_Kbdillum_Down), "Key_Kbdillum_Down" },
    { GetInputCode(Key_Kbdillum_Up), "Key_Kbdillum_Up" },
    { GetInputCode(Key_Eject), "Key_Eject" },
    { GetInputCode(Key_Sleep), "Key_Sleep" },
    { GetInputCode(Key_App1), "Key_App1" },
    { GetInputCode(Key_App2), "Key_App2" },
    { GetInputCode(Key_Audio_Rewind), "Key_Audio_Rewind" },
    { GetInputCode(Key_Audio_Fastforward), "Key_Audio_Fastforward" },
    { GetInputCode(Key_Left_Soft), "Key_Left_Soft" },
    { GetInputCode(Key_Right_Soft), "Key_Right_Soft" },
    { GetInputCode(Key_Call), "Key_Call" },
    { GetInputCode(Key_Call_End), "Key_Call_End" },
    
	{ GetInputCode(MOUSE_LEFT),        "Mouse Left" },
	{ GetInputCode(MOUSE_MIDDLE),      "Mouse Middle" },
	{ GetInputCode(MOUSE_RIGHT),       "Mouse Right" },
	{ GetInputCode(MOUSE_X1),          "Mouse X1" },
	{ GetInputCode(MOUSE_X2),          "Mouse X2" },
	{ GetInputCode(MOUSE_POS_X),       "Mouse Position X" },
	{ GetInputCode(MOUSE_POS_Y),       "Mouse Position Y" },
	{ GetInputCode(MOUSE_VEL_X),       "Mouse Velocity X" },
	{ GetInputCode(MOUSE_VEL_Y),       "Mouse Velocity Y" },
	{ GetInputCode(MOUSE_VEL_WHEEL_X), "Mouse Wheel Velocity X" },
	{ GetInputCode(MOUSE_VEL_WHEEL_Y), "Mouse Wheel Velocity Y" },

	{ GetInputCode(BUTTON_A),             "Button A" },
	{ GetInputCode(BUTTON_B),             "Button B" },
	{ GetInputCode(BUTTON_X),             "Button X" },
	{ GetInputCode(BUTTON_Y),             "Button Y" },
	{ GetInputCode(BUTTON_BACK),          "Button BACK" },
	{ GetInputCode(BUTTON_GUIDE),         "Button GUIDE" },
	{ GetInputCode(BUTTON_START),         "Button START" },
	{ GetInputCode(BUTTON_LEFTSTICK),     "Button Left Stick" },
	{ GetInputCode(BUTTON_RIGHTSTICK),    "Button Right Stick" },
	{ GetInputCode(BUTTON_LEFTSHOULDER),  "Button Left Shoulder" },
	{ GetInputCode(BUTTON_RIGHTSHOULDER), "Button Right Shoulder" },
	{ GetInputCode(BUTTON_DPAD_UP),       "Button DPAD Up" },
	{ GetInputCode(BUTTON_DPAD_DOWN),     "Button DPAD Down" },
	{ GetInputCode(BUTTON_DPAD_LEFT),     "Button DPAD Left" },
	{ GetInputCode(BUTTON_DPAD_RIGHT),    "Button DPAD Right" },
	{ GetInputCode(BUTTON_MISC1),         "Button Misc 1" },
	{ GetInputCode(BUTTON_PADDLE1),       "Button Paddle 1" },
	{ GetInputCode(BUTTON_PADDLE2),       "Button Paddle 2" },
	{ GetInputCode(BUTTON_PADDLE3),       "Button Paddle 3" },
	{ GetInputCode(BUTTON_PADDLE4),       "Button Paddle 4" },
	{ GetInputCode(BUTTON_TOUCHPAD),      "Button TOUCHPAD" },
	{ GetInputCode(AXIS_LEFTX),           "Axis Left X" },
	{ GetInputCode(AXIS_LEFTY),           "Axis Left Y" },
	{ GetInputCode(AXIS_RIGHTX),          "Axis Right X" },
	{ GetInputCode(AXIS_RIGHTY),          "Axis Right Y" },
	{ GetInputCode(AXIS_TRIGGERLEFT),     "Axis Trigger Left" },
	{ GetInputCode(AXIS_TRIGGERRIGHT),    "Axis Trigger Right" }
};

int GetInputCode(KeyboardInput scancode) {
	return (int)scancode;
}

int GetInputCode(MouseInput input) {
	return (int)input + (int)KEY_INPUT_COUNT;
}

int GetInputCode(ControllerInput input) {
	return (int)input + (int)KEY_INPUT_COUNT + (int)MOUSE_INPUT_COUNT;
}

const char* GetInputCodeName(int code) {
	auto itr = s_inputNames.find(code);

    return itr != s_inputNames.end() 
        ? itr->second 
        : nullptr;
}

// Don't log name because that forces a type (string or enum etc. etc.)
// todo: should add null protection to builder if this is going to check here

InputAxisBuilder InputMap::CreateAxis(const InputName& name) {
    if (_AxisExists(name)) {
        print("w~Tried to create an axis that already exists");
        throw nullptr;
    }

    InputAxis* axis = &Axes.emplace(name, InputAxis{}).first->second;
    return InputAxisBuilder(this, axis, name);
}

AxisGroupBuilder InputMap::CreateGroupAxis(const InputName& name) {
    if (_AxisGroupExists(name)) {
        print("w~Tried to create an group axis that already exists");
        throw nullptr;
    }

    AxisGroup* group = &GroupAxes.emplace(name, AxisGroup{}).first->second;
    return AxisGroupBuilder(this, group, name);
}

void InputMap::RemoveAxis(const InputName& name) {
    auto axis = Axes.find(name);

    if (axis == Axes.end()) {
        print("w~Tried to remove axis that does not exist");
        return;
    }

    _RemoveAxisMapping(name, axis->second);
    Axes.erase(axis);
}

void InputMap::RemoveGroupAxis(const InputName& name) {
    auto group = GroupAxes.find(name);

    if (group == GroupAxes.end()) {
        print("w~Tried to remove group axis that does not exist");
        return;
    }

    for (const InputName& axisName : group->second.axes) {
        auto axis = Axes.find(axisName);

        if (axis == Axes.end()) {
            print("w~Tried to remove a group axis which has had some of its axes removed."
                    " Remove all axes before removing the group.");
            return;
        }

        // group axes use the group name for the mapping, so pass 'name' not 'axisName'
        _RemoveAxisMapping(name, axis->second);
    }

    GroupAxes.erase(name);
}

vec2 InputMap::GetAxis(const InputName& axis) {
	if (!_AxisGroupExists(axis)) {
		return _GetAxisNoRecurse(axis, false);
    }

	const AxisGroup& group = GroupAxes.at(axis);

    if (_FailsMask(group.settings.mask)) {
		return vec2(0.f);
    }

	vec2 sum = vec2(0.f);

    if (group.settings.useOnlyLatestInput) {
        InputName lastStateNameUsed = {};
        int lastFrameUsed = 0;

        for (const InputName& name : group.axes) {
            for (const auto& [code, component] : Axes.at(name).components) {
                InputState& state = State.at(code);
                if (state.frameLastUpdated > lastFrameUsed) {
                    lastStateNameUsed = name;
                    lastFrameUsed = state.frameLastUpdated;
                }
            }
        }

        sum += _GetAxisNoRecurse(lastStateNameUsed, group.settings.useOnlyInputSetThisFrame);
    }

    else {
        for (const InputName& name : group.axes) {
            sum += _GetAxisNoRecurse(name, group.settings.useOnlyInputSetThisFrame);
        }
    }

    sum = _ApplySettings(sum, group.settings);

	return sum;
}

float InputMap::GetButton(const InputName& button) {
    return GetAxis(button).x;
}

bool InputMap::GetOnce(const InputName& button) {
	bool down = GetButton(button) > 0.f;

	if (!down) { // If button isn't down, clear state
		OnceButtonIsDown.erase(button);
		return false;
	}

	// Button is down

    if (OnceButtonIsDown.count(button) != 0) { // If was already down
        return false;
    }

	OnceButtonIsDown.insert(button);

	return true;
}

bool InputMap::IsUsingController() const {
    return lastInputWasController;
}

void InputMap::SetActiveMask(const InputMask& mask) {
    activeMask = mask;
}

void InputMap::SetViewportBounds(vec2 screenMin, vec2 screenSize) {
    ViewportMin = screenMin;
    ViewportSize = screenSize;
}

vec2 InputMap::MapToViewport(float screenX, float screenY) {
    return clamp((vec2(screenX, screenY) - ViewportMin) / ViewportSize, vec2(-1.f), vec2(1.f));
}

void InputMap::UpdateStates(float deltaTime) {
    // I want this to actually calculate falloff base on a setting
    // but only need it for mouse velocity right now, and that should have immediate falloff

    State[GetInputCode(MOUSE_VEL_X)].value = 0;
    State[GetInputCode(MOUSE_VEL_Y)].value = 0;
    State[GetInputCode(MOUSE_VEL_WHEEL_X)].value = 0;
    State[GetInputCode(MOUSE_VEL_WHEEL_Y)].value = 0;
}

bool InputMap::_FailsMask(const InputMask& mask) {
	return mask.size() != 0 && mask != activeMask;
}

bool InputMap::_AxisExists(const InputName& axis) {
    return Axes.find(axis) != Axes.end();
}

bool InputMap::_AxisGroupExists(const InputName& axis) {
    return GroupAxes.find(axis) != GroupAxes.end();
}

vec2 InputMap::_GetAxisNoRecurse(const InputName& axisName, bool useOnlyInputSetThisFrameOverride) {
    if (!_AxisExists(axisName)) {
		return vec2(0.f);
    }

	const InputAxis& axis = Axes.at(axisName);

    if (_FailsMask(axis.settings.mask)) {
		return vec2(0.f);
    }

    bool useOnlyInputSetThisFrame = axis.settings.useOnlyInputSetThisFrame || useOnlyInputSetThisFrameOverride;

	vec2 sum = vec2(0.f);

    if (axis.settings.useOnlyLatestInput) {
        vec2 lastStateValueUsed = {};
        int lastFrameUsed = 0;

        for (const auto& [code, component] : axis.components) {
            InputState& state = State.at(code);

            if (state.frameLastUpdated > lastFrameUsed) {
                lastStateValueUsed = state.value * component;
                lastFrameUsed = state.frameLastUpdated;
            }
        }

        if (useOnlyInputSetThisFrame && lastFrameUsed != activeFrame) {
            return vec2(0.f);
        }

        sum += lastStateValueUsed;
    }

    else {
        for (const auto& [code, component] : axis.components) {
            InputState& state = State[code];

            if (useOnlyInputSetThisFrame && state.frameLastUpdated != activeFrame) {
                continue;
            }

		    sum += state.value * component;
        }
    }

    sum = _ApplySettings(sum, axis.settings);

	return sum;
}

vec2 InputMap::_ApplySettings(vec2 in, const InputAxisSettings& settings) {
    if (length(in) < settings.deadzone) {
        return vec2(0.f);
    }

    if (settings.normalized) {
        in = safe_normalize(in);
    }

    if (settings.limitToUnit) {
        in = limit(in, 1.f);
    }

    for (const auto& process : settings.userPipeline) {
        in = process(in);
    }

    return in;
}

void InputMap::_RemoveAxisMapping(const InputName& axisName, const InputAxis& axis) {
    for (const auto& [code, _] : axis.components) {
        auto& list = Mapping.at(code);
        list.erase(axisName);

        // clean up list if empty
        if (list.size() == 0) {
            Mapping.erase(code);
        }
    }
}

InputAxisBuilder::InputAxisBuilder(InputMap* map, InputAxis* axis, const InputName& name)
    : InputAxisSettingsBuilder (&axis->settings)
    , m_map                    (map)
    , m_axis                   (axis)
    , m_name                   (name) {}

InputAxisBuilder& InputAxisBuilder::Map(InputCode code, vec2 weight) {
    m_axis->components.emplace(code, weight);
    m_map->Mapping[code].insert(m_name);
    return *this;
}

InputAxisBuilder& InputAxisBuilder::MapButton(InputCode code, float weight) {
    return Map(code, vec2(weight, 0.f));
}

AxisGroupBuilder::AxisGroupBuilder(InputMap* map, AxisGroup* axis, const InputName& name)
    : InputAxisSettingsBuilder (&axis->settings)
    , m_map                    (map)
    , m_axis                   (axis)
    , m_name                   (name) {}

AxisGroupBuilder& AxisGroupBuilder::Map(const InputName& name) {
    m_axis->axes.push_back(name);

    for (const auto& c : m_map->Axes[name].components) {
        m_map->Mapping[c.first].insert(m_name);
    }
        
    return *this;
}

// internal

void InputMap::SetActiveFrame(int frame) {
    activeFrame = frame;
}

void InputMap::SetState(int code, float state) {
	// all valid states get registered when the context is created,
	// so check here to stop map from growing
	if (State.count(code) == 0) {
        print("w~Tried to set state of invalid input code. %d -> %f", code, state);
		return;
	}
    
    lastInputWasController = GetInputCode(ControllerInput::BUTTON_A) <= code
                          && GetInputCode(ControllerInput::CONTROLLER_INPUT_COUNT) > code;

    State[code] = { state, activeFrame };
}

const std::unordered_set<InputName>& InputMap::GetMapping(InputCode code) {
    static std::unordered_set<InputName> _default;
        
    auto itr = Mapping.find(code);
    return itr != Mapping.end() ? itr->second : _default;
}

float InputMap::GetRawState(InputCode code) {
    auto itr = State.find(code);
    return itr != State.end() ? itr->second.value : 0.f;
}

bool InputAxisSettings::operator==(const InputAxisSettings& other) const {
	return deadzone    == other.deadzone
		&& limitToUnit == other.limitToUnit
		&& normalized  == other.normalized
		&& mask        == other.mask;
}

bool InputAxisSettings::operator!=(const InputAxisSettings& other) const {
	return !operator==(other);
}

bool InputAxis::operator==(const InputAxis& other) const {
	return std::equal(components.begin(), components.end(), other.components.begin())
		&& settings == other.settings;
}

bool InputAxis::operator!=(const InputAxis& other) const {
	return !operator==(other);
}

bool AxisGroup::operator==(const AxisGroup& other) const {
	return std::equal(axes.begin(), axes.end(), other.axes.begin())
		&& settings == other.settings;
}

bool AxisGroup::operator!=(const AxisGroup& other) const {
	return !operator==(other);
}

InputMap::InputMap() {
    ViewportMin = vec2(0, 0);
    ViewportSize = vec2(1, 1);

    activeMask = {};
    activeFrame = 0;

    // populate initial states, kinda sucks to have to do this
    // but needed for iterating available states by using the keys of State
    // also could turn into a vector now that the input rage is linear

    State[GetInputCode(Key_A)].value = 0.f;
    State[GetInputCode(Key_B)].value = 0.f;
    State[GetInputCode(Key_C)].value = 0.f;
    State[GetInputCode(Key_D)].value = 0.f;
    State[GetInputCode(Key_E)].value = 0.f;
    State[GetInputCode(Key_F)].value = 0.f;
    State[GetInputCode(Key_G)].value = 0.f;
    State[GetInputCode(Key_H)].value = 0.f;
    State[GetInputCode(Key_I)].value = 0.f;
    State[GetInputCode(Key_J)].value = 0.f;
    State[GetInputCode(Key_K)].value = 0.f;
    State[GetInputCode(Key_L)].value = 0.f;
    State[GetInputCode(Key_M)].value = 0.f;
    State[GetInputCode(Key_N)].value = 0.f;
    State[GetInputCode(Key_O)].value = 0.f;
    State[GetInputCode(Key_P)].value = 0.f;
    State[GetInputCode(Key_Q)].value = 0.f;
    State[GetInputCode(Key_R)].value = 0.f;
    State[GetInputCode(Key_S)].value = 0.f;
    State[GetInputCode(Key_T)].value = 0.f;
    State[GetInputCode(Key_U)].value = 0.f;
    State[GetInputCode(Key_V)].value = 0.f;
    State[GetInputCode(Key_W)].value = 0.f;
    State[GetInputCode(Key_X)].value = 0.f;
    State[GetInputCode(Key_Y)].value = 0.f;
    State[GetInputCode(Key_Z)].value = 0.f;
    State[GetInputCode(Key_1)].value = 0.f;
    State[GetInputCode(Key_2)].value = 0.f;
    State[GetInputCode(Key_3)].value = 0.f;
    State[GetInputCode(Key_4)].value = 0.f;
    State[GetInputCode(Key_5)].value = 0.f;
    State[GetInputCode(Key_6)].value = 0.f;
    State[GetInputCode(Key_7)].value = 0.f;
    State[GetInputCode(Key_8)].value = 0.f;
    State[GetInputCode(Key_9)].value = 0.f;
    State[GetInputCode(Key_0)].value = 0.f;
    State[GetInputCode(Key_Return)].value = 0.f;
    State[GetInputCode(Key_Escape)].value = 0.f;
    State[GetInputCode(Key_Backspace)].value = 0.f;
    State[GetInputCode(Key_Tab)].value = 0.f;
    State[GetInputCode(Key_Space)].value = 0.f;
    State[GetInputCode(Key_Minus)].value = 0.f;
    State[GetInputCode(Key_Equals)].value = 0.f;
    State[GetInputCode(Key_Left_Bracket)].value = 0.f;
    State[GetInputCode(Key_Right_Bracket)].value = 0.f;
    State[GetInputCode(Key_Backslash)].value = 0.f;
    State[GetInputCode(Key_Nonushash)].value = 0.f;
    State[GetInputCode(Key_Semicolon)].value = 0.f;
    State[GetInputCode(Key_Apostrophe)].value = 0.f;
    State[GetInputCode(Key_Grave)].value = 0.f;
    State[GetInputCode(Key_Comma)].value = 0.f;
    State[GetInputCode(Key_Period)].value = 0.f;
    State[GetInputCode(Key_Slash)].value = 0.f;
    State[GetInputCode(Key_Lock_Caps)].value = 0.f;
    State[GetInputCode(Key_F1)].value = 0.f;
    State[GetInputCode(Key_F2)].value = 0.f;
    State[GetInputCode(Key_F3)].value = 0.f;
    State[GetInputCode(Key_F4)].value = 0.f;
    State[GetInputCode(Key_F5)].value = 0.f;
    State[GetInputCode(Key_F6)].value = 0.f;
    State[GetInputCode(Key_F7)].value = 0.f;
    State[GetInputCode(Key_F8)].value = 0.f;
    State[GetInputCode(Key_F9)].value = 0.f;
    State[GetInputCode(Key_F10)].value = 0.f;
    State[GetInputCode(Key_F11)].value = 0.f;
    State[GetInputCode(Key_F12)].value = 0.f;
    State[GetInputCode(Key_Printscreen)].value = 0.f;
    State[GetInputCode(Key_Lock_Scroll)].value = 0.f;
    State[GetInputCode(Key_Pause)].value = 0.f;
    State[GetInputCode(Key_Insert)].value = 0.f;
    State[GetInputCode(Key_Home)].value = 0.f;
    State[GetInputCode(Key_Page_Up)].value = 0.f;
    State[GetInputCode(Key_Delete)].value = 0.f;
    State[GetInputCode(Key_End)].value = 0.f;
    State[GetInputCode(Key_Page_Down)].value = 0.f;
    State[GetInputCode(Key_Right)].value = 0.f;
    State[GetInputCode(Key_Left)].value = 0.f;
    State[GetInputCode(Key_Down)].value = 0.f;
    State[GetInputCode(Key_Up)].value = 0.f;
    State[GetInputCode(Key_Numlockclear)].value = 0.f;
    State[GetInputCode(KeyPad_Divide)].value = 0.f;
    State[GetInputCode(KeyPad_Multiply)].value = 0.f;
    State[GetInputCode(KeyPad_Minus)].value = 0.f;
    State[GetInputCode(KeyPad_Plus)].value = 0.f;
    State[GetInputCode(KeyPad_Enter)].value = 0.f;
    State[GetInputCode(KeyPad_1)].value = 0.f;
    State[GetInputCode(KeyPad_2)].value = 0.f;
    State[GetInputCode(KeyPad_3)].value = 0.f;
    State[GetInputCode(KeyPad_4)].value = 0.f;
    State[GetInputCode(KeyPad_5)].value = 0.f;
    State[GetInputCode(KeyPad_6)].value = 0.f;
    State[GetInputCode(KeyPad_7)].value = 0.f;
    State[GetInputCode(KeyPad_8)].value = 0.f;
    State[GetInputCode(KeyPad_9)].value = 0.f;
    State[GetInputCode(KeyPad_0)].value = 0.f;
    State[GetInputCode(KeyPad_Period)].value = 0.f;
    State[GetInputCode(Key_Nonusbackslash)].value = 0.f;
    State[GetInputCode(Key_Application)].value = 0.f;
    State[GetInputCode(Key_Power)].value = 0.f;
    State[GetInputCode(KeyPad_Equals)].value = 0.f;
    State[GetInputCode(Key_F13)].value = 0.f;
    State[GetInputCode(Key_F14)].value = 0.f;
    State[GetInputCode(Key_F15)].value = 0.f;
    State[GetInputCode(Key_F16)].value = 0.f;
    State[GetInputCode(Key_F17)].value = 0.f;
    State[GetInputCode(Key_F18)].value = 0.f;
    State[GetInputCode(Key_F19)].value = 0.f;
    State[GetInputCode(Key_F20)].value = 0.f;
    State[GetInputCode(Key_F21)].value = 0.f;
    State[GetInputCode(Key_F22)].value = 0.f;
    State[GetInputCode(Key_F23)].value = 0.f;
    State[GetInputCode(Key_F24)].value = 0.f;
    State[GetInputCode(Key_Execute)].value = 0.f;
    State[GetInputCode(Key_Help)].value = 0.f;
    State[GetInputCode(Key_Menu)].value = 0.f;
    State[GetInputCode(Key_Select)].value = 0.f;
    State[GetInputCode(Key_Stop)].value = 0.f;
    State[GetInputCode(Key_Again)].value = 0.f;
    State[GetInputCode(Key_Undo)].value = 0.f;
    State[GetInputCode(Key_Cut)].value = 0.f;
    State[GetInputCode(Key_Copy)].value = 0.f;
    State[GetInputCode(Key_Paste)].value = 0.f;
    State[GetInputCode(Key_Find)].value = 0.f;
    State[GetInputCode(Key_Mute)].value = 0.f;
    State[GetInputCode(Key_Volume_Up)].value = 0.f;
    State[GetInputCode(Key_Volume_Down)].value = 0.f;
    State[GetInputCode(KeyPad_Comma)].value = 0.f;
    State[GetInputCode(KeyPad_Equalsas400)].value = 0.f;
    State[GetInputCode(Key_International1)].value = 0.f;
    State[GetInputCode(Key_International2)].value = 0.f;
    State[GetInputCode(Key_International3)].value = 0.f;
    State[GetInputCode(Key_International4)].value = 0.f;
    State[GetInputCode(Key_International5)].value = 0.f;
    State[GetInputCode(Key_International6)].value = 0.f;
    State[GetInputCode(Key_International7)].value = 0.f;
    State[GetInputCode(Key_International8)].value = 0.f;
    State[GetInputCode(Key_International9)].value = 0.f;
    State[GetInputCode(Key_Lang1)].value = 0.f;
    State[GetInputCode(Key_Lang2)].value = 0.f;
    State[GetInputCode(Key_Lang3)].value = 0.f;
    State[GetInputCode(Key_Lang4)].value = 0.f;
    State[GetInputCode(Key_Lang5)].value = 0.f;
    State[GetInputCode(Key_Lang6)].value = 0.f;
    State[GetInputCode(Key_Lang7)].value = 0.f;
    State[GetInputCode(Key_Lang8)].value = 0.f;
    State[GetInputCode(Key_Lang9)].value = 0.f;
    State[GetInputCode(Key_Alterase)].value = 0.f;
    State[GetInputCode(Key_Sysreq)].value = 0.f;
    State[GetInputCode(Key_Cancel)].value = 0.f;
    State[GetInputCode(Key_Clear)].value = 0.f;
    State[GetInputCode(Key_Prior)].value = 0.f;
    State[GetInputCode(Key_Return2)].value = 0.f;
    State[GetInputCode(Key_Separator)].value = 0.f;
    State[GetInputCode(Key_Out)].value = 0.f;
    State[GetInputCode(Key_Oper)].value = 0.f;
    State[GetInputCode(Key_Clearagain)].value = 0.f;
    State[GetInputCode(Key_Crsel)].value = 0.f;
    State[GetInputCode(Key_Exsel)].value = 0.f;
    State[GetInputCode(KeyPad_00)].value = 0.f;
    State[GetInputCode(KeyPad_000)].value = 0.f;
    State[GetInputCode(Key_Thousandsseparator)].value = 0.f;
    State[GetInputCode(Key_Decimalseparator)].value = 0.f;
    State[GetInputCode(Key_Currencyunit)].value = 0.f;
    State[GetInputCode(Key_Currencysubunit)].value = 0.f;
    State[GetInputCode(KeyPad_Left_Paren)].value = 0.f;
    State[GetInputCode(KeyPad_Right_Paren)].value = 0.f;
    State[GetInputCode(KeyPad_Left_Brace)].value = 0.f;
    State[GetInputCode(KeyPad_Right_Brace)].value = 0.f;
    State[GetInputCode(KeyPad_Tab)].value = 0.f;
    State[GetInputCode(KeyPad_Backspace)].value = 0.f;
    State[GetInputCode(KeyPad_A)].value = 0.f;
    State[GetInputCode(KeyPad_B)].value = 0.f;
    State[GetInputCode(KeyPad_C)].value = 0.f;
    State[GetInputCode(KeyPad_D)].value = 0.f;
    State[GetInputCode(KeyPad_E)].value = 0.f;
    State[GetInputCode(KeyPad_F)].value = 0.f;
    State[GetInputCode(KeyPad_Xor)].value = 0.f;
    State[GetInputCode(KeyPad_Power)].value = 0.f;
    State[GetInputCode(KeyPad_Percent)].value = 0.f;
    State[GetInputCode(KeyPad_Less)].value = 0.f;
    State[GetInputCode(KeyPad_Greater)].value = 0.f;
    State[GetInputCode(KeyPad_Ampersand)].value = 0.f;
    State[GetInputCode(KeyPad_Dblampersand)].value = 0.f;
    State[GetInputCode(KeyPad_Verticalbar)].value = 0.f;
    State[GetInputCode(KeyPad_Dblverticalbar)].value = 0.f;
    State[GetInputCode(KeyPad_Colon)].value = 0.f;
    State[GetInputCode(KeyPad_Hash)].value = 0.f;
    State[GetInputCode(KeyPad_Space)].value = 0.f;
    State[GetInputCode(KeyPad_At)].value = 0.f;
    State[GetInputCode(KeyPad_Exclam)].value = 0.f;
    State[GetInputCode(KeyPad_Memstore)].value = 0.f;
    State[GetInputCode(KeyPad_Memrecall)].value = 0.f;
    State[GetInputCode(KeyPad_Memclear)].value = 0.f;
    State[GetInputCode(KeyPad_Memadd)].value = 0.f;
    State[GetInputCode(KeyPad_Memsubtract)].value = 0.f;
    State[GetInputCode(KeyPad_Memmultiply)].value = 0.f;
    State[GetInputCode(KeyPad_Memdivide)].value = 0.f;
    State[GetInputCode(KeyPad_Plusminus)].value = 0.f;
    State[GetInputCode(KeyPad_Clear)].value = 0.f;
    State[GetInputCode(KeyPad_Clearentry)].value = 0.f;
    State[GetInputCode(KeyPad_Binary)].value = 0.f;
    State[GetInputCode(KeyPad_Octal)].value = 0.f;
    State[GetInputCode(KeyPad_Decimal)].value = 0.f;
    State[GetInputCode(KeyPad_Hexadecimal)].value = 0.f;
    State[GetInputCode(Key_Left_Control)].value = 0.f;
    State[GetInputCode(Key_Left_Shift)].value = 0.f;
    State[GetInputCode(Key_Left_Alt)].value = 0.f;
    State[GetInputCode(Key_Left_GUI)].value = 0.f;
    State[GetInputCode(Key_Right_Control)].value = 0.f;
    State[GetInputCode(Key_Right_Shift)].value = 0.f;
    State[GetInputCode(Key_Right_Alt)].value = 0.f;
    State[GetInputCode(Key_Right_GUI)].value = 0.f;
    State[GetInputCode(Key_Mode)].value = 0.f;
    State[GetInputCode(Key_Audion_Exit)].value = 0.f;
    State[GetInputCode(Key_Audio_Previous)].value = 0.f;
    State[GetInputCode(Key_Audio_Stop)].value = 0.f;
    State[GetInputCode(Key_Audio_Play)].value = 0.f;
    State[GetInputCode(Key_Audio_Mute)].value = 0.f;
    State[GetInputCode(Key_Media_Select)].value = 0.f;
    State[GetInputCode(Key_WWW)].value = 0.f;
    State[GetInputCode(Key_Mail)].value = 0.f;
    State[GetInputCode(Key_Calculator)].value = 0.f;
    State[GetInputCode(Key_Computer)].value = 0.f;
    State[GetInputCode(Key_AcSearch)].value = 0.f;
    State[GetInputCode(Key_AcHome)].value = 0.f;
    State[GetInputCode(Key_AcBack)].value = 0.f;
    State[GetInputCode(Key_AcForward)].value = 0.f;
    State[GetInputCode(Key_AcStop)].value = 0.f;
    State[GetInputCode(Key_AcRefresh)].value = 0.f;
    State[GetInputCode(Key_AcBookmarks)].value = 0.f;
    State[GetInputCode(Key_Brightness_Down)].value = 0.f;
    State[GetInputCode(Key_Brightness_Up)].value = 0.f;
    State[GetInputCode(Key_Display_Switch)].value = 0.f;
    State[GetInputCode(Key_Kbdillum_Toggle)].value = 0.f;
    State[GetInputCode(Key_Kbdillum_Down)].value = 0.f;
    State[GetInputCode(Key_Kbdillum_Up)].value = 0.f;
    State[GetInputCode(Key_Eject)].value = 0.f;
    State[GetInputCode(Key_Sleep)].value = 0.f;
    State[GetInputCode(Key_App1)].value = 0.f;
    State[GetInputCode(Key_App2)].value = 0.f;
    State[GetInputCode(Key_Audio_Rewind)].value = 0.f;
    State[GetInputCode(Key_Audio_Fastforward)].value = 0.f;
    State[GetInputCode(Key_Left_Soft)].value = 0.f;
    State[GetInputCode(Key_Right_Soft)].value = 0.f;
    State[GetInputCode(Key_Call)].value = 0.f;
    State[GetInputCode(Key_Call_End)].value = 0.f;

    State[GetInputCode(MOUSE_LEFT)].value = 0.f;
    State[GetInputCode(MOUSE_MIDDLE)].value = 0.f;
    State[GetInputCode(MOUSE_RIGHT)].value = 0.f;
    State[GetInputCode(MOUSE_X1)].value = 0.f;
    State[GetInputCode(MOUSE_X2)].value = 0.f;
    State[GetInputCode(MOUSE_POS_X)].value = 0.f;
    State[GetInputCode(MOUSE_POS_Y)].value = 0.f;
    State[GetInputCode(MOUSE_VEL_X)].value = 0.f;
    State[GetInputCode(MOUSE_VEL_Y)].value = 0.f;
    State[GetInputCode(MOUSE_VEL_WHEEL_X)].value = 0.f;
    State[GetInputCode(MOUSE_VEL_WHEEL_Y)].value = 0.f;

    State[GetInputCode(BUTTON_A)].value = 0.f;
    State[GetInputCode(BUTTON_B)].value = 0.f;
    State[GetInputCode(BUTTON_X)].value = 0.f;
    State[GetInputCode(BUTTON_Y)].value = 0.f;
    State[GetInputCode(BUTTON_BACK)].value = 0.f;
    State[GetInputCode(BUTTON_GUIDE)].value = 0.f;
    State[GetInputCode(BUTTON_START)].value = 0.f;
    State[GetInputCode(BUTTON_LEFTSTICK)].value = 0.f;
    State[GetInputCode(BUTTON_RIGHTSTICK)].value = 0.f;
    State[GetInputCode(BUTTON_LEFTSHOULDER)].value = 0.f;
    State[GetInputCode(BUTTON_RIGHTSHOULDER)].value = 0.f;
    State[GetInputCode(BUTTON_DPAD_UP)].value = 0.f;
    State[GetInputCode(BUTTON_DPAD_DOWN)].value = 0.f;
    State[GetInputCode(BUTTON_DPAD_LEFT)].value = 0.f;
    State[GetInputCode(BUTTON_DPAD_RIGHT)].value = 0.f;
    State[GetInputCode(BUTTON_MISC1)].value = 0.f;
    State[GetInputCode(BUTTON_PADDLE1)].value = 0.f;
    State[GetInputCode(BUTTON_PADDLE2)].value = 0.f;
    State[GetInputCode(BUTTON_PADDLE3)].value = 0.f;
    State[GetInputCode(BUTTON_PADDLE4)].value = 0.f;
    State[GetInputCode(BUTTON_TOUCHPAD)].value = 0.f;
    State[GetInputCode(AXIS_LEFTX)].value = 0.f;
    State[GetInputCode(AXIS_LEFTY)].value = 0.f;
    State[GetInputCode(AXIS_RIGHTX)].value = 0.f;
    State[GetInputCode(AXIS_RIGHTY)].value = 0.f;
    State[GetInputCode(AXIS_TRIGGERLEFT)].value = 0.f;
    State[GetInputCode(AXIS_TRIGGERRIGHT)].value = 0.f;
}
