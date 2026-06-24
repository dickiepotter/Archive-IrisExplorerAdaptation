#include "IntUpdater.h"
#include <iostream>

//=============================================================================
// IntUpdater.cpp
//
// Registers the IntUpdater node, declares its fields, and implements the
// constructor plus the increase/decrease step logic. See IntUpdater.h for the
// overview of the "increase/decrease by offset" pattern.
//=============================================================================

//-----------------------------------------------------------------------------
// Node-database registration.
//
// Registers this node under the name "IntUpdater" with a factory function and
// RTTI handle, parented to X3DChildNode's database so it inherits child-node
// fields. The name string MUST match exactly across the codebase / scene files.
//
// The two-argument form just below is an earlier variant (no parent database)
// kept commented out as a historical note.
//-----------------------------------------------------------------------------
H3DNodeDatabase IntUpdater::database("IntUpdater", &(newInstance<IntUpdater>),
		typeid( IntUpdater ), &X3DChildNode::database);
//H3DNodeDatabase IntUpdater::database("IntUpdater", &(newInstance<IntUpdater>),
//		typeid( IntUpdater ));

//-----------------------------------------------------------------------------
// Field registration: exposes value, increase, decrease and offset as
// INPUT_OUTPUT fields so they can be both set and routed from a scene.
//-----------------------------------------------------------------------------
namespace IntUpdaterInternals {
	FIELDDB_ELEMENT( IntUpdater, value, INPUT_OUTPUT );
	FIELDDB_ELEMENT( IntUpdater, increase, INPUT_OUTPUT );
	FIELDDB_ELEMENT( IntUpdater, decrease, INPUT_OUTPUT );
	FIELDDB_ELEMENT( IntUpdater, offset, INPUT_OUTPUT );
}

//-----------------------------------------------------------------------------
// Constructor.
//
// Allocates the value/offset fields and the two trigger fields. The triggers
// are built with +1 ("increase") and -1 ("decrease") so they step in opposite
// directions; each holds a back-pointer to this node (passed as "this").
//-----------------------------------------------------------------------------
IntUpdater::IntUpdater(int initialOffset):
	value(new SFInt32),
	offset(new SFInt32(initialOffset)),
	increase(new ValUpdater(this, 1)),
	decrease(new ValUpdater(this, - 1))
{
	// Historical no-op temporary (kept to match original behaviour exactly).
	SFInt32();
}

//-----------------------------------------------------------------------------
// ValUpdater constructor: store the owning node and the step sign.
//-----------------------------------------------------------------------------
IntUpdater::ValUpdater::ValUpdater(IntUpdater * owningUpdater, int direction):
	owningUpdater(owningUpdater),
	direction(direction)
{
}

//-----------------------------------------------------------------------------
// ValUpdater::update -- apply one step to the owner's value.
//
// First lets the SFBool base finish its normal update (so the field's own
// machinery stays consistent), then reads the current value, adds
// (direction * offset) -- +offset for increase, -offset for decrease -- and
// writes the result back to the parent's "value" field.
//
// The std::cerr trace lines are intentionally left commented out (debug aid).
//-----------------------------------------------------------------------------
void IntUpdater::ValUpdater::update()
{
	SFBool::update();
	int newValue = owningUpdater->value->getValue();
//	std::cerr << "updating from " << newValue;
	newValue += direction * owningUpdater->offset->getValue();
//	std::cerr << " to " << newValue << std::endl;
	owningUpdater->value->setValue(newValue);

}
