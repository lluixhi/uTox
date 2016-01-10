#include "main.h"

#include "ui_edits.h"
#include "ui_buttons.h"
#include "ui_dropdown.h"

// Application-wide language setting
UI_LANG_ID LANG;

/***** MAYBE_I18NAL_STRING helpers start *****/

void maybe_i18nal_string_set_plain(MAYBE_I18NAL_STRING *mis, char_t *str, STRING_IDX length) {
    mis->plain.str = str;
    mis->plain.length = length;
    mis->i18nal = UI_STRING_ID_INVALID;
}

void maybe_i18nal_string_set_i18nal(MAYBE_I18NAL_STRING *mis, UI_STRING_ID string_id) {
    mis->plain.str = NULL;
    mis->plain.length = 0;
    mis->i18nal = string_id;
}

STRING* maybe_i18nal_string_get(MAYBE_I18NAL_STRING *mis) {
    if(mis->plain.str) {
        return &mis->plain;
    } else {
        return SPTRFORLANG(LANG, mis->i18nal);
    }
}

_Bool maybe_i18nal_string_is_valid(MAYBE_I18NAL_STRING *mis) {
    return (mis->plain.str || ((UI_STRING_ID_INVALID != mis->i18nal) && (mis->i18nal < NUM_STRS)));
}

/***** MAYBE_I18NAL_STRING helpers end *****/

void draw_avatar_image(UTOX_NATIVE_IMAGE *image, int x, int y, uint32_t width, uint32_t height, uint32_t targetwidth, uint32_t targetheight)
{
    /* get smallest of width or height */
    double scale = (width > height) ?
                      (double)targetheight / height :
                      (double)targetwidth / width;

    image_set_scale(image, scale);
    image_set_filter(image, FILTER_BILINEAR);

    /* set position to show the middle of the image in the center  */
    int xpos = (int) ((double)width * scale / 2 - (double)targetwidth / 2);
    int ypos = (int) ((double)height * scale / 2 - (double)targetheight / 2);

    draw_image(image, x, y, targetwidth, targetheight, xpos, ypos);

    image_set_scale(image, 1.0);
    image_set_filter(image, FILTER_NEAREST);
}

/* Top left self interface Avatar, name, statusmsg, status icon */
static void draw_user_badge(int UNUSED(x), int UNUSED(y), int UNUSED(width), int UNUSED(height)){
    if (tox_thread_init) {
        /* Only draw the user badge if toxcore is running */
        /*draw avatar or default image */
        if (self_has_avatar()) {
            draw_avatar_image(self.avatar.image, SIDEBAR_AVATAR_LEFT, SIDEBAR_AVATAR_TOP,
                              self.avatar.width, self.avatar.height, BM_CONTACT_WIDTH, BM_CONTACT_WIDTH);
        } else {
            drawalpha(BM_CONTACT, SIDEBAR_AVATAR_LEFT, SIDEBAR_AVATAR_TOP,
                      BM_CONTACT_WIDTH, BM_CONTACT_WIDTH, COLOR_MENU_TEXT);
        }
        /* Draw name */
        setcolor(!button_name.mouseover ? COLOR_MENU_TEXT : COLOR_MENU_SUBTEXT);
        setfont(FONT_SELF_NAME);
        drawtextrange(SIDEBAR_NAME_LEFT, SIDEBAR_NAME_WIDTH, SIDEBAR_NAME_TOP, self.name, self.name_length);

        /*&Draw current status message
        @TODO: separate these colors if needed (COLOR_MAIN_HINTTEXT) */
        setcolor(!button_statusmsg.mouseover ? COLOR_MENU_SUBTEXT : COLOR_MAIN_HINTTEXT);
        setfont(FONT_STATUS);
        drawtextrange(SIDEBAR_STATUSMSG_LEFT, SIDEBAR_STATUSMSG_WIDTH, SIDEBAR_STATUSMSG_TOP,
                      self.statusmsg, self.statusmsg_length);

        /* Draw status button icon */
        drawalpha(BM_STATUSAREA, SELF_STATUS_ICON_LEFT, SELF_STATUS_ICON_TOP, BM_STATUSAREA_WIDTH, BM_STATUSAREA_HEIGHT,
                  button_status.mouseover ? COLOR_BACKGROUND_LIST_HOVER : COLOR_BACKGROUND_LIST);
        uint8_t status = tox_connected ? self.status : 3;
        drawalpha(BM_ONLINE + status, SELF_STATUS_ICON_LEFT + BM_STATUSAREA_WIDTH / 2 - BM_STATUS_WIDTH / 2,
                  SELF_STATUS_ICON_TOP + BM_STATUSAREA_HEIGHT / 2 - BM_STATUS_WIDTH / 2, BM_STATUS_WIDTH, BM_STATUS_WIDTH,
                  status_color[status]);

        /* Draw online/all friends filter text. */
        setcolor(!button_filter_friends.mouseover ? COLOR_MENU_SUBTEXT : COLOR_MAIN_HINTTEXT);
        setfont(FONT_STATUS);
        drawtextrange(SIDEBAR_FILTER_FRIENDS_LEFT, SIDEBAR_FILTER_FRIENDS_WIDTH, SIDEBAR_FILTER_FRIENDS_TOP,
                      list_get_filter() ? S(FILTER_ONLINE)    : S(FILTER_ALL),
                      list_get_filter() ? SLEN(FILTER_ONLINE) : SLEN(FILTER_ALL) );
    } else {
        drawalpha(BM_CONTACT, SIDEBAR_AVATAR_LEFT, SIDEBAR_AVATAR_TOP,
                  BM_CONTACT_WIDTH, BM_CONTACT_WIDTH, COLOR_MENU_TEXT);

        setcolor(!button_name.mouseover ? COLOR_MENU_TEXT : COLOR_MENU_SUBTEXT);
        setfont(FONT_SELF_NAME);
        drawtextrange(SIDEBAR_NAME_LEFT, SIDEBAR_NAME_WIDTH, SIDEBAR_NAME_TOP, (char_t *)"Not connected!", 14);
    }
}

/* Header for friend chat window */
static void draw_friend(int x, int y, int w, int height){
    FRIEND *f = selected_item->data;

    // draw avatar or default image
    if (friend_has_avatar(f)) {
        draw_avatar_image(f->avatar.image, MAIN_LEFT + utox_scale(5), utox_scale(5), f->avatar.width, f->avatar.height, BM_CONTACT_WIDTH, BM_CONTACT_WIDTH);
    } else {
        drawalpha(BM_CONTACT, MAIN_LEFT + utox_scale(5), utox_scale(5), BM_CONTACT_WIDTH, BM_CONTACT_WIDTH, COLOR_MAIN_TEXT);
    }

    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_TITLE);

    if (f->alias) {
        drawtextrange(MAIN_LEFT + utox_scale(30 ), utox_window_width - utox_scale(64 ), utox_scale(9 ), f->alias, f->alias_length);
    } else {
        drawtextrange(MAIN_LEFT + utox_scale(30 ), utox_window_width - utox_scale(64 ), utox_scale(9 ), f->name, f->name_length);
    }

    setcolor(COLOR_MAIN_SUBTEXT);
    setfont(FONT_STATUS);
    drawtextrange(MAIN_LEFT + utox_scale(30 ), utox_window_width - utox_scale(64 ), utox_scale(16 ), f->status_message, f->status_length);

    if (f->typing) {
        int typing_y = ((y + height) + CHAT_BOX_TOP - utox_scale(7 ));
        setfont(FONT_MISC);
        // @TODO: separate these colors if needed
        setcolor(COLOR_MAIN_HINTTEXT);
        drawtextwidth_right(x, MESSAGES_X - NAME_OFFSET, typing_y, f->name, f->name_length);
        drawtextwidth(x + MESSAGES_X, x + w, typing_y, S(IS_TYPING), SLEN(IS_TYPING));
    }
}

