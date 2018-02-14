/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_INPUT_EVENT_H
#define SOLARUS_INPUT_EVENT_H

#include "solarus/core/Common.h"
#include "solarus/core/EnumInfo.h"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <queue>

#include <SFML/Window/Event.hpp>

namespace Solarus {

class Point;
class Rectangle;

/**
 * \brief Represents a low-level event.
 *
 * This low-level class provides an API for input events (keyboard, joypad and window events).
 * It encapsulates the library-dependent events.
 */
class InputEvent {

  public:

    /**
     * Enumeration of keyboard keys.
     * This is a Solarus dedicated enumeration.
     * To make the SFML encapsulation easier, this enumeration is compatible
     * with the sf::Keyboard::Key enumeration from SFML.
     * If you intend to replace SFML by another library one day, the API of the
     * Input class will not have to change
     * (only its implementation will).
     * /!\ Now that SFML is in place theses defines have changed, but some keys are missing
     * and redirected as Unknown.
     * The numeric values of the enumeration can be changed without breaking
     * savegames because savegames use a string name to store the commands.
     */
    enum class KeyboardKey {
      NONE                     = sf::Keyboard::Key::Unknown,
      BACKSPACE                = sf::Keyboard::Key::BackSpace,
      TABULATION               = sf::Keyboard::Key::Tab,
      CLEAR                    = sf::Keyboard::Key::Unknown,
      RETURN                   = sf::Keyboard::Key::Return,
      PAUSE                    = sf::Keyboard::Key::Pause,
      ESCAPE                   = sf::Keyboard::Key::Escape,
      SPACE                    = sf::Keyboard::Key::Space,
      EXCLAMATION_MARK         = sf::Keyboard::Key::Unknown, //TODO check
      DOULE_QUOTE              = sf::Keyboard::Key::Quote, //TODO check
      HASH                     = sf::Keyboard::Key::Unknown,
      DOLLAR                   = sf::Keyboard::Unknown,
      AMPERSAND                = sf::Keyboard::Unknown,
      SINGLE_QUOTE             = sf::Keyboard::Quote,
      LEFT_PARENTHESIS         = sf::Keyboard::Unknown,
      RIGHT_PARENTHESIS        = sf::Keyboard::Unknown,
      ASTERISK                 = sf::Keyboard::Key::Unknown,
      PLUS                     = sf::Keyboard::Key::Add,
      COMMA                    = sf::Keyboard::Key::Comma,
      MINUS                    = sf::Keyboard::Key::Subtract,
      PERIOD                   = sf::Keyboard::Key::Period,
      SLASH                    = sf::Keyboard::Key::Slash,
      NUMBER_0                 = sf::Keyboard::Key::Num0,
      NUMBER_1                 = sf::Keyboard::Key::Num1,
      NUMBER_2                 = sf::Keyboard::Key::Num2,
      NUMBER_3                 = sf::Keyboard::Key::Num3,
      NUMBER_4                 = sf::Keyboard::Key::Num4,
      NUMBER_5                 = sf::Keyboard::Key::Num5,
      NUMBER_6                 = sf::Keyboard::Key::Num6,
      NUMBER_7                 = sf::Keyboard::Key::Num7,
      NUMBER_8                 = sf::Keyboard::Key::Num8,
      NUMBER_9                 = sf::Keyboard::Key::Num9,
      COLON                    = sf::Keyboard::Key::Unknown,
      SEMICOLON                = sf::Keyboard::Key::SemiColon,
      LESS                     = sf::Keyboard::Key::Unknown,
      EQUALS                   = sf::Keyboard::Key::Equal,
      GREATER                  = sf::Keyboard::Key::Unknown,
      QUESTION_MARK            = sf::Keyboard::Key::Unknown,
      AT                       = sf::Keyboard::Key::Unknown,
      LEFT_BRACKET             = sf::Keyboard::Key::LBracket,
      BACKSLASH                = sf::Keyboard::Key::BackSlash,
      RIGHT_BRACKET            = sf::Keyboard::Key::RBracket,
      CARET                    = sf::Keyboard::Key::Unknown,
      UNDERSCORE               = sf::Keyboard::Key::Unknown,
      BACKQUOTE                = sf::Keyboard::Key::Unknown,
      a                        = sf::Keyboard::Key::A,
      b                        = sf::Keyboard::Key::B,
      c                        = sf::Keyboard::Key::C,
      d                        = sf::Keyboard::Key::D,
      e                        = sf::Keyboard::Key::E,
      f                        = sf::Keyboard::Key::F,
      g                        = sf::Keyboard::Key::G,
      h                        = sf::Keyboard::Key::H,
      i                        = sf::Keyboard::Key::I,
      j                        = sf::Keyboard::Key::J,
      k                        = sf::Keyboard::Key::K,
      l                        = sf::Keyboard::Key::L,
      m                        = sf::Keyboard::Key::M,
      n                        = sf::Keyboard::Key::N,
      o                        = sf::Keyboard::Key::O,
      p                        = sf::Keyboard::Key::P,
      q                        = sf::Keyboard::Key::Q,
      r                        = sf::Keyboard::Key::R,
      s                        = sf::Keyboard::Key::S,
      t                        = sf::Keyboard::Key::T,
      u                        = sf::Keyboard::Key::U,
      v                        = sf::Keyboard::Key::V,
      w                        = sf::Keyboard::Key::W,
      x                        = sf::Keyboard::Key::X,
      y                        = sf::Keyboard::Key::Y,
      z                        = sf::Keyboard::Key::Z,
      KEY_DELETE               = sf::Keyboard::Key::Delete,

