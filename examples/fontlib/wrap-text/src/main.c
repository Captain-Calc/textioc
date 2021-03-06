#include <graphx.h>
#include <fontlibc.h>
#include <textioc.h>

#include "fonts/fonts.h"


// Debugging
#include <stdio.h>
#define dbgout ((char*)0xFB0000)
#define dbgerr ((char*)0xFC0000)
#define dbg_sprintf sprintf


void setup_fontlib_textio(void) {

	/* Setup the FontLib wrapper. */
	textio_library_routines_t routines = TEXTIO_FONTLIB_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

void print_text(char *text, uint24_t xPos, uint8_t yPos, uint24_t max_line_width) {
	
	char *curr_line, *next_line, *curr_char;
	uint8_t line_spacing = 13;
	textio_output_data_t data = TEXTIO_FONTLIB_OUTPUT_DATA;
	textio_output_data_t *output_data = &data;
	
	curr_line = text;

	for (;;) {
		
		// It is important to remember that textio_GetLineWidth() retrieves the width of all characters between line and eol, INCLUSIVE.
		next_line = textio_GetLinePtr(output_data, curr_line, 1);
		if (curr_line == next_line)
			return;
		
		if (output_data->print_format == TEXTIO_FORMAT_RIGHT_MARGIN_FLUSH) {
			fontlib_SetCursorPosition(max_line_width - xPos - textio_GetLineWidth(output_data, curr_line, next_line - 1), yPos);
		} else if (output_data->print_format == TEXTIO_FORMAT_CENTERED) {
			fontlib_SetCursorPosition((max_line_width - xPos - textio_GetLineWidth(output_data, curr_line, next_line - 1)) / 2, yPos);
		} else {
			fontlib_SetCursorPosition(xPos, yPos);
		};
		
		curr_char = curr_line;
		while (curr_char < next_line) {
			if (*curr_char == '\t') {
				fontlib_DrawString("    ");
			} else if (*curr_char != '\n') {
				fontlib_DrawGlyph(*curr_char);
			};
			curr_char++;
		};
		curr_line = next_line;
		yPos += line_spacing;
		if (yPos > 240 - line_spacing)
			return;
	};
}

void main(void) {
	
	char text[] = {"\tThis line starts with a tab. The dimensions of the highlighted window are 140 pixels wide by 240 pixels tall. The initial text position is (0, 0). When the text reaches the bottom of the window, any text that will not fit will be truncated."};

	/* Start the graphics */
	gfx_Begin();
	
	/* Setup source library. */
	setup_fontlib_textio();
	fontlib_SetFont(test_font, 0);
	fontlib_SetWindow(0, 0, 140, 240);
	
	/* Print the text */
	print_text(text, 0, 5, 140);
	
	/* Outline the window. */
	gfx_SetColor(224);
	gfx_Rectangle_NoClip(0, 0, 140, 240);
	
	/* Wait for keypress */
	while (!os_GetCSC());
	
	/* Close the graphics */
	gfx_End();
	exit(0);
}