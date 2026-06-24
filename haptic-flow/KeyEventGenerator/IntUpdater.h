#ifndef _FLOATUPDATER_H
#define _FLOATUPDATER_H

//=============================================================================
// IntUpdater.h
//
// PURPOSE
// -------
// IntUpdater is an H3DAPI node that holds an integer "value" and lets a scene
// nudge it up or down by a fixed step ("offset") through two trigger fields:
//
//     increase  -- when fired, does:  value += offset
//     decrease  -- when fired, does:  value -= offset
//
// This is handy in an X3D scene for things like stepping through frames,
// adjusting a count, or driving any integer parameter from button/key events.
//
// THE INCREASE / DECREASE "UPDATER" PATTERN
// -----------------------------------------
// The two trigger fields are instances of the nested ValUpdater class. Both
// point back at their owning IntUpdater (the "parent") and each carries a sign
// (+1 for increase, -1 for decrease). ValUpdater derives from
// AutoUpdate< SFBool >, which means: whenever something routes an event into
// it, H3D automatically calls its update() method. update() then applies
// "sign * offset" to the parent's value. So routing any event into "increase"
// bumps the value up by one offset, and routing into "decrease" bumps it down.
//
// RELATIONSHIP TO Updater<T>
// --------------------------
// This class is a hand-written, int-specialised copy of the generic Updater<T>
// template (see Updater.h). It does NOT derive from that template; the two
// simply implement the same idea. The commented-out block below shows the
// original (never-finalised) attempt to base IntUpdater on Updater< int >.
//
// NAMING QUIRKS PRESERVED
// -----------------------
// The include guard reads _FLOATUPDATER_H even though this is IntUpdater.h --
// a copy/paste artefact from FloatUpdater.h. It is left untouched on purpose
// so as not to change which translation units see this header.
//=============================================================================

#include <SFInt32.h>
#include <SFBool.h>
#include <X3DChildNode.h>
#include "keg_api.h"

//-----------------------------------------------------------------------------
// HISTORICAL NOTE (preserved verbatim): the original intent was to inherit the
// shared logic from the Updater<int> template instead of duplicating it here.
// That approach was abandoned; the standalone class below is what is actually
// used. Kept as a record of the original design.
//-----------------------------------------------------------------------------
/*
#include "Updater.h"
#include <Types/H3DBasicTypes.h>

class KEG_API IntUpdater : public Updater< int > {
	public:
		static H3DNodeDatabase database;
};
*/

using namespace H3D;

/// \brief Integer value with "increase by offset" / "decrease by offset"
///        trigger fields. See the file header for the full pattern.
class KEG_API IntUpdater : public X3DChildNode {
	public:
		/// Construct the node.
		/// \param initialOffset Initial step size stored in the "offset" field.
		IntUpdater(int initialOffset = 0);

		/// \brief Auto-updating trigger field: on any incoming event it steps
		///        the parent IntUpdater's value by (sign * offset).
		///
		/// Deriving from AutoUpdate< SFBool > is what makes update() fire
		/// automatically when an event is routed in.
		class ValUpdater : public AutoUpdate< SFBool > {
			public:
				/// \param owningUpdater The IntUpdater this trigger belongs to.
				/// \param direction     +1 for the increase trigger, -1 for
				///                      the decrease trigger.
				ValUpdater(IntUpdater * owningUpdater, int direction);

				/// Apply one step (+/- offset) to the parent's value.
				void update();
			protected:
				/// Back-pointer to the owning node. (Original name: "parent".)
				IntUpdater * owningUpdater;

				/// Step direction/sign: +1 increases, -1 decreases.
				/// (Original member name: "sign".)
				int direction;
		};


		/// The current integer value being maintained.
		auto_ptr< SFInt32 > value;

		/// The step size added/subtracted on each trigger.
		auto_ptr< SFInt32 > offset;

		/// Trigger field: adds one "offset" to "value" when fired.
		auto_ptr< ValUpdater > increase;

		/// Trigger field: subtracts one "offset" from "value" when fired.
		auto_ptr< ValUpdater > decrease;

		/// H3D database entry registering this node as "IntUpdater".
		static H3DNodeDatabase database;
};
#endif // _FLOATUPDATER_H
