#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <list>
#include "basic_shape.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define LINE 0
#define SPHERE 1
#define CUBE 2
#define GROUND 3

#define MAX_ENEMY_SIZE 5
#define MIN_ENEMY_SIZE 1
#define MAX_D_TO_MAIN_ROLE 50

using namespace::std;

class node{
private:
	static void append_sibling_node( node *, node * );
	void (*cd_operation)(node *, node *) ;
	static int nodenumber;
public:
	
	node();

	int nodeid;
	int nodetype;
	int nodelevel;

	node *parent;
	
	typedef list<node*>  nodelist;	
	nodelist child;

	Primitive primitive;

	float size;
	float center[3];

	D3DXMATRIX matLocal;
	D3DXMATRIX matScale;
	D3DXMATRIX matSelfSpin;
	D3DXMATRIX matModelView;
	D3DXMATRIX matParent;

	// node construction & destruction
	void create( int type, float size );
	void release();
	void push_child( node * );
	void calc_center();
	
	// node operations
	void selfspin( float, float, float );
	void rotate( float, float, float );
	void translate( float, float, float );
	void scale( float, float, float );
	void draw( D3DXMATRIX& );
	void reset();
	
	WCHAR* get_type_string();
	void collision_detection( node* );
};

typedef list<node*>  nodelist;

class RootScene{
public:
	void create();
	
	void reset();
	void release();
	void draw();
	void roll( float );
	node *root;

	Primitive ground_plan ;
};

class Enemy {
public:
	node *create();
	void draw( D3DXMATRIX& );
	void random_create( int );
	node *root;
};

class Main_role{
public:
	node *create();
	node *root;
};

extern Primitive ground_plan;
extern Main_role main_role;
extern Enemy enemy;
extern RootScene rs;