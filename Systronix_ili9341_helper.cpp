// debug display.cpp
// simple debug display page handling
//
// TODO: make this into a real class?
//
// for Arial_12 font:
// y line position is at cap height above baseline; descenders are -4px from baseline (pipe, -3 for g, j, p, q, y
// and brackets, braces and parens; baseline is -12 from cap height;
// ascenders = cap height; total word height is 12px+3px = 15px pixels (16 px for pipes)
// For this program, 18px per line gives a leading of 2 or 3 px between lines
// rightmost x pixels are at 317


//---------------------------< I N C L U D E S >--------------------------------------------------------------

#include <SALT_debug_display.h>

ILI9341_t3 tft = ILI9341_t3 (DISP_CS_PIN, DH_CL_PIN, PERIPH_RST, MOSI_PIN, SCK_PIN, MISO_PIN);


//---------------------------< N E X T _ I N D E X _ G E T >--------------------------------------------------
//
// Essentially an up-only counter that wraps to zero when an increment would send it beyond the bounds set by
// MAX_LINES.  indexes are 0 based so must never be set to a value equal to or greater than MAX_LINES.
//

uint8_t SALT_debug_display::next_index_get (uint8_t old_index)
	{
	if (MAX_LINES <= 1 + old_index)				// an increment makes this index >= MAX_LINE
		return 0;								// so make it zero
	else
		return 1 + old_index;					// else bump the index
	}


//---------------------------< S C R O L L _ D I S P L A Y >--------------------------------------------------
//
// Scrolls the display up by one text line.  The intent is to allow scrolling by n lines but sonly scrolling
// by 1 line has been implemented and tested so count is forced to 1 regardless of how this function is called.
//
// leaves screen.bot_index pointing at the next available location
//

uint8_t SALT_debug_display::scroll_display (uint8_t count)
	{
	uint8_t line;
	uint8_t	index;
	uint8_t next_index;

	count = 1;																// force single line scrolling only
	
	if (MAX_LINES < count)													// essentially disabled
		return 0xFF;														// TODO: reset count to MAX_LINES

	for (uint8_t i=0; i<count; i++)											// for when we allow multi-line scrolling
		{
		index = screen.top_index;											// get index of top line
		for (line=0; line<screen.line_count; line++)						// for each line in the display (this only tested with full display)
			{
			next_index = next_index_get (index);							// point to line that will become the next top line
			
			tft.setCursor(LINE_X_START, LINE_00_Y + (LINE_HEIGHT * line));	// position at start of line
			tft.setTextColor (ILI9341_BLACK);								// same as background
			tft.printf ("%s", screen.screen_line[index].text);				// overwrite in background color to erase the line

			if (line >= (screen.line_count -1))								// line is 0 based, screen.line_count is 1 based; one of them must be adjusted
				{															// if at the bottom of the display, last line has been blanked
				screen.line_count -= (0 == screen.line_count) ? 1 : 0;		// NOT TESTED; so reset the line counter
				if (0 == screen.line_count)									// NOT TESTED; when last line scrolls off of the display
					screen.bot_index = screen.top_index;					// NOT TESTED; set indexes to point at the same location (same as starting with top = bot = 0)
				break;														// done; there is now a blank space for new text
				}
																			// scroll next-to-top into the top line
			tft.setCursor(LINE_X_START, LINE_00_Y + (LINE_HEIGHT * line));	// reposition at start of line
			tft.setTextColor (screen.screen_line[next_index].color);		// set new text color
			tft.printf ("%s", screen.screen_line[next_index].text);			// write new text to the display

			index = next_index;												// loop back to erase and rewrite the next line
			}

		screen.top_index = next_index_get (screen.top_index);				// reset the global top index
		}

	return SUCCESS;
	}


//---------------------------< D I S P L A Y _ L I N E _ W R I T E >------------------------------------------
//
// adds a line to the screen struct and writes it to the display; leaves screen.bot_index pointing to next
// location in the struct
//

void SALT_debug_display::display_line_write (const char* text, uint16_t color)
	{
	if (MAX_LINES <= screen.line_count)										// if the array is full
		{
		screen.line_count = MAX_LINES;										// make sure no more than MAX_LINES of text
		scroll_display (1);													// scroll up 1 line
		screen.line_count = MAX_LINES-1;									// after a scroll there is one less line
		}

	strcpy (screen.screen_line[screen.bot_index].text, text);				// add new line
	screen.screen_line[screen.bot_index].color = color;						// and its color
	screen.screen_line[screen.bot_index].x = LINE_X_START;					// save the line's cursor position
	screen.screen_line[screen.bot_index].y = LINE_00_Y + (LINE_HEIGHT * screen.line_count);
	
	tft.setCursor(screen.screen_line[screen.bot_index].x, screen.screen_line[screen.bot_index].y);	// position at start of line
	tft.setTextColor (screen.screen_line[screen.bot_index].color);			// set new text color
	tft.printf ("%s", screen.screen_line[screen.bot_index].text);			// write new text

	screen.last_index = screen.bot_index;									// save this for possible rewrite
	screen.bot_index = next_index_get (screen.bot_index);					// set new bottom line index to point at next available location
	screen.line_count = (MAX_LINES <= screen.line_count) ? MAX_LINES : 1 + screen.line_count;	// do not over count but tally this new line
	}


//---------------------------< D I S P L A Y _ L I N E _ R E W R I T E >--------------------------------------
//
// rewrites the last displayed line in the screen struct and writes it to the display; leaves screen.bot_index
// unchanged (pointing to next location in the struct)
//

void SALT_debug_display::display_line_rewrite (const char* text, uint16_t color)
	{
	tft.setCursor(screen.screen_line[screen.last_index].x, screen.screen_line[screen.last_index].y);	// position at start of line
	tft.setTextColor (ILI9341_BLACK);										// same as background
	tft.printf ("%s", screen.screen_line[screen.last_index].text);			// overwrite in background color to erase the line

	strcpy (screen.screen_line[screen.last_index].text, text);				// reset to new text
	screen.screen_line[screen.last_index].color = color;						// reset its color
	
	tft.setCursor(screen.screen_line[screen.last_index].x, screen.screen_line[screen.last_index].y);	// position at start of line
	tft.setTextColor (screen.screen_line[screen.last_index].color);			// set new text color
	tft.printf ("%s", screen.screen_line[screen.last_index].text);			// write new text
	}


//---------------------------< S C R E E N _ C L E A R >------------------------------------------------------
//
// blank the screen and reset the screen struct
//

void SALT_debug_display::screen_clear (void)
	{
	screen.top_index = 0;
	screen.bot_index = 0;
	screen.line_count = 0;
	
	for (uint8_t line=0; line<MAX_LINES; line++)
		{
		screen.screen_line[line].x = 0;							// these two, x & y, not currently used
		screen.screen_line[line].y = 0;

		screen.screen_line[line].color = ILI9341_BLACK;			// same as 0
		
		memset (screen.screen_line[line].text, '\0', LINE_LEN);	// fill the the text with zeros
		}
	tft.fillScreen(ILI9341_BLACK);								// and write the display black	
	}
