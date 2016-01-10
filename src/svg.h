/* Scroll bar rounded corners top and bottom */
#define  BM_SCROLLHALF_WIDTH SCROLL_WIDTH
#define  BM_SCROLLHALF_HEIGHT (SCROLL_WIDTH / 2)
/* No idea */
#define  BM_STATUSAREA_WIDTH (utox_scale(10 ))
#define  BM_STATUSAREA_HEIGHT (utox_scale(20 ))
/* Text button on the add a new friend page */
#define _BM_ADD_WIDTH           9
#define  BM_ADD_WIDTH           (utox_scale(9 ))
#define  BM_STATUS_WIDTH        (utox_scale(5 ))
#define  BM_STATUS_NOTIFY_WIDTH (utox_scale(7 ))
#define  BM_NMSG_WIDTH          (utox_scale(9 ))
/* Search and buttons toggle switch */
#define _BM_THREE_BAR_WIDTH 14
#define  BM_THREE_BAR_WIDTH (utox_scale(14 ))
/* Standard large size button */
#define  BM_LBUTTON_WIDTH  (utox_scale(26 ))
#define  BM_LBUTTON_HEIGHT (utox_scale(20 ))
/* Standard small size button */
#define  BM_SBUTTON_WIDTH  (utox_scale(26 ))
#define  BM_SBUTTON_HEIGHT (utox_scale(10 ))
/* File transfer buttons */
#define  BM_FT_WIDTH   (utox_scale(125 ))
#define  BM_FT_HEIGHT  (utox_scale(26 ))
#define  BM_FTM_WIDTH  (utox_scale(113 ))
#define  BM_FTB_WIDTH  (utox_scale(13 ))
#define  BM_FTB_HEIGHT (utox_scale(14 ))
#define  BM_FT_CAP_WIDTH (utox_scale(15 ))
/* something to do with contacts? */
#define  BM_CONTACT_WIDTH (utox_scale(20 ))
/* no idea */
#define _BM_LBICON_WIDTH  11
#define  BM_LBICON_WIDTH (utox_scale(11 ))
#define _BM_LBICON_HEIGHT 10
#define  BM_LBICON_HEIGHT (utox_scale(10 ))
/* small file transfer button maybe? */
#define  BM_FB_WIDTH      (utox_scale(6 ))
#define  BM_FB_HEIGHT     (utox_scale(5 ))

/* small button placements */
#define  BM_CHAT_BUTTON_WIDTH          (utox_scale(20 ))
#define  BM_CHAT_BUTTON_HEIGHT         (utox_scale(20 ))
/* camera box */
#define _BM_CHAT_BUTTON_OVERLAY_WIDTH  (14        )
#define  BM_CHAT_BUTTON_OVERLAY_WIDTH  (utox_scale(14 ))
#define _BM_CHAT_BUTTON_OVERLAY_HEIGHT (14        )
#define  BM_CHAT_BUTTON_OVERLAY_HEIGHT (utox_scale(14 ))
/* Large chat button */
#define  BM_CHAT_SEND_WIDTH    (utox_scale(28 ))
#define  BM_CHAT_SEND_HEIGHT   (utox_scale(20 ))
/* Chat speech bubble */
#define _BM_CHAT_SEND_OVERLAY_WIDTH  (20        )
#define  BM_CHAT_SEND_OVERLAY_WIDTH  (utox_scale(20 ))
#define _BM_CHAT_SEND_OVERLAY_HEIGHT (16        )
#define  BM_CHAT_SEND_OVERLAY_HEIGHT (utox_scale(16 ))

#define _BM_FILE_WIDTH      (11        )
#define  BM_FILE_WIDTH      (utox_scale(11 ))
#define _BM_FILE_HEIGHT     (10        )
#define  BM_FILE_HEIGHT     (utox_scale(10 ))

#define _BM_FILE_BIG_WIDTH  (22        )
#define  BM_FILE_BIG_WIDTH  (utox_scale(22 ))
#define _BM_FILE_BIG_HEIGHT (20        )
#define  BM_FILE_BIG_HEIGHT (utox_scale(20 ))

#define _BM_CI_WIDTH 10
#define  BM_CI_WIDTH (utox_scale(10 ))

void *svg_data;

_Bool svg_draw(_Bool needmemory);
