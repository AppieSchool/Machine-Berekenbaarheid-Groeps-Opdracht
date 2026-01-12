#include "ImageLoader.h"

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
  #include <OpenGL/gl.h>
#elif defined(_WIN32)
#include <GL/gl.h>
#else
#include <GL/gl.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"

using namespace std;


using namespace std;

unsigned int LoadTextureFromFile(const char* filename, int* outWidth, int* outHeight)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4); // Force RGBA
    
    if (!data) {
        return 0;
    }
    
    // Create OpenGL texture
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    // Free image data
    stbi_image_free(data);
    
    if (outWidth) *outWidth = width;
    if (outHeight) *outHeight = height;
    
    return textureId;
}

void FreeTexture(unsigned int textureId)
{
    GLuint id = textureId;
    glDeleteTextures(1, &id);
}
