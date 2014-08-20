#include "game.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f429i_discovery_lcd.h"
//Player1
int16_t player1X = 10;
int16_t player1Y = 10;
uint16_t player1W = 80;
uint16_t player1H = 10;
uint8_t player1IsReversed = 1;

//Player2
int16_t player2X = LCD_PIXEL_WIDTH - 20;
int16_t player2Y = LCD_PIXEL_HEIGHT - 20;
uint16_t player2W = 80;
uint16_t player2H = 10;
uint8_t player2IsReversed = 0;

//Ball
uint16_t ballSize = 5;
int16_t ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
int16_t ballY = ( LCD_PIXEL_HEIGHT - 5 ) / 2;
int16_t ballVX = 5;
int16_t ballVY = 5;
uint8_t ballIsRun = 0;

//Point

uint16_t PointSize = 10;
int16_t PointX = 20;
int16_t PointY = 30;
uint8_t Eat = 0;

int8_t MX = 10;
int8_t MY = 10;

//Mode
uint8_t demoMode = 0; //0

//Score
uint8_t Score[2] = {48,48};

void
BallReset()
{
	ballSize = 15;
	ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
	ballY = ( LCD_PIXEL_HEIGHT - 5 ) / 2;

	ballVX = 5;
	ballVY = 5;

	ballIsRun = 1;
}

//Point reset
void
PointReset()
{		
		if ( ballX - MX <= 40 || ballX - MX >= LCD_PIXEL_WIDTH-40)
		{
			MX *= -1;
			PointX = (LCD_PIXEL_WIDTH / 2) - MX;
		}
		else
			PointX = ballX -MX;
		if ( ballY - MY <= 40 || ballY - MY >= LCD_PIXEL_HEIGHT-40)
		{
			MY *= -1;
			PointY = (LCD_PIXEL_HEIGHT / 2) - MY;
		}
		else
			PointY = ballY - MY;
	Eat = 0;
}

void
GAME_EventHandler1()
{
	if( STM_EVAL_PBGetState( BUTTON_USER ) ){

		player1IsReversed = 0;

		while( STM_EVAL_PBGetState( BUTTON_USER ) );

		player1IsReversed = 1;
	}
}

void
GAME_EventHandler2()
{
	if( IOE_TP_GetState()->TouchDetected ){

		player2IsReversed = 1;

		while( IOE_TP_GetState()->TouchDetected );

		player2IsReversed = 0;
	}
}


void
GAME_EventHandler3()
{
	if( ballIsRun == 0 ){
		BallReset();
	}
}

//check whether the point was eaten
void
GAME_EventHandler4()
{
	if( Eat == 1 ){
		PointReset();
	}
}

