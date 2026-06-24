#include "FloatUpdater.h"
#include <iostream>

//=============================================================================
// FloatUpdater.cpp
//
// Registers the FloatUpdater node, declares its fields, and implements the
// constructor plus the increase/decrease step logic. See FloatUpdater.h for
// the overview of the "increase/decrease by offset" pattern. This is the
// float twin of IntUpdater; the one functional difference is that the float
// version keeps active std::cerr trace output in update() (the int version has
// those traces commented out).
//=============================================================================

//-----------------------------------------------------------------------------
// Node-database registration.
//
// Registers this node under the name "FloatUpdater" with a factory function
// and RTTI handle, parented to X3DChildNode's database. The name string MUST
// match exactly across the codebase / scene files. The two-argument form just
// below is an earlier variant kept commented out as a historical note.
//-----------------------------------------------------------------------------
H3DNodeDatabase FloatUpdater::database("FloatUpdater", &(newInstance<FloatUpdater>),
		typeid( FloatUpdater ), &X3DChildNode::database);
//H3DNodeDatabase FloatUpdater::database("FloatUpdater", &(newInstance<FloatUpdater>),
//		typeid( FloatUpdater ));

//-----------------------------------------------------------------------------
// Field registration: exposes value, increase, decrease and offset as
// INPUT_OUTPUT fields so they can be both set and routed from a scene.
//-----------------------------------------------------------------------------
namespace FloatUpdaterInternals {
	FIELDDB_ELEMENT( FloatUpdater, value, INPUT_OUTPUT );
	FIELDDB_ELEMENT( FloatUpdater, increase, INPUT_OUTPUT );
	FIELDDB_ELEMENT( FloatUpdater, decrease, INPUT_OUTPUT );
	FIELDDB_ELEMENT( FloatUpdater, offset, INPUT_OUTPUT );
}

//-----------------------------------------------------------------------------
// Constructor.
//
// Allocates the value/offset fields and the two trigger fields. The triggers
// are built with +1 ("increase") and -1 ("decrease") so they step in opposite
// directions; each holds a back-pointer to this node (passed as "this").
//-----------------------------------------------------------------------------
FloatUpdater::FloatUpdater(float initialOffset):
	value(new SFFloat),
	offset(new SFFloat(initialOffset)),
	increase(new ValUpdater(this, 1)),
	decrease(new ValUpdater(this, - 1))
{
	// Historical no-op temporary (kept to match original behaviour exactly).
	SFFloat();
}

//-----------------------------------------------------------------------------
// ValUpdater constructor: store the owning node and the step sign.
//-----------------------------------------------------------------------------
FloatUpdater::ValUpdater::ValUpdater(FloatUpdater * owningUpdater, int direction):
	owningUpdater(owningUpdater),
	direction(direction)
{
}

//-----------------------------------------------------------------------------
// ValUpdater::update -- apply one step to the owner's value.
//
// First lets the SFBool base finish its normal update, then reads the current
// value, adds (direction * offset) -- +offset for increase, -offset for
// decrease -- and writes the result back. The std::cerr lines print the before
// and after values; they are kept active here exactly as in the original.
//-----------------------------------------------------------------------------
void FloatUpdater::ValUpdater::update()
{
	SFBool::update();
	float newValue = owningUpdater->value->getValue();
	std::cerr << "updating from " << newValue;
	newValue += direction * owningUpdater->offset->getValue();
	std::cerr << " to " << newValue << std::endl;
	owningUpdater->value->setValue(newValue);

}
