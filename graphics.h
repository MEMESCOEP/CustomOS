 void InitScreen();
 void PutPixel(int X, int Y, int RGB);
 void PutRect(int X, int Y, int Width, int Height, int RGB);
 void PutLine(int X1, int Y1, int X2, int Y2, int RGB);
 
void DrawPixel(){
char* video  = (char*)0xb8000;
   video [0] = 0x20; // space character
   video [1] = 0x12; // color of the pixel


}