static void draw_group(int UNUSED(x), int UNUSED(y), int UNUSED(w), int UNUSED(height)){
    GROUPCHAT *g = selected_item->data;

    drawalpha(BM_GROUP, MAIN_LEFT + utox_scale(5), utox_scale(5), BM_CONTACT_WIDTH, BM_CONTACT_WIDTH, COLOR_MAIN_TEXT);

    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_TITLE);
    drawtextrange(MAIN_LEFT + utox_scale(30 ), utox_window_width - utox_scale(32 ), utox_scale(1 ), g->name, g->name_length);

    setcolor(COLOR_MAIN_SUBTEXT);
    setfont(FONT_STATUS);
    drawtextrange(MAIN_LEFT + utox_scale(30 ), utox_window_width - utox_scale(32 ), utox_scale(8 ), g->topic, g->topic_length);

    uint32_t i = 0;
    int k = MAIN_LEFT + utox_scale(30 );

    uint64_t time = get_time();

    unsigned int pos_y = 15;
    while (i < g->peers) {
        uint8_t *name = g->peername[i];
        if (name) {
            uint8_t buf[134];
            memcpy(buf, name + 1, name[0]);
            memcpy(buf + name[0], ", ", 2);

            int w = textwidth(buf, name[0] + 2);
            if (i == g->our_peer_number) {
                setcolor(COLOR_GROUP_SELF);
            } else if (time - g->last_recv_audio[i] <= (uint64_t)1 * 1000 * 1000 * 1000) {
                setcolor(COLOR_GROUP_AUDIO);
            } else {
                setcolor(COLOR_GROUP_PEER);
            }

            if (k + w >= (utox_window_width - utox_scale(32 ))) {
                if (pos_y == 15) {
                    pos_y += 6;
                    k = MAIN_LEFT + utox_scale(30 );
                } else {
                    drawtext(k, utox_scale(pos_y), (uint8_t*)"...", 3);
                    break;
                }
            }

            drawtext(k, utox_scale(pos_y), buf, name[0] + 2);

            k += w;
        }
        i++;
    }
}

/* Draw an invite to be a friend window */
static void draw_friend_request(int UNUSED(x), int UNUSED(y), int UNUSED(w), int UNUSED(height)){
    FRIENDREQ *req = selected_item->data;

    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_SELF_NAME);
    drawstr(MAIN_LEFT + utox_scale(5), utox_scale(10), FRIENDREQUEST);

    setcolor(COLOR_MAIN_SUBTEXT);
    setfont(FONT_STATUS);
    drawtextrange(MAIN_LEFT + utox_scale(5 ), utox_window_width, utox_scale(20 ), req->msg, req->length);
}

/* Draw add a friend window */
static void draw_add_friend(int UNUSED(x), int UNUSED(y), int UNUSED(w), int height){
    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_SELF_NAME);
    drawstr(MAIN_LEFT + utox_scale(5), utox_scale(10), ADDFRIENDS);

    setcolor(COLOR_MAIN_SUBTEXT);
    setfont(FONT_TEXT);
    drawstr(MAIN_LEFT + utox_scale(5), MAIN_TOP + utox_scale(5), TOXID);



    drawstr(MAIN_LEFT + utox_scale(5), MAIN_TOP + utox_scale(29), MESSAGE);

    if (options.proxy_type && !options.udp_enabled) {
        int push = UTOX_STR_WIDTH(TOXID);
        setfont(FONT_MISC);
        setcolor(C_RED);
        drawstr(MAIN_LEFT + utox_scale(10) + push, MAIN_TOP + utox_scale(6), DNS_DISABLED);
    }

    if (addfriend_status) {
        setfont(FONT_MISC);
        setcolor(C_RED);

        STRING *str;
        switch(addfriend_status) {
        case ADDF_SENT:
            str = SPTR(REQ_SENT); break;
        case ADDF_DISCOVER:
            str = SPTR(REQ_RESOLVE); break;
        case ADDF_BADNAME:
            str = SPTR(REQ_INVALID_ID); break;
        case ADDF_NONAME:
            str = SPTR(REQ_EMPTY_ID); break;
        case ADDF_TOOLONG: //if message length is too long.
            str = SPTR(REQ_LONG_MSG); break;
        case ADDF_NOMESSAGE: //if no message (message length must be >= 1 byte).
            str = SPTR(REQ_NO_MSG); break;
        case ADDF_OWNKEY: //if user's own key.
            str = SPTR(REQ_SELF_ID); break;
        case ADDF_ALREADYSENT: //if friend request already sent or already a friend.
            str = SPTR(REQ_ALREADY_FRIENDS); break;
        case ADDF_BADCHECKSUM: //if bad checksum in address.
            str = SPTR(REQ_BAD_CHECKSUM); break;
        case ADDF_SETNEWNOSPAM: //if the friend was already there but the nospam was different.
            str = SPTR(REQ_BAD_NOSPAM); break;
        case ADDF_NOMEM: //if increasing the friend list size fails.
            str = SPTR(REQ_NO_MEMORY); break;
        case ADDF_UNKNOWN: //for unknown error.
        case ADDF_NONE: //this case must never be rendered, but if it does, assume it's an error
        default:
            str = SPTR(REQ_UNKNOWN); break;
        }

        drawtextmultiline(MAIN_LEFT + utox_scale(5), utox_window_width - BM_SBUTTON_WIDTH - utox_scale(5 ), MAIN_TOP + utox_scale(83), 0, height, font_small_lineheight, str->str, str->length, 0xFFFF, 0, 0, 0, 1);
    }
}

/* Draw the text for profile password window */
static void draw_profile_password(int UNUSED(x), int UNUSED(y), int UNUSED(w), int height){
    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_SELF_NAME);
    drawstr(MAIN_LEFT + utox_scale(5), utox_scale(10), PROFILE_PASSWORD);

    setcolor(COLOR_MAIN_SUBTEXT);
    setfont(FONT_TEXT);
    drawstr(MAIN_LEFT + utox_scale(5), MAIN_TOP + utox_scale(5), PROFILE_PASSWORD);
}

/* Top bar for user settings */
static void draw_settings_header(int UNUSED(x), int UNUSED(y), int UNUSED(width), int UNUSED(height)){
    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_SELF_NAME);
    drawstr(MAIN_LEFT + utox_scale(5), utox_scale(5), UTOX_SETTINGS);
    #ifdef GIT_VERSION
        int x = MAIN_LEFT + utox_scale(5 ) + UTOX_STR_WIDTH(UTOX_SETTINGS) + utox_scale(5 );
        setfont(FONT_TEXT);
        drawtext(x, utox_scale(5), (uint8_t*)GIT_VERSION, strlen(GIT_VERSION));
    #endif
}