      KP0                      = sf::Keyboard::Key::Numpad0,
      KP1                      = sf::Keyboard::Key::Numpad1,
      KP2                      = sf::Keyboard::Key::Numpad2,
      KP3                      = sf::Keyboard::Key::Numpad3,
      KP4                      = sf::Keyboard::Key::Numpad4,
      KP5                      = sf::Keyboard::Key::Numpad5,
      KP6                      = sf::Keyboard::Key::Numpad6,
      KP7                      = sf::Keyboard::Key::Numpad7,
      KP8                      = sf::Keyboard::Key::Numpad8,
      KP9                      = sf::Keyboard::Key::Numpad9,
      KP_PERIOD                = sf::Keyboard::Key::Period,
      KP_DIVIDE                = sf::Keyboard::Key::Divide,
      KP_MULTIPLY              = sf::Keyboard::Key::Multiply,
      KP_MINUS                 = sf::Keyboard::Key::Subtract,
      KP_PLUS                  = sf::Keyboard::Key::Add,
      KP_ENTER                 = sf::Keyboard::Key::Return, //TODO check
      KP_EQUALS                = sf::Keyboard::Key::Equal,

      UP                       = sf::Keyboard::Key::Up,
      DOWN                     = sf::Keyboard::Key::Down,
      RIGHT                    = sf::Keyboard::Key::Right,
      LEFT                     = sf::Keyboard::Key::Left,
      INSERT                   = sf::Keyboard::Key::Insert,
      HOME                     = sf::Keyboard::Key::Home,
      END                      = sf::Keyboard::Key::End,
      PAGE_UP                  = sf::Keyboard::Key::PageUp,
      PAGE_DOWN                = sf::Keyboard::Key::PageDown,

      F1                       = sf::Keyboard::Key::F1,
      F2                       = sf::Keyboard::Key::F2,
      F3                       = sf::Keyboard::Key::F3,
      F4                       = sf::Keyboard::Key::F4,
      F5                       = sf::Keyboard::Key::F5,
      F6                       = sf::Keyboard::Key::F6,
      F7                       = sf::Keyboard::Key::F7,
      F8                       = sf::Keyboard::Key::F8,
      F9                       = sf::Keyboard::Key::F9,
      F10                      = sf::Keyboard::Key::F10,
      F11                      = sf::Keyboard::Key::F11,
      F12                      = sf::Keyboard::Key::F12,
      F13                      = sf::Keyboard::Key::F13,
      F14                      = sf::Keyboard::Key::F14,
      F15                      = sf::Keyboard::Key::F15,

      NUMLOCK                  = sf::Keyboard::Key::Unknown,
      CAPSLOCK                 = sf::Keyboard::Key::Unknown,
      SCROLLOCK                = sf::Keyboard::Key::Unknown,
      RIGHT_SHIFT              = sf::Keyboard::Key::RShift,
      LEFT_SHIFT               = sf::Keyboard::Key::LShift,
      RIGHT_CONTROL            = sf::Keyboard::Key::RControl,
      LEFT_CONTROL             = sf::Keyboard::Key::LControl,
      RIGHT_ALT                = sf::Keyboard::Key::RAlt,
      LEFT_ALT                 = sf::Keyboard::Key::LAlt,
      RIGHT_META               = sf::Keyboard::Key::RSystem,
      LEFT_META                = sf::Keyboard::Key::LSystem
    };

    /**
     * Enumeration of mouse buttons.
     * This is a Solarus dedicated enumeration.
     * To make the SFML encapsulation easier, this enumeration is compatible
     * with sf::Mouse::Button definitions from SFML.
     * If you intend to replace SFML by another library one day, the API of the
     * Input class will not have to change
     * (only its implementation will).
     */
    enum class MouseButton {
      NONE            = -1,
      LEFT            = sf::Mouse::Button::Left,
      MIDDLE          = sf::Mouse::Button::Middle,
      RIGHT           = sf::Mouse::Button::Right,
      X1              = sf::Mouse::Button::XButton1,
      X2              = sf::Mouse::Button::XButton2,
    };

