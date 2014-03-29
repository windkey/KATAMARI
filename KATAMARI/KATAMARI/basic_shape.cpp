#include "StdAfx.h"
#include "basic_shape.h"
#include "d3d_control.h"
#include "node.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>
#include <vector>

LPDIRECT3DVERTEXBUFFER9 line_buffer=NULL ;

CUSTOMVERTEX *sphere_vertices=NULL;
short *sphere_indices = NULL;

CUSTOMVERTEX *ground_vertices = NULL ;
short *ground_indices = NULL ;

CUSTOMVERTEX *cube_vertices = NULL ;
short *cube_indices = NULL ;

/*
CUSTOMVERTEX ground_vertices[] = {
	{ -100.0f,0.0f,-100.0f, GROUND_COLOR, },
	{ 100.0f,0.0f,-100.0f, GROUND_COLOR, },
	{ 100.0f,0.0f,100.0f, GROUND_COLOR, },
	{ -100.0f,0.0f,100.0f, GROUND_COLOR, }
};

short ground_indices[] =
{
	0,2,3,
	0,1,2,
};

CUSTOMVERTEX cube_vertices[] = 
{
	{ -0.5f, 0.5f, -0.5f, CUBE_COLOR,  },
	{ 0.5f, 0.5f, -0.5f, CUBE_COLOR, },
	{ -0.5f, -0.5f, -0.5f, CUBE_COLOR,  },
	{ 0.5f,	-0.5f, -0.5f, CUBE_COLOR,  },
	{ -0.5f, 0.5f, 0.5f, D3DCOLOR_XRGB(255,255,255),  },
	{ 0.5f,	0.5f, 0.5f, D3DCOLOR_XRGB(255,255,255), },
	{ -0.5f, -0.5f, 0.5f, CUBE_COLOR, },
	{ 0.5f, -0.5f, 0.5f, CUBE_COLOR,},
};

short cube_indices[] =
{
        0, 1, 2,    // side 1
        2, 1, 3,
        4, 0, 6,    // side 2
        6, 0, 2,
        7, 5, 6,    // side 3
        6, 5, 4,
        3, 1, 7,    // side 4
        7, 1, 5,
        4, 5, 0,    // side 5
        0, 5, 1,
        3, 7, 2,    // side 6
        2, 7, 6,
};
*/
CUSTOMVERTEX lines[] = 
{
	{ 0.0f, 0.1f, 0.0f, XAXIS_COLOR,  },
	{ 5.0f, 0.1f, 0.0f, XAXIS_COLOR, },
	{ 0.0f, 0.1f, 0.0f, YAXIS_COLOR,  },
	{ 0.0f, 5.1f, 0.0f, YAXIS_COLOR, },
	{ 0.0f, 0.1f, 0.0f, ZAXIS_COLOR, },
	{ 0.0f, 0.1f, 5.0f, ZAXIS_COLOR,  },
};

Primitive::Primitive(){
	node_i = NULL;
	node_v = NULL;
}

void Primitive::load_obj(){
	vector<float> vertices;
	vector<short> faces;
	ifstream ifile;
	CUSTOMVERTEX *v; 
	short *f;
	DWORD color;
	switch (p_type ){
	case CUBE:
		ifile.open( "models/cube.obj" );
		color = CUBE_COLOR;
		break;
	case SPHERE:
		ifile.open( "models/sphere.obj" );
		color = SPHERE_COLOR;
		break;
	case GROUND:
		ifile.open( "models/ground.obj" );
		color = GROUND_COLOR;
		break;
	}
	if( !ifile )
		exit ( -1 );
	while( ifile ){
		char ch;
		ifile >> ch;
		if( !ifile )
			break;
		if( ch == 'v' ){
			float x,y,z;
			ifile >> x ;
			ifile >> y ;
			ifile >> z ;
			vertices.push_back( x );
			vertices.push_back( y );
			vertices.push_back( z );
		}
		else if( ch == 'f' ){
			short x,y,z;
			ifile >> x ;
			ifile >> y ;
			ifile >> z ;
			faces.push_back( x );
			faces.push_back( y );
			faces.push_back( z );
		}
	}
	int i;
	vertex_number = vertices.size()/3;
	v = new CUSTOMVERTEX[vertex_number];
	
	for( i=0; i < vertex_number ; i++ ){
		v[i].X = vertices[i*3+0];
		v[i].Y = vertices[i*3+1];
		v[i].Z = vertices[i*3+2];
		v[i].COLOR = color;
	}
	face_number = faces.size()/3;
	f = new short[face_number*3];
	for( i=0; i < face_number*3 ; i++ ){
		f[i] = faces[i];
	}
	ifile.close();
	node_v = v;
	node_i = f;
	switch (p_type ){
	case CUBE:
		cube_vertices = v;
		cube_indices = f ;
		break;
	case SPHERE:
		sphere_vertices = v;
		sphere_indices = f;
		break;
	case GROUND:
		ground_vertices = v;
		ground_indices = f;
		break;
	}
}

