#ifndef LCD_H
#define LCD_H


#define I2C_SDA_GPIO       8
#define I2C_SCL_GPIO       9
#define I2C_FREQ_HZ        100000

#define LCD_ADDR            0x27


void LCD_init(void);
void LCD(int col, int row, const char *line);


#endif