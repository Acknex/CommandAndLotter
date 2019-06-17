#ifndef UI_MENU
#define UI_MENU


int uimenu_last_layer = 0;
int uimenu_window_id = 0;
FONT * uimenu_default_font = "Arial#16";

#define UIMENU_TYPE_TEXT        1
#define UIMENU_TYPE_IMAGE       2
#define UIMENU_TYPE_BUTTON      3
#define UIMENU_TYPE_DROPDOWN    4
#define UIMENU_TYPE_RADIO       5
#define UIMENU_TYPE_CHECKBOX    6
#define UIMENU_TYPE_SOLIDCOLOR  7
#define UIMENU_TYPE_TEXTBUTTON  8
#define UIMENU_TYPE_SLIDER      9
#define UIMENU_TYPE_TEXTINPUT   10
#define UIMENU_TYPE_NUMINPUT    11

#define UIMENU_WINDOW_TITLE_BAR_SIZE    32
#define UIMENU_WINDOW_BORDER_SIZE       4
#define UIMENU_WINDOW_CONTENT_PADDING   8

#define UIMENU_WINDOW_BORDER_COLOR_R    176
#define UIMENU_WINDOW_BORDER_COLOR_G    179
#define UIMENU_WINDOW_BORDER_COLOR_B    184

#define UIMENU_WINDOW_WINDOW_COLOR_R    193
#define UIMENU_WINDOW_WINDOW_COLOR_G    196
#define UIMENU_WINDOW_WINDOW_COLOR_B    201

#define UIMENU_WINDOW_FIELD_COLOR_R    255
#define UIMENU_WINDOW_FIELD_COLOR_G    255
#define UIMENU_WINDOW_FIELD_COLOR_B    255

#define UIMENU_WINDOW_TITLEBAR_COLOR_R    0
#define UIMENU_WINDOW_TITLEBAR_COLOR_G    0
#define UIMENU_WINDOW_TITLEBAR_COLOR_B    129

#define UIMENU_WINDOW_TITLEBAR_INACTIVE_COLOR_R    64
#define UIMENU_WINDOW_TITLEBAR_INACTIVE_COLOR_G    64
#define UIMENU_WINDOW_TITLEBAR_INACTIVE_COLOR_B    64

#define UIMENU_WINDOW_TITLETEXT_COLOR_R    255
#define UIMENU_WINDOW_TITLETEXT_COLOR_G    255
#define UIMENU_WINDOW_TITLETEXT_COLOR_B    255

#define UIMENU_WINDOW_TITLETEXT_INACTIVE_COLOR_R    255
#define UIMENU_WINDOW_TITLETEXT_INACTIVE_COLOR_G    255
#define UIMENU_WINDOW_TITLETEXT_INACTIVE_COLOR_B    255

#define UIMENU_SKILL_TEXT 0
#define UIMENU_SKILL_FONT 1
#define UIMENU_SKILL_COLOR 2
#define UIMENU_SKILL_PRESSED 5

typedef struct uimenu_element_t
{
    //Public
    int type;
    var x, y;
    var width, height;
    BMAP * bmap;    
    BMAP * bmap_active;
    BMAP * bmap_hover;    
    void * callback;
    int element_index;
    var * var_pointer;
    
    var skill[10]; // Properties

    // Private
    struct uimenu_element_t * _next; // Linked list
    struct uimenu_element_t * _related; // next item in radiogroup, etc.
    void * _parent; // uimenu_window_t
    void * _child; // uimenu_window_t
    int _is_initialized;
    int _is_dirty;
    
} uimenu_element_t;

typedef struct uimenu_window_t
{
    // Public
    int id;
    var x, y;
    var width, height;
    int layer;
    char * title;

    // Private
    PANEL * _panel;
    PANEL * _content_panel;
    uimenu_element_t * _first; // Linked list
    struct uimenu_window_t * _child; 
    struct uimenu_window_t * _next; 
    struct uimenu_window_t * _parent;
    var _moving_start_offset[3];
    int _is_initialized;
    int _is_visible;
    int _is_borderless;
    int _is_dirty;
    int _is_moving;
    int _is_titlebar_active;
    int _title_element_index;
    int _title_element_bg_index;
    BMAP * _title_element_bg_current_bmap;
    BMAP * _title_element_bg_active_bmap;
    BMAP * _title_element_bg_inactive_bmap;
    BMAP * _content_panel_bg_bmap;
    BMAP * _forced_background;
} uimenu_window_t;

typedef struct uimenu_listitem_t
{
    char * text;
    var value;
} uimenu_listitem_t;

// Framework
void uimenu_update();
void uimenu_update_chain(uimenu_window_t * window);

// Public
uimenu_window_t * uimenu_window_create(var x, var y, var width, var height, char * title);
uimenu_window_t * uimenu_window_create_borderless(var x, var y, var width, var height);
uimenu_element_t * uimenu_element_create(int type, var x, var y, var width, var height);

void uimenu_add_element_to_window(uimenu_window_t * window, uimenu_element_t * element);
void uimenu_slave_window_to_window(uimenu_window_t * master, uimenu_window_t * slave);

uimenu_element_t * uimenu_make_text(var x, var y, var width, var height, char * text, COLOR * colorBGR, FONT * font);
uimenu_element_t * uimenu_make_button(var x, var y, var width, var height, BMAP * bmap, BMAP * bmapHover, BMAP * bmapActive, void * callback);
uimenu_element_t * uimenu_make_button(var x, var y, BMAP * bmap, BMAP * bmapHover, BMAP * bmapActive, void * callback);
uimenu_element_t * uimenu_make_simple_button(var x, var y, var width, var height, char * text, FONT * font, void * callback);
uimenu_element_t * uimenu_make_simple_button(var x, var y, var height, char * text, FONT * font, void * callback);
uimenu_element_t * uimenu_make_image(var x, var y, var width, var height, BMAP * bmap);

void uimenu_window_initialize(uimenu_window_t * window);
void uimenu_window_to_top(uimenu_window_t * window);
int uimenu_is_top_window(uimenu_window_t * window);

void uimenu_window_show(uimenu_window_t * window);
void uimenu_window_hide(uimenu_window_t * window);

void uimenu_window_destroy(uimenu_window_t * window);

// Private
BMAP * uimenu__make_button_graphic(int width, int height, int pressed, int tab);
BMAP * uimenu__make_field_graphic(int width, int height);
BMAP * uimenu__make_slider_bg_graphic(int width);
BMAP * uimenu__make_slider_knob_graphic(int width, int height);

VECTOR * uimenu__get_element_screen_offset(uimenu_element_t * element);
VECTOR * uimenu__get_cursor_offset_to_window(uimenu_element_t * element);
int uimenu__is_cursor_in_window_titlebar(uimenu_window_t * window);
int uimenu__is_cursor_in_window(uimenu_window_t * window);
int uimenu__is_cursor_in_element(uimenu_element_t * element);

void uimenu__window_initialize(uimenu_window_t * window);
void uimenu__element_initialize(uimenu_window_t * window, uimenu_element_t * element);
void uimenu__sync_panel_with_window(uimenu_window_t * window);
void uimenu__window_make_dirty(uimenu_window_t * window);
void uimenu__window_update(uimenu_window_t * window);
void uimenu__element_update(uimenu_element_t * element);

uimenu_window_t * uimenu__first_window = NULL;
uimenu_window_t * uimenu__top_window = NULL;

#endif