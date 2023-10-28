#pragma once
 

// --EDITOR THIRD PARTY CONFIG/DEFINES/MACROS--
// We set all our definitions to configure the editors third party libraries before we include them.
#include "EditorThirdPartyConfig.h"

// --STB--
#include "stb_image.h"			// Image importer
#include "stb_image_resize.h"	// Image resizer
#include "stb_image_write.h"	// Image Exporter
#include "stb_perlin.h"			// Perlin noise generator

// --ASSIMP--
#include "Importer.hpp"		// General 3D model importer
#include "scene.h"			// Output data structure
#include "postprocess.h"		// Post processing flags


