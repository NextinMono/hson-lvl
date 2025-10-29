add_executable (${HE_LEVEL_NAME}
	"he-level.cpp" 
	"graphics/graphics.cpp" 
	"graphics/graphics.h" 
	"ui/panel.h" 
	"app.cpp" 
	"app.h" 
	 
	"graphics/camera.h" 
	"graphics/camera.cpp" 
	"log.h" 
	"log.cpp" 
	"graphics/model.h" 
	"graphics/model.cpp"
    "panels/object-inspector.h"
	"graphics/shaders/vs.h"
	"graphics/shaders/ps.h"
	"graphics/shaders/ps_color.h" 
	"graphics/shaders/vs_matless.h"   
	"math/aabb.h"
	"math/ray.h"
	"types.h"
	"math/ray.cpp" 
	"app/service.h" 
	"app/applistener.h"
	"app/camera-service.h"
	"app/camera-service.cpp"
 	"app/object-service.h"
)

target_precompile_headers(${HE_LEVEL_NAME} PRIVATE "pch.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${HE_LEVEL_NAME} PROPERTY CXX_STANDARD 20)
endif()
