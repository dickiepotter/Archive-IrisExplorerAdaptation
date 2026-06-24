#ifndef _FLOATUPDATER_H
#define _FLOATUPDATER_H

//=============================================================================
// FloatUpdater.h
//
// PURPOSE
// -------
// FloatUpdater is the floating-point counterpart of IntUpdater. It holds a
// float "value" and exposes two trigger fields that nudge it by a fixed step
// ("offset"):
//
//     increase  -- when fired, does:  value += offset
//     decrease  -- when fired, does:  value -= offset
//
// THE INCREASE / DECREASE "UPDATER" PATTERN
// -----------------------------------------
// The "increase" and "decrease" fields are instances of the nested ValUpdater
// class. Each one stores a back-pointer to its owning FloatUpdater (the
// "parent") and a sign (+1 for increase, -1 for decrease). Because ValUpdater
// derives from AutoUpdate< SFBool >, H3D automatically calls its update()
// whenever an event is routed into it; update() then applies "sign * offset"
// to the parent's value. Routing an event into "increase" therefore raises the
// value by one offset, and into "decrease" lowers it by one offset.
//
// RELATIONSHIP TO Updater<T>
// --------------------------
// Like IntUpdater, this is a hand-written, float-specialised copy of the
// generic Updater<T> template (Updater.h); it does not derive from it. The
// commented-out block below preserves the original (abandoned) attempt to base
// FloatUpdater on Updater< float >.
//
// NOTE: include-guard name. This header legitimately owns the _FLOATUPDATER_H
// guard; IntUpdater.h happens to reuse the same name as a copy/paste artefact.
// Both are left as-is to avoid changing which units include which header.
//=============================================================================

#include <SFFloat.h>
#include <SFBool.h>
#include <X3DChildNode.h>
#include "keg_api.h"

#include "Updater.h"
#include <Types/H3DBasicTypes.h>

//-----------------------------------------------------------------------------
// HISTORICAL NOTE (preserved verbatim): the original intent was to inherit the
// shared logic from Updater< float > (and X3DChildNode) rather than duplicate
// it. That approach was abandoned in favour of the standalone class below.
//-----------------------------------------------------------------------------
/*
class KEG_API FloatUpdater : public Updater< float >, public X3DChildNode {
	public:
		static H3DNodeDatabase database;
};
*/

using namespace H3D;

/// \brief Float value with "increase by offset" / "decrease by offset"
///        trigger fields. See the file header for the full pattern.
class KEG_API FloatUpdater : public X3DChildNode {
	public:
		/// Construct the node.
		/// \param initialOffset Initial step size stored in the "offset" field.
		FloatUpdater(float initialOffset = 0);

		/// \brief Auto-updating trigger field: on any incoming event it steps
		///        the parent FloatUpdater's value by (sign * offset).
		///
		/// Deriving from AutoUpdate< SFBool > makes update() fire
		/// automatically when an event is routed in.
		class ValUpdater : public AutoUpdate< SFBool > {
			public:
				/// \param owningUpdater The FloatUpdater this trigger belongs to.
				/// \param direction     +1 for the increase trigger, -1 for
				///                      the decrease trigger.
				ValUpdater(FloatUpdater * owningUpdater, int direction);

				/// Apply one step (+/- offset) to the parent's value.
				void update();
			protected:
				/// Back-pointer to the owning node. (Original name: "parent".)
				FloatUpdater * owningUpdater;

				/// Step direction/sign: +1 increases, -1 decreases.
				/// (Original member name: "sign".)
				int direction;
		};


		/// The current float value being maintained.
		auto_ptr< SFFloat > value;

		/// The step size added/subtracted on each trigger.
		auto_ptr< SFFloat > offset;

		/// Trigger field: adds one "offset" to "value" when fired.
		auto_ptr< ValUpdater > increase;

		/// Trigger field: subtracts one "offset" from "value" when fired.
		auto_ptr< ValUpdater > decrease;

		/// H3D database entry registering this node as "FloatUpdater".
		static H3DNodeDatabase database;
};
#endif // _FLOATUPDATER_H