void Primitive::save_d3dxcreate2file( int type, float dsize, char *filepath ){
	
	D3DXCreateSphere(
		d3d_ctrl.d3ddev, 
		dsize,
		30,
		30,
		&node_mesh,  
		0
		);
	LPD3DXMESH clone;
	node_mesh ->CloneMeshFVF(D3DXMESH_SYSTEMMEM, D3DFVF_XYZ, d3d_ctrl.d3ddev, &clone); // Trying to get only the xyz data out
	node_mesh = clone;
	vertex_number = node_mesh->GetNumVertices();
	face_number = node_mesh->GetNumFaces();	

	node_mesh->GetIndexBuffer( &i_buffer );
	node_mesh->GetVertexBuffer( &v_buffer );
	
	VOID* pVoid;    // a void pointer
	
	float *tmp_v = new float[vertex_number*3];
	short *tmp_i = new short[face_number*3];

	// lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(tmp_v, pVoid, vertex_number*3*sizeof(float));
    v_buffer->Unlock();

	// lock i_buffer and load the indices into it
	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(tmp_i, pVoid, face_number*3*sizeof(short));
	i_buffer->Unlock();

	ofstream ofile( filepath );
	if( !ofile )
		exit(-1);
	int i;
	for( i = 0 ; i < vertex_number*3 ; i+=3 ){
		ofile << "v " << 	tmp_v[i] << ' ' <<  tmp_v[i+1] << ' '	<< tmp_v[i+2] << endl;
	}
	for( i = 0 ; i < face_number*3 ; i+=3 ){
		ofile << "f " << 	tmp_i[i] << ' ' <<  tmp_i[i+1] << ' '	<< tmp_i[i+2] << endl;
	}
	ofile.close();
}

void Primitive::create( int type, float size ){
	p_type = type;
	switch (p_type ){
	case CUBE:
		if( !cube_vertices ){
			load_obj( );
			create_vbuffer();
			create_ibuffer();
		}
		else{
			node_v = cube_vertices;
			node_i = cube_indices;
			vertex_number = 8;
			face_number = 6*2;

			create_vbuffer();
			create_ibuffer();
		}
		/*
		node_v = cube_vertices;
		node_i = cube_indices;

		vertex_number = 8;
		face_number = 6*2;

		create_vbuffer();
		create_ibuffer();
		*/
		break;
	case SPHERE:	
		if( !sphere_vertices ){
			load_obj();
			create_vbuffer();
			create_ibuffer();
		}
		else{
			node_v = sphere_vertices;
			node_i = sphere_indices;
			vertex_number = 872;
			face_number = 1740;

			create_vbuffer();
			create_ibuffer();
		}
		break;
	case GROUND:
		if( !ground_vertices ){
			load_obj();
			create_vbuffer();
			create_ibuffer();
		}
		else{
			node_v = ground_vertices;
			node_i = ground_indices;
			vertex_number = 4;
			face_number = 2;

			create_vbuffer();
			create_ibuffer();
		}
		/*
		node_v = ground_vertices;
		node_i = ground_indices;

		vertex_number = 4;
		face_number = 2;
		
		create_vbuffer();
		create_ibuffer();
		*/
		break;
	}
}

void Primitive::create_vbuffer(){
	d3d_ctrl.d3ddev->CreateVertexBuffer(vertex_number*sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);
	
	VOID* pVoid;    // a void pointer

    // lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, node_v, vertex_number*sizeof(CUSTOMVERTEX));
    v_buffer->Unlock();
}

void Primitive::create_ibuffer(){
	d3d_ctrl.d3ddev->CreateIndexBuffer(face_number*3*sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);

	VOID* pVoid;    // a void pointer

    // lock i_buffer and load the indices into it
    i_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, node_i, face_number*3*sizeof(short));
    i_buffer->Unlock();
}

void Primitive::release(){
	if( i_buffer)
		i_buffer->Release();
	if( v_buffer )
		v_buffer->Release();
}

void Primitive::draw(){
	d3d_ctrl.d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	d3d_ctrl.d3ddev->SetIndices(i_buffer);
	d3d_ctrl.d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertex_number, 0, face_number);
}