/* draw switch profile top bar */
/* Text content for settings page */
static void draw_settings_text_profile(int x, int y, int w, int h){
    setcolor(COLOR_MAIN_TEXT);
    drawstr(MAIN_LEFT + utox_scale(5 ), y + utox_scale(5   ), NAME);
    drawstr(MAIN_LEFT + utox_scale(5 ), y + utox_scale(30  ), STATUSMESSAGE);
    setfont(FONT_SELF_NAME);
    drawstr(MAIN_LEFT + utox_scale(5 ), y + utox_scale(55  ), TOXID);
    setfont(FONT_TEXT);
    drawstr(MAIN_LEFT + utox_scale(5 ), y + utox_scale(75  ), LANGUAGE);
    drawstr(MAIN_LEFT + utox_scale(5 ), y + utox_scale(100 ), PROFILE_PASSWORD);

    setfont(FONT_MISC);
    setcolor(C_RED);
    drawstr(MAIN_LEFT + utox_scale(40 ), y + utox_scale(123 ), PROFILE_PW_WARNING);
    drawstr(MAIN_LEFT + utox_scale(40 ), y + utox_scale(130 ), PROFILE_PW_NO_RECOVER);
}

static void draw_settings_text_network(int x, int y, int w, int UNUSED(height)){
    setfont(FONT_MISC);
    setcolor(C_RED);
    drawstr(MAIN_LEFT  + utox_scale(5   ), y + utox_scale(5 ), WARNING);

    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_TEXT);
    drawstr(MAIN_LEFT  + utox_scale(5   ), y + utox_scale(15 ), IPV6);
    drawstr(MAIN_LEFT  + utox_scale(55  ), y + utox_scale(15 ), UDP);
    drawstr(MAIN_LEFT  + utox_scale(5   ), y + utox_scale(30 ), PROXY);
    setfont(FONT_SELF_NAME);
    drawtext(MAIN_LEFT + utox_scale(132 ), y + utox_scale(42 ), (uint8_t*)":", 1);
}

static void draw_settings_text_ui(int x, int y, int w, int UNUSED(height)){
    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_TEXT);
    drawstr(MAIN_LEFT + utox_scale(75 ), y + utox_scale(5 ), DPI);
    drawstr(MAIN_LEFT + utox_scale(5  ), y + utox_scale(5 ), THEME);
    drawstr(MAIN_LEFT + utox_scale(5  ), y + utox_scale(30 ), LOGGING);
    drawstr(MAIN_LEFT + utox_scale(5  ), y + utox_scale(55 ), CLOSE_TO_TRAY);
    drawstr(MAIN_LEFT + utox_scale(5  ) + UTOX_STR_WIDTH(CLOSE_TO_TRAY) + utox_scale(10 ), y + utox_scale(55 ), START_IN_TRAY);
    drawstr(MAIN_LEFT + utox_scale(5  ), y + utox_scale(80 ), AUTO_STARTUP);
    drawstr(MAIN_LEFT + utox_scale(5  ), y + utox_scale(105 ), SEND_TYPING_NOTIFICATIONS);
}

static void draw_settings_text_av(int x, int y, int w, int UNUSED(height)){
    setcolor(COLOR_MAIN_TEXT);
    setfont(FONT_TEXT);
    drawstr(MAIN_LEFT + utox_scale(5   ), y + utox_scale(5  ),  RINGTONE);
    drawstr(MAIN_LEFT + utox_scale(60  ), y + utox_scale(5  ),  PUSH_TO_TALK);
    #ifdef AUDIO_FILTERING
    drawstr(MAIN_LEFT + utox_scale(120 ), y + utox_scale(5  ),  AUDIOFILTERING);
    #endif
    drawstr(MAIN_LEFT + utox_scale(5   ), y + utox_scale(35  ), AUDIOINPUTDEVICE);
    drawstr(MAIN_LEFT + utox_scale(5   ), y + utox_scale(60  ), AUDIOOUTPUTDEVICE);
    drawstr(MAIN_LEFT + utox_scale(5   ), y + utox_scale(85  ), VIDEOINPUTDEVICE);
    setfont(FONT_SELF_NAME);
    drawstr(MAIN_LEFT + utox_scale(5   ), y + utox_scale(115 ), PREVIEW);
}

static void draw_settings_sub_header(int x, int y, int w, int UNUSED(height)){
    setfont(FONT_SELF_NAME);

    /* Draw the text and bars for general settings */
    setcolor(!button_settings_sub_profile.mouseover ? COLOR_MAIN_TEXT : COLOR_MAIN_SUBTEXT);
    int x_right_edge = x + utox_scale(5 ) + UTOX_STR_WIDTH(PROFILE) + utox_scale(5 );
    drawstr( x + utox_scale(5 ), y + utox_scale(5 ), PROFILE);
    if (panel_settings_profile.disabled) {
        drawhline( x, y + utox_scale(15 ), x_right_edge, COLOR_EDGE_NORMAL);
    } else {
        drawhline( x, y + 0, x_right_edge, COLOR_EDGE_ACTIVE);
        drawhline( x, y + 1, x_right_edge, COLOR_EDGE_ACTIVE);
    }
    drawvline( x_right_edge, y + utox_scale(0 ), y + utox_scale(15 ), COLOR_EDGE_NORMAL);

    /* Draw the text and bars for network settings */
    setcolor(!button_settings_sub_net.mouseover ? COLOR_MAIN_TEXT : COLOR_MAIN_SUBTEXT);
    x = x_right_edge;
    x_right_edge = x_right_edge + utox_scale(5 ) + UTOX_STR_WIDTH(NETWORK) + utox_scale(5 );
    drawstr( x + utox_scale(5 ), y + utox_scale(5 ), NETWORK);
    if (panel_settings_net.disabled) {
        drawhline( x, y + utox_scale(15 ), x_right_edge, COLOR_EDGE_NORMAL);
    } else {
        drawhline( x, y + 0, x_right_edge, COLOR_EDGE_ACTIVE);
        drawhline( x, y + 1, x_right_edge, COLOR_EDGE_ACTIVE);
    }
    drawvline( x_right_edge, y + utox_scale(0 ), y + utox_scale(15  ), COLOR_EDGE_NORMAL);

    /* Draw the text and bars for User interface settings */
    setcolor(!button_settings_sub_ui.mouseover ? COLOR_MAIN_TEXT : COLOR_MAIN_SUBTEXT);
    x = x_right_edge;
    x_right_edge = x_right_edge + utox_scale(5 ) + UTOX_STR_WIDTH(USER_INTERFACE) + utox_scale(5 );
    drawstr( x + utox_scale(5 ), y + utox_scale(5 ), USER_INTERFACE);
    if (panel_settings_ui.disabled) {
        drawhline( x, y + utox_scale(15 ), x_right_edge, COLOR_EDGE_NORMAL);
    } else {
        drawhline( x, y + 0, x_right_edge, COLOR_EDGE_ACTIVE);
        drawhline( x, y + 1, x_right_edge, COLOR_EDGE_ACTIVE);
    }
    drawvline( x_right_edge, y + utox_scale(0 ), y + utox_scale(15  ), COLOR_EDGE_NORMAL);

    /* Draw the text and bars for A/V settings */
    setcolor(!button_settings_sub_av.mouseover ? COLOR_MAIN_TEXT : COLOR_MAIN_SUBTEXT);
    x = x_right_edge;
    x_right_edge = x_right_edge + utox_scale(5 ) + UTOX_STR_WIDTH(AUDIO_VIDEO) + utox_scale(400 ); /* stretch to end of window */
    drawstr( x + utox_scale(5 ), y + utox_scale(5 ), AUDIO_VIDEO);
    if (panel_settings_av.disabled) {
        drawhline( x, y + utox_scale(15 ), x_right_edge, COLOR_EDGE_NORMAL);
    } else {
        drawhline( x, y + 0, x_right_edge, COLOR_EDGE_ACTIVE);
        drawhline( x, y + 1, x_right_edge, COLOR_EDGE_ACTIVE);
    }
}

