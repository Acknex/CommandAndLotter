#include "ui_menu.h"

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

    // Increment the layers, so every window has 100 layers to work with.
    // 99 effective layers for bordered windows since one is used for the nested content panel
    uimenu_last_layer += 100;
    
    // Establish window linked list for updates
    window->id = uimenu_window_id++;
    window->_next = uimenu_first_window;
    uimenu_first_window = window;

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
    element->_next = window->_first;
    window->_first = element;
}

uimenu_element_t * uimenu_make_text(var x, var y, var width, var height, char * text, COLOR * colorBGR, FONT * font)
{
    uimenu_element_t * element = uimenu_element_create(UIMENU_TYPE_TEXT, x, y, width, height);
    
    element->skill[UIMENU_SKILL_TEXT] = text;
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

uimenu_element_t * uimenu_make_image(var x, var y, var width, var height, BMAP * bmap)
{
    uimenu_element_t * element = uimenu_element_create(UIMENU_TYPE_IMAGE, x, y, width, height);
    
    element->bmap = bmap;

    return element;
}

void uimenu_window_initialize(uimenu_window_t * window)
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
        window->_panel = pan_create(NULL, window->layer);
        window->_content_panel = pan_create(NULL, window->layer + 1);

        // Window Borders and colors
        window->_panel->flags |= (LIGHT);
        //window->_content_panel->flags |= (LIGHT);
        vec_set(window->_panel->blue, vector(UIMENU_WINDOW_BORDER_COLOR_B, UIMENU_WINDOW_BORDER_COLOR_G, UIMENU_WINDOW_BORDER_COLOR_R));
        //vec_set(window->_content_panel->blue, vector(UIMENU_WINDOW_WINDOW_COLOR_B, UIMENU_WINDOW_WINDOW_COLOR_G, UIMENU_WINDOW_WINDOW_COLOR_R));

        
        // Add title bar
        window->_title_element_bg_bmap = bmap_createblack(
            window->width - (UIMENU_WINDOW_BORDER_SIZE * 2), 
            UIMENU_WINDOW_TITLE_BAR_SIZE - UIMENU_WINDOW_BORDER_SIZE, 
            565);
        bmap_fill(window->_title_element_bg_bmap, 
            vector(UIMENU_WINDOW_TITLEBAR_COLOR_B, UIMENU_WINDOW_TITLEBAR_COLOR_G, UIMENU_WINDOW_TITLEBAR_COLOR_R), 
            100);

        window->_title_element_bg_index = pan_setwindow(window->_panel, 0, 
            UIMENU_WINDOW_BORDER_SIZE, // x
            UIMENU_WINDOW_BORDER_SIZE, // y
            window->width - (UIMENU_WINDOW_BORDER_SIZE * 2), // width
            UIMENU_WINDOW_TITLE_BAR_SIZE - UIMENU_WINDOW_BORDER_SIZE, // height
            window->_title_element_bg_bmap, 0, 0);

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
            vector(UIMENU_WINDOW_TITLETEXT_COLOR_B, UIMENU_WINDOW_TITLETEXT_COLOR_G, UIMENU_WINDOW_TITLETEXT_COLOR_R));

    }

    uimenu_sync_panel_with_window(window);

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

    if(window->_first != NULL)
        uimenu_element_initialize(window, window->_first);

    window->_is_initialized = true;    
}

void uimenu_element_initialize(uimenu_window_t * window, uimenu_element_t * element)
{
    if(window == NULL) return; // Fail gracefully
    if(element == NULL) return; // Fail gracefully
    if(window->_content_panel == NULL) return; // Fail gracefully

    if(element->_is_initialized == false)
    {
        switch(element->type)
        {
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
                element->skill[UIMENU_SKILL_FONT], 
                str_create(element->skill[UIMENU_SKILL_TEXT]));
                pan_setcolor(window->_content_panel, 1, element->element_index, element->skill[UIMENU_SKILL_COLOR]);
                break;

            case UIMENU_TYPE_SOLIDCOLOR:
                element->bmap = bmap_createblack(element->width, element->height, 565);
                bmap_fill(element->bmap, element->skill[UIMENU_SKILL_COLOR], 100);
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
    }
    
    if(element->_next != NULL)
        uimenu_element_initialize(window, element->_next);
}