    static void initialize();
    static void quit();
    static bool is_initialized();

    // retrieve the current event
    static std::unique_ptr<InputEvent> get_event(sf::Window& window);
    static void push_event(const sf::Event& event);

    // global information
    static void set_key_repeat(bool repeat);
    static bool is_shift_down();
    static bool is_control_down();
    static bool is_alt_down();
    static bool is_caps_lock_on();
    static bool is_num_lock_on();
    static bool is_key_down(KeyboardKey key);
    static bool is_joypad_button_down(int button);
    static bool is_mouse_button_down(MouseButton button);
    static bool is_finger_down(int finger_id);
    static int get_joypad_axis_state(int axis);
    static int get_joypad_hat_direction(int hat);
    static Point get_global_mouse_position();
    static bool get_global_finger_position(int finger_id, Point& finger_xy);
    static bool get_global_finger_pressure(int finger_id, float& finger_pressure);

    // event type
    bool is_valid() const;
    bool is_keyboard_event() const;
    bool is_joypad_event() const;
    bool is_mouse_event() const;
    bool is_finger_event() const;
    bool is_window_event() const;

    // keyboard
    bool is_keyboard_key_pressed() const;
    bool is_keyboard_key_pressed(KeyboardKey key) const;
    bool is_keyboard_key_pressed(const KeyboardKey* keys) const;
    bool is_keyboard_direction_key_pressed() const;
    bool is_keyboard_non_direction_key_pressed() const;

    bool is_keyboard_key_released() const;
    bool is_keyboard_key_released(KeyboardKey key) const;
    bool is_keyboard_key_released(const KeyboardKey* keys) const;
    bool is_keyboard_direction_key_released() const;
    bool is_keyboard_non_direction_key_released() const;

    bool is_with_shift() const;
    bool is_with_control() const;
    bool is_with_alt() const;

    KeyboardKey get_keyboard_key() const;

    bool is_character_pressed() const;
    std::string get_character() const;

    static void simulate_key_pressed(KeyboardKey key);
    static void simulate_key_released(KeyboardKey key);

    // joypad
    static bool is_joypad_enabled();
    static void set_joypad_enabled(bool joypad_enabled);

    bool is_joypad_button_pressed() const;
    bool is_joypad_button_released() const;
    int get_joypad_button() const;
    bool is_joypad_axis_moved() const;
    int get_joypad_axis() const;
    int get_joypad_axis_state() const;
    bool is_joypad_axis_centered() const;
    bool is_joypad_hat_moved() const;
    int get_joypad_hat() const;
    int get_joypad_hat_direction() const;
    bool is_joypad_hat_centered() const;

    // mouse
    bool is_mouse_button_pressed() const;
    bool is_mouse_button_pressed(MouseButton button) const;
    bool is_mouse_button_released() const;
    bool is_mouse_button_released(MouseButton button) const;

    MouseButton get_mouse_button() const;
    bool get_mouse_position(Point& mouse_xy) const;

    // touch finger
    bool is_finger_pressed() const;
    bool is_finger_pressed(int finger_id) const;
    bool is_finger_released() const;
    bool is_finger_released(int finger_id) const;
    bool is_finger_moved() const;
    bool is_finger_moved(int finger_id) const;

    int get_finger() const;
    Point get_finger_position() const;
    Point get_finger_distance() const;
    float get_finger_pressure() const;

    // functions common to keyboard, joypad and mouse
    int get_direction() const;
    bool is_pressed() const;
    bool is_direction_pressed() const;
    bool is_non_direction_pressed() const;
    bool is_released() const;

    // window event
    bool is_window_closing() const;

  private:

    explicit InputEvent(const sf::Event &event);

    static const KeyboardKey directional_keys[];  /**< array of the keyboard directional keys */
    static bool initialized;                      /**< Whether the input manager is initialized. */
    static bool joypad_enabled;                   /**< true if joypad support is enabled
                                                   * (may be true even without joypad plugged) */

    static std::map<KeyboardKey, std::string>
      keyboard_key_names;                         /**< Names of all existing keyboard keys. */
    static std::map<MouseButton, std::string>
      mouse_button_names;                         /**< Names of all existing mouse buttons. */
    static bool repeat_keyboard;                  /**< True to handle repeat KEYDOWN and KEYUP events. */

    static std::queue<sf::Event> simulated_events; /**< queue of simulated events */

    const sf::Event internal_event;               /**< the internal event encapsulated */

};

template <>
struct SOLARUS_API EnumInfoTraits<InputEvent::KeyboardKey> {
  static const std::string pretty_name;

  static const EnumInfo<InputEvent::KeyboardKey>::names_type names;
};

template <>
struct SOLARUS_API EnumInfoTraits<InputEvent::MouseButton> {
  static const std::string pretty_name;

  static const EnumInfo<InputEvent::MouseButton>::names_type names;
};

}

#endif