static void draw_background(int UNUSED(x), int UNUSED(y), int width, int height){
    /* Default background                */
    drawrect(0, 0, width, height, COLOR_BACKGROUND_MAIN);
    /* Friend list (roster) background   */
    drawrect(0, 0, SIDEBAR_WIDTH, height, COLOR_BACKGROUND_LIST);
    /* Current user badge background     */
    drawrect(0, 0, SIDEBAR_WIDTH, ROSTER_TOP, COLOR_BACKGROUND_MENU);

    if (!panel_chat.disabled){
        /* Top frame for main chat panel */
        drawrect (MAIN_LEFT, 0,                         width, MAIN_TOP_FRAME_THICK, COLOR_BACKGROUND_ALT);
        drawhline(MAIN_LEFT, MAIN_TOP_FRAME_THICK - 1,  width,                       COLOR_EDGE_NORMAL);
        /* Frame for the bottom chat text entry box */
        drawrect (MAIN_LEFT, height + CHAT_BOX_TOP,     width, height, COLOR_BACKGROUND_ALT);
        drawhline(MAIN_LEFT, height + CHAT_BOX_TOP,     width,         COLOR_EDGE_NORMAL);
    }
    // Chat and chat header separation
    if (panel_settings_master.disabled) {
        drawhline(MAIN_LEFT, MAIN_TOP_FRAME_THICK - 1, width, COLOR_EDGE_NORMAL);
    } else {
        drawhline(MAIN_LEFT, MAIN_TOP_FRAME_THIN  - 1, width, COLOR_EDGE_NORMAL);
    }
}


/* These remain for legacy reasons, PANEL_MAIN calls these by default when not given it's own function to call */
static void  background_draw(PANEL *UNUSED(p), int UNUSED(x), int UNUSED(y), int width, int height){ return; }
static _Bool background_mmove(PANEL *UNUSED(p), int UNUSED(x), int UNUSED(y), int UNUSED(width), int UNUSED(height), int UNUSED(mx), int UNUSED(my), int UNUSED(dx), int UNUSED(dy)) { return 0; }
static _Bool background_mdown(PANEL *UNUSED(p)) { return 0; }
static _Bool background_mright(PANEL *UNUSED(p)) { return 0; }
static _Bool background_mwheel(PANEL *UNUSED(p), int UNUSED(height), double UNUSED(d), _Bool UNUSED(smooth)) { return 0; }
static _Bool background_mup(PANEL *UNUSED(p)) { return 0; }
static _Bool background_mleave(PANEL *UNUSED(p)) { return 0; }

// Scrollbar or friend list
SCROLLABLE scrollbar_roster = {
    .panel = {
        .type = PANEL_SCROLLABLE,
    },
    .color = C_SCROLL,
    .x = 2,
    .left = 1,
    .small = 1,
},

// Scrollbar in chat window
scrollbar_friend = {
    .panel = {
        .type = PANEL_SCROLLABLE,
    },
    .color = C_SCROLL,
},

scrollbar_group = {
    .panel = {
        .type = PANEL_SCROLLABLE,
    },
    .color = C_SCROLL,
},

// Color is not used for settings
// @TODO
scrollbar_settings = {
    .panel = {
        .type = PANEL_SCROLLABLE,
    },
    .color = C_SCROLL,
};

/* */
MESSAGES messages_friend = {
    .panel = {
        .type = PANEL_MESSAGES,
        .content_scroll = &scrollbar_friend,
    }
},

messages_group = {
    .panel = {
        .type = PANEL_MESSAGES,
        .content_scroll = &scrollbar_group,
    },
    .type = 1
};

/* uTox panel draw hierarchy. */
PANEL panel_root,
        panel_side_bar,
            panel_self,
            panel_quick_buttons,
            panel_roster,
                panel_roster_list,
            panel_lower_buttons,
        panel_main,
            panel_chat,
                panel_group_chat,
                panel_friend_chat,
                panel_friend_request,
            panel_overhead,
                panel_profile_password,
                panel_add_friend,
                panel_settings_master,
                    panel_settings_subheader,
                    panel_settings_profile,
                    panel_settings_net,
                    panel_settings_ui,
                    panel_settings_av;

/* Root panel, hold all the other panels */
PANEL panel_root = {
    .type = PANEL_NONE,
    .drawfunc = draw_background,
    .disabled = 0,
    .child = (PANEL*[]) {
        &panel_side_bar, &panel_main,
        NULL
    }
},

/* Left side bar, holds the user, the roster, and the setting buttons */
panel_side_bar = {
    .type = PANEL_NONE,
    .disabled = 0,
    .child = (PANEL*[]) {
        &panel_self,
        &panel_quick_buttons,
        &panel_roster,
        NULL
    }
},
    /* The user badge and buttons */
    panel_self = {
        .type = PANEL_NONE,
        .disabled = 0,
        .drawfunc = draw_user_badge,
        .child = (PANEL*[]) {
            (void*)&button_avatar, (void*)&button_name,       (void*)&button_status,
                                   (void*)&button_statusmsg,
            NULL
        }
    },
    /* Left sided toggles */
    panel_quick_buttons = {
        .type = PANEL_NONE,
        .disabled = 0,
        .child = (PANEL*[]) {
            (void*)&button_filter_friends,
            //(void*)&button_create_group, TODO MOVE TO ROSTER
            (void*)&edit_search,
            (void*)&button_settings,
            (void*)&button_add_new_contact,
            NULL
        }
    },
    /* The friends and group was called list */
    panel_roster = {
        .type = PANEL_NONE,
        .disabled = 0,
        .child = (PANEL*[]) {
            // TODO rename these
            (void*)&panel_roster_list,
            (void*)&scrollbar_roster,
            NULL
        }
    },
        panel_roster_list = {
            .type = PANEL_LIST,
            .content_scroll = &scrollbar_roster,
        },
