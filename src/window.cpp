#include "window.hpp"
#include <iostream>


using namespace MTB;
Window::Window(std::string title, unsigned short width, unsigned short height){
  this->m_title = title;
  this->m_width = width;
  this->m_height = height;
  unsigned int value_mask,
    value_list[2];
  
  this->m_connection = xcb_connect(NULL, NULL);
  if(xcb_connection_has_error(this->m_connection) > 0){
    std::cout << "ERROR::Failed to create xcb connection" << std::endl;
  } else{
    std::cout << "Created xcb connection" << std::endl;
  }
  this->m_screen = xcb_setup_roots_iterator(xcb_get_setup(this->m_connection)).data;
  this->m_window = xcb_generate_id(this->m_connection);
  
  // Create window
  value_mask = XCB_CW_EVENT_MASK;
  value_list[0] = XCB_EVENT_MASK_EXPOSURE;
  xcb_create_window(this->m_connection, XCB_COPY_FROM_PARENT,
		    this->m_window, this->m_screen->root,
		    0, 0, width, height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
		    this->m_screen->root_visual, value_mask, value_list);

  // Set title of window
  xcb_change_property(this->m_connection, XCB_PROP_MODE_REPLACE,
		      this->m_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
		        this->m_title.length(),
			this->m_title.c_str());
  // Setup window delete
  this->m_cookie = xcb_intern_atom(this->m_connection, 1, 12,
				   "WM_PROTOCOLS");
  this->m_reply = xcb_intern_atom_reply(this->m_connection,
					this->m_cookie, 0);
  this->m_delete_cookie = xcb_intern_atom(this->m_connection, 0, 16,
					  "WM_DELETE_WINDOW");
  this->m_delete_reply = xcb_intern_atom_reply(this->m_connection,
					       this->m_delete_cookie, 0);
  
  xcb_change_property(this->m_connection, XCB_PROP_MODE_REPLACE,
		      this->m_window, (*this->m_reply).atom,
		      4, 32, 1, &(*this->m_delete_reply).atom);
  // Show window
  xcb_map_window(this->m_connection, this->m_window);
  xcb_flush(this->m_connection);
}
Window::~Window(){
  xcb_disconnect(this->m_connection);
}
bool Window::is_closed(){

  
  xcb_generic_event_t *event;
  while((event = xcb_poll_for_event(this->m_connection))){
    switch(event->response_type & ~0x80){
    case XCB_CLIENT_MESSAGE:
      if(((xcb_client_message_event_t*)event)->data.data32[0] ==
	 this->m_delete_reply->atom){
	return true;
      }
      break;
    }
  }
  return false;
}
