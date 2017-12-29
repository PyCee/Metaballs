#ifndef MTB_WINDOW_H
#define MTB_WINDOW_H

#include <string>
#include <xcb/xcb.h>

namespace MTB{
  class Window{
  private:
    std::string m_title;
    unsigned short m_width, m_height;
    // if linux
    xcb_connection_t *m_connection;
    xcb_screen_t *m_screen;
    xcb_window_t m_window;
    xcb_intern_atom_cookie_t m_cookie;
    xcb_intern_atom_reply_t* m_reply;
    xcb_intern_atom_cookie_t m_delete_cookie;
    xcb_intern_atom_reply_t* m_delete_reply;
    // end if linux
  public:
    Window(std::string, unsigned short, unsigned short);
    ~Window();
    bool is_closed();
  };
};

#endif /* MTB_WINDOW_H */
