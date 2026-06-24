//////////////////////////////////////////////////////////////////////////////
/// \file FieldedVectorImageVolume.h
/// \brief Header file for FieldedVectorImageVolume
///
/// FieldedVectorImageVolume extends VHTK's VectorImageVolume with a single
/// read-back field, `vector`, that continuously exposes the volume's sampled
/// vector at the current device position. In other words, it makes the result
/// of VectorImageVolume::getVector() available as a routable H3D field so that
/// other nodes / Python / X3D can react to "what vector does the volume hold
/// right where the device is?".
//////////////////////////////////////////////////////////////////////////////
#ifndef __FIELDEDVECTORIMAGEVOLUME_H
#define __FIELDEDVECTORIMAGEVOLUME_H

#include <VHTK.hh>
#include <VectorImageVolume.hh>
#include <iostream>
#include "torque_api.h"

//TODO: test: does .vector always have the value we want it to have ?

// this thing should NOT change coordinate space !

namespace H3D {

	/// \class FieldedVectorImageVolume
	/// \brief A VectorImageVolume that publishes its sampled vector through a
	///        routable `vector` field.
	///
	/// The base VHTK::VectorImageVolume can be asked, via getVector(transform,
	/// position), for the vector stored at a given spatial location. This
	/// subclass wraps that query in an H3D field (SFVecVal, exposed as `vector`)
	/// whose value is recomputed whenever its inputs change. The field is routed
	/// from the device position and a transform, so it always reflects the
	/// volume's vector at the current device location.
	class TORQUE_API FieldedVectorImageVolume: public VHTK::VectorImageVolume {
		public:
			/// \class SFVecVal
			/// \brief The field type backing the `vector` member.
			///
			/// This is a typed field producing an SFVec3f from two inputs:
			///   - route 0: an SFVec3f, the device position to sample at, and
			///   - route 1: an SFMatrix4f, a transform passed to getVector().
			/// The Types<...> list declares those expected input field types.
			/// `container` is a back-pointer to the owning volume so update()
			/// can call its getVector() sampling method.
			class SFVecVal: public TypedField< SFVec3f, Types<SFVec3f,SFMatrix4f> >{
			//class SFVecVal: public TypedField< SFVec3f, SFVec3f >{
			public:
				/// Recompute the sampled vector.
				///
				/// Called by the field network whenever an input route changes.
				/// Reads the device position (route 0) and the transform
				/// (route 1), asks the owning volume for the vector stored at
				/// that position, and stores it as this field's value.
				///
				/// NOTE ON COORDINATE SPACE: the result is stored directly,
				/// without applying any inverse transform, so `value` is left in
				/// whatever frame getVector() returns it in (likely global). The
				/// commented-out alternatives below record an experiment with
				/// transforming the sampled vector back through the inverse of the
				/// transform; that path is intentionally not taken (see the
				/// file-level "should NOT change coordinate space" note).
				virtual void update()
				{
					// Route 0: position at which to sample the vector volume.
					Vec3f devicePosition =
						static_cast< SFVec3f * > ( routes_in[0] )->getValue();

					// Route 1: transform forwarded to the volume's sampler.
					Matrix4f samplingTransform =
						static_cast< SFMatrix4f * > ( routes_in[1] )->getValue();
				//	Matrix4f samplingTransform = Matrix4f();

					// Ask the owning volume for the vector stored at this position.
					Vec3f sampledVector =
						container->getVector( samplingTransform, devicePosition );

					//value = (samplingTransform.transformInverse()) * sampledVector;
					value = sampledVector; //actually its likely to be global
					//value = sampledVector;
					//std::cerr << "in vector.update() " << devicePosition << ":" << value << std::endl;
				}

				/// Back-pointer to the volume that owns this field, used to reach
				/// its getVector() sampling method. Set by the volume's constructor.
				FieldedVectorImageVolume * container;
			};

			/// Constructor.
			///
			/// \param _metadata    Optional metadata node (H3D convention).
			/// \param _image       Optional source image field for the volume.
			/// \param _imageObject Optional image-object field for the volume.
			FieldedVectorImageVolume( H3D::Inst< H3D::SFNode   > _metadata    = 0,
					H3D::Inst< SFImage       > _image       = 0,
					H3D::Inst< SFImageObject > _imageObject = 0 );


			/// The H3DNodeDatabase for this node.
			static H3DNodeDatabase database;

			/// Routable field exposing the volume's sampled vector at the current
			/// device position (see SFVecVal).
			auto_ptr< SFVecVal > vector;
	};

}


#endif //__FIELDEDVECTORIMAGEVOLUME_H
