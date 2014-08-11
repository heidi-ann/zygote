// Try funky initialization in if.

#include <memory>
#include <tuple>
#include <utility>

extern "C" {
#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif
#ifdef USE_GL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
}

#include "texture_atlas.hpp"

#include "engine.hpp"
#include "image.hpp"



// std::map<GraphicsContext*, std::shared_ptr<ResourceCache<TextureAtlas>>> TextureAtlas::atlas_caches;



// Need to inherit constructors manually.
// NOTE: This will, and are required to, copy the message.
TextureAtlas::LoadException::LoadException(const char *message): std::runtime_error(message) {}
TextureAtlas::LoadException::LoadException(const std::string &message): std::runtime_error(message) {}



std::shared_ptr<TextureAtlas> TextureAtlas::new_resource(const std::string resource_name) {
    return std::make_shared<TextureAtlas>(resource_name + ".png");
}



TextureAtlas::TextureAtlas(const std::string image_path):
    image(image_path, true),
    unit_w(Engine::get_tile_size()),
    unit_h(Engine::get_tile_size()),
    unit_columns(image.width  / unit_w),
    unit_rows   (image.height / unit_h),
    textures(unit_columns * unit_rows)
{
    glGenTextures(1, &gl_texture);
    
    if (gl_texture == 0) {
        LOG(ERROR) << "Unable to generate GL texture.";
        throw TextureAtlas::LoadException("Unable to generate GL texture");
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetError();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.store_width, image.store_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
    if (int e = glGetError()) {
        std::stringstream hex_error_code;
        hex_error_code << std::hex << e;
        glDeleteTextures(1, &gl_texture);
        throw TextureAtlas::LoadException("Unable to load texture into GPU: " + hex_error_code.str());
    }
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}



TextureAtlas::~TextureAtlas() {
    glDeleteTextures(1, &gl_texture);
}



GLuint TextureAtlas::get_gl_texture() {
    return gl_texture;
}


std::pair<int,int> TextureAtlas::get_atlas_size() {
    return std::make_pair(image.width, image.height);
}


int TextureAtlas::get_texture_count() {
    return unit_columns * unit_rows;
}


std::pair<int,int> TextureAtlas::get_unit_size() {
    return std::make_pair(unit_w, unit_h);
}


std::pair<float,float> TextureAtlas::get_unit_size_ratio() {
    return std::make_pair(float(unit_w) / float(image.store_width),
                          float(unit_h) / float(image.store_height));
}


std::pair<int,int> TextureAtlas::index_to_units(int index) {
    return std::make_pair(index % unit_columns,
                          index / unit_columns);
}


std::pair<float,float> TextureAtlas::units_to_floats(std::pair<int,int> units) {
    return std::make_pair(float(units.first  * unit_w) / float(image.store_width),
                          float(image.height - units.second * unit_h) / float(image.store_height));
}


std::tuple<float,float,float,float> TextureAtlas::index_to_coords(int index) {
    std::pair<int,int> units = index_to_units(index);
    return std::make_tuple<float,float,float,float>
        (
         float((units.first    ) * unit_w) / float(image.store_width),
         float((units.first + 1) * unit_w) / float(image.store_width),
         float(image.height - (units.second + 1) * unit_h) / float(image.store_height),
         float(image.height - (units.second    ) * unit_h) / float(image.store_height)
         );
}
