//======================================================================================
//	Ed Kurlyak 2023 Perspective Correct Texturemapping
//	BORLAND C++ 3.1 Source Code
//======================================================================================


#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <memory.h>
#include <dos.h>

typedef struct tagBITMAPFILEHEADER {
  unsigned int   bfType;
  unsigned long    bfSize;
  unsigned int   bfReserved1;
  unsigned int   bfReserved2;
  unsigned long    bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
  unsigned long  biSize;
  unsigned long  biWidth;
  unsigned long  biHeight;
  unsigned int   biPlanes;
  unsigned int   biBitCount;
  unsigned long  biCompression;
  unsigned long  biSizeImage;
  unsigned long  biXPelsPerMeter;
  unsigned long  biYPelsPerMeter;
  unsigned long  biClrUsed;
  unsigned long  biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD{
  unsigned char rgbBlue;
  unsigned char rgbGreen;
  unsigned char rgbRed;
  unsigned char Reserverd;
}RGBQUAD;

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)


typedef float  matrix4x4[4][4];

struct tex_coord2
{
	int tu, tv;
};

struct vector3
{

	float x,y,z;

	vector3 operator - (vector3 v_in)
	{
		vector3 v_out;

		v_out.x = x - v_in.x;
		v_out.y = y - v_in.y;
		v_out.z = z - v_in.z;

		return v_out;
	}

	vector3 &operator = (vector3 &v_in)
	{
		x = v_in.x;
		y = v_in.y;
		z = v_in.z;

		return *this;
	}
};

unsigned char  my_video_buffer[64000];
unsigned char far * video_buffer =(unsigned char *) 0xA0000000L;
int back_lpitch = 320;

unsigned char *pRes = NULL;

unsigned int nViewWidth = 320;
unsigned int nViewHeight = 200;

unsigned char nVertCount = 24;
unsigned char nIndexCount = 12;

void DrawCube ();
vector3 Mat4x4_Vect3_Mul(vector3 vec1, matrix4x4 mat1);

float dxdyl, dudyl, dvdyl, dzdyl, dxdyr, dudyr, dvdyr, dzdyr;
float ul, ur, vl, vr, zl, zr, xl, xr;

void Draw_Textured_Triangle(vector3 v1, tex_coord2 t1,
		  vector3 v2, tex_coord2 t2,
		  vector3 v3, tex_coord2 t3);
void Draw_Textured_Poly(int y1, int y2);

vector3 vert_buff_transformed[24];

vector3 vert_buff[24] = {-5.000000,-5.000000,-5.000000,
		-5.000000,-5.000000,5.000000,
		5.000000,-5.000000,5.000000,
		5.000000,-5.000000,-5.000000,
		-5.000000,5.000000,-5.000000,
		5.000000,5.000000,-5.000000,
		5.000000,5.000000,5.000000,
		-5.000000,5.000000,5.000000,
		-5.000000,-5.000000,-5.000000,
		5.000000,-5.000000,-5.000000,
		5.000000,5.000000,-5.000000,
		-5.000000,5.000000,-5.000000,
		5.000000,-5.000000,-5.000000,
		5.000000,-5.000000,5.000000,
		5.000000,5.000000,5.000000,
		5.000000,5.000000,-5.000000,
		5.000000,-5.000000,5.000000,
		-5.000000,-5.000000,5.000000,
		-5.000000,5.000000,5.000000,
		5.000000,5.000000,5.000000,
		-5.000000,-5.000000,5.000000,
		-5.000000,-5.000000,-5.000000,
		-5.000000,5.000000,-5.000000,
		-5.000000,5.000000,5.000000 };


tex_coord2 tex_coord[24] = {
		63,63,
		63,0,
		0,0,
		0,63,
		0,63,
		63,63,
		63,0,
		0,0,
		0,63,
		63,63,
		63,0,
		0,0,
		0,63,
		63,63,
		63,0,
		0,0,
		0,63,
		63,63,
		63,0,
		0,0,
		0,63,
		63,63,
		63,0,
		0,0};

unsigned int index_buff[36] = {
		0,2,1, // 1 triangle
		2,0,3,		// 2 triangle
		4,6,5,		// 3 triangle
		6,4,7,		// 4 triangle
		8,10,9,		// 5 triangle
		10,8,11,	// 6 triangle
		12,14,13,	// 7 triangle
		14,12,15,	// 8 triangle
		16,18,17,	// 9 triangle
		18,16,19,	// 10 triangle
		20,22,21,	// 11 triangle
		22,20,23};	// 12 triangle


