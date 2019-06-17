#include "ui_menu.h"

//#pragma region Framework

void uimenu_update()
{
    if(uimenu__first_window != NULL)
        uimenu_update_chain(uimenu__first_window);
}

void uimenu_update_chain(uimenu_window_t * window)
{
    uimenu__window_update(window);
    if(window->_next != NULL)
        uimenu_update_chain(window->_next);
}

//#pragma endregion

//#pragma region Public

uimenu_window_t * uimenu_window_create(var x, var y, var width, var height, char * title)
{
    uimenu_window_t * window = sys_malloc(sizeof(uimenu_window_t));
    memset(window, 0, sizeof(uimenu_window_t));
    window->x = x;
    window->y = y;
    window->width = width + (UIMENU_WINDOW_BORDER_SIZE * 2);
    window->height = height + (UIMENU_WINDOW_BORDER_SIZE * 2) + UIMENU_WINDOW_TITLE_BAR_SIZE + 1;
    window->layer = uimenu_last_layer;
    window->title = title;

    window->_panel = NULL;
    window->_parent = NULL;
    window->_first = NULL;
    window->_is_initialized = false;
    window->_is_visible = false;
    window->_is_dirty = true;
    window->_is_borderless = false;
    window->_is_moving = false;
    window->_is_titlebar_active = false;

    // Increment the layers, so every window has 100 layers to work with.
    // 99 effective layers for bordered windows since one is used for the nested content panel
    uimenu_last_layer += 100;
    
    // Establish window linked list for updates
    window->id = uimenu_window_id++;
    window->_next = uimenu__first_window;
    uimenu__first_window = window;

    return window;
}

uimenu_window_t * uimenu_window_create_borderless(var x, var y, var width, var height)
{
    uimenu_window_t * window = uimenu_window_create(x, y, width, height, "");
    window->width = width;
    window->height = height;
    window->_is_borderless = true;
    return window;
}

uimenu_element_t * uimenu_element_create(int type, var x, var y, var width, var height)
{
    uimenu_element_t * element = sys_malloc(sizeof(uimenu_element_t));
    memset(element, 0, sizeof(uimenu_element_t));
    element->type = type;
    element->x = x;
    element->y = y;
    element->width = width;
    element->height = height;

    element->bmap = NULL;
    element->bmap_active = NULL;
    element->bmap_hover = NULL;
    element->callback = NULL;
    
    element->_next = NULL;
    element->_related = NULL;
    element->_parent = NULL;
    element->_child = NULL;

    element->_is_dirty = true;
    element->_is_initialized = false;

    return element;
}



void uimenu_add_element_to_window(uimenu_window_t * window, uimenu_element_t * element)
{
    if(window == NULL) error("UIMENU: window must not be NULL");
    if(element == NULL) error("UIMENU: element must not be NULL");

    element->_parent = window;
    if(element->_related != NULL) element->_related->_parent = window;

    element->_next = window->_first;
    window->_first = element;
}

void uimenu_slave_window_to_window(uimenu_window_t * master, uimenu_window_t * slave)
{
    if(slave == NULL) error("UIMENU: slave must not be NULL");

    slave->_parent = master;
    if(master != NULL)
    {
        slave->_child = master->_child;
        master->_child = slave;
    }
}



uimenu_element_t * uimenu_make_text(var x, var y, var width, var height, char * text, COLOR * colorBGR, FONT * font)
{
    uimenu_element_t * element = uimenu_element_create(UIMENU_TYPE_TEXT, x, y, width, height);
    
    element->skill[UIMENU_SKILL_TEXT] = str_create(text);
    if(colorBGR == NULL)
        element->skill[UIMENU_SKILL_COLOR] = vector(255, 255, 255);
    else
        element->skill[UIMENU_SKILL_COLOR] = colorBGR;

    if(font == NULL)
        element->skill[UIMENU_SKILL_FONT] = uimenu_default_font;
    else
        element->skill[UIMENU_SKILL_FONT] = font;

    return element;
}

uimenu_element_t * uimenu_make_button(var x, var y, var width, var height, BMAP * bmap, BMAP * bmapHover, BMAP * bmapActive, void * callback)
{
    uimenu_element_t * element = uimenu_element_create(UIMENU_TYPE_BUTTON, x, y, width, height);
    
    element->bmap = bmap;
    element->bmap_active = bmapActive;
    element->bmap_hover = bmapHover;
    element->callback = callback;

    return element;
}

