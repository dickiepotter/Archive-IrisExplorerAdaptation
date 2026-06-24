#include "KeyEventGenerator.h"

//=============================================================================
// KeyEventGenerator.cpp
//
// This file does three things:
//   1. Registers KeyEventGenerator with the H3D node database so it can be
//      created by name (e.g. from an X3D scene file).
//   2. Declares which fields are exposed, and in which direction, via the
//      FIELDDB_ELEMENT macros.
//   3. Implements the constructor and the key-down callback that performs the
//      toggle.
//=============================================================================

//-----------------------------------------------------------------------------
// Node-database registration.
//
// This static H3DNodeDatabase instance adds KeyEventGenerator to H3D's global
// registry of node types. The arguments are:
//   - "KeyEventGenerator" : the type name X3D/scene files use to instantiate
//                           the node. MUST match exactly across the codebase.
//   - &(newInstance<KeyEventGenerator>) : factory function H3D calls to make
//                           a new instance of this node.
//   - typeid( KeyEventGenerator ) : RTTI handle used to match C++ type to name.
//   - &KeySensor::database : the parent database, so this node inherits all of
//                           KeySensor's registered fields.
//-----------------------------------------------------------------------------
H3DNodeDatabase KeyEventGenerator::database(
		"KeyEventGenerator",
		&(newInstance<KeyEventGenerator>),
		typeid( KeyEventGenerator ),
		&KeySensor::database );

//-----------------------------------------------------------------------------
// Field registration.
//
// Each FIELDDB_ELEMENT entry tells H3D about one field on the node: its owning
// class, the member name, and its access direction (OUTPUT_ONLY, INPUT_OUTPUT,
// etc.). The first seven entries re-register fields inherited from KeySensor so
// they appear on this node too; the last two register this node's own "keys"
// and "event" fields.
//
// NOTE: the inherited KeySensor field entries must be left exactly as-is --
// their member names are defined by the base class, not by this file.
//-----------------------------------------------------------------------------
namespace KeyEventGeneratorInternals {
	FIELDDB_ELEMENT( KeyEventGenerator, actionKeyPress, OUTPUT_ONLY );
	FIELDDB_ELEMENT( KeyEventGenerator, actionKeyRelease, OUTPUT_ONLY );
	FIELDDB_ELEMENT( KeyEventGenerator, altKey, OUTPUT_ONLY );
	FIELDDB_ELEMENT( KeyEventGenerator, controlKey, OUTPUT_ONLY );
	FIELDDB_ELEMENT( KeyEventGenerator, keyPress, OUTPUT_ONLY );
	FIELDDB_ELEMENT( KeyEventGenerator, keyRelease, OUTPUT_ONLY );
	FIELDDB_ELEMENT( KeyEventGenerator, shiftKey, OUTPUT_ONLY );
	FIELDDB_ELEMENT( KeyEventGenerator, keys, INPUT_OUTPUT );
	FIELDDB_ELEMENT( KeyEventGenerator, event, OUTPUT_ONLY );

}


//-----------------------------------------------------------------------------
// Constructor.
//
// The member-initialiser list:
//   - allocates the "event" output field as a fresh SFBool,
//   - allocates the "keys" input/output field as a fresh SFString,
//   - initialises the internal toggle state to false (off).
// Then the body enables key sensing so the node actually receives keyboard
// callbacks from the windowing layer.
//-----------------------------------------------------------------------------
KeyEventGenerator::KeyEventGenerator(void):
	event(new SFBool),
	keys(new SFString),
	isToggledOn(false)
{
	// Historical no-op: this default-constructs a temporary KeySensor and
	// throws it away. The real base initialisation happens implicitly. Kept
	// to preserve original behaviour exactly.
	KeySensor();

	// Turn the sensor on so keyboard events are delivered to this node.
	this->enabled->setValue(true);
}

//-----------------------------------------------------------------------------
// Key-down callback.
//
// Invoked by the H3D windowing layer each time a key is pressed. The behaviour
// is:
//   1. Let the base KeySensor do its normal processing first (this is what
//      populates the inherited keyPress / modifier output fields, etc.).
//   2. Check whether the pressed key is one of our configured trigger keys.
//      keys->getValue() returns the trigger string; we look for the pressed
//      character within it. find(...) returns string::npos when the character
//      is not present, so "!= string::npos" means "this key is a trigger".
//   3. If it is a trigger, invert the internal toggle state and publish the
//      new value to the "event" output field. Each trigger press therefore
//      flips event between true and false.
//-----------------------------------------------------------------------------
void KeyEventGenerator::glutKeyboardDown (int stroke, int modifiers, bool special_key)
{
	// Step 1: preserve the base class's standard key handling.
	KeySensor::glutKeyboardDown(stroke, modifiers, special_key);

	// Step 2: is the pressed character one of the configured trigger keys?
	if( keys->getValue().find(char(stroke) ) != string::npos){
		// Step 3: flip the toggle and push it to the output field.
		isToggledOn = !isToggledOn;
		event->setValue(isToggledOn);
	}
}

