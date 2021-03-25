/*
//	be/be
//	Tools for game development with OpenGL.
//
//	Elijah Shadbolt
//	2019
*/

// UTILITY INCLUDES
#include <array>
#include <vector>
#include <string>

// LOCAL DEPDENDENCY INCLUDES
#include "be/game.hpp"
#include "be/logger.hpp"
#include "be/mem/gl.hpp"
#include "be/mem/soil.hpp"
#include "be/mem/ft.hpp"
#include "be/mem/fmod.hpp"

// LOCAL LEAF INCLUDES
#include "be/need.hpp"
#include "be/gl.hpp"
#include "be/application.hpp"
#include "be/soil.hpp"
#include "be/ft.hpp"
#include "be/uniform.hpp"
#include "be/input.hpp"

// PINK
#include "be/pink/trs.hpp"
#include "be/pink/camera.hpp"
#include "be/pink/unlit.hpp"
#include "be/pink/text_label.hpp"
#include "be/pink/model.hpp"
#include "be/pink/skybox.hpp"

// BASIC_ASSETS
#include "be/basic_assets/quad.hpp"
#include "be/basic_assets/cube.hpp"
#include "be/basic_assets/textures.hpp"
#include "be/basic_assets/fonts.hpp"