uimenu_element_t * uimenu_make_button(var x, var y, BMAP * bmap, BMAP * bmapHover, BMAP * bmapActive, void * callback)
{
    return uimenu_make_button(x, y, bmap_width(bmap), bmap_height(bmap), bmap, bmapHover, bmapActive, callback);
}

uimenu_element_t * uimenu_make_simple_button(var x, var y, var width, var height, char * text, FONT * font, void * callback)
{
    FONT * finalFont = font;
    if(font == NULL)
        finalFont = uimenu_default_font;
    
    uimenu_element_t * element = uimenu_element_create(UIMENU_TYPE_TEXTBUTTON, x, y, width, height);
    var stringWidth = str_width(text, finalFont);

    element->_related = uimenu_make_text(x + (width / 2) - (stringWidth / 2) , y + 5, width, height, text, vector(0, 0, 0), finalFont);
    element->callback = callback;

    return element;    
}

uimenu_element_t * uimenu_make_simple_button(var x, var y, var height, char * text, FONT * font, void * callback)
{
    FONT * finalFont = font;
    if(font == NULL)
        finalFont = uimenu_default_font;

    var stringWidth = str_width(text, finalFont) + 20;

    uimenu_element_t * element = uimenu_element_create(UIMENU_TYPE_TEXTBUTTON, x, y, stringWidth, height);

    element->_related = uimenu_make_text(x + 10, y + 5, stringWidth, height, text, vector(0, 0, 0), finalFont);
    element->callback = callback;

    return element;    
}

uimenu_element_t * uimenu_make_image(var x, var y, var width, var height, BMAP * bmap)
{
    uimenu_element_t * element = uimenu_element_create(UIMENU_TYPE_IMAGE, x, y, width, height);
    
    element->bmap = bmap;

    return element;
}



void uimenu_window_initialize(uimenu_window_t * window)
{
    // Sorry for this, but I like my code sorted, so this is a "public wrapper"
    uimenu__window_initialize(window);
}

void uimenu_window_to_top(uimenu_window_t * window)
{    
    if(!uimenu_is_top_window(window))
    {
        window->layer = uimenu_last_layer;
        uimenu__window_make_dirty(uimenu__top_window);
        uimenu__top_window = window;
        uimenu__window_make_dirty(window);

        uimenu_last_layer += 100;        
    }
}

int uimenu_is_top_window(uimenu_window_t * window)
{
    if(uimenu__top_window != NULL)
    {
        if(uimenu__top_window->id == window->id)
        {
            return TRUE;
        }
    }

    return FALSE;
}



void uimenu_window_show(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully

    if(window->_is_initialized)
    {
        window->_panel->flags |= SHOW;
        if(window->_is_borderless == FALSE)
            window->_content_panel->flags |= SHOW;

        window->_is_visible = TRUE;
        // window->_is_dirty = TRUE;
        uimenu__window_make_dirty(window);
    }
}

void uimenu_window_hide(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully

    if(window->_is_initialized)
    {
        window->_panel->flags &= ~SHOW;
        if(window->_is_borderless == FALSE)
            window->_content_panel->flags &= ~SHOW;

        window->_is_visible = FALSE;
    }
}

void uimenu_window_destroy(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully
    
    // Honestly can't be arsed with freeing memory properly for all the minutiae.
    // We don't have that much menu stuff anyway, so just hide it.
    uimenu_window_hide(window);

    // Find and remove node from linked list
    // uimenu_window_t * tmpWnd = uimenu__first_window;
    // while(tmpWnd != NULL)
    // {
    //     if(tmpWnd->_next != NULL && tmpWnd->_next->id == window->id)
    //     {
    //         tmpWnd->_next = window->_next;
    //         break;
    //     }

    //     tmpWnd = tmpWnd->_next;
    // }

    // window->_next = NULL;
    // free(window);
}

//#pragma endregion

//#pragma region Private

