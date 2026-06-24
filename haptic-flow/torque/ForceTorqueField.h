//////////////////////////////////////////////////////////////////////////////
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
/// \file ForceTorqueField.h
/// \brief Header file for ForceTorqueField
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __FORCETORQUEFIELD_H__
#define __FORCETORQUEFIELD_H__

#include <iostream>
#include <H3DForceEffect.h>
#include <VHTKVectorDataNode.hh>
#include <SFVec3f.h>
#include "HapticForceTorqueField.h"

namespace H3D {
	/// \ingroup Nodes
	/// \class ForceTorqueField
	/// \brief Scene-graph force-effect node that applies a constant force and
	///        torque to every haptics device, continuously.
	///
	/// HOW THIS NODE WORKS:
	///   ForceTorqueField is the *scene-graph* (X3D-addressable) half of a
	///   two-part design. It carries the user-editable `force` and `torque`
	///   fields and, on every scene traversal, manufactures a fresh haptic-thread
	///   effect object (HapticForceTorqueField) that performs the actual
	///   per-cycle force computation on the haptics device.
	///
	///   See traverseSG() below for the per-frame hand-off, and
	///   HapticForceTorqueField for the force/torque math itself.
	///
	///   The original H3DAPI export decoration is preserved (commented out)
	///   directly above the active declaration for historical reference; this
	///   library exports the type with its own TORQUE_API macro instead.
	//	class H3DAPI_API ForceTorqueField: public H3DForceEffect {
	class TORQUE_API ForceTorqueField: public H3DForceEffect {
		public:
			/// Constructor.
			///
			/// \param _force        Field holding the constant linear force to render.
			/// \param _torque       Field holding the constant torque to render.
			/// \param _metadata     Optional metadata node (H3D convention).
			/// \param _vectorVolume Optional vector-data volume that may supply the
			///                      force/torque (see the `vectorVolume` field).
			ForceTorqueField( Inst< SFVec3f > _force = 0,
					Inst< SFVec3f > _torque = 0,
					Inst< SFNode  > _metadata = 0,
					Inst< VHTK::SFVectorDataNode > _vectorVolume = 0  );

			/// Per-traversal hook called by the scene-graph engine.
			///
			/// On each scene-graph traversal, while haptics are enabled, this
			/// creates a brand-new HapticForceTorqueField effect that captures the
			/// current `force` and `torque` field values and registers it with
			/// every haptics device for this frame via addForceEffectToAll().
			///
			/// The transform handed to the haptic effect is an identity Matrix4f
			/// here, meaning the stored force is rendered as-is (no reorientation).
			/// The commented-out alternative below shows the original variant that
			/// used the accumulated forward matrix (ti.getAccForwardMatrix()) so the
			/// force would be expressed in the node's accumulated world frame; it is
			/// retained as a historical note.
			///
			/// \param ti Traversal info supplied by the engine; used to query
			///           whether haptics are enabled and to register the effect.
			virtual void ForceTorqueField::traverseSG( TraverseInfo &ti ) {
				if( ti.hapticsEnabled() ) {
//					ti.addForceEffectToAll( new HapticForceTorqueField( ti.getAccForwardMatrix(),
//								force->getValue(), torque->getValue(), true ) );
					ti.addForceEffectToAll( new HapticForceTorqueField( Matrix4f(),
								force->getValue(), torque->getValue(), true ) );
				}
			}

			/// The force to render.
			///
			/// <b>Access type: </b> inputOutput \n
			/// <b>Default value: </b> Vec3f( 0, 0, 0 ) \n
			auto_ptr< SFVec3f > force;

			/// The torque to render.
			///
			/// <b>Access type: </b> inputOutput \n
			/// <b>Default value: </b> Vec3f( 0, 0, 0 ) \n
			auto_ptr< SFVec3f > torque;

			// Can contain a VHTKVectorDataNode
			auto_ptr< VHTK::SFVectorDataNode > vectorVolume;

			/// The H3DNodeDatabase for this node.
			static H3DNodeDatabase database;
	};
}

#endif
