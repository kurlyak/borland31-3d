//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <memory.h>
#include <dos.h>
#include <stdlib.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

int colors[12] = { 14, 95, 200,
		   203, 103, 55,
		   44, 84, 22,
		   244, 22, 127 };

typedef float  matrix4x4[4][4];

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

float dxdyl, dxdyr;
float xl, xr;

unsigned char  my_video_buffer[64000];
unsigned char far * video_buffer =(unsigned char *) 0xA0000000L;
int back_lpitch = 320;

unsigned int nViewWidth = 320;
unsigned int nViewHeight = 200;

unsigned char nVertCount = 24;
unsigned char nIndexCount = 12;

void DrawCube ();
vector3 Mat4x4_Vect3_Mul(matrix4x4 mat1, vector3 vec1);

void Draw_Color_Triangle(vector3 v1,
		  vector3 v2,
		  vector3 v3, int color);
void Draw_Color_Poly(int y1, int y2, int color);


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

float Vec3Dot(vector3 v1, vector3 v2)
{
	float temp = v1.x*v2.x + v1.y*v2.y + v1.z* v2.z;
	return temp;
}

vector3 Vec3Cross(vector3 v1, vector3 v2)
{
	vector3 temp;

	temp.x = v1.y * v2.z - v1.z * v2.y;
	temp.y = v1.z * v2.x - v1.x * v2.z;
	temp.z = v1.x * v2.y - v1.y * v2.x;

	return temp;
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


	angle+= PI / 50.0f;
	if(angle> PI2)
		angle = 0.0f;

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
	float fZNear = 0.1f;

	float    h, w, Q;
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

		float s = (v2.x - v1.x)*(v3.y - v1.y) - (v2.y - v1.y)*(v3.x - v1.x);

		if(s <= 0 )
			continue;

		Draw_Color_Triangle(v1, v2, v3, colors[i]);
     }

	delay(45);
}

void Draw_Color_Triangle(vector3 v1,
			  vector3 v2,
			  vector3 v3, int color)
{
	int side;
	float x1, x2, x3;
	float y1, y2, y3;

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


	#define swapfloat(x, y) tempf = x; x = y; y = tempf;

	if (y1 > y2)
	{
		swapfloat(x1, x2);
		swapfloat(y1, y2);
	}

	if (y1 > y3)
	{
		swapfloat(x1, x3);
		swapfloat(y1, y3);
	}

	if (y2 > y3)
	{
		swapfloat(x2, x3);
		swapfloat(y2, y3);
	}
	#undef swapfloat

	y1i = (int) floor (y1);
	y2i = (int) floor (y2);
	y3i = (int) floor (y3);


	if ((y1i == y2i && y1i == y3i)
	 || ((int) x1 == (int) x2 && (int) x1 == (int) x3))
	     return;


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

		xl = x1;

		if (y1 < y2)
		{

			dyr = y2 - y1;
			dxdyr = (x2 - x1) / dyr;

			xr = x1;
			Draw_Color_Poly(y1i, y2i, color);

		}

		if (y2 < y3)
		{
			dyr = y3 - y2;
			dxdyr = (x3 - x2) / dyr;

			xr = x2;
			Draw_Color_Poly(y2i, y3i, color);
		}
	}
	else
	{

		dyr = y3 - y1;
		dxdyr = (x3 - x1) / dyr;

		xr = x1;

		if (y1 < y2)
		{

			dyl = y2 - y1;
			dxdyl = (x2 - x1) / dyl;

			xl = x1;

			Draw_Color_Poly(y1i, y2i, color);

		}
		if (y2 < y3)
		{

			dyl = y3 - y2;
			dxdyl = (x3 - x2) / dyl;

			xl = x2;

			Draw_Color_Poly(y2i, y3i, color);
		}
	}
}

void Draw_Color_Poly(int y1, int y2, int color)
{

    unsigned char *screen_ptr  = NULL;
    screen_ptr = my_video_buffer + (y1 * back_lpitch);

    for (int yi = y1; yi<y2; yi++)
    {

	for (int xi=(int)xl; xi<(int)xr; xi++)
	{
		screen_ptr[xi] = color;
	}

		xl+=dxdyl;
		xr+=dxdyr;
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