BMAP * uimenu__make_button_graphic(int width, int height, int pressed, int tab)
{
    BMAP * final = bmap_createblack(width, height, 565);
    BMAP * canvas = bmap_createblack(width, height, 565);

    bmap_fill(final, vector(UIMENU_WINDOW_WINDOW_COLOR_B * 0.75, UIMENU_WINDOW_WINDOW_COLOR_G * 0.75, UIMENU_WINDOW_WINDOW_COLOR_R * 0.75), 100);
    bmap_fill(canvas, vector(UIMENU_WINDOW_WINDOW_COLOR_B * 1.25, UIMENU_WINDOW_WINDOW_COLOR_G * 1.25, UIMENU_WINDOW_WINDOW_COLOR_R * 1.25), 100);

    bmap_blitpart(final, canvas, vector(pressed, pressed, 0), vector(width - 1, height - 1, 0), vector(0, 0, 0), vector(width - 1, height - 1, 0));

    bmap_fill(canvas, vector(UIMENU_WINDOW_WINDOW_COLOR_B, UIMENU_WINDOW_WINDOW_COLOR_G, UIMENU_WINDOW_WINDOW_COLOR_R), 100);
    bmap_blitpart(final, canvas, vector(1, 1, 0), vector(width - 2, height - 2 - tab, 0), vector(0, 0, 0), vector(width - 2, height - 2 - tab, 0));

    bmap_purge(canvas);
    return final;    
}

BMAP * uimenu__make_field_graphic(int width, int height)
{
    BMAP * final = bmap_createblack(width, height, 565);
    BMAP * canvas = bmap_createblack(width, height, 565);

    bmap_fill(final, vector(UIMENU_WINDOW_WINDOW_COLOR_B * 0.75, UIMENU_WINDOW_WINDOW_COLOR_G * 0.75, UIMENU_WINDOW_WINDOW_COLOR_R * 0.75), 100);
    bmap_fill(canvas, vector(UIMENU_WINDOW_WINDOW_COLOR_B * 1.25, UIMENU_WINDOW_WINDOW_COLOR_G * 1.25, UIMENU_WINDOW_WINDOW_COLOR_R * 1.25), 100);

    bmap_blitpart(final, canvas, vector(1, 1, 0), vector(width - 1, height - 1, 0), vector(0, 0, 0), vector(width - 1, height - 1, 0));

    bmap_fill(canvas, vector(UIMENU_WINDOW_FIELD_COLOR_B, UIMENU_WINDOW_FIELD_COLOR_G, UIMENU_WINDOW_FIELD_COLOR_R), 100);
    bmap_blitpart(final, canvas, vector(1, 1, 0), vector(width - 2, height - 2, 0), vector(0, 0, 0), vector(width - 2, height - 2, 0));

    bmap_purge(canvas);
    return final;    
}



VECTOR * uimenu__get_element_screen_offset(uimenu_element_t * element)
{
    if(element == NULL) error("UIMENU: element must not be NULL");

    return vector( ((uimenu_window_t*)element->_parent)->_content_panel->pos_x + element->x, ((uimenu_window_t*)element->_parent)->_content_panel->pos_y + element->y, 0);
}

VECTOR * uimenu__get_cursor_offset_to_window(uimenu_window_t * window)
{
    if(window == NULL) error("UIMENU: window must not be NULL");

    return vector(mouse_pos.x - window->x, mouse_pos.y - window->y, 0);
}

int uimenu__is_cursor_in_window_titlebar(uimenu_window_t * window)
{
    if(window == NULL) error("UIMENU: window must not be NULL");

    VECTOR * cursorOffsetToWindow = uimenu__get_cursor_offset_to_window(window);
    if(cursorOffsetToWindow->x > UIMENU_WINDOW_BORDER_SIZE && cursorOffsetToWindow->x < (window->width - UIMENU_WINDOW_BORDER_SIZE))
    {
        if(cursorOffsetToWindow->y > UIMENU_WINDOW_BORDER_SIZE && cursorOffsetToWindow->y < (UIMENU_WINDOW_TITLE_BAR_SIZE + UIMENU_WINDOW_BORDER_SIZE))
        {
            return TRUE;
        }
    }

    return FALSE;    
}

int uimenu__is_cursor_in_window(uimenu_window_t * window)
{
    if(window == NULL) error("UIMENU: window must not be NULL");

    VECTOR * cursorOffsetToWindow = uimenu__get_cursor_offset_to_window(window);
    if(cursorOffsetToWindow->x > 0 && cursorOffsetToWindow->x < window->width)
    {
        if(cursorOffsetToWindow->y > 0 && cursorOffsetToWindow->y < window->height)
        {
            return TRUE;
        }
    }

    return FALSE;    
}

