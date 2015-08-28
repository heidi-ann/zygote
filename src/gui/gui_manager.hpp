#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include <memory>
#include <iostream>
#include "object.hpp"
#include "gui_text.hpp"
class Component;
class ComponentGroup;
struct MouseInputEvent;


///
/// The GUIManager class manages all the components which have been
/// added to the component tree. It is responsible for parsing the
/// tree and getting its vertex and texture data to render the GUI.
///
/// The class inherits from object so that it can be added to the
/// rendering system and we batch all the geometry together to speed
/// up the rendering calls. It is responsible for dispatching events
/// to the required component e.g. mouse clicks.
///
class GUIManager : public Object {
    ///
    /// The root component of the tree
    ///
    std::shared_ptr<ComponentGroup> root;

    ///
    /// The text objects for all the component
    ///
    std::vector<std::shared_ptr<GUIText>> components_text;

    ///
    /// Generate the texture data for this component and its sub components
    ///
    void generate_texture_data();

    ///
    /// Generate the vertex data for this component and its sub components
    ///
    void generate_vertex_data();

    ///
    /// Generate the text data for this component and its sub componets
    ///
    void generate_text_data();

    ///
    /// Load the texture for this component
    ///
    void load_textures();

    ///
    /// Initialise the shaders used by this component
    /// @return boolean indicating success or failure of the operation (true is success)
    bool init_shaders();

    ///
    /// Recalculate the offsets for components - used on a resize event
    /// @param parent the parent of the current component - this recursively
    /// walks the tree of components
    ///
    void regenerate_offsets(std::shared_ptr<Component> parent);


public:
    GUIManager();
    virtual ~GUIManager();

    std::shared_ptr<RenderableComponent> renderable_component;

    ///
    /// Callback function to handle mouse events
    /// @param event the mouse event to be handled
    ///
    void mouse_callback_function(MouseInputEvent event);

    ///
    /// Update components in the component tree
    ///
    void update_components();

    ///
    /// Parse all the components and generate the vertex and texture data
    ///
    void  parse_components();

    ///
    /// Go through the tree and find a component to handle the click event.
    /// The first component in bounds for the click and that has an on_click
    /// function will be called and then the search terminated. This is a DFS.
    /// @param root the component to search from
    /// @param mouse_x the mouse x location
    /// @param mouse_y the mouse y location
    /// @param curr_x_offset the current x offset from the origin
    /// @param curr_y_offset the current y offset from the origin
    /// @return indicates if the click has been handled
    ///
    bool handle_mouse_click(std::shared_ptr<Component> root, int mouse_x, int mouse_y, int curr_x_offset, int curr_y_offset);

    ///
    /// Set the root component of the component tree
    /// @param component the root
    ///
    void set_root(std::shared_ptr<ComponentGroup> component) { root = component; }

    ///
    /// Get the root component of the tree
    /// @return the root
    ///
    std::shared_ptr<ComponentGroup> get_root() { return root; }

    ///
    /// Render the text
    ///
    void render_text();

    std::shared_ptr<RenderableComponent>  get_renderable_component() override;
};

#endif
