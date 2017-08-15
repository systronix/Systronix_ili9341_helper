#ifndef SALT_DEBUG_DISPLAY_H_
#define SALT_DEBUG_DISPLAY_H_

//---------------------------< I N C L U D E S >--------------------------------------------------------------

#include <Arduino.h>
#include <SALT.h>
#include <ILI9341_t3.h>
#include <font_Arial.h> 			// from ILI9341_t3

//---------------------------< D E F I N E S >----------------------------------------------------------------

#define	LINE_X_START	5
#define	LINE_00_Y		5		// top line of display at cap height is 5px below top edge of display
#define	LINE_HEIGHT		18		// give leading of 2-3px (2 for pipe character, 3 for all other descenders)
#define	MAX_LINES		13		// we can only display this many lines
#define	LINE_LEN		48		// 32 chars including null terminator; writing 32 uppercase characters will overrun the display (display will wrap)


class Systronix_ili9341_helper
	{
	private:
	protected:

		struct text_line
			{
			uint8_t		x;				// line begins at x pixels away from left edge of display; for this test default is 5px
			uint8_t		y;				// at cap height, top line of the display is at (5px, 5px)
			uint16_t	color;			// default is ILI9341_CYAN
			char		text[LINE_LEN];	// for simplicity, each null terminated text string shall be no-longer than 32 chars
			};

		struct
			{
			uint8_t		top_index;		// index into screen_line[] array for first or top line displayed on screen
			uint8_t		bot_index;		// index into screen_line[] array for last or bottom line displayed on screen
			uint8_t		last_index;		// index of the last line written
			uint8_t		line_count;		// number of lines displayed
			text_line	screen_line [MAX_LINES];	// array of text_line structs
			} screen;

		uint8_t	next_index_get (uint8_t old_index);
		uint8_t	scroll_display (uint8_t count);


	public:
		void	display_line_write (const char* text, uint16_t color);
		void	display_line_rewrite (const char* text, uint16_t color);
		void	screen_clear (void);
	};

extern	Systronix_ili9341_helper display;
#endif	// SALT_DEBUG_DISPLAY_H_