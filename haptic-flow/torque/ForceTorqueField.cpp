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
/// \file ForceTorqueField.cpp
/// \brief cpp file for ForceTorqueField
///
//
//////////////////////////////////////////////////////////////////////////////
#include "ForceTorqueField.h"
#include <iostream>

using namespace H3D;

// ---------------------------------------------------------------------------
// Node-database registration.
//
// This static H3DNodeDatabase instance registers ForceTorqueField with the H3D
// runtime so it can be created by name. The registration ties together:
//   - the X3D/runtime type-name string "ForceTorqueField",
//   - a factory function (newInstance<ForceTorqueField>) the engine calls to
//     construct an instance,
//   - the C++ runtime type (typeid) used for identification, and
//   - the parent class's database (H3DForceEffect::database), so this node
//     inherits all of H3DForceEffect's fields.
// ---------------------------------------------------------------------------
H3DNodeDatabase ForceTorqueField::database( "ForceTorqueField",
		&(newInstance<ForceTorqueField>),
		typeid( ForceTorqueField ),
		&H3DForceEffect::database );

// ---------------------------------------------------------------------------
// Field registration.
//
// Each FIELDDB_ELEMENT entry registers one member field with the node database
// under the member's own name, making it addressable from X3D and Python and
// giving it the stated access type. The member-name tokens (force, torque,
// vectorVolume) must match the data-member names declared in the header.
// ---------------------------------------------------------------------------
namespace ForceTorqueFieldInternals {
	FIELDDB_ELEMENT( ForceTorqueField, force, INPUT_OUTPUT );
	FIELDDB_ELEMENT( ForceTorqueField, torque, INPUT_OUTPUT );
	FIELDDB_ELEMENT( ForceTorqueField, vectorVolume, INPUT_OUTPUT );
}

/// Constructor.
///
/// Forwards metadata to the H3DForceEffect base class, binds each field
/// instance to its member, then performs the standard H3D node start-up:
///   1. set type_name (the runtime type-name string for this node),
///   2. call database.initFields() so the field database knows about and wires
///      up this instance's fields,
///   3. initialise the force and torque to the zero vector so that, until the
///      user assigns values, the node applies no force and no torque.
ForceTorqueField::ForceTorqueField( Inst< SFVec3f > _force,
		Inst< SFVec3f > _torque,
		Inst< SFNode>  _metadata,
		Inst< VHTK::SFVectorDataNode > _vectorVolume ) :
	H3DForceEffect( _metadata ),
	force( _force ), torque( _torque), vectorVolume(_vectorVolume) {

		type_name = "ForceTorqueField";
		database.initFields( this );
		force->setValue( Vec3f( 0,0,0 ) );
		torque->setValue( Vec3f( 0,0,0 ) );
	}