/* Main panel, holds the overhead/settings, or the friend/group containers */
panel_main = {
    .type = PANEL_NONE,
    .disabled = 0,
    .child = (PANEL*[]) {
        (void*)&panel_chat,
        (void*)&panel_overhead,
        NULL
    }
},
    /* Chat panel, friend or group, depending on what's selected */
    panel_chat = {
        .type = PANEL_NONE,
        .disabled = 1,
        .child = (PANEL*[]) {
            (void*)&panel_group_chat,
            (void*)&panel_friend_chat,
            (void*)&panel_friend_request,
            NULL
        }
    },
        panel_group_chat = {
            .type = PANEL_NONE,
            .disabled = 1,
            .drawfunc = draw_group,
            .child = (PANEL*[]) {
                (void*)&scrollbar_group,
                (void*)&edit_msg_group, // this needs to be one of the first, to get events before the others
                (void*)&messages_group,
                (void*)&button_group_audio,
                (void*)&button_chat_send,
                NULL
            }
        },
        panel_friend_chat ={
            .type = PANEL_NONE,
            .disabled = 1,
            .drawfunc = draw_friend,
            .child = (PANEL*[]) {
                (void*)&scrollbar_friend,
                (void*)&edit_msg, // this needs to be one of the first, to get events before the others
                (void*)&messages_friend,
                (void*)&button_call_audio, (void*)&button_call_video,
                (void*)&button_send_file, (void*)&button_send_screenshot, (void*)&button_chat_send,
                NULL
            }
        },
        panel_friend_request = {
            .type = PANEL_NONE,
            .disabled = 1,
            .drawfunc = draw_friend_request,
            .child = (PANEL*[]) {
                (void*)&button_accept_friend,
                NULL
            }
        },
    /* Settings master panel, holds the lower level settings */
    panel_overhead = {
        .type = PANEL_NONE,
        .disabled = 0,
        .child = (PANEL*[]) {
            (void*)&panel_profile_password,
            (void*)&panel_add_friend,
            (void*)&panel_settings_master,
            NULL
        }
    },
        panel_profile_password = {
            .type = PANEL_NONE,
            .disabled = 0,
            .drawfunc = draw_profile_password,
            .child = (PANEL*[]) {
                (void*)&edit_profile_password,
                NULL
            }
        },
        panel_add_friend = {
            .type = PANEL_NONE,
            .disabled = 1,
            .drawfunc = draw_add_friend,
            .child = (PANEL*[]) {
                (void*)&button_send_friend_request,
                (void*)&edit_add_id, (void*)&edit_add_msg,
                NULL
            }
        },
        panel_settings_master = {
            .type = PANEL_NONE,
            .disabled = 1,
            .drawfunc = draw_settings_header,
            .child = (PANEL*[]) {
                (void*)&panel_settings_subheader,
                NULL
            }
        },
            panel_settings_subheader = {
                .type = PANEL_NONE,
                .disabled = 0,
                .drawfunc = draw_settings_sub_header,
                .child = (PANEL*[]) {
                    (void*)&button_settings_sub_profile,
                    (void*)&button_settings_sub_net,
                    (void*)&button_settings_sub_ui,
                    (void*)&button_settings_sub_av,
                    (void*)&scrollbar_settings,
                    (void*)&panel_settings_profile,
                    (void*)&panel_settings_net,
                    (void*)&panel_settings_ui,
                    (void*)&panel_settings_av,
                    NULL
                }
            },

            /* Panel to draw settings page */
            panel_settings_profile = {
                .type = PANEL_NONE,
                .disabled = 0,
                .drawfunc = draw_settings_text_profile,
                .content_scroll = &scrollbar_settings,
                .child = (PANEL*[]) {
                    (void*)&edit_name,
                    (void*)&edit_status,
                    // Text: Tox ID
                    (void*)&edit_toxid,
                    (void*)&button_copyid,
                    // User's tox id
                    #ifdef EMOJI_IDS
                    (void*)&button_change_id_type,
                    #endif
                    (void*)&dropdown_language,
                    (void*)&edit_profile_password,
                    (void*)&button_lock_uTox,
                    NULL
                }
            },

            panel_settings_net = {
                .type = PANEL_NONE,
                /* Disabled by default, enabled by network button */
                .disabled = 1,
                .drawfunc = draw_settings_text_network,
                .content_scroll = &scrollbar_settings,
                .child = (PANEL*[]) {
                    (void*)&edit_proxy_ip,
                    (void*)&edit_proxy_port,
                    (void*)&dropdown_proxy,
                    (void*)&dropdown_ipv6,
                    (void*)&dropdown_udp,
                    NULL
                }
            },

            panel_settings_ui = {
                .type = PANEL_NONE,
                .drawfunc = draw_settings_text_ui,
                .disabled = 1,
                .content_scroll = &scrollbar_settings,
                .child = (PANEL*[]) {
                    (void*)&dropdown_dpi,
                    (void*)&dropdown_theme,
                    (void*)&dropdown_logging,
                    (void*)&dropdown_close_to_tray, (void*)&dropdown_start_in_tray,
                    (void*)&dropdown_auto_startup,
                    (void*)&dropdown_typing_notes,
                    NULL
                }
            },

            panel_settings_av = {
                .type = PANEL_NONE,
                .disabled = 1,
                .drawfunc = draw_settings_text_av,
                .content_scroll = &scrollbar_settings,
                .child = (PANEL*[]) {
                    (void*)&button_callpreview,
                    (void*)&dropdown_push_to_talk,
                    (void*)&button_videopreview,
                    (void*)&dropdown_audio_in,
                    (void*)&dropdown_audio_out,
                    (void*)&dropdown_video,
                    (void*)&dropdown_audible_notification,
                    (void*)&dropdown_audio_filtering,
                    NULL
                }
            };