vector3 Mat4x4_Vect3_Mul(vector3 vec1, matrix4x4 mat1)
{
	vector3 temp;

	temp.x = vec1.x * mat1[0][0] +
		vec1.y * mat1[1][0] +
		vec1.z * mat1[2][0] + mat1[3][0];

	temp.y = vec1.x * mat1[0][1] +
		vec1.y * mat1[1][1] + vec1.z *
		mat1[2][1] + mat1[3][1];

	temp.z = vec1.x * mat1[0][2] +
		vec1.y * mat1[1][2] + vec1.z *
		mat1[2][2] + mat1[3][2];

	return temp;
}

void Load_BMP()
{
	FILE *fp;

	//texture size 64x64 pixels, 8 bit color depth
	fp = fopen("d:\\borlandc\\my\\texture.bmp","rb");
	if(fp==NULL) printf("Error Open File");

	BITMAPFILEHEADER bfh;

	fread(&bfh, sizeof(bfh), 1, fp);

	BITMAPINFOHEADER bih;

	fread(&bih, sizeof(bih), 1, fp);

	RGBQUAD rgbPal[256];
	fread(&rgbPal, 256 * 4,1,fp);

	fseek(fp, bfh.bfOffBits, SEEK_SET);

	unsigned char *nStartData;
	nStartData = new unsigned char [bih.biWidth*bih.biHeight];
	fread(nStartData,bih.biWidth*bih.biHeight,1,fp);

	pRes = nStartData;

	fclose(fp);

	//register color address VGA
	#define PALETTE_MASK            0x3c6
	#define PALETTE_REGISTER_RD     0x3c7    //register VGA for reading
	#define PALETTE_REGISTER_WR     0x3c8    //register VGA for writing
	#define PALETTE_DATA            0x3c9    //color data 3 byte following

	//one after another r,g,b
	for(int i = 0; i<256; i++)
	{
		//register color has 6 bit - 64
		//picture has color 8 bit - 256
		//convert i.e. 256/64 = 4

		unsigned char r = rgbPal[i].rgbRed/4;
		unsigned char g = rgbPal[i].rgbGreen/4;
		unsigned char b = rgbPal[i].rgbBlue/4;

		outp(PALETTE_MASK, 0xff);
		outp(PALETTE_REGISTER_WR, i);
		outp(PALETTE_DATA, r);
		outp(PALETTE_DATA, g);
		outp(PALETTE_DATA, b);
	}
}

void DrawCube ()
{
	static float angle = 0.0f;

	matrix4x4 mWorldRotX = {
		1, 0, 0, 0,
		0, cos(angle), sin(angle), 0,
		0,-sin(angle),  cos(angle), 0,
		0, 0, 0, 1};


	matrix4x4 mWorldRotY = {
		cos(angle), 0, -sin(angle), 0,
		0, 1, 0, 0,
		sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1};

	matrix4x4 mWorldRotZ={
		cos(angle), sin(angle), 0, 0,
		-sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1};

	angle+= PI/45.0;
	if(angle>PI2)
		angle = 0;

	matrix4x4 mWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 20, 1};

	matrix4x4 mView={
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1};

	float fFov =(float) 3.1415926/2.0; // FOV 90 degree
	float fAspect = 4.0 / 3.0;
	float fZFar = 100.0f;
	float fZNear = (float)0.1f;

	float h, w, Q;
	w = (float)(1/tan(fFov*0.5f))/fAspect;
	h = (float)1/tan(fFov*0.5f);
	Q = fZFar/(fZFar - fZNear);
	/*
	matrix4x4 mProj={
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, Q, 1,
		0, 0, -Q*fZNear, 0};
	*/

	matrix4x4 mProj={
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1};

	float alpha = (0.5f*nViewWidth-0.5f);
	float beta  = (0.5f*nViewHeight-0.5f);

	matrix4x4 mScreen={
	    alpha,  0,        0,    0,
	    0,      -beta,  0,    0,
	    0,              0,              1,    0,
	    alpha,  beta,   0,    1};

	unsigned int i;

	for (i = 0; i < nVertCount; i++)
	{
		vector3 v;
		v = Mat4x4_Vect3_Mul(vert_buff[i], mWorldRotX);
		v = Mat4x4_Vect3_Mul(v, mWorldRotY);
		v = Mat4x4_Vect3_Mul(v, mWorldRotZ);
		v = Mat4x4_Vect3_Mul(v, mWorld);
		v = Mat4x4_Vect3_Mul(v, mView);
		v = Mat4x4_Vect3_Mul(v, mProj);

		v.x = v.x / v.z;
		v.y = v.y / v.z;

		v = Mat4x4_Vect3_Mul(v, mScreen);

		vert_buff_transformed[i] = v;
	}

	for (i = 0; i < nIndexCount; i++)
	{
		vector3 v1 = vert_buff_transformed[index_buff[i * 3 + 0]];
		vector3 v2 = vert_buff_transformed[index_buff[i * 3 + 1]];
		vector3 v3 = vert_buff_transformed[index_buff[i * 3 + 2]];

		tex_coord2 t1 = tex_coord[index_buff[i * 3 + 0]];
		tex_coord2 t2 = tex_coord[index_buff[i * 3 + 1]];
		tex_coord2 t3 = tex_coord[index_buff[i * 3 + 2]];

		float s = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);

		if(s <= 0)
			continue;

		Draw_Textured_Triangle(v1, t1, v2, t2, v3, t3);
	}

	delay(45);
}