void uimenu_sync_panel_with_window(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully
    if(window->_panel == NULL) return; // Fail gracefully

    int fixContent = FALSE;

    if(window->_is_dirty == TRUE)
    {
        window->_panel->pos_x = window->x;
        window->_panel->pos_y = window->y;

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
        }

        window->_is_dirty = false;
    }

    // If out of bounds, fix in next update
    if((window->_panel->pos_x + window->_panel->size_x) > screen_size.x)
    {
        window->x = screen_size.x - window->_panel->size_x;
        window->_is_dirty = true;
    }
    if(window->_panel->pos_x < 0)
    {
        window->_panel->pos_x = 0;
        window->_is_dirty = true;
        if(window->_is_borderless == FALSE) fixContent = true;
    }
    if((window->_panel->pos_y + window->_panel->size_y) > screen_size.y)
    {
        window->y = screen_size.y - window->_panel->size_y;
        window->_is_dirty = true;
    }
    if(window->_panel->pos_y < 0)
    {
        window->_panel->pos_y = 0;
        window->_is_dirty = true;
        if(window->_is_borderless == FALSE) fixContent = true;
    }

    if(fixContent == TRUE)
    {
        window->_content_panel->pos_x = window->_panel->pos_x + UIMENU_WINDOW_BORDER_SIZE;
        window->_content_panel->pos_y = window->_panel->pos_y + UIMENU_WINDOW_TITLE_BAR_SIZE + 1;
        window->_content_panel->size_x = window->_panel->size_x - (UIMENU_WINDOW_BORDER_SIZE * 2);
        window->_content_panel->size_y = window->_panel->size_y - (UIMENU_WINDOW_BORDER_SIZE + UIMENU_WINDOW_TITLE_BAR_SIZE);
    }
}

void uimenu_window_to_top(uimenu_window_t * window)
{    
    window->layer = uimenu_last_layer;
    window->_is_dirty = true;

    uimenu_last_layer += 100;
}

VECTOR * uimenu_get_element_window_offset(uimenu_element_t * element)
{
    if(element == NULL) error("UIMENU: element must not be NULL");

    return vector( ((uimenu_window_t*)element->_parent)->x + element->x, ((uimenu_window_t*)element->_parent)->y + element->y, 0);
}

VECTOR * uimenu_get_cursor_offset_to_window(uimenu_window_t * window)
{
    if(window == NULL) error("UIMENU: window must not be NULL");

    return vector(mouse_pos.x - window->x, mouse_pos.y - window->y, 0);
}

int uimenu_is_cursor_in_window_titlebar(uimenu_window_t * window)
{
    if(window == NULL) error("UIMENU: element must not be NULL");

    VECTOR * cursorOffsetToWindow = uimenu_get_cursor_offset_to_window(window);
    if(cursorOffsetToWindow->x > UIMENU_WINDOW_BORDER_SIZE && cursorOffsetToWindow->x < (window->width - UIMENU_WINDOW_BORDER_SIZE))
    {
        if(cursorOffsetToWindow->y > UIMENU_WINDOW_BORDER_SIZE && cursorOffsetToWindow->y < (UIMENU_WINDOW_TITLE_BAR_SIZE + UIMENU_WINDOW_BORDER_SIZE))
        {
            return TRUE;
        }
    }

    return FALSE;    
}

void uimenu_window_show(uimenu_window_t * window)
{
    window->_panel->flags |= SHOW;
    if(window->_is_borderless == FALSE)
        window->_content_panel->flags |= SHOW;

    window->_is_visible = TRUE;
}

void uimenu_window_hide(uimenu_window_t * window)
{
    window->_panel->flags &= ~SHOW;
    if(window->_is_borderless == FALSE)
        window->_content_panel->flags &= ~SHOW;

    window->_is_visible = FALSE;
}

void uimenu_window_update(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully

    // Drag+Move window code
    if(window->_is_visible == TRUE && window->_is_borderless == FALSE)
    {
        if(window->_is_moving == FALSE && mouse_left && uimenu_is_cursor_in_window_titlebar(window))
        {
            window->_is_moving = TRUE;
            window->_moving_start_offset = uimenu_get_cursor_offset_to_window(window);
        }
        else if(window->_is_moving && mouse_left)
        {
            window->x = mouse_pos.x - window->_moving_start_offset->x;
            window->y = mouse_pos.y - window->_moving_start_offset->y;
            window->_is_dirty = TRUE;
        }
        else if(window->_is_moving && !mouse_left)
        {
            window->_is_moving = FALSE;
        }
    }

    uimenu_sync_panel_with_window(window);
}

void uimenu_window_destroy(uimenu_window_t * window)
{
    if(window == NULL) return; // Fail gracefully
    
    // Honestly can't be arsed with freeing memory properly for all the minutiae.
    // We don't have that much menu stuff anyway, so just hide it.
    uimenu_window_hide(window);

    // Find and remove node from linked list
    // uimenu_window_t * tmpWnd = uimenu_first_window;
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

void uimenu_update()
{
    if(uimenu_first_window != NULL)
        uimenu_update_chain(uimenu_first_window);
}

void uimenu_update_chain(uimenu_window_t * window)
{
    uimenu_window_update(window);
    if(window->_next != NULL)
        uimenu_update_chain(window->_next);
}