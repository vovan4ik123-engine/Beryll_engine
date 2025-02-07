#pragma once

// This file includes all libs needed for engine.
// Include this file only inside engine.

#include "SDL3/include/SDL3/SDL.h"
#include "SDL3/include/SDL3/SDL_main.h"
#include "SDL3/include/SDL3/SDL_system.h"
#include "SDL3/include/SDL3/SDL_log.h"

#include "SDL3_image/include/SDL3_image/SDL_image.h"

#include "SDL3_mixer/include/SDL3_mixer/SDL_mixer.h"

#include "SDL3_net/include/SDL3_net/SDL_net.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/compatibility.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "assimp/include/assimp/scene.h"
#include "assimp/include/assimp/Importer.hpp"
#include "assimp/include/assimp/postprocess.h"

#include "sqlite/sqlite3.h"

#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "bullet/BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"
#include "bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"
#include "bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h"

// OpenGL 4.3 (GLSL #version 430) == GLES 3.0 (GLSL #version 300 es).
