
// This file contains terminal escape codes under the ansi standard.
// You can use this to have finer control over the text and it's layout in a terminal.

// -------------------------------------------------------------------------------------------------
// C0 Control codes
#define TERM_ESC "\e"
#define TERM_BEL "\a"
#define TERM_BACKSPACE "\b"
#define TERM_TAB "\t"
#define TERM_LINE_FEED "\n"
#define TERM_FORM_FEED "\f"
#define TERM_CARRIAGE_RETURN "\r"

// -------------------------------------------------------------------------------------------------
// FE Escape Codes
#define TERM_SS2 TERM_ESC "N"
#define TERM_SS3 TERM_ESC "O"
#define TERM_DCS TERM_ESC "P"
#define TERM_CSI TERM_ESC "["
#define TERM_ST  TERM_ESC "\\"
#define TERM_OSC TERM_ESC "]"

// -------------------------------------------------------------------------------------------------
// CSI Commands

// Device Status Report || Get Cursor Position as `ESC[n;mR` where n == row, m == col
#define TERM_GET_CURSOR TERM_CSI "6n"

// Cursor Move
#define TERM_CURSOR_UP(n)                  TERM_CSI stringify(n) "A"
#define TERM_CURSOR_DOWN(n)                TERM_CSI stringify(n) "B"
#define TERM_CURSOR_FORWARD(n)             TERM_CSI stringify(n) "C"
#define TERM_CURSOR_BACK(n)                TERM_CSI stringify(n) "D"
#define TERM_CURSOR_NEXT_LINE(n)           TERM_CSI stringify(n) "E"
#define TERM_CURSOR_PREVIOUS_LINE(n)       TERM_CSI stringify(n) "F"
#define TERM_CURSOR_HORIZONTAL_ABSOLUTE(n) TERM_CSI stringify(n) "G"
#define TERM_CURSOR_POSITION(row, col)     TERM_CSI stringify(row); stringify(col) "H"

#define TERM_ERASE_IN_DISPLAY(erase_code)           TERM_CSI stringify(erase_code) "J"
#define TERM_ERASE_FROM_CURSOR_TO_END_DISPLAY       TERM_ERASE_IN_DISPLAY(0)
#define TERM_ERASE_FROM_CURSOR_TO_BEGINNING_DISPLAY TERM_ERASE_IN_DISPLAY(1)
#define TERM_CLEAR_ENTIRE_SCREEN                    TERM_ERASE_IN_DISPLAY(2)
#define TERM_CLEAR_ENTIRE_SCREEN_AND_SCROLLBACK     TERM_ERASE_IN_DISPLAY(3)

// 0 clear from cursor to end of line
// 1 clear from cursor to beginning of line
// 2 clear entire line
#define TERM_ERASE_IN_LINE(erase_code)           TERM_CSI stringify(erase_code) "K"
#define TERM_ERASE_FROM_CURSOR_TO_END_LINE       TERM_ERASE_IN_LINE(0)
#define TERM_ERASE_FROM_CURSOR_TO_BEGINNING_LINE TERM_ERASE_IN_LINE(1)
#define TERM_ERASE_ENTIRE_LINE                   TERM_ERASE_IN_LINE(2)

#define CSI_SCROLL_UP(n)   TERM_CSI stringify(n) "S"
#define CSI_SCROLL_DOWN(n) TERM_CSI stringify(n) "T"

// Acts like CSI_CURSOR_POSITON but with different handling in certain terminal modes.
#define TERM_HORIZONTAL_VERTICAL_POSITION(row, col) TERM_CSI stringify(row); stringify(col) "f"

#define TERM_AUX_PORT_ON TERM_CSI "5i"
#define TERM_AUX_PORT_OFF TERM_CSI "4i"

// -------------------------------------------------------------------------------------------------
// SGR[Select Graphic Rendition] Sets colors and style of characters

// Colors
#define TERM_STYLE_RESET TERM_CSI "0" "m"

#define TC_END TERM_STYLE_RESET
#define TC(r, g, b) TERM_FOREGROUND_RGB(r, g, b)
#define TC_WHITE TC(255,255,255)

#define TERM_FOREGROUND_RGB(r, g, b) TERM_CSI "38;2;"stringify(r)";"stringify(g)";"stringify(b) "m"
#define TERM_BACKGROUND_RGB(r, g, b) TERM_CSI "48;2;"stringify(r)";"stringify(g)";"stringify(b) "m"
#define TERM_DEFAULT_FOREGROUND TERM_CSI "39" "m"
#define TERM_DEFAULT_BACKGROUND TERM_CSI "49" "m"

#define TERM_UNDERLINE_RGB(r, g, b) TERM_CSI "58;2;"stringify(r)";"stringify(g)";"stringify(b) "m"
#define TERM_DEFAULT_UNDERLINE TERM_CSI "59" "m"

