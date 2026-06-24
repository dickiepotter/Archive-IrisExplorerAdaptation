from H3DInterface import *

# =============================================================================
#  test_vis.py  --  adapt a single-vector field into a multi-vector field
# =============================================================================
#
#  Tiny H3DAPI helper: wraps an incoming SFVec3f as a one-element MFVec3f so a
#  single sampled vector can feed a node that expects a list of vectors.
#
#  The module-level instance "convertor" is exposed by H3D as a field of this
#  script node, so its name is preserved.
# -----------------------------------------------------------------------------

class SingleToMultiVectorAdapter(TypedField(MFVec3f, (SFVec3f))):
	def update(self, event):
		incoming_routes = self.getRoutesIn()
		single_vector = incoming_routes[0].getValue()
		# (The two lines below build a one-element MFVec3f the long way; kept
		#  for reference but unused.)
		one_element_field = MFVec3f(1)
		one_element_field.push_back(single_vector)
		#print single_vector, one_element_field.getValue()
		return MFVec3f(single_vector).getValue()

convertor = SingleToMultiVectorAdapter()
