//////////////////////////////////////////////////////////////////////////////
/// \file FieldedVectorImageVolume.cpp
/// \brief cpp file for FieldedVectorImageVolume
///
/// Registers the node with the H3D runtime and wires the `vector` field's
/// back-pointer to its owning volume so the field can sample it.
//////////////////////////////////////////////////////////////////////////////
#include "FieldedVectorImageVolume.h"

using namespace H3D;

// ---------------------------------------------------------------------------
// Node-database registration.
//
// Registers FieldedVectorImageVolume with the H3D runtime under the type-name
// string "FieldedVectorImageVolume", supplying:
//   - a factory function (newInstance<FieldedVectorImageVolume>),
//   - the C++ runtime type (typeid) for identification, and
//   - the parent class's database (VectorImageVolume::database) so this node
//     inherits all of VectorImageVolume's fields.
// ---------------------------------------------------------------------------
H3DNodeDatabase FieldedVectorImageVolume::database( "FieldedVectorImageVolume",
		&(newInstance<FieldedVectorImageVolume>),
		typeid( FieldedVectorImageVolume ),
		&VectorImageVolume::database );

// ---------------------------------------------------------------------------
// Field registration.
//
// Registers the `vector` member field with the node database under its own
// name, making it addressable from X3D and Python with inputOutput access. The
// member-name token (vector) must match the data-member name in the header.
// ---------------------------------------------------------------------------
namespace FieldedVectorImageVolumeInternals {
	FIELDDB_ELEMENT( FieldedVectorImageVolume, vector, INPUT_OUTPUT );
}

// Constructors

/// Constructor.
///
/// Forwards metadata, image and image-object fields to the base
/// VHTK::VectorImageVolume, and creates the `vector` field as a fresh SFVecVal.
/// The body then sets that field's `container` back-pointer to this volume so
/// SFVecVal::update() can call getVector() on the correct instance.
FieldedVectorImageVolume::FieldedVectorImageVolume(Inst< SFNode> _metadata,
	Inst< SFImage > _image, Inst<SFImageObject> _imageObject):
	VHTK::VectorImageVolume(_metadata, _image, _imageObject),vector(new SFVecVal())
{
	vector->container = this;
}