void ui_set_scale(uint8_t scale) {
    if(ui_scale != scale) {
        ui_scale = scale;
    }

    list_scale();

    /* DEFAULT positions */
        panel_side_bar.x = 0;
        panel_side_bar.y = 0;
        panel_side_bar.width = SIDEBAR_WIDTH;

        scrollbar_roster.panel.y        = ROSTER_TOP;
        scrollbar_roster.panel.width    = MAIN_LEFT;
        scrollbar_roster.panel.height   = ROSTER_BOTTOM;

        panel_roster_list.x         = 0;
        panel_roster_list.y         = ROSTER_TOP;
        panel_roster_list.width     = MAIN_LEFT;
        panel_roster_list.height    = ROSTER_BOTTOM;

        panel_main.x = MAIN_LEFT;
        panel_main.y = 0;

        scrollbar_settings.panel.y        = utox_scale(16 );
        scrollbar_settings.content_height = utox_scale(150 );

        panel_settings_master.y  = MAIN_TOP_FRAME_THIN;
        panel_settings_profile.y = utox_scale(16 );
        panel_settings_net.y     = utox_scale(16 );
        panel_settings_ui.y      = utox_scale(16 );
        panel_settings_av.y      = utox_scale(16 );

        scrollbar_friend.panel.y        = MAIN_TOP;
        scrollbar_friend.panel.height   = CHAT_BOX_TOP;
        messages_friend.panel.y         = MAIN_TOP;
        messages_friend.panel.height    = CHAT_BOX_TOP - utox_scale(5 );
        messages_friend.panel.width     = -SCROLL_WIDTH;

        scrollbar_group.panel.y         = MAIN_TOP;
        scrollbar_group.panel.height    = CHAT_BOX_TOP;
        messages_group.panel.y          = MAIN_TOP;
        messages_group.panel.height     = CHAT_BOX_TOP;
        messages_group.panel.width      = -SCROLL_WIDTH;

    setscale_fonts();

    setfont(FONT_SELF_NAME);

    /* Button Structs  */
        /* Self badge box*/
        PANEL b_avatar = {
            .type   = PANEL_BUTTON,
            .x      = SIDEBAR_AVATAR_LEFT,
            .y      = SIDEBAR_AVATAR_TOP,
            .width  = BM_CONTACT_WIDTH,
            .height = BM_CONTACT_WIDTH,
        },

        b_name = {
            .type   = PANEL_BUTTON,
            .x      = SIDEBAR_NAME_LEFT,
            .y      = SIDEBAR_NAME_TOP,
            .width  = textwidth(self.name, self.name_length) - utox_scale(4 ),
            .height = SIDEBAR_NAME_HEIGHT - utox_scale(1 ),
        },

        b_statusmsg = {
            .type   = PANEL_BUTTON,
            .x      = SIDEBAR_STATUSMSG_LEFT,
            .y      = SIDEBAR_STATUSMSG_TOP,
            .width  = textwidth(self.statusmsg, self.statusmsg_length) - utox_scale(4 ),
            .height = SIDEBAR_STATUSMSG_HEIGHT - utox_scale(1 ),
        },

        b_status_button = {
            .type   = PANEL_BUTTON,
            .x      = SELF_STATUS_ICON_LEFT,
            .y      = SELF_STATUS_ICON_TOP,
            .width  = BM_STATUSAREA_WIDTH,
            .height = BM_STATUSAREA_HEIGHT,
        },

        /* Buttons */
        b_filter_friends = {
            .type   = PANEL_BUTTON,
            .y      = SIDEBAR_FILTER_FRIENDS_TOP,
            .x      = SIDEBAR_FILTER_FRIENDS_LEFT,
            .width  = SIDEBAR_FILTER_FRIENDS_WIDTH,
            .height = SIDEBAR_FILTER_FRIENDS_HEIGHT,
        },


        /* TODO MOVE THIS TO ROSTER */
        b_create_group = {
            .type   = PANEL_BUTTON,
            .y      = SIDEBAR_BUTTON_TOP,
            .x      = SIDEBAR_BUTTON_LEFT,
            .width  = SIDEBAR_BUTTON_WIDTH,
            .height = SIDEBAR_BUTTON_HEIGHT,
        },

        b_add_new_contact = {
            .type   = PANEL_BUTTON,
            .y      = ROSTER_BOTTOM,
            .x      = SIDEBAR_BUTTON_LEFT,
            .width  = SIDEBAR_BUTTON_WIDTH,
            .height = SIDEBAR_BUTTON_HEIGHT,
        },
        b_settings = {
            .type   = PANEL_BUTTON,
            .y      = ROSTER_BOTTOM,
            .x      = SIDEBAR_BUTTON_LEFT,
            .width  = SIDEBAR_BUTTON_WIDTH,
            .height = SIDEBAR_BUTTON_HEIGHT,
        },

        b_copyid = {
            .type = PANEL_BUTTON,
            .x = utox_scale(33),
            .y = utox_scale(53),
            .width = BM_SBUTTON_WIDTH,
            .height = BM_SBUTTON_HEIGHT,
        },

        /* setfont(FONT_SELF_NAME); needed for the next 4 buttons */
        b_settings_sub_profile = {
            .type   = PANEL_BUTTON,
            .x      = utox_scale(1  ),                           /* Nudged 1px as a buffer */
            .y      = utox_scale(1  ),
            .width  = utox_scale(9  ) + UTOX_STR_WIDTH(PROFILE), /* Nudged 1px as a buffer */
            .height = utox_scale(14 ),
        },

        b_settings_sub_net = {
            .type   = PANEL_BUTTON,
            .x      = utox_scale(11 ) + UTOX_STR_WIDTH(PROFILE), /* Nudged 1px as a buffer */
            .y      = utox_scale(1  ),
            .width  = utox_scale(9  ) + UTOX_STR_WIDTH(NETWORK), /* Nudged 1px as a buffer */
            .height = utox_scale(14 ),
        },

        b_settings_sub_ui = {
            .type   = PANEL_BUTTON,
            .x      = utox_scale(21 ) + UTOX_STR_WIDTH(PROFILE) + UTOX_STR_WIDTH(NETWORK), /* Nudged 1px as a buffer */
            .y      = utox_scale(1  ),
            .width  = utox_scale(9  ) + UTOX_STR_WIDTH(USER_INTERFACE),                    /* Nudged 1px as a buffer */
            .height = utox_scale(14 ),
        },

        b_settings_sub_av = {
            .type   = PANEL_BUTTON,
            .x      = utox_scale(31  ) + /* Nudged 1px as a buffer */
                      UTOX_STR_WIDTH(PROFILE) + UTOX_STR_WIDTH(NETWORK) + UTOX_STR_WIDTH(USER_INTERFACE),
            .y      = utox_scale(1   ),
            .width  = utox_scale(400 ), /* Fill the rest of the space for this button */
            .height = utox_scale(14  ),
        },

        #ifdef EMOJI_IDS
        b_change_id_type = {
            .type = PANEL_BUTTON,
            .x = utox_scale(80),
            .y = utox_scale(53),
            .width = BM_SBUTTON_WIDTH,
            .height = BM_SBUTTON_HEIGHT,
        },
        #endif

        b_send_friend_request = {
            .type = PANEL_BUTTON,
            .x = -utox_scale(5) - BM_SBUTTON_WIDTH,
            .y = MAIN_TOP + utox_scale(84),
            .width = BM_SBUTTON_WIDTH,
            .height = BM_SBUTTON_HEIGHT,
        },

        b_call_audio = {
            .type = PANEL_BUTTON,
            .x = -utox_scale(62 ),
            .y = utox_scale(5 ),
            .width = BM_LBUTTON_WIDTH,
            .height = BM_LBUTTON_HEIGHT,
        },

        b_call_video = {
            .type = PANEL_BUTTON,
            .x = -utox_scale(31 ),
            .y = utox_scale(5 ),
            .width = BM_LBUTTON_WIDTH,
            .height = BM_LBUTTON_HEIGHT,
        },

        b_group_audio = {
            .type = PANEL_BUTTON,
            .x = -utox_scale(31 ),
            .y = utox_scale(5 ),
            .width = BM_LBUTTON_WIDTH,
            .height = BM_LBUTTON_HEIGHT,
        },

        b_accept_friend = {
            .type = PANEL_BUTTON,
            .x = utox_scale(5),
            .y = MAIN_TOP + utox_scale(5),
            .width = BM_SBUTTON_WIDTH,
            .height = BM_SBUTTON_HEIGHT,
        },

        b_callpreview = {
            .type   = PANEL_BUTTON,
            .x      = utox_scale(5   ),
            .y      = utox_scale(125 ),
            .width  = BM_LBUTTON_WIDTH,
            .height = BM_LBUTTON_HEIGHT,
        },

        b_videopreview = {
            .type   = PANEL_BUTTON,
            .x      = utox_scale(35  ),
            .y      = utox_scale(125 ),
            .width  = BM_LBUTTON_WIDTH,
            .height = BM_LBUTTON_HEIGHT,
        },

        /* bottom left button in chat */
        b_send_file = {
            .type   = PANEL_BUTTON,
            .x      =   utox_scale(3 ),
            .y      = -utox_scale(23 ),
            .width  = BM_CHAT_BUTTON_WIDTH,
            .height = BM_CHAT_BUTTON_HEIGHT,
        },

        /* button to the right of b_chat_left */
        b_send_screenshot = {
            .type   = PANEL_BUTTON,
            .x      =   utox_scale(4 ) + BM_CHAT_BUTTON_WIDTH,
            .y      = -utox_scale(23 ),
            .width  = BM_CHAT_BUTTON_WIDTH,
            .height = BM_CHAT_BUTTON_HEIGHT,
        },

        b_chat_send = {
            .type   = PANEL_BUTTON,
            .x      =  -utox_scale(3 ) - BM_CHAT_SEND_WIDTH,
            .y      = -utox_scale(23 ),
            .width  = BM_CHAT_SEND_WIDTH,
            .height = BM_CHAT_SEND_HEIGHT,
        },

        b_lock_uTox = {
            .type   = PANEL_BUTTON,
            .x      =   utox_scale(5 ),
            .y      = utox_scale(125 ),
            .width  = BM_SBUTTON_WIDTH,
            .height = BM_SBUTTON_HEIGHT,
        };

    /* Set the button panels */
        button_avatar.panel = b_avatar;
        button_name.panel = b_name;
        button_statusmsg.panel = b_statusmsg;
        button_status.panel = b_status_button;

        button_filter_friends.panel = b_filter_friends;

        button_add_new_contact.panel = b_add_new_contact;
        button_settings.panel = b_settings;
        button_create_group.panel = b_create_group;

        button_copyid.panel = b_copyid;
        button_settings_sub_profile.panel = b_settings_sub_profile;
        button_settings_sub_net.panel = b_settings_sub_net;
        button_settings_sub_ui.panel = b_settings_sub_ui;
        button_settings_sub_av.panel = b_settings_sub_av;
        #ifdef EMOJI_IDS
        button_change_id_type.panel = b_change_id_type;
        #endif
        button_send_friend_request.panel = b_send_friend_request;
        button_call_audio.panel          = b_call_audio;
        button_call_video.panel          = b_call_video;
        button_group_audio.panel         = b_group_audio;
        button_accept_friend.panel       = b_accept_friend;
        button_callpreview.panel         = b_callpreview;
        button_videopreview.panel        = b_videopreview;
        button_send_file.panel           = b_send_file;
        button_send_screenshot.panel     = b_send_screenshot;
        button_chat_send.panel           = b_chat_send;
        button_lock_uTox.panel           = b_lock_uTox;

    /* Drop down structs */
        setfont(FONT_TEXT);

        PANEL d_notifications = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(15  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        },

        d_push_to_talk = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(60 ),
            .y      = utox_scale(15 ),
            .height = utox_scale(12 ),
            .width  = utox_scale(20 )
        },

        #ifdef AUDIO_FILTERING
        d_audio_filtering = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(120 ),
            .y      = utox_scale(15  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        },
        #endif

        d_audio_in = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(45  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(180 )
        },

        d_audio_out = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(70  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(180 )
        },

        d_video = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(95  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(180 )
        },

        d_dpi = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(75  ),
            .y      = utox_scale(15  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(100 )
        },

        d_language = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(84  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(100 )
        },

        d_proxy = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(40  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(60  )
        },

        d_ipv6 = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(24  ),
            .y      = utox_scale(13  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        },

        d_udp = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(74  ),
            .y      = utox_scale(13  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        },

        d_logging = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(39  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        },

        d_theme = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(15  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(60  )
        },

        d_close_to_tray = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(63  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        },

        d_start_in_tray = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ) + UTOX_STR_WIDTH(CLOSE_TO_TRAY) + utox_scale(10 ),
            .y      = utox_scale(63  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        },

        d_auto_startup = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(87  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        },

        d_typing_notes = {
            .type   = PANEL_DROPDOWN,
            .x      = utox_scale(5   ),
            .y      = utox_scale(114 ),
            .height = utox_scale(12  ),
            .width  = utox_scale(20  )
        };

    /* Drop down panels */
        dropdown_audio_in.panel = d_audio_in;
        dropdown_audio_out.panel = d_audio_out;
        dropdown_video.panel = d_video;
        dropdown_dpi.panel = d_dpi;
        dropdown_language.panel = d_language;
        dropdown_proxy.panel = d_proxy;
        dropdown_ipv6.panel = d_ipv6;
        dropdown_udp.panel = d_udp;
        dropdown_logging.panel = d_logging;
        dropdown_audible_notification.panel = d_notifications;
        dropdown_push_to_talk.panel = d_push_to_talk;
        dropdown_close_to_tray.panel = d_close_to_tray;
        dropdown_start_in_tray.panel = d_start_in_tray;
        dropdown_theme.panel = d_theme;
        dropdown_auto_startup.panel = d_auto_startup;

        #ifdef AUDIO_FILTERING
        dropdown_audio_filtering.panel = d_audio_filtering;
        #endif
        dropdown_typing_notes.panel = d_typing_notes;

    /* Text entry boxes */
        PANEL e_name = {
            .type   = PANEL_EDIT,
            .x      = utox_scale(5  ),
            .y      = utox_scale(14 ),
            .height = utox_scale(12 ),
            .width  = -utox_scale(5 )
        },

        e_status = {
            .type   = PANEL_EDIT,
            .x      = utox_scale(5  ),
            .y      = utox_scale(38 ),
            .height = utox_scale(12 ),
            .width  = -utox_scale(5 )
        },

        e_toxid = {
            .type   = PANEL_EDIT,
            .x      = utox_scale(3  ),
            .y      = utox_scale(63 ),
            .height = utox_scale(12 ),
            .width  = -utox_scale(5 )
        },

        e_add_id = {
            .type   = PANEL_EDIT,
            .x      = utox_scale(5  ),
            .y      = utox_scale(14 ) + MAIN_TOP,
            .height = utox_scale(12 ),
            .width  = -utox_scale(5 ),
        },

        e_add_msg = {
            .type   = PANEL_EDIT,
            .x      = utox_scale(5  ),
            .y      = utox_scale(38 ) + MAIN_TOP,
            .height = utox_scale(42 ),
            .width  = -utox_scale(5 ),
        },

        e_profile_password = {
            .type   = PANEL_EDIT,
            .x      = utox_scale(5  ),  /* move the edit depending on what page! */
            .y      = utox_scale(44 ) + (utox_scale(65 ) * panel_profile_password.disabled),
            .height = utox_scale(12 ),
            .width  = -utox_scale(5 ),
        },

        /* Message entry box for friends and groups */
        e_msg = {
            .type   = PANEL_EDIT,
            .x      =   utox_scale(5 ) + BM_CHAT_BUTTON_WIDTH * 2, /* Make space for the left button  */
            .y      = -utox_scale(23 ),
            .width  = -utox_scale(32 ),
            .height =  utox_scale(20 ),
            /* text is 8 high. 8 * 2.5 = 20. */
        },

        e_msg_group = {
            .type   = PANEL_EDIT,
            .x      =   utox_scale(3 ),
            .y      = -utox_scale(23 ),
            .width  = -utox_scale(10 ) - BM_CHAT_SEND_WIDTH,
            .height =  utox_scale(20 ),
        },

        e_search = {
            .type   = PANEL_EDIT,
            .y      = SIDEBAR_SEARCH_TOP,
            .x      = SIDEBAR_SEARCH_LEFT,
            .width  = SIDEBAR_SEARCH_WIDTH,
            .height = SIDEBAR_SEARCH_HEIGHT,
        },

        e_proxy_ip = {
            .type   = PANEL_EDIT,
            .x      = utox_scale(70 ),
            .y      = utox_scale(40 ),
            .height = utox_scale(12 ),
            .width  = utox_scale(60 )
        },

        e_proxy_port = {
            .type   = PANEL_EDIT,
            .x      = utox_scale(135 ),
            .y      = utox_scale(40  ),
            .height = utox_scale(12  ),
            .width  = utox_scale(30  )
        };

    /* Text entry panels */
        edit_name.panel = e_name;
        edit_status.panel = e_status;
        edit_toxid.panel = e_toxid;
        edit_add_id.panel = e_add_id;
        edit_add_msg.panel = e_add_msg;
        edit_profile_password.panel = e_profile_password;
        edit_msg.panel = e_msg;
        edit_msg_group.panel = e_msg_group;
        edit_search.panel = e_search;
        edit_proxy_ip.panel = e_proxy_ip;
        edit_proxy_port.panel = e_proxy_port;

    setscale();
}

