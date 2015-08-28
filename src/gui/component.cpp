#include <mutex>
#include "event_manager.hpp"

#include "component.hpp"

int Component::get_new_id() {
    static int next_component_id = 0;

    std::lock_guard<std::mutex> lock(component_mutex);

    // 1, 2, 3, ...
    return ++next_component_id;
}

void Component::call_on_click() {
    EventManager::get_instance()->add_event(on_click_func); //puts the function on the event queue
}

Component::Component(std::function<void (void)> on_click, float _width,
                     float _height, float _x_offset, float _y_offset) :
    parent(nullptr),
    vertex_data(nullptr), size_vertex_data(0),
    texture_data(nullptr), size_texture_data(0),
    id(0), width(_width), height(_height),
    width_pixels(0),height_pixels(0),
    x_offset(_x_offset), y_offset(_y_offset),
    x_offset_pixels(0), y_offset_pixels(0),
    clickable(false), on_click_func(on_click),
    visible(true)
{
    id =  get_new_id();
}

Component::Component():
    parent(nullptr),
    vertex_data(nullptr),
    size_vertex_data(0),
    texture_data(nullptr),
    size_texture_data(0),
    id(get_new_id()),
    width(0.0f),
    height(0.0f),
    width_pixels(0),
    height_pixels(0),
    x_offset(0.0f),
    y_offset(0.0f),
    x_offset_pixels(0),
    y_offset_pixels(0),
    clickable(false),
    on_click_func([] (){ return; }),
    visible(true)
{
    id =  get_new_id();
}


Component::~Component() {
    delete[] vertex_data;
    delete[] texture_data;
}
void Component::set_texture_atlas(std::shared_ptr<TextureAtlas> _texture_atlas) {
    texture_atlas = _texture_atlas;
}
void Component::set_on_click(std::function<void (void)> func) {
    on_click_func= func;
}

void Component::clear_on_click() {
    on_click_func = [] () {};
    clickable = false;
}

void Component::set_width(float width) {
    this->width = width;
}

void Component::set_height(float height) {
    this->height = height;
}

const std::map<int, std::shared_ptr<Component>>* Component::get_components() {
    component_no_children_exception exception;
    throw exception;
}

