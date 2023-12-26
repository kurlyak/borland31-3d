//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//	BORLAND C++ 3.1 Source Code
//======================================================================================

#include <graphics.h>
#include <conio.h>
#include <dos.h>
#include <math.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

struct vector3
{
	float x,y,z;
};

enum { A, B, C, D, E, F, G, H };

vector3 vert_buff[8] = {
	-4.0, -4.0, -4.0,
	 4.0, -4.0, -4.0,
	-4.0,  4.0, -4.0,
	 4.0,  4.0, -4.0,

	-4.0, -4.0,  4.0,
	 4.0, -4.0,  4.0,
	-4.0,  4.0,  4.0,
	 4.0,  4.0,  4.0 };

 vector3 vert_buff_res[8];

 /*
 C - D          G - H
 |   |          |   |
 A - B 		E - F
 */

 unsigned int index_buff[24] = {
	 A, B,
	 B, D,
	 D, C,
	 C, A,

	 E, F,
	 F, H,
	 H, G,
	 G, E,

	 A, E,
	 B, F,
	 D, H,
	 C, G };

typedef float matrix4x4[4][4];

vector3 Vec3_Mat4x4_Mul(vector3 v, matrix4x4 m)
{
	vector3 t;

	t.x = 	v.x * m[0][0] +
		v.y * m[1][0] +
		v.z * m[2][0] +
			m[3][0];

	t.y = 	v.x * m[0][1] +
		v.y * m[1][1] +
		v.z * m[2][1] +
		      m[3][1];

	t.z = 	v.x * m[0][2] +
		v.y * m[1][2] +
		v.z * m[2][2] +
		      m[3][2];

	return t;

}

float Vec3Dot(vector3 v1, vector3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vector3 Vec3Cross(vector3 v1, vector3 v2)
{
	vector3 t = { 	v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x };

	return t;
}

vector3 Vec3Normalize(vector3 v)
{
	float len = sqrt( (v.x * v.x) + (v.y * v.y) + (v.z * v.z) );
	vector3 t = { v.x / len, v.y / len, v.z / len };
	return t;
}

void Draw_Cube()
{
	static float angle = 0.0;

	matrix4x4 mxRotateY = {
		cos(angle),	0.0,	-sin(angle),	0.0,
		0.0,		1.0,	0.0,		0.0,
		sin(angle),	0.0,	cos(angle),	0.0,
		0.0,		0.0,	0.0,		1.0 };

	angle = angle + PI / 100.0f;
	if(angle > PI2)
	angle = 0.0;

	vector3 vRight = { 1.0, 0.0, 0.0 };
	vector3 vUp = { 0.0, 1.0, 0.0 };
	vector3 vPos = { 0.0, 10.0, -20.0 };
	vector3 vLook = { -1.0 * vPos.x, -1.0 * vPos.y, -1.0 * vPos.z };
	//vector3 vLook = { 0.0, 0.0, 1.0 };
	vLook = Vec3Normalize(vLook);

	vUp = Vec3Cross(vLook, vRight);
	vUp = Vec3Normalize(vUp);
	vRight = Vec3Cross(vUp, vLook);
	vRight = Vec3Normalize(vRight);

	float xp = - Vec3Dot(vPos, vRight);
	float yp = - Vec3Dot(vPos, vUp);
	float zp = - Vec3Dot(vPos, vLook);

	matrix4x4 mView = {
		vRight.x,	vUp.x,	vLook.x,	0.0,
		vRight.y,       vUp.y,  vLook.y,        0.0,
		vRight.z,       vUp.z,  vLook.z,        0.0,
		xp,		yp,	zp,		1.0 };


	for ( int i = 0; i < 8; i++ )
	{
		vector3 v = Vec3_Mat4x4_Mul(vert_buff[i], mxRotateY);
		v = Vec3_Mat4x4_Mul(v, mView);

		//v.z = v.z + 15.0;
		v.x = v.x / v.z;
		v.y = v.y / v.z;

		v.x = v.x / (640.0 / 480.0);

		v.x = v.x * 640.0 / 2.0 + 640.0 / 2.0;
		v.y =-v.y * 480.0 / 2.0 + 480.0 / 2.0;

		vert_buff_res[i] = v;
	}

	for ( i = 0; i < 12; i++ )
	{
		vector3 v1 = vert_buff_res[index_buff[i * 2]];
		vector3 v2 = vert_buff_res[index_buff[i * 2 + 1]];

		line(v1.x, v1.y, v2.x, v2.y);
	}

	 delay(25);
}

int main()
{

	int gdevice = DETECT, gmode;
	initgraph(&gdevice, &gmode, "D:\\BORLANDC\\BGI");

	setcolor(14);
	setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

	while( !kbhit() )
	{
		cleardevice();
		Draw_Cube();
	}

	closegraph();

	return 0;
}