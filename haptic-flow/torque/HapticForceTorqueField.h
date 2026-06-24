/////////////////////////////////////////////////////////////////////////////
//    Copyright 2004, SenseGraphics AB
//
//    This file is part of H3D API.
//
//    H3D API is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3D API is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3D API; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at
//    www.sensegraphics.com for more information.
//
//
/// \file HapticForceTorqueField.h
/// \brief Header file for HapticForceTorqueField
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __HAPTICFORCETORQUEFIELD_H__
#define __HAPTICFORCETORQUEFIELD_H__

#include <HapticForceEffect.h>
#include "torque_api.h"
#include <iostream>

namespace H3D {

	/// \class HapticForceTorqueField
	/// \brief A low-level haptic effect that renders a fixed force together
	///        with a fixed torque on the haptics device.
	///
	/// WHERE THIS FITS IN THE PIPELINE:
	///   H3D separates the *scene-graph* description of a force effect from the
	///   *haptic-thread* object that actually computes the forces sent to the
	///   device every haptic cycle. The scene-graph node ForceTorqueField (see
	///   ForceTorqueField.h) creates one of these HapticForceTorqueField
	///   objects on each scene traversal and hands it to the haptics engine.
	///   The engine then calls calculateForces() at the (high) haptic rate.
	///
	/// THE FORCE/TORQUE MODEL:
	///   The effect stores a constant force vector and a constant torque vector.
	///   On every haptic cycle it returns:
	///       output force  = (rotation part of transform) * force
	///       output torque = torque
	///   i.e. the linear force is re-expressed into the device/world frame by
	///   rotating it with the orientation captured in the transform, while the
	///   torque is passed through unchanged. Only the rotation part of the
	///   transform is applied (no translation, no scale) because a force is a
	///   free vector: its magnitude is frame-independent and only its direction
	///   needs to be reoriented.
	//	class H3DAPI_API HapticForceTorqueField: public HapticForceEffect {
	class TORQUE_API HapticForceTorqueField: public HapticForceEffect {
		public:
			/// Constructor.
			///
			/// \param _transform   The coordinate transform whose rotation part
			///                     is used to reorient the stored force vector
			///                     into the frame expected by the device. Passed
			///                     straight through to the HapticForceEffect base
			///                     class (which stores it as `transform`).
			/// \param _force       The constant linear force to render (before
			///                     rotation), as a free vector.
			/// \param _torque      The constant torque to render, passed through
			///                     unchanged.
			/// \param _interpolate Whether the haptics engine should interpolate
			///                     this effect's output between updates; forwarded
			///                     to the base class.
			HapticForceTorqueField( const H3D::ArithmeticTypes::Matrix4f & _transform,
					const Vec3f &_force, const Vec3f &_torque,
					bool _interpolate ):
				HapticForceEffect( _transform, _interpolate ),
				force( _force ), torque(_torque) {}

			/// Compute the force/torque to apply for this haptic cycle.
			///
			/// Called by the haptics engine at the haptic rate. The returned
			/// EffectOutput carries both the linear force and the torque:
			///   - force:  the stored force rotated by the transform's rotation
			///             part so its direction is correct in the target frame.
			///   - torque: the stored torque, returned verbatim.
			///
			/// \param input Per-cycle device state supplied by the engine (device
			///              position, velocity, etc.). Not needed here because the
			///              force and torque are constant.
			/// \return The linear force and torque to send to the device.
			EffectOutput virtual calculateForces( const EffectInput &input ) {
				//TODO: find where transform comes from and if
				//we need to adjust torque to it
				return EffectOutput( transform.getRotationPart() * force, torque );
			}

		protected:
			/// The constant linear force to render (in its original, un-rotated
			/// frame). Rotated by transform.getRotationPart() in calculateForces().
			Vec3f force;

			/// The constant torque to render. Returned unchanged.
			Vec3f torque;
	};
}

#endif
