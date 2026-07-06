/*
 * TALLER DE SISTEMAS EMBEBIDOS (TdSE) - FIUBA
 * Driver LCD 16x2 - Adaptación a Módulo I2C (PCF8574)
 */

#include "display.h"
#include "main.h"
#include "systick.h"

/* Manejador de la HAL para el puerto I2C1 (generado en main.c) */
extern I2C_HandleTypeDef hi2c1;

/* Dirección base del módulo I2C desplazada 1 bit a la izquierda (requerimiento HAL) */
#define LCD_I2C_ADDRESS (0x27 << 1) // Si no funciona, probar con (0x3F << 1)

/* Máscaras de bits para el expansor PCF8574 */
#define LCD_RS 0x01  // Bit 0: Register select
#define LCD_RW 0x02  // Bit 1: Read/Write
#define LCD_EN 0x04  // Bit 2: Enable
#define LCD_BL 0x08  // Bit 3: Backlight (Retroiluminación)

//=====[Declaración de prototipos privados]====================================
static void lcd_send_cmd (char cmd);
static void lcd_send_data (char data);

//=====[Implementación de funciones públicas]==================================

void displayInit( displayConnection_t connection )
{
//    display_t display;
//    display.connection = connection;

    /* Retardo inicial de estabilización eléctrica (50 ms) */
    HAL_Delay(50);

    /* Secuencia de inicialización estricta del HD44780 en modo 4 bits */
    lcd_send_cmd(0x30);
    HAL_Delay(5);       // 5000 us = 5 ms

    lcd_send_cmd(0x30);
    HAL_Delay(1);       // 1000 us = 1 ms

    lcd_send_cmd(0x30);
    HAL_Delay(10);      // 10000 us = 10 ms

    lcd_send_cmd(0x20); /* ¡Comando crucial! Pasa el LCD a modo 4-bits */
    HAL_Delay(10);      // 10000 us = 10 ms

    /* Configuración fina del display */
    lcd_send_cmd(0x28); /* Function set: 4-bits, 2 lineas, 5x8 puntos */
    HAL_Delay(1);

    lcd_send_cmd(0x08); /* Display control: Apagar display temporalmente */
    HAL_Delay(1);

    lcd_send_cmd(0x01); /* Clear display: Borrar memoria RAM */
    HAL_Delay(2);       // 2000 us = 2 ms

    lcd_send_cmd(0x06); /* Entry mode set: Auto-incrementar el cursor */
    HAL_Delay(1);

    lcd_send_cmd(0x0C); /* Display control: Encender display, cursor invisible */
    HAL_Delay(1);
}

void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY )
{
    uint8_t address = 0;

    /* Mapeo de coordenadas X,Y a la memoria DDRAM física del LCD */
    switch( charPositionY ) {
        case 0: address = 0x00 + charPositionX; break;
        case 1: address = 0x40 + charPositionX; break;
        case 2: address = 0x14 + charPositionX; break;
        case 3: address = 0x54 + charPositionX; break;
        default: return;
    }

    /* El comando para setear dirección requiere poner el Bit 7 en 1 (0x80) */
    lcd_send_cmd(0x80 | address);
}

/* Función vital para la Arquitectura No Bloqueante (1 caracter por tick) */
void displayDataWrite(const char data)
{
    lcd_send_data(data);
}

/* Función de compatibilidad heredada */
void displayStringWrite( const char * str )
{
    while (*str) {
        displayDataWrite(*str++);
    }
}

//=====[Implementación de funciones privadas (El Corazón I2C)]================

static void lcd_send_cmd (char cmd)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];

    /* Aislar los 4 bits altos y los 4 bits bajos */
    data_u = (cmd & 0xF0);
    data_l = ((cmd << 4) & 0xF0);

    /* Empaquetar la ráfaga: Datos | Luz ON | EN | RW=0 | RS=0 (Instrucción) */
    data_t[0] = data_u | LCD_BL | LCD_EN; /* Pulso EN Arriba  */
    data_t[1] = data_u | LCD_BL;          /* Pulso EN Abajo   */
    data_t[2] = data_l | LCD_BL | LCD_EN; /* Pulso EN Arriba  */
    data_t[3] = data_l | LCD_BL;          /* Pulso EN Abajo   */

    /* Transmitir bloque de 4 bytes */
    HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDRESS, data_t, 4, 100);
}

static void lcd_send_data (char data)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];

    /* Aislar los 4 bits altos y los 4 bits bajos */
    data_u = (data & 0xF0);
    data_l = ((data << 4) & 0xF0);

    /* Empaquetar la ráfaga: Datos | Luz ON | EN | RW=0 | RS=1 (Dato) */
    data_t[0] = data_u | LCD_BL | LCD_EN | LCD_RS; /* Pulso EN Arriba  */
    data_t[1] = data_u | LCD_BL |          LCD_RS; /* Pulso EN Abajo   */
    data_t[2] = data_l | LCD_BL | LCD_EN | LCD_RS; /* Pulso EN Arriba  */
    data_t[3] = data_l | LCD_BL |          LCD_RS; /* Pulso EN Abajo   */

    /* Transmitir bloque de 4 bytes */
    HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDRESS, data_t, 4, 100);
}