void
GAME_Update()
{
	//Player1
	LCD_SetTextColor( LCD_COLOR_BLACK );
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );

	if( demoMode == 0 ){

		if( player1IsReversed )
			player1X -= 12;
		else
			player1X += 12;

		if( player1X <= 0 )
			player1X = 0;
		else if( player1X + player1W >= LCD_PIXEL_WIDTH )
			player1X = LCD_PIXEL_WIDTH - player1W;

		//Player2
		if( player2IsReversed )
			player2X -= 12;
		else
			player2X += 12;

		if( player2X <= 0 )
			player2X = 0;
		else if( player2X + player2W >= LCD_PIXEL_WIDTH )
			player2X = LCD_PIXEL_WIDTH - player2W;


		//Ball
		if( ballIsRun == 1 ){

			LCD_SetTextColor( LCD_COLOR_BLACK );
			LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );
			LCD_DrawFullRect( PointX,PointY,PointSize,PointSize);
			
			//Touch wall
			ballX += ballVX;
			if( ballX <= 0 ){
				ballX = 0;
				ballVX *= -1;
			}
			else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
				ballX = LCD_PIXEL_WIDTH - ballSize;
				ballVX *= -1;
			}
			
			//PONG!
			ballY += ballVY;

			if ( ballY <= PointY + PointSize && ballY >= PointY)
			{
					if (ballX <= PointX + PointSize && ballX >= PointX)
					{
							PointReset();
							if (Score[1] <= 56 )
							{
								Score[1] = Score[1] + 1;
							}
							else
							{
								Score[1] = 48;
								Score[0] = Score[0] + 1;
							}												
					}	
			}

			else if ( ballY + ballSize <= PointY + PointSize && ballY + ballSize >= PointY)
			{
					if (ballX + ballSize <= PointX + PointSize && ballX + ballSize >= PointX)
					{
							if (ballSize > 5)
							{ 
								ballSize -= 3;
							}
							
							PointReset();

							if (Score[1] <= 56 )
							{
								Score[1] = Score[1] + 1;
							}
							else
							{
								Score[1] = 48;
								Score[0] = Score[0] + 1;
							}												
					}	
			}
			if( ballY + ballSize >= player2Y ){
				if( ballX + ballSize >= player2X && ballX <= player2X + player2W ){
					if( ballX  <= player2X + player2W/5 ){
						ballVY =-6;
						ballVX =-14;
					}
					else if( ballX <= player2X + 2*player2W/5 ){
						ballVY =-6;
						ballVX = 14;
					}
					else if( ballX <= player2X + 3*player2W/5 ){
						ballVY =-12;
						ballVX =-6;
					}
					else if( ballX <= player2X + 4*player2W/5 ){
						ballVY =-14;
						ballVX =-6;
					}
					else if( ballX <= player2X + player2W ){
						ballVY =-14;
						ballVX = 6;
					}

				}
				else
					BallReset();
			}

			if( ballY <= player1Y + player1H ){
					if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
						if( ballX  <= player1X + player1W/5 ){
							ballVY = 6;
							ballVX =-14;
						}
						else if( ballX <= player1X +2*player1W/5 ){
							ballVY = 6;
							ballVX = 14;
						}
						else if( ballX <= player1X+ 3*player1W/5 ){
							ballVY = 12;
							ballVX = 0;
						}
						else if( ballX <= player1X + 4*player1W/5 ){
							ballVY = 14;
							ballVX =-6;
						}
						else if( ballX <= player1X+ player1W ){
							ballVY = 14;
							ballVX = 6;
						}

					}
					else
						BallReset();
				}
			}


		}
		else{	//if demoMode == 1

			//Player1 move
			if( ballVY < 0 ){
				if( player1X + player1W/2 < ballX + ballSize/2 ){
					player1X += 8;
					player2X += 2;
				}
				else{
					player1X -= 8;
					player2X -= 2;
				}
			}

			//Player2 move
			if( ballVY > 0 ){
				if( player2X + player2W/2 < ballX + ballSize/2 ){
					player1X += 2;
					player2X += 8;
				}
				else{
					player1X -= 2;
					player2X -= 8;
				}

			}

			if( player1X <= 0 )
				player1X = 0;
			else if( player1X + player1W >= LCD_PIXEL_WIDTH )
				player1X = LCD_PIXEL_WIDTH - player1W;

			if( player2X <= 0 )
				player2X = 0;
			else if( player2X + player2W >= LCD_PIXEL_WIDTH )
				player2X = LCD_PIXEL_WIDTH - player2W;


			//Ball
			if( ballIsRun == 1 ){

				LCD_SetTextColor( LCD_COLOR_BLACK );
				LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );

				//Touch wall
				ballX += ballVX;
				if( ballX <= 0 ){
					ballX = 0;
					ballVX *= -1;
				}
				else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
					ballX = LCD_PIXEL_WIDTH - ballSize;
					ballVX *= -1;
				}

				//PONG!
				ballY += ballVY;
				if( ballY + ballSize >= player2Y ){
					if( ballX + ballSize >= player2X && ballX <= player2X + player2W ){
					if( ballX - ballSize <= player2Y + player2W/4 ){
						ballVY =-3;
						ballVX =-7;
					}
					else if( ballX >= player2Y + player2W - player2W/4 ){
						ballVY =-3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player2Y + player2W/2 ){
						ballVY =-7;
						ballVX =-3;
					}
					else if( ballX > player2Y + player2W/2 ){
						ballVY =-7;
						ballVX = 3;
					}
					else{
						ballVY =-9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}

			if( ballY <= player1Y + player1H ){
				if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
					if( ballX - ballSize <= player1Y + player1W/4 ){
						ballVY = 3;
						ballVX =-7;
					}
					else if( ballX >= player1Y + player1W - player1W/4 ){
						ballVY = 3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player1Y + player1W/2 ){
						ballVY = 7;
						ballVX =-3;
					}
					else if( ballX > player1Y + player1W/2 ){
						ballVY = 7;
						ballVX = 3;
					}
					else{
						ballVY = 9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}
		}
	}
}

	void
GAME_Render()
{	
	LCD_SetTextColor( LCD_COLOR_RED);
	LCD_DrawFullRect( PointX,PointY,PointSize,PointSize);
	LCD_DisplayStringLine(160, Score);
	LCD_SetTextColor( LCD_COLOR_WHITE );
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
	LCD_SetTextColor( LCD_COLOR_BLUE);
	LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );
	//LCD_DrawLine( 10, LCD_PIXEL_HEIGHT / 2, LCD_PIXEL_WIDTH - 20, LCD_DIR_HORIZONTAL );
}