int uimenu__is_cursor_in_element(uimenu_element_t * element)
{
    if(element == NULL) error("UIMENU: element must not be NULL");

    VECTOR * elementScreenOffset = uimenu__get_element_screen_offset(element);
    if(mouse_pos.x >= elementScreenOffset->x && mouse_pos.x <= (elementScreenOffset->x + element->width))
    {
        if(mouse_pos.y >= elementScreenOffset->y && mouse_pos.y <= (elementScreenOffset->y + element->height))
        {
            return TRUE;
        }
    }

    return FALSE;    
}



void uimenu__window_initialize(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully

    if(window->_is_initialized == TRUE) return;
    
    if(window->_is_borderless == TRUE)
    {
        window->_panel = pan_create(NULL, window->layer);
        window->_content_panel = window->_panel;
    }
    else
    {        
        // If no other window is top window yet, bring to top
        if(uimenu__top_window == NULL)
            uimenu__top_window = window;

        window->_panel = pan_create(NULL, window->layer);
        window->_content_panel = pan_create(NULL, window->layer + 1);

        // Window Borders and colors
        window->_panel->flags |= (LIGHT);
        vec_set(window->_panel->blue, vector(UIMENU_WINDOW_BORDER_COLOR_B, UIMENU_WINDOW_BORDER_COLOR_G, UIMENU_WINDOW_BORDER_COLOR_R));
                
        // Add title bar
        // Active titlebar
        window->_title_element_bg_active_bmap = bmap_createblack(
            window->width - (UIMENU_WINDOW_BORDER_SIZE * 2), 
            UIMENU_WINDOW_TITLE_BAR_SIZE - UIMENU_WINDOW_BORDER_SIZE, 
            565);
        bmap_fill(window->_title_element_bg_active_bmap, 
            vector(UIMENU_WINDOW_TITLEBAR_COLOR_B, UIMENU_WINDOW_TITLEBAR_COLOR_G, UIMENU_WINDOW_TITLEBAR_COLOR_R), 
            100);

        // Inactive Titlebar
        window->_title_element_bg_inactive_bmap = bmap_createblack(
            window->width - (UIMENU_WINDOW_BORDER_SIZE * 2), 
            UIMENU_WINDOW_TITLE_BAR_SIZE - UIMENU_WINDOW_BORDER_SIZE, 
            565);
        bmap_fill(window->_title_element_bg_inactive_bmap, 
            vector(UIMENU_WINDOW_TITLEBAR_INACTIVE_COLOR_B, UIMENU_WINDOW_TITLEBAR_INACTIVE_COLOR_G, UIMENU_WINDOW_TITLEBAR_INACTIVE_COLOR_R), 
            100);

        window->_is_titlebar_active = FALSE;
        window->_title_element_bg_current_bmap = window->_title_element_bg_inactive_bmap;
    
        window->_title_element_bg_index = pan_setwindow(window->_panel, 0, 
            UIMENU_WINDOW_BORDER_SIZE, // x
            UIMENU_WINDOW_BORDER_SIZE, // y
            window->width - (UIMENU_WINDOW_BORDER_SIZE * 2), // width
            UIMENU_WINDOW_TITLE_BAR_SIZE - UIMENU_WINDOW_BORDER_SIZE, // height
            window->_title_element_bg_current_bmap, 0, 0);

        // Add title text
        window->_title_element_index = pan_setstring(window->_panel, 
            0, 
            UIMENU_WINDOW_BORDER_SIZE * 2, // x
            UIMENU_WINDOW_BORDER_SIZE * 2, // y
            uimenu_default_font, 
            str_create(window->title));
        pan_setcolor(window->_panel, 
            1,
            window->_title_element_index, 
            vector(UIMENU_WINDOW_TITLETEXT_INACTIVE_COLOR_B, UIMENU_WINDOW_TITLETEXT_INACTIVE_COLOR_G, UIMENU_WINDOW_TITLETEXT_INACTIVE_COLOR_R));

    }

    uimenu__sync_panel_with_window(window);

    // Set Background color for content panel AFTER getting the correct sizes
    if(window->_is_borderless == FALSE)
    {
        window->_content_panel_bg_bmap = bmap_createblack(
            window->_content_panel->size_x, 
            window->_content_panel->size_y, 
            565);
        bmap_fill(window->_content_panel_bg_bmap, 
            vector(UIMENU_WINDOW_WINDOW_COLOR_B, UIMENU_WINDOW_WINDOW_COLOR_G, UIMENU_WINDOW_WINDOW_COLOR_R), 
            100);
        window->_content_panel->bmap = window->_content_panel_bg_bmap;
    }

    if(window->_forced_background != NULL)
        window->_content_panel->bmap = window->_forced_background;

    if(window->_first != NULL)
        uimenu__element_initialize(window, window->_first);

    window->_is_initialized = true;    
}