/* Use the preprocessor to build function prototypes for all user interactions
 * These are functions that are (must be) defined elsewehere. The preprocessor in this case creates the prototypes that
 * will then be used by panel_draw_sub to call the correct function
*/
#define FUNC(x, ret, ...) static ret (* x##func[])(void* p, ##__VA_ARGS__) = { \
    (void*)background_##x, \
    (void*)messages_##x, \
    (void*)list_##x, \
    (void*)button_##x, \
    (void*)dropdown_##x, \
    (void*)edit_##x, \
    (void*)scroll_##x, \
};

FUNC(draw, void, int x, int y, int width, int height)
FUNC(mmove, _Bool, int x, int y, int width, int height, int mx, int my, int dx, int dy)
FUNC(mdown, _Bool)
FUNC(mright, _Bool)
FUNC(mwheel, _Bool, int height, double d)
FUNC(mup, _Bool)
FUNC(mleave, _Bool)

#undef FUNC

/* Use the preprocessor to add code to adjust the x,y cords for panels or sub panels.
 * If neg value place x/y from the right/bottom of panel.
 *
 * change the relative
 *
 * if w/h <0 use parent panel width (maybe?)    */
#define FIX_XY_CORDS_FOR_SUBPANELS() {\
    int relx = (p->x < 0) ? width + p->x : p->x;\
    int rely = (p->y < 0) ? height + p->y : p->y;\
    x += relx; \
    y += rely; \
    width = (p->width <= 0) ? width + p->width - relx : p->width; \
    height = (p->height <= 0) ? height + p->height - rely : p->height; }\

