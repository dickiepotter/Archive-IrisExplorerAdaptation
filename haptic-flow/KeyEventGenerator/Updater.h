#ifndef _INTUPDATER_H
#define _INTUPDATER_H

//=============================================================================
// Updater.h
//
// WHAT THIS IS
// ------------
// Updater<T> is a *generic template* version of the "value with increment /
// decrement triggers" pattern. The idea is:
//   - "value"  holds the current value of type T.
//   - "offset" is the step size.
//   - "increase" and "decrease" are trigger fields. When fired, they add or
//     subtract one "offset" from "value".
//
// IMPORTANT -- THIS TEMPLATE IS CURRENTLY UNUSED
// ----------------------------------------------
// IntUpdater and FloatUpdater do NOT actually derive from Updater<T>. They are
// standalone, hand-written copies of this same pattern (one specialised for
// int via SFInt32, one for float via SFFloat). This template was presumably
// an attempt to factor the shared logic into one place, but the concrete
// classes ended up duplicating it instead. The two key differences in the
// concrete versions are:
//   - their nested ValUpdater derives from AutoUpdate< SFBool > (so it
//     auto-fires when routed), whereas this template's ValUpdater derives
//     from plain SFBool;
//   - they each register an H3DNodeDatabase (the line below is commented out
//     here because a bare template cannot be registered without a concrete
//     type).
// The include guard intentionally reads _INTUPDATER_H (a historical naming
// quirk that is left untouched to avoid altering compilation behaviour).
//
// Note the historical commented-out database/FIELDDB block is preserved
// verbatim in the .cpp file as a record of the original registration intent.
//=============================================================================

#include "keg_api.h"
#include <X3DChildNode.h>
#include <SField.h>
#include <SFBool.h>

using namespace H3D;

/// \brief Generic "incrementable value" template (currently unused; see the
///        file header for why IntUpdater/FloatUpdater duplicate it instead).
///
/// \tparam T The numeric type held by the value/offset fields.
template <class T> class KEG_API Updater  {
	public:
		/// Construct with an initial step size.
		/// \param initialOffset Starting value of the "offset" step field.
		Updater(T initialOffset = 0);

		/// \brief Trigger field that adjusts the parent's value by +/- offset.
		///
		/// One instance is created for increasing and one for decreasing,
		/// distinguished by the sign passed to the constructor.
		class ValUpdater : public SFBool {
			public:
				/// \param owningUpdater  The Updater whose value this adjusts.
				/// \param direction      +1 to add offset, -1 to subtract it.
				ValUpdater(Updater< T > * owningUpdater, int direction);

				/// Apply one step (+/- offset) to the parent's value.
				void update();
			protected:
				/// Back-pointer to the Updater that owns this trigger.
				/// (Original member name: "parent".)
				Updater< T > * owningUpdater;

				/// Direction/sign of the step: +1 to increase, -1 to decrease.
				/// (Original member name: "sign".)
				int direction;
		};


		/// The current value being maintained.
		auto_ptr< SField< T > > value;

		/// The step size added or subtracted on each trigger.
		auto_ptr< SField< T > > offset;

		/// Trigger that adds one "offset" to "value".
		auto_ptr< ValUpdater > increase;

		/// Trigger that subtracts one "offset" from "value".
		auto_ptr< ValUpdater > decrease;

		// Concrete subclasses would register a database here; a bare template
		// cannot, so this is intentionally left commented out (historical).
		//static H3DNodeDatabase database;
};

#endif // _INTUPDATER_H