void Draw_Textured_Triangle(vector3 v1, tex_coord2 t1,
			  vector3 v2, tex_coord2 t2,
			  vector3 v3, tex_coord2 t3)
{

	int side;
	float x1, x2, x3;
	float y1, y2, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
	float tempf;
	int y1i, y2i, y3i;
	float dxdy1, dxdy2, dxdy3;

	float dyl, dyr;

	x1 = v1.x + 0.5f;
	y1 = v1.y + 0.5f;
	x2 = v2.x + 0.5f;
	y2 = v2.y + 0.5f;
	x3 = v3.x + 0.5f;
	y3 = v3.y + 0.5f;

	iz1 = 1.0f / v1.z;
	iz2 = 1.0f / v2.z;
	iz3 = 1.0f / v3.z;

	uiz1 = t1.tu * iz1;
	viz1 = t1.tv * iz1;
	uiz2 = t2.tu * iz2;
	viz2 = t2.tv * iz2;
	uiz3 = t3.tu * iz3;
	viz3 = t3.tv * iz3;

	#define swapfloat(x, y) tempf = x; x = y; y = tempf;

	if (y1 > y2)
	{
		swapfloat(x1, x2);
		swapfloat(y1, y2);
		swapfloat(iz1, iz2);
		swapfloat(uiz1, uiz2);
		swapfloat(viz1, viz2);
	}

	if (y1 > y3)
	{
		swapfloat(x1, x3);
		swapfloat(y1, y3);
		swapfloat(iz1, iz3);
		swapfloat(uiz1, uiz3);
		swapfloat(viz1, viz3);
	}

	if (y2 > y3)
	{
		swapfloat(x2, x3);
		swapfloat(y2, y3);
		swapfloat(iz2, iz3);
		swapfloat(uiz2, uiz3);
		swapfloat(viz2, viz3);
	}

	#undef swapfloat

	y1i = (int) floor (y1);
	y2i = (int) floor (y2);
	y3i = (int) floor (y3);

/*
	if ((y1i == y2i && y1i == y3i)
	 || ((int) x1 == (int) x2 && (int) x1 == (int) x3))
	     return;
  */
	if(y1i < y2i && y2i < y3i)
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		side = dxdy2 > dxdy1;
	}


	if (y1i == y2i)
		side = x1 > x2;
	if (y2i == y3i)
		side = x3 > x2;

	if (!side)
	{
		dyl = y3 - y1;
		dxdyl = (x3 - x1) / dyl;
		dudyl = (uiz3 - uiz1) / dyl;
		dvdyl = (viz3 - viz1) / dyl;
		dzdyl = (iz3 - iz1) / dyl;

		xl = x1;
		ul = uiz1;
		vl = viz1;
		zl = iz1;

		if (y1i < y2i)
		{
			dyr = y2 - y1;
			dxdyr = (x2 - x1) / dyr;
			dudyr = (uiz2 - uiz1) / dyr;
			dvdyr = (viz2 - viz1) / dyr;
			dzdyr = (iz2 - iz1) / dyr;

			xr = x1;
			ur = uiz1;
			vr = viz1;
			zr = iz1;

			Draw_Textured_Poly(y1i, y2i);

		}

		if (y2i < y3i)
		{
			dyr = y3 - y2;
			dxdyr = (x3 - x2) / dyr;
			dudyr = (uiz3 - uiz2) / dyr;
			dvdyr = (viz3 - viz2) / dyr;
			dzdyr = (iz3 - iz2) / dyr;

			xr = x2;
			ur = uiz2;
			vr = viz2;
			zr = iz2;

			Draw_Textured_Poly(y2i, y3i);
		}
	}
	else
	{

		dyr = y3 - y1;
		dxdyr = (x3 - x1) / dyr;
		dudyr = (uiz3 - uiz1) / dyr;
		dvdyr = (viz3 - viz1) / dyr;
		dzdyr = (iz3 - iz1) / dyr;

		xr = x1;
		ur = uiz1;
		vr = viz1;
		zr = iz1;

		if (y1i < y2i)
		{

			dyl = y2 - y1;
			dxdyl = (x2 - x1) / dyl;
			dudyl = (uiz2 - uiz1) / dyl;
			dvdyl = (viz2 - viz1) / dyl;
			dzdyl = (iz2 - iz1) / dyl;

			xl = x1;
			ul = uiz1;
			vl = viz1;
			zl = iz1;

			Draw_Textured_Poly(y1i, y2i);

		}
		if (y2i < y3i)
		{
			dyl = y3 - y2;
			dxdyl = (x3 - x2) / dyl;
			dudyl = (uiz3 - uiz2) / dyl;
			dvdyl = (viz3 - viz2) / dyl;
			dzdyl = (iz3 - iz2) / dyl;

			xl = x2;
			ul = uiz2;
			vl = viz2;
			zl = iz2;

			Draw_Textured_Poly(y2i, y3i);
		}
	}
}

