#include "Updater.h"

//=============================================================================
// Updater.cpp
//
// Implementation of the generic Updater<T> template. As explained in the
// header, this template is currently UNUSED -- IntUpdater and FloatUpdater are
// standalone copies rather than instantiations of it. The code is kept for
// reference and to document the shared "increase/decrease by offset" pattern.
//=============================================================================

//-----------------------------------------------------------------------------
// HISTORICAL NOTE (preserved verbatim).
//
// The block below records the original intended node-database registration and
// field declarations for a concrete Updater. It is commented out because a
// bare template cannot be registered, and because IntUpdater/FloatUpdater now
// carry their own (corrected) registrations in their own .cpp files. Left
// unchanged as a record of the original design.
//-----------------------------------------------------------------------------
//
/*
H3DNodeDatabase Updater::database("Updater", &(newInstance<Updater>),
		typeid( IntUpdater ), &Node::database);

namespace IntUpdaterInternals {
	FIELDDB_ELEMENT( IntUpdater, value, INPUT_OUTPUT );
	FIELDDB_ELEMENT( IntUpdater, increase, INPUT_ONLY );
	FIELDDB_ELEMENT( IntUpdater, decrease, INPUT_ONLY );

}
*/

//-----------------------------------------------------------------------------
// Constructor.
//
// Allocates the value/offset fields and builds the two trigger fields, passing
// +1 to "increase" and -1 to "decrease" so they step in opposite directions.
//-----------------------------------------------------------------------------
template <class T> Updater<T>::Updater(T initialOffset):
	value(new SField< T >),
	offset(new SField< T >(initialOffset)),
	increase(new ValUpdater(this, 1)),
	decrease(new ValUpdater(this, - 1))
{
	// Historical no-op temporary (kept to match original behaviour exactly).
	SField<T>();
}

//-----------------------------------------------------------------------------
// ValUpdater constructor: just stores the owning Updater and the step sign.
//-----------------------------------------------------------------------------
template <class T> Updater<T>::ValUpdater::ValUpdater(Updater<T> * owningUpdater, int direction):
	owningUpdater(owningUpdater),
	direction(direction)
{
}

//-----------------------------------------------------------------------------
// ValUpdater::update -- apply one step to the owner's value.
//
// Reads the current value, adds (direction * offset) -- i.e. +offset when this
// is the "increase" trigger, -offset when it is the "decrease" trigger -- and
// writes the result back. This is exactly the accumulation logic shared with
// IntUpdater/FloatUpdater.
//-----------------------------------------------------------------------------
template <class T> void Updater<T>::ValUpdater::update()
{
	T newValue = owningUpdater->value->getValue();
	newValue += direction * owningUpdater->offset->getValue();
	owningUpdater->value->setValue(newValue);

}
