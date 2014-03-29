#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>

using namespace::std;

#define XAXIS_COLOR D3DCOLOR_XRGB(255,0,0 )
#define YAXIS_COLOR D3DCOLOR_XRGB(0,255,0 )
#define ZAXIS_COLOR D3DCOLOR_XRGB(0,0,255 )
#define CUBE_COLOR D3DCOLOR_XRGB(255,0,0 )
#define GROUND_COLOR D3DCOLOR_XRGB(128,64,250 )
#define SPHERE_COLOR D3DCOLOR_XRGB(255,200,0)

struct CUSTOMVERTEX {
	float X,Y,Z; DWORD COLOR;
};

class Primitive{
public:
	Primitive();
	void create_vbuffer();
	void create_ibuffer();
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DINDEXBUFFER9 i_buffer;
	int vertex_number;
	int face_number;
	CUSTOMVERTEX *node_v;
	short *node_i;
	ID3DXMesh* node_mesh;

	int p_type;
	void save_d3dxcreate2file( int, float, char *);
	void load_obj();
	void draw();
	void release();
	void create( int, float );
};

extern LPDIRECT3DVERTEXBUFFER9 line_buffer;
extern CUSTOMVERTEX lines[];