void Draw_Textured_Poly(int y1, int y2)
{
	int dx;
	float ui, vi, zi;
	float du, dv, dz;

	unsigned char *screen_ptr  = NULL;
	screen_ptr = my_video_buffer + (y1 * back_lpitch);

	for (int yi = y1; yi<y2; yi++)
	{

		if ((dx = (xr - xl))>0)
		{
			du = (ur - ul)/(xr - xl);
			dv = (vr - vl)/(xr - xl);
			dz = (zr - zl)/(xr - xl);
		}
		else
		{
			du = (ur - ul);
			dv = (vr - vl);
			dz = (zr - zl);
		}

		int x1 = (int)xl;
		float itx = xl - x1;

		ui = ul + itx * du;
		vi = vl + itx * dv;
		zi = zl + itx * dz;

		for (int xi=(int)xl; xi<(int)xr; xi++)
		{
			float z = 1.0f/zi;
			float u = ui * z;
			float v = vi * z;

			int t = (int)u  + (((int)v) << 6);

			screen_ptr[xi] = pRes[t];

			ui+=du;
			vi+=dv;
			zi+=dz;
		}

		xl+=dxdyl;
		ul+=dudyl;
		vl+=dvdyl;
		zl+=dzdyl;

		xr+=dxdyr;
		ur+=dudyr;
		vr+=dvdyr;
		zr+=dzdyr;

		screen_ptr+=back_lpitch;
	}
}

#define VGA_INPUT_STATUS_1   0x3DA
#define VGA_VSYNC_MASK 0x08

void Wait_For_Vsync(void)
{
	while (inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK)
	{
	}
	while (! (inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK) )
	{
	}
}

int main ()
{

	_asm{
		mov ah, 0x0;
		mov al,0x13; //set video mode 13h - 320x200x256 colors
		int 10h;  //set video mode call interrupt
		}

	Load_BMP();

	while(!kbhit())
	{
		//Wait_For_Vsync();

		memset(my_video_buffer,0,64000);

		DrawCube() ;

		Wait_For_Vsync();
		_fmemcpy(video_buffer, my_video_buffer, 64000);
	}

	_asm{
		mov ah, 0x0;
		mov al,0x03; //set text video mode
		int 10h;  //set video mode call interrupt
		}

	return 0;
}
