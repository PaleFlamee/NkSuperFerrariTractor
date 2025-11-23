// #include "NkSuperFerrariTractor.h"

// #include "LineTrack.h"
// 八通道循迹
// #define P1			DL_GPIO_readPins(GPIO_Gray_PIN_Gray_1_PORT,GPIO_Gray_PIN_Gray_1_PIN)
// #define P2			DL_GPIO_readPins(GPIO_Gray_PIN_Gray_2_PORT,GPIO_Gray_PIN_Gray_2_PIN)
// #define P3			DL_GPIO_readPins(GPIO_Gray_PIN_Gray_3_PORT,GPIO_Gray_PIN_Gray_3_PIN)
// #define P4			DL_GPIO_readPins(GPIO_Gray_PIN_Gray_4_PORT,GPIO_Gray_PIN_Gray_4_PIN)
// #define P5			DL_GPIO_readPins(GPIO_Gray_PIN_Gray_5_PORT,GPIO_Gray_PIN_Gray_5_PIN)
// #define P6			DL_GPIO_readPins(GPIO_Gray_PIN_Gray_6_PORT,GPIO_Gray_PIN_Gray_6_PIN)
// #define P7			DL_GPIO_readPins(GPIO_Gray_PIN_Gray_7_PORT,GPIO_Gray_PIN_Gray_7_PIN)
// #define P8			DL_GPIO_readPins(GPIO_Gray_PIN_Gray_8_PORT,GPIO_Gray_PIN_Gray_8_PIN)
// int xunji(void) 			//输出差速，左轮速度为middle+x，右轮速度为middle-x
// {
//     if(P4!=0)
// 	{
// 		return -10;
// 	}
// 	else if(P5!=0)
// 	{
// 		return 10;
// 	}
//     else if(P3!=0)
// 	{
// 		return -18;
// 	}
// 	else if(P6!=0)
// 	{
// 		return 18;
// 	}
// 	else if(P2!=0)
// 	{
// 		return -22;
// 	}
// 	else if(P7!=0)
// 	{
// 		return 22;
// 	}
// 	else if(P1!=0)
// 	{
// 		return -34;
// 	}
// 	else if(P8!=0)
// 	{
// 		return 34;
// 	}
// 	return 0;
// }

// int xunji_highspeed_left(void) 			//输出差速，左轮速度为middle+x，右轮速度为middle-x
// {
//     if(P1!=0&&P2!=0)
//     {
//         return -110;
//     }
//     else if(P1!=0)
// 	{
// 		return -70;
// 	}
//     else if(P2!=0)
// 	{
// 		return -40;
// 	}
//     else if(P3!=0)
// 	{
// 		return -20;
// 	}
//     else if(P4!=0)
// 	{
// 		return -10;
// 	}

// 	return 0;
// }

// int xunji_highspeed_right(void) 			//输出差速，左轮速度为middle+x，右轮速度为middle-x
// {
//     if(P8!=0&&P7!=0)
//     {
//         return 110;
//     }
//     else if(P8!=0)
// 	{
// 		return 70;
// 	}
//     else if(P7!=0)
// 	{
// 		return 40;
// 	}
//     else if(P6!=0)
// 	{
// 		return 20;
// 	}
// 	else if(P5!=0)
// 	{
// 		return 10;
// 	}

// 	return 0;
// }

// int xunji_lowspeed_left(void) 			//输出差速，左轮速度为middle+x，右轮速度为middle-x
// {
//     if(P1!=0)
// 	{
// 		return -45;
// 	}
//     else if(P2!=0)
// 	{
// 		return -25;
// 	}
//     else if(P3!=0)
// 	{
// 		return -16;
// 	}
//     else if(P4!=0)
// 	{
// 		return -10;
// 	}

// 	return 0;
// }

// int xunji_lowspeed_right(void) 			//输出差速，左轮速度为middle+x，右轮速度为middle-x
// {
//     if(P8!=0)
// 	{
// 		return 45;
// 	}
//     else if(P7!=0)
// 	{
// 		return 25;
// 	}
//     else if(P6!=0)
// 	{
// 		return 16;
// 	}
// 	else if(P5!=0)
// 	{
// 		return 10;
// 	}

// 	return 0;
// }
