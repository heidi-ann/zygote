#include "python_embed_headers.hpp"

#include <string>
#include <boost/python.hpp>
#include <iostream>
#include "entity.hpp"
#include "game_engine.hpp"

namespace py = boost::python;

///
/// This is the shared object file that is generated to create the hooks
/// from the python code into the C++ objects.
/// If you want to add new methods to either Enity or GameEngine that you
/// want accessible to the python code, they have to be defined here.
///
BOOST_PYTHON_MODULE(wrapper_functions) {
    py::class_<Entity, boost::noncopyable>("Entity", py::no_init)
        .def_readwrite("id",      &Entity::id)
        .def_readwrite("name",    &Entity::name)
        .def("__set_game_speed",  &Entity::__set_game_speed)
        .def("get_instructions",  &Entity::get_instructions)
        .def("get_retrace_steps", &Entity::get_retrace_steps)
        .def("monologue",         &Entity::monologue)
        .def("move_east",         &Entity::move_east)
        .def("move_west",         &Entity::move_west)
        .def("move_north",        &Entity::move_north)
        .def("move_south",        &Entity::move_south)
        .def("print_debug",       &Entity::py_print_debug)
        .def("print_dialogue",    &Entity::py_print_dialogue)
        .def("read_message",      &Entity::read_message)
        .def("update_status",     &Entity::py_update_status)
        .def("walkable",          &Entity::walkable)
        .def("callback_test",     &Entity::callback_test)
        .def("get_name",          &Entity::get_name)
        .def("get_location",      &Entity::get_location)
        .def("focus",             &Entity::focus)
        .def("get_sprite",        &Entity::get_sprite)
        .def("set_sprite",        &Entity::set_sprite)
        .def("start_animating",   &Entity::start_animating)
        .def("pause_animating",   &Entity::pause_animating)
        .def("get_number_of_animation_frames",&Entity::get_number_of_animation_frames)
        .def("set_animation_frame",&Entity::set_animation_frame);
        
    py::class_<GameEngine, boost::noncopyable>("GameEngine", py::no_init)
        .def("add_object",        &GameEngine::add_object)
        .def("get_level_location",&GameEngine::get_level_location)
        .def("print_debug",       &GameEngine::print_debug)
        .def("change_level",      &GameEngine::change_level);
}
