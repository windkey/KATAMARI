#pragma once
#define SCREEN_WIDTH 1420
#define SCREEN_HEIGHT 690
//define vertex formate we are using
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE )

#define CAMERA_POSITION D3DXVECTOR3(-5.0f,30.0f,-30.0f)
#define CAMERA_POS -1.0f,30.0f,-30.0f
#define LOOKAT_POS 0.0f,1.0f,0.0f
#define UP_DIR 0.0f,1.0f,0.0f

#define MOVE_SPEED 0.5f
#define ROTATE_SPEED (float)M_PI/4.0f
#define MAX_ROTATE (float)M_PI/2.0f

#define F1_INFO L"<-,->: rotate main role\n\
	up,down: straight forward/backward\n\
	mouse L+move: camera rotate\n\
	f: wireframe/solid mode\n\
	p: switch static/dynamic camera\n\n"
#define STRING_SIZE 2048

#define VK_A 0X41
#define VK_B 0X42
#define VK_C 0X43
#define VK_D 0X44
#define VK_E 0X45
#define VK_F 0X46
#define VK_G 0X47
#define VK_H 0X48
#define VK_I 0X49
#define VK_J 0X4A
#define VK_K 0X4B
#define VK_L 0X4C
#define VK_M 0X4D
#define VK_N 0X4E
#define VK_O 0X4F
#define VK_P 0X50
#define VK_Q 0X51
#define VK_R 0X52
#define VK_S 0X53
#define VK_T 0X54
#define VK_U 0X55
#define VK_V 0X56
#define VK_W 0X57
#define VK_X 0X58
#define VK_Y 0X59
#define VK_Z 0X5A

#include "node.h"
#include <d3d9.h>
#include <d3dx9.h>

class d3d_control
{
private:
	LPDIRECT3D9 d3d;

	int prev_mouse_pos[2];
	int mouse_pos[2];
	bool mouse_clicking;
	bool using_camera;
	ID3DXLine *Line;
	
	void init_axis();
	void init_text();
	void view_transform();
	void projection_transform();
	
public:
	LPDIRECT3DDEVICE9 d3ddev;
	int screen_width;
	int screen_height;

	d3d_control(void);
	~d3d_control(void);
	void world_reset();
	bool fillmode_changed;
	bool is_solid;
	void initD3D(HWND hWnd);
	void render_frame();
	void cleanD3D();
	bool input_handler( MSG );
	void draw_axis( D3DXMATRIX& );
	void draw_text();
	void setWorldTransform( D3DXMATRIX& );
	void setViewTransform( D3DXMATRIX& );
};

class Vector3{
public:
	Vector3();
	Vector3( float, float, float );
	float x,y,z;
	Vector3 operator-( Vector3& );
};

class Camera {
private:
	//initial camera position
	//also used when calling reset()
	Vector3 init_camera_pos;
	Vector3 init_lookat_pos;
	Vector3 init_up_dir;

	//calculated camera position
	//after calling LookAtLH or any move
	Vector3 camera_pos;
	Vector3 lookat_pos;
	Vector3 up_dir;
	Vector3 right_dir;
	Vector3 forward_dir;

	D3DXMATRIX matInit;
	D3DXMATRIX matViewMatrix;
public:
	Camera();
	void create( Vector3, Vector3, Vector3);
	void vectorTranslate( Vector3, D3DXMATRIX );
	void vectorRotate( Vector3 );
	void set();
	void rotate( float, float, float );
	void translate( float, float, float );
	void LookAtLH( Vector3,Vector3,Vector3);
	void reset();
	void move_up_dir( float );
	void move_right_dir( float );
	void mouse_move( int , int );
};

extern WCHAR *s_information;
extern Camera camera;
extern d3d_control d3d_ctrl;