void uimenu__element_initialize(uimenu_window_t * window, uimenu_element_t * element)
{
    if(window == NULL) return; // Fail gracefully
    if(element == NULL) return; // Fail gracefully
    if(window->_content_panel == NULL) return; // Fail gracefully

    if(element->_is_initialized == false)
    {
        switch(element->type)
        {
            case UIMENU_TYPE_TEXTBUTTON:
                element->bmap = uimenu__make_button_graphic(element->width, element->height, 0, 0);
                element->bmap_hover = element->bmap;
                element->bmap_active = uimenu__make_button_graphic(element->width, element->height, 1, 0);

                element->element_index = pan_setbutton(window->_content_panel, 0, 0, 
                element->x, 
                element->y, 
                element->bmap_active, // bmapOn
                element->bmap, // bmapOff
                element->bmap, // bmapHover
                element->bmap, // bmapHoverOff
                NULL, NULL, NULL);
                break;

            case UIMENU_TYPE_BUTTON:
                element->element_index = pan_setbutton(window->_content_panel, 0, 0, 
                element->x, 
                element->y, 
                element->bmap, // bmapOn
                element->bmap, // bmapOff
                element->bmap_hover, // bmapHover
                element->bmap, // bmapHoverOff
                element->callback, NULL, NULL);
                break;

            case UIMENU_TYPE_TEXT:
                element->element_index = pan_setstring(window->_content_panel, 0,
                    element->x, 
                    element->y, 
                    (FONT*)element->skill[UIMENU_SKILL_FONT], 
                    (STRING*)element->skill[UIMENU_SKILL_TEXT]);
                pan_setcolor(window->_content_panel, 1, element->element_index, element->skill[UIMENU_SKILL_COLOR]);
                break;

            case UIMENU_TYPE_SOLIDCOLOR:
                element->bmap = bmap_createblack(element->width, element->height, 565);
                bmap_fill(element->bmap, element->skill[UIMENU_SKILL_COLOR], 100);
                // !! Intentional fall-through !!
            case UIMENU_TYPE_IMAGE:
                element->element_index = pan_setwindow(window->_content_panel, 0, 
                element->x,
                element->y,
                element->width, 
                element->height,
                element->bmap, 0, 0);
                break;

            default:
                // error("UIMENU: Invalid element type");
                break;
        }

        if(element->_related != NULL)
        {
            if(element->_related->_is_initialized == FALSE)
                uimenu__element_initialize(window, element->_related);
        }

        element->_is_initialized = true;
    }
    
    if(element->_next != NULL)
        uimenu__element_initialize(window, element->_next);
}

