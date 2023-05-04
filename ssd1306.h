#include "stm32f1xx_hal.h"
#include "font.h"

extern I2C_HandleTypeDef hi2c1;

typedef struct {
	uint16_t x;
	uint16_t y;
}ssd1306_Cursor;


#define ssd1306_port hi2c1
#define ssd1306_addr (0x3C << 1)

#define ssd1306_WIDTH  128
#define ssd1306_HEIGHT 64
#define BUFFER_SIZE 		ssd1306_WIDTH * ssd1306_HEIGHT / 8

#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0

//---- SET DISPLAY
#define ON 0xAF
#define OFF 0xAE
//----

// --- ENTIRE DISPLAY
#define ENTIRE_1 0xA4
#define ENTIRE_2 0xA5
//----

//------ MODE HOW PIXELS BE RENDERING ------
#define HORIZONTAL 0x00
#define VERTICAL 0x01
#define ADDRESSING 0x10
//----

//--- SET COLUMN ADDRESS ----
#define LOW_COLUMN 0x00
#define HIGH_COLUMN 0x10

//-----------


//--- SET PAGE ADDRESS ----

//-----------
#define DISPLAY_OFFSET 0xD3
#define COM_SCAN 0xC8
#define CLOCK 0xD5
#define PRE_CHARG 0xD9


#define MAM 0x20 // Set Memory Addresing Mode
#define PAGE_ADDR 0x22

#define PAGE_START 0xB0

#define START_LINE 0x40 // This is a start row on display and the last row is 0x7F
#define CONTRAST 0x81

#define MUL_RAT 0xA8

typedef enum {
	BLACK = 0x00,
	WHITE = 0x01
}COLORS;

#define NORMAL_DISPLAY 0xA6
#define INVERSE_DISPLAY 0xA7

#define ON_RE_MAP 0xA1
#define OFF_RE_MAP 0xA0
#define PINS_CONF 0xDA
#define Vcomh 0xDB
char ssd1306_DrawChar(char ch, FontDef Font, COLORS color);
void ssd1306_WriteCommand(uint8_t cmd);
void ssd1306_WriteData(uint8_t* buffer, size_t buffer_size);
void ssd1306_SetDisplay(const uint8_t set);
void ssd1306_Init();
void ssd1306_UpdateScrean();
void ssd1306_SetDisplayOn(const uint8_t on);
void ssd1306_Fill(COLORS color);
void ssd1306_SetContrast(const uint8_t value) ;


