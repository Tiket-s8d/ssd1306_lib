#include "ssd1306.h"

static uint8_t SSD1306_Buffer[BUFFER_SIZE];
static ssd1306_Cursor cursor;

void ssd1306_WriteCommand(uint8_t cmd){
	 HAL_I2C_Mem_Write(&ssd1306_port, ssd1306_addr, 0x00, 1, &cmd, 1, HAL_MAX_DELAY);
}
void ssd1306_WriteData(uint8_t* buffer, size_t buffer_size){
    HAL_I2C_Mem_Write(&ssd1306_port, ssd1306_addr, 0x40, 1, buffer, buffer_size, HAL_MAX_DELAY);

}

void ssd1306_SetDisplay(const uint8_t set){
	if(set){
		ssd1306_WriteCommand(ON);
	}else{
		ssd1306_WriteCommand(OFF);
	}
}

void ssd1306_Init(){
	cursor.x = 50;
	cursor.y = 20;
	HAL_Delay(200);
	ssd1306_SetDisplay(0);
	ssd1306_WriteCommand(MAM);
	ssd1306_WriteCommand(HORIZONTAL);

	ssd1306_WriteCommand(PAGE_START);
    ssd1306_WriteCommand(COM_SCAN);
    ssd1306_WriteCommand(LOW_COLUMN);
    ssd1306_WriteCommand(HIGH_COLUMN);

    ssd1306_WriteCommand(START_LINE);

    ssd1306_SetContrast(0xFF);

    ssd1306_WriteCommand(ON_RE_MAP);
    ssd1306_WriteCommand(NORMAL_DISPLAY);

    ssd1306_WriteCommand(MUL_RAT);
    ssd1306_WriteCommand(0x3F);

    ssd1306_WriteCommand(ENTIRE_1);

    ssd1306_WriteCommand(DISPLAY_OFFSET);
    ssd1306_WriteCommand(0x00);

    ssd1306_WriteCommand(CLOCK);
    ssd1306_WriteCommand(0xF0);

    ssd1306_WriteCommand(PRE_CHARG);
    ssd1306_WriteCommand(PAGE_ADDR);

    ssd1306_WriteCommand(PINS_CONF);
    ssd1306_WriteCommand(0x12);

    ssd1306_WriteCommand(Vcomh);
    ssd1306_WriteCommand(0x20);

    ssd1306_WriteCommand(0x8D);
    ssd1306_WriteCommand(0x14);
    ssd1306_SetDisplayOn(1);


    ssd1306_Fill(BLACK);


    ssd1306_UpdateScrean();


}


void ssd1306_UpdateScrean(){
	for(uint8_t i = 0; i < ssd1306_HEIGHT/8; i++){
		ssd1306_WriteCommand(PAGE_START+i);
		ssd1306_WriteCommand(LOW_COLUMN + SSD1306_X_OFFSET_LOWER);
		ssd1306_WriteCommand(HIGH_COLUMN + SSD1306_X_OFFSET_UPPER);
		ssd1306_WriteData(&SSD1306_Buffer[ssd1306_WIDTH*i],ssd1306_WIDTH);
	}
}


void ssd1306_SetDisplayOn(const uint8_t on) {
    uint8_t value;
    if (on) {
        value = 0xAF;   // Display on
    } else {
        value = 0xAE;   // Display off
    }
    ssd1306_WriteCommand(value);
}

void ssd1306_Fill(COLORS color){
	for(uint32_t i =0; i < sizeof(SSD1306_Buffer);i++){
		SSD1306_Buffer[i] = (color == BLACK) ? 0x00 : 0xFF;
	}

}
void ssd1306_SetContrast(const uint8_t value) {
    const uint8_t kSetContrastControlRegister = CONTRAST;
    ssd1306_WriteCommand(kSetContrastControlRegister);
    ssd1306_WriteCommand(value);
}



void ssd1306_DrawCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,COLORS par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= ssd1306_WIDTH || par_y >= ssd1306_HEIGHT) {
        return;
    }

    do {
    	ssd1306_DrawPixel(par_x - x, par_y + y, par_color);
        ssd1306_DrawPixel(par_x + x, par_y + y, par_color);
        ssd1306_DrawPixel(par_x + x, par_y - y, par_color);
        ssd1306_DrawPixel(par_x - x, par_y - y, par_color);
        e2 = err;


        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    return;
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, COLORS color){
	if(x >= ssd1306_WIDTH || y>= ssd1306_HEIGHT){
		// Nothing work when you write out side of buffer
		return;
	}

	if(color == WHITE){
		SSD1306_Buffer[x+(y/8)*ssd1306_WIDTH] |= 1<<(y%8);
	}else{
		SSD1306_Buffer[x+(y/8)*ssd1306_WIDTH] &= ~(1 << (y % 8));
	}

}

void ssd1306_DrawSquare(uint8_t x, uint8_t y, uint8_t size){
	uint8_t x1 = x+size;
	uint8_t y1 = y+size;
	ssd1306_DrawLine(x,y,x1,y,WHITE);
	ssd1306_DrawLine(x1,y,x1,y1,WHITE);
	ssd1306_DrawLine(x,y,x,y1,WHITE);
	ssd1306_DrawLine(x,y1,x1,y1,WHITE);
}

void ssd1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color) {
	int32_t deltaX = abs(x2 - x1);
	int32_t deltaY = abs(y2 - y1);
	int32_t signX = ((x1 < x2) ? 1 : -1);
	int32_t signY = ((y1 < y2) ? 1 : -1);
	int32_t error = deltaX - deltaY;
	int32_t error2;

	ssd1306_DrawPixel(x2, y2, color);

	while((x1 != x2) || (y1 != y2)) {
		ssd1306_DrawPixel(x1, y1, color);
	    error2 = error * 2;
	    if(error2 > -deltaY) {
	    	error -= deltaY;
	        x1 += signX;
	    }

	    if(error2 < deltaX) {
	    	error += deltaX;
	        y1 += signY;
	    }
	}
}

char ssd1306_DrawChar(char ch, FontDef Font, COLORS color){

	uint32_t b;
	for(uint32_t i = 0; i< Font.FontHeight;i++){
		 b = Font.data[(ch-32)*Font.FontHeight+i];
		for(uint32_t j = 0 ; j < Font.FontWidth; j++){
			if((b << j) &0x8000){
				ssd1306_DrawPixel((cursor.x+j), (cursor.y+i), color);
			}else{
				ssd1306_DrawPixel((cursor.x+j), (cursor.y+i), !color);
			}
		}
	}
	cursor.x += Font.FontWidth;
	return ch;
}

char ssd1306_DrawString(char *str, FontDef Font, COLORS color){
	while(*str){
		if(ssd1306_DrawChar(*str, Font, color)!= *str){
			return 0;
		}
		str++;

	}


}