static void panel_update(PANEL *p, int x, int y, int width, int height)
{
    FIX_XY_CORDS_FOR_SUBPANELS();

    if(p->type == PANEL_MESSAGES) {
        MESSAGES *m = (void*)p;
        m->width = width;
        if(!p->disabled) {
            messages_updateheight(m);
        }
    }

    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            panel_update(subp, x, y, width, height);
        }
    }
}

void ui_size(int width, int height) {
    panel_update(&panel_root, 0, 0, width, height);
    tooltip_reset();
}

void ui_mouseleave(void)
{
    panel_mleave(&panel_root);
    tooltip_reset();
    redraw();
}

static void panel_draw_sub(PANEL *p, int x, int y, int width, int height)
{
    FIX_XY_CORDS_FOR_SUBPANELS();

    if (p->content_scroll) {
        pushclip(x, y, width, height);
        y -= scroll_gety(p->content_scroll, height);
    }

    if (p->type) {
        drawfunc[p->type - 1](p, x, y, width, height);
    } else {
        if (p->drawfunc) {
            p->drawfunc(x, y, width, height);
        }
    }

    PANEL **pp = p->child, *subp;
    if (pp) {
        while((subp = *pp++)) {
            if (!subp->disabled) {
                panel_draw_sub(subp, x, y, width, height);
            }
        }
    }

    if (p->content_scroll) {
        popclip();
    }
}

void panel_draw(PANEL *p, int x, int y, int width, int height)
{
    FIX_XY_CORDS_FOR_SUBPANELS();

    //pushclip(x, y, width, height);

    if(p->type) {
        drawfunc[p->type - 1](p, x, y, width, height);
    } else {
        if(p->drawfunc) {
            p->drawfunc(x, y, width, height);
        }
    }

    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                panel_draw_sub(subp, x, y, width, height);
            }
        }
    }

    //popclip();

    dropdown_drawactive();
    contextmenu_draw();
    tooltip_draw();

    enddraw(x, y, width, height);
}

_Bool panel_mmove(PANEL *p, int x, int y, int width, int height, int mx, int my, int dx, int dy)
{
    if (p == &panel_root) {
        mouse.x = mx;
        mouse.y = my;
    }

    mx -= (p->x < 0) ? width + p->x : p->x;
    my -= (p->y < 0) ? height + p->y : p->y;
    FIX_XY_CORDS_FOR_SUBPANELS();

    int mmy = my;

    if(p->content_scroll) {
        int scroll_y = scroll_gety(p->content_scroll, height);
        if(my < 0) {
            mmy = -1;
        } else if (my >= height) {
            mmy = 1024 * 1024 * 1024;//large value
        } else {
            mmy = my + scroll_y;
        }
        y -= scroll_y;
        my += scroll_y;
    }

    _Bool draw = p->type ? mmovefunc[p->type - 1](p, x, y, width, height, mx, mmy, dx, dy) : 0;
    // Has to be called before children mmove
    if (p == &panel_root) {
        draw |= tooltip_mmove();
    }
    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                draw |= panel_mmove(subp, x, y, width, height, mx, my, dx, dy);
            }
        }
    }

    if (p == &panel_root) {
        draw |= contextmenu_mmove(mx, my, dx, dy);
        if(draw) {
            redraw();
        }
    }

    return draw;
}

static _Bool panel_mdown_sub(PANEL *p)
{
    if(p->type && mdownfunc[p->type - 1](p)) {
        return 1;
    }

    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                if(panel_mdown_sub(subp)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

void panel_mdown(PANEL *p)
{
    if(contextmenu_mdown() || tooltip_mdown()) {
        redraw();
        return;
    }

    _Bool draw = edit_active();
    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                if(panel_mdown_sub(subp)) {
                    draw = 1;
                    break;
                }
            }
        }
    }

    if(draw) {
        redraw();
    }
}

_Bool panel_dclick(PANEL *p, _Bool triclick)
{
    _Bool draw = 0;
    if(p->type == PANEL_EDIT) {
        draw = edit_dclick((EDIT*)p, triclick);
    } else if(p->type == PANEL_MESSAGES) {
        draw = messages_dclick((MESSAGES*)p, triclick);
    }

    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                draw = panel_dclick(subp, triclick);
                if(draw) {
                    break;
                }
            }
        }
    }

    if ( draw && p == &panel_root ) {
        redraw();
    }

    return draw;
}

_Bool panel_mright(PANEL *p)
{
    _Bool draw = p->type ? mrightfunc[p->type - 1](p) : 0;
    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                draw |= panel_mright(subp);
            }
        }
    }

    if ( draw && p == &panel_root ) {
        redraw();
    }

    return draw;
}

_Bool panel_mwheel(PANEL *p, int x, int y, int width, int height, double d, _Bool smooth)
{
    FIX_XY_CORDS_FOR_SUBPANELS();

    _Bool draw = p->type ? mwheelfunc[p->type - 1](p, height, d) : 0;
    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                draw |= panel_mwheel(subp, x, y, width, height, d, smooth);
            }
        }
    }

    if ( draw && p == &panel_root ) {
        redraw();
    }

    return draw;
}

_Bool panel_mup(PANEL *p)
{
    _Bool draw = p->type ? mupfunc[p->type - 1](p) : 0;
    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                draw |= panel_mup(subp);
            }
        }
    }

    if (p == &panel_root) {
        draw |= contextmenu_mup();
        tooltip_mup();
        if(draw) {
            redraw();
        }
    }

    return draw;
}

_Bool panel_mleave(PANEL *p)
{
    _Bool draw = p->type ? mleavefunc[p->type - 1](p) : 0;
    PANEL **pp = p->child, *subp;
    if(pp) {
        while((subp = *pp++)) {
            if(!subp->disabled) {
                draw |= panel_mleave(subp);
            }
        }
    }

    if (p == &panel_root) {
        draw |= contextmenu_mleave();
        if(draw) {
            redraw();
        }
    }

    return draw;
}
