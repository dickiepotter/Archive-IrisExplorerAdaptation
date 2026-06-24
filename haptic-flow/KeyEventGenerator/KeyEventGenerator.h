#ifndef _KEYEVENTGENERATOR_H
#define _KEYEVENTGENERATOR_H

//=============================================================================
// KeyEventGenerator.h
//
// PURPOSE
// -------
// KeyEventGenerator is an H3DAPI node that turns a chosen set of keyboard keys
// into a single boolean "toggle" output. It extends the standard H3D KeySensor
// node, which already reports raw keyboard activity (key presses/releases,
// modifier-key state, etc.). On top of that raw stream, this node watches for
// a configurable list of "trigger" keys and flips a boolean every time one of
// them is pressed.
//
// TYPICAL USE (in an X3D scene)
// -----------------------------
//   - Set the "keys" field to a string containing the characters that should
//     act as triggers (e.g. " " for the space bar, or "ab" for the 'a' and
//     'b' keys).
//   - Route the "event" boolean output to whatever you want to toggle on and
//     off (a switch, a visibility flag, an effect, etc.).
//   Each press of a trigger key inverts "event": false -> true -> false ...
//
// RELATIONSHIP TO KeySensor (the base class)
// ------------------------------------------
// KeySensor provides the inherited output fields actionKeyPress,
// actionKeyRelease, altKey, controlKey, keyPress, keyRelease and shiftKey, plus
// the virtual keyboard callbacks (such as glutKeyboardDown) that the H3D
// windowing layer invokes when the user types. We override the key-down
// callback to add our toggle behaviour while still letting the base class do
// its normal work.
//=============================================================================

#include "keg_api.h"
#include <KeySensor.h>
#include <SFBool.h>
#include <SFString.h>

using namespace H3D;

/// \brief A KeySensor that toggles a boolean output whenever one of a chosen
///        set of keys is pressed.
///
/// The node registers two of its own fields with the H3D database:
///   - \c keys  (INPUT_OUTPUT) : the set of characters that act as triggers.
///   - \c event (OUTPUT_ONLY)  : the toggle state, inverted on each trigger.
/// It also inherits and re-registers the KeySensor output fields.
class KEG_API KeyEventGenerator: public KeySensor {
	public:
		/// Construct the node, allocate its fields and enable key sensing.
		KeyEventGenerator(void);

		// Note: an "up" (key release) handler exists in the base class but is
		// intentionally not overridden here; only key-down events drive the
		// toggle. The historical declaration is kept as a reminder.
		//void glutKeyboardUp(int stroke, int modifiers, bool special_key);

		/// \brief Called by the H3D windowing layer when a key is pressed.
		///
		/// \param stroke      Raw key code for the pressed key.
		/// \param modifiers   Bit flags describing active modifier keys.
		/// \param special_key True for "special" (non-character) keys.
		///
		/// Overrides KeySensor::glutKeyboardDown to add the toggle behaviour;
		/// see the .cpp file for the full explanation.
		void glutKeyboardDown (int stroke, int modifiers, bool special_key);

		/// Boolean toggle output. Inverted each time a trigger key is pressed.
		auto_ptr< SFBool > event;

		/// The set of trigger characters to watch for, as a string.
		auto_ptr< SFString > keys;

		/// H3D type database entry that registers this node under the name
		/// "KeyEventGenerator" (see the .cpp file).
		static H3DNodeDatabase database;

	protected:
		/// Internal toggle state that "event" mirrors. Starts false and is
		/// inverted on every trigger-key press. (Original field name: "on".)
		bool isToggledOn;
};

#endif // _KEYEVENTGENERATOR_H