void uimenu__sync_panel_with_window(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully
    if(window->_panel == NULL) return; // Fail gracefully

    int fixContent = FALSE;
    int fixTitlebar = FALSE;

    if(window->_is_dirty == TRUE)
    {
        if(window->_parent == NULL)
        {
            window->_panel->pos_x = window->x;
            window->_panel->pos_y = window->y;
        }
        else
        {
            window->_panel->pos_x = window->_parent->_content_panel->pos_x + window->x;
            window->_panel->pos_y = window->_parent->_content_panel->pos_y + window->y;
        }

        if(window->width > 0)
            window->_panel->size_x = window->width;
        else // Negative number is percentage relative to screen size
            window->_panel->size_x = screen_size.x * (-window->width / 100.0);

        if(window->height > 0)
            window->_panel->size_y = window->height;
        else // Negative number is percentage relative to screen size
            window->_panel->size_y = screen_size.y * (-window->height / 100.0);
            
        layer_sort(window->_panel, window->layer);

        if(window->_is_borderless == FALSE)
        {
            layer_sort(window->_content_panel, window->layer + 1);
            fixContent = TRUE;

            if(uimenu_is_top_window(window) && window->_is_titlebar_active == FALSE)
            {
                window->_title_element_bg_current_bmap = window->_title_element_bg_active_bmap;
                fixTitlebar = TRUE;
            }
            else if(!uimenu_is_top_window(window) && window->_is_titlebar_active == TRUE)
            {
                window->_title_element_bg_current_bmap = window->_title_element_bg_inactive_bmap;
                fixTitlebar = TRUE;
            }

            if(fixTitlebar == TRUE)
            {
                pan_setwindow(window->_panel, window->_title_element_bg_index, 
                    UIMENU_WINDOW_BORDER_SIZE, // x
                    UIMENU_WINDOW_BORDER_SIZE, // y
                    window->width - (UIMENU_WINDOW_BORDER_SIZE * 2), // width
                    UIMENU_WINDOW_TITLE_BAR_SIZE - UIMENU_WINDOW_BORDER_SIZE, // height
                    window->_title_element_bg_current_bmap, 0, 0);
            }
        }

        window->_is_dirty = false;
    }

    // Don't do this if we're a slaved window
    if(window->_parent == NULL)
    {
        // If out of bounds, fix in next update
        if((window->_panel->pos_x + window->_panel->size_x) > screen_size.x)
        {
            window->x = screen_size.x - window->_panel->size_x;
            // window->_is_dirty = true;
            uimenu__window_make_dirty(window);
        }
        if(window->_panel->pos_x < 0)
        {
            window->_panel->pos_x = 0;
            // window->_is_dirty = true;
            uimenu__window_make_dirty(window);
            if(window->_is_borderless == FALSE) fixContent = true;
        }
        if((window->_panel->pos_y + window->_panel->size_y) > screen_size.y)
        {
            window->y = screen_size.y - window->_panel->size_y;
            // window->_is_dirty = true;
            uimenu__window_make_dirty(window);
        }
        if(window->_panel->pos_y < 0)
        {
            window->_panel->pos_y = 0;
            // window->_is_dirty = true;
            uimenu__window_make_dirty(window);
            if(window->_is_borderless == FALSE) fixContent = true;
        }
    }

    if(fixContent == TRUE)
    {
        window->_content_panel->pos_x = window->_panel->pos_x + UIMENU_WINDOW_BORDER_SIZE;
        window->_content_panel->pos_y = window->_panel->pos_y + UIMENU_WINDOW_TITLE_BAR_SIZE + 1;
        window->_content_panel->size_x = window->_panel->size_x - (UIMENU_WINDOW_BORDER_SIZE * 2);
        window->_content_panel->size_y = window->_panel->size_y - (UIMENU_WINDOW_BORDER_SIZE + UIMENU_WINDOW_TITLE_BAR_SIZE);
    }
}

void uimenu__window_make_dirty(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully
    window->_is_dirty = TRUE;
    if(window->_child != NULL)
        uimenu__window_make_dirty(window->_child);
}

void uimenu__window_update(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully

    // Drag+Move window code
    if(window->_is_visible == TRUE)
    {
        if(window->_is_borderless == FALSE)
        {
            if(window->_is_moving == FALSE && mouse_left && uimenu__is_cursor_in_window_titlebar(window))
            {
                window->_is_moving = TRUE;
                vec_set(window->_moving_start_offset, uimenu__get_cursor_offset_to_window(window));
            }
            else if(window->_is_moving && mouse_left)
            {
                window->x = mouse_pos.x - window->_moving_start_offset[0];
                window->y = mouse_pos.y - window->_moving_start_offset[1];
                // window->_is_dirty = TRUE;
                uimenu__window_make_dirty(window);
            }
            else if(window->_is_moving && !mouse_left)
            {
                window->_is_moving = FALSE;
                // window->_is_dirty = TRUE;
                uimenu__window_make_dirty(window);
            }
        }
        
        uimenu__element_update(window->_first);
    }

    uimenu__sync_panel_with_window(window);
}

void uimenu__element_update(uimenu_element_t * element)
{
    if(element == NULL) return; // Fail gracefully

    if(element->type == UIMENU_TYPE_TEXTBUTTON)
    {
        if(uimenu__is_cursor_in_element(element))
        {
            if(mouse_left)
            {
                element->skill[UIMENU_SKILL_PRESSED] = TRUE;
            }
            else if(element->skill[UIMENU_SKILL_PRESSED] == TRUE && !mouse_left)
            {
                element->skill[UIMENU_SKILL_PRESSED] = FALSE;
                
                // Callback click event
                function callback();
                callback = element->callback;
                if(callback != NULL)
                {
                    callback();
                }
            }
        }
        else if(element->skill[UIMENU_SKILL_PRESSED] == TRUE)
        {
            // Don't trigger if mouse moves outside
            element->skill[UIMENU_SKILL_PRESSED] = FALSE;
        }
    }

    if(element->_next)
        uimenu__element_update(element->_next);
}

//#pragma endregion