// Attributes
#define TERM_BOLD                          TERM_CSI "1" "m"
#define TERM_FAINT                         TERM_CSI "2" "m"
#define TERM_ITALIC                        TERM_CSI "2" "m"
#define TERM_UNDERLINE                     TERM_CSI "4" "m"
#define TERM_SLOW_BLINK                    TERM_CSI "5" "m"
#define TERM_RAPID_BLINK                   TERM_CSI "6" "m"
#define TERM_REVERSE_FOREGROUND_BACKGROUND TERM_CSI "7" "m"
#define TERM_CONCEAL                       TERM_CSI "8" "m" // Rarely supported
#define TERM_CROSS_CONCEAL                 TERM_CSI "9" "m"

#define TERM_PRIMARY_FONT TERM_CSI "10" "m"
#define TERM_ALT_FONT_1   TERM_CSI "11" "m"
#define TERM_ALT_FONT_2   TERM_CSI "12" "m"
#define TERM_ALT_FONT_3   TERM_CSI "13" "m"
#define TERM_ALT_FONT_4   TERM_CSI "14" "m"
#define TERM_ALT_FONT_5   TERM_CSI "15" "m"
#define TERM_ALT_FONT_6   TERM_CSI "16" "m"
#define TERM_ALT_FONT_7   TERM_CSI "17" "m"
#define TERM_ALT_FONT_8   TERM_CSI "18" "m"
#define TERM_ALT_FONT_9   TERM_CSI "19" "m"

#define TERM_GOTHIC                           TERM_CSI "20" "m" // Rarely supported
#define TERM_DOUBLE_UNDERLINE                 TERM_CSI "21" "m"
#define TERM_NORMAL_INTENSITY                 TERM_CSI "22" "m"
#define TERM_NO_ITALIC_OR_BLACKLETTER         TERM_CSI "23" "m"
#define TERM_NO_UNDERLINE                     TERM_CSI "24" "m"
#define TERM_NO_BLINKING                      TERM_CSI "25" "m"
#define TERM_PROPORTINAL_SPACING              TERM_CSI "26" "m"
#define TERM_NO_REVERSE_FOREGROUND_BACKGROUND TERM_CSI "27" "m"
#define TERM_NO_CONCEAL                       TERM_CSI "28" "m"
#define TERM_NO_REVEAL                        TERM_CSI "29" "m"

#define TERM_FOREGROUND_1 TERM_CSI "30" "m"
#define TERM_FOREGROUND_2 TERM_CSI "31" "m"
#define TERM_FOREGROUND_3 TERM_CSI "32" "m"
#define TERM_FOREGROUND_4 TERM_CSI "33" "m"
#define TERM_FOREGROUND_5 TERM_CSI "34" "m"
#define TERM_FOREGROUND_6 TERM_CSI "35" "m"
#define TERM_FOREGROUND_7 TERM_CSI "36" "m"
#define TERM_FOREGROUND_8 TERM_CSI "37" "m"

#define TERM_BACKGROUND_1 TERM_CSI "40" "m"
#define TERM_BACKGROUND_2 TERM_CSI "41" "m"
#define TERM_BACKGROUND_3 TERM_CSI "42" "m"
#define TERM_BACKGROUND_4 TERM_CSI "43" "m"
#define TERM_BACKGROUND_5 TERM_CSI "44" "m"
#define TERM_BACKGROUND_6 TERM_CSI "45" "m"
#define TERM_BACKGROUND_7 TERM_CSI "46" "m"
#define TERM_BACKGROUND_8 TERM_CSI "47" "m"

#define TERM_NO_PROPORTIONAL_SPACING TERM_CSI "50" "m"
#define TERM_FRAMED                  TERM_CSI "51" "m"
#define TERM_ENCIRCLED               TERM_CSI "52" "m"
#define TERM_OVERLINED               TERM_CSI "53" "m"
#define TERM_NO_ENFRAME_OR_ENCIRCLE  TERM_CSI "54" "m"
#define TERM_NO_OVERLINED            TERM_CSI "55" "m"

//      Rarely supported
#define TERM_IDOEOGRAM_RIGHT_SIDE_LINE        TERM_CSI "60" "m"
#define TERM_IDOEOGRAM_DOUBLE_RIGHT_SIDE_LINE TERM_CSI "61" "m"
#define TERM_IDOEOGRAM_LEFT_SIDE_LINE         TERM_CSI "62" "m"
#define TERM_IDOEOGRAM_DOUBLE_LEFT_SIDE_LINE  TERM_CSI "63" "m"
#define TERM_IDOEOGRAM_STRESS_MARKING         TERM_CSI "64" "m"
#define TERM_NO_IDEOGRAM                      TERM_CSI "65" "m"

// -------------------------------------------------------------------------------------------------
// Save && Restore Cursor
#define TERM_SAVE_CURSOR    TERM_ESC "7"
#define TERM_RESTORE_CURSOR TERM_ESC "8"


// -------------------------------------------------------------------------------------------------
// Resources
// [https://en.wikipedia.org/wiki/ANSI_escape_code]

