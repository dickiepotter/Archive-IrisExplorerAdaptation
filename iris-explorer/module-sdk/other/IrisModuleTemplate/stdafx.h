#pragma once

#define _INTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
#define __IvStaticImport

#include <inttypes.h>

#include <cx/Typedefs.h>
#include <cx/ModuleCommand.h>
#include <cx/DataOps.h>
#include <cx/PortAccess.h>
#include <cx/Coerce.h>
#include <cx/cxMetaType.h>
#include <cx/TypeMismatchMsg.h>
#include <cx/UserFuncs.h>
#include <cx/cxGeneric.h>
#include <cx/UDT.h>
#include <cx/Info.h>
#include <cx/DataAccess.h>
#include <cx/cxParameter.h>
#include <cx/cxGeometry.h>
#include <cx/Geometry.h>
#include <cx/winMcw.h>

#include <cx/cxGeometry.api.h>
#include <cx/cxParameter.api.h>
#include <cx/cxLattice.api.h>
#include <cx/cxPick.api.h>

#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPath.h>

#include <Inventor/draggers/SoDragger.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>

#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFVec2f.h>

#include <Inventor/sensors/SoSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/projectors/SbLineProjector.h>

#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoInfo.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/actions/SoWriteAction.h>

#define __IvStaticImport

#pragma warning( disable: 4309 )

