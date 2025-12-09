#pragma once

#include <string>

// Simple PNG image loader for OpenGL textures
// Returns the OpenGL texture ID, or 0 on failure
// Sets outWidth and outHeight to the image dimensions

unsigned int LoadTextureFromFile(const char* filename, int* outWidth, int* outHeight);

// Free a texture when no longer needed
void FreeTexture(unsigned int textureId);
