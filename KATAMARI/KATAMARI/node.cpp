#include "StdAfx.h"
#include "node.h"
#include "d3d_control.h"
#include "basic_shape.h"
#include "time.h"


extern LPDIRECT3DVERTEXBUFFER9 line_buffer ;

Main_role main_role;
Enemy enemy;
Camera camera;
RootScene rs;

int node::nodenumber = 0;

node::node(){
	this->parent = NULL;
	this->size = 1.0f;
}

void node::create( int type, float size ){
	// node setting
	this->nodeid = ++nodenumber;
	this->nodetype = type;
	this->size = size;
	this->nodelevel = this->parent?this->parent->nodelevel+1:0;

	D3DXMatrixIdentity(&matLocal);
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matParent);
	D3DXMatrixIdentity(&matModelView);
	D3DXMatrixIdentity(&matSelfSpin);
	this->cd_operation = append_sibling_node;

	primitive.create( type, size );
	scale( size, size, size );
}

void node::push_child( node * ch ){
	child.push_back( ch );
}

void node::release(){
	for( nodelist::iterator it=child.begin(); it != child.end(); ++it ){
		(*it)->release();
		delete (*it);
		(*it) = NULL;
	}
}

void node::selfspin( float x, float y, float z){
	D3DXMATRIX tmp;
	D3DXMatrixRotationX(&tmp, x);
	matSelfSpin =  tmp * matSelfSpin;
	D3DXMatrixRotationY(&tmp, y);
	matSelfSpin =  tmp * matSelfSpin;
	D3DXMatrixRotationZ(&tmp, z);
	matSelfSpin =  tmp * matSelfSpin;
}

void node::rotate( float x, float y, float z){
	D3DXMATRIX tmp;
	D3DXMatrixRotationX(&tmp, x);
	matModelView = tmp * matModelView;
	D3DXMatrixRotationY(&tmp, y);
	matModelView = tmp * matModelView;
	D3DXMatrixRotationZ(&tmp, z);
	matModelView = tmp * matModelView;
}

void node::translate( float x, float y, float z){
	D3DXMATRIX tmp;
	D3DXMatrixTranslation(&tmp, x, y, z);
	matModelView = tmp * matModelView;
}

void node::scale( float x, float y, float z){
	D3DXMATRIX tmp;
	D3DXMatrixScaling(&tmp, x, y, z);
	matScale = tmp * matScale ;
}

void node::calc_center(){
	center[0] = matLocal._41;
	center[1] = matLocal._42;
	center[2] = matLocal._43;
}

WCHAR * node::get_type_string(){
	switch( nodetype ){
	case CUBE:
		return L"cube";
		break;
	case SPHERE:
		return L"sphere";
		break;
	case GROUND:
		return L"ground";
		break;
	case LINE:
		return L"line";
		break;
	default:
		return L"unknown";
	}
}

void node::draw( D3DXMATRIX& matWorld ){

	D3DXMatrixIdentity(&matLocal);

	matLocal = matSelfSpin * matModelView * matParent * matWorld;

	calc_center();

	swprintf_s(s_information,STRING_SIZE, L"%sid: %d, type:%s, (%.2f, %.2f, %.2f), size:%.0f \n",
		s_information,
		this->nodeid,
		get_type_string(),
		center[0],
		center[1],
		center[2],
		this->size
		);

	// draw lines for debug
	d3d_ctrl.draw_axis( matLocal );
	
	d3d_ctrl.setWorldTransform( (matScale*matLocal) );
	primitive.draw();
	
	//draw child
	int count = 0;
	count = 0;
	for( nodelist::iterator it = child.begin(); it != child.end() ; it++ ){
		if( count == 0 ){
			for( int i = 0 ; i < nodelevel ; ++i ){
				swprintf_s( s_information,STRING_SIZE, L"%s    ", s_information );
			};
			swprintf_s( s_information,STRING_SIZE, L"%s     id: %d's child: \n",
				s_information,
				this->nodeid
				);
		}
		(*it)->draw( matLocal );
		count++;
	}
	if( count != 0 ){
		for( int i = 0 ; i < nodelevel ; ++i ){
				swprintf_s( s_information,STRING_SIZE, L"%s    ", s_information );
			};
		swprintf_s( s_information,STRING_SIZE, L"%s     end of id: %d's child: \n",
			s_information,
			this->nodeid
			);
	}
}

void node::reset(){
	D3DXMatrixIdentity( &matModelView );
	D3DXMatrixIdentity(&matSelfSpin);
	for( nodelist::iterator it = child.begin(); it != child.end() ; it++ ){
		(*it)->reset();
	}
}

bool bounding_sphere_checking( node* a, node* b ){
	float ax,ay,az,bx,by,bz,ra,rb;
	float distance, rlength;
	//calculate radius of the objects
	ra = a->size/2.0f;
	rb = b->size/2.0f;
	ax = a->center[0];
	ay = a->center[1];
	az = a->center[2];
	bx = b->center[0];
	by = b->center[1];
	bz = b->center[2];
	distance = (ax-bx)*(ax-bx) + (ay-by)*(ay-by)+(az-bz)*(az-bz);
	rlength = (ra+rb)*(ra+rb);
	return (distance<=rlength);
}

void calculate_collision_point( node *a, node *b ){
	D3DXMATRIX tmp;
	float ax,ay,az,bx,by,bz,ra,rb;
	float distance_a2b,diff;

	ra = a->size/2.0f;
	rb = b->size/2.0f;

	ax = a->center[0];	ay = a->center[1];	az = a->center[2];
	bx = b->center[0];	by = b->center[1];	bz = b->center[2];

	distance_a2b = sqrt( (ax-bx)*(ax-bx) + (ay-by)*(ay-by) + (az-bz)*(az-bz) );
	diff = -((ra+rb)-distance_a2b)/distance_a2b;
	
	D3DXMatrixTranslation( &tmp, diff*(ax-bx),diff*(ay-by),diff*(az-bz) );
	b->matLocal = tmp * b->matLocal;
}

void node::append_sibling_node( node *a, node *b ){
	//append the enemy into main_role
	//remove the enemy
	a->push_child( b );
	node *bparent = b->parent;
	bparent->child.remove( b );
	b->nodelevel = a->nodelevel+1;

	//calculate the joint displacement
	calculate_collision_point( a, b );
	//calculate local displacement
	D3DXMATRIX tmp;
	D3DXMatrixInverse( &tmp, NULL, &(a->matLocal) );
	b->matParent = b->matLocal* tmp;
}

void node::collision_detection( node *a ){
	//test itself
	if( bounding_sphere_checking( this, a ) ){
		cd_operation( this, a );
		return ;
	}

	//test child
	for( nodelist::iterator it = child.begin(); it != child.end() ; it++ ){
		(*it)->collision_detection( a );
	}
}

node *Main_role::create(){
	//
	//decide the size of main role
	//create the main role
	//set the parent to NULL
	//set the distance from ground_plane to main role

	float size;
	size = 4.0f;
	node *main_char = new node;
	main_char->parent = NULL;
	main_char->create( SPHERE, size );
	D3DXMatrixTranslation(&(main_char->matParent),0.0f,size/2.0f,0.0f);
	main_char->calc_center();
	root = main_char;

	return root;
}

void Enemy::random_create( int create_number ){
	for( ; create_number >0 ; create_number-- ){
		node *new_enemy = new node;
		float node_size = (float)(rand()%MAX_ENEMY_SIZE) + (float)MIN_ENEMY_SIZE;
		int node_type = ( rand()%2 == 0 )? CUBE : SPHERE ;
		float x, z;
		x = (float)(rand()%(2*MAX_D_TO_MAIN_ROLE) - MAX_D_TO_MAIN_ROLE);
		z = (float)(rand()%(2*MAX_D_TO_MAIN_ROLE) - MAX_D_TO_MAIN_ROLE);

		new_enemy->parent = root;
		new_enemy->create( node_type, node_size );
		D3DXMatrixTranslation(&(new_enemy->matParent),x,node_size/2.0f,z);
		new_enemy->calc_center();
		root->push_child( new_enemy );
	}
}


node *Enemy::create(){
	//init random seed
	srand((unsigned int)time(NULL));
	//simulate some init ememy
	//set the distance from ground_plane to enemy

	float size = 2.0f;
	root = new node;
	root->parent = NULL;
	root->nodelevel = 0;

	random_create( 10 );

	return root;
}

void Enemy::draw( D3DXMATRIX &matWorld ){
	nodelist child = root->child;
	for( nodelist::iterator it = child.begin(); it != child.end() ; it++ ){
		(*it)->draw( matWorld );
	}
}

void RootScene::reset(){
	root->reset();
	camera.reset();
}

void RootScene::release(){
	if( root )
		root->release();
}

void RootScene::draw(){
	if( !root )
		return;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	swprintf_s(s_information,STRING_SIZE, F1_INFO ); 

	//draw background
	ground_plan.draw();

	//draw enemy
	enemy.draw( matWorld );

	//draw main_role
	main_role.root->draw( matWorld );

	//detect all enemy
	nodelist ch = enemy.root->child;
	for( nodelist::iterator it = ch.begin(); it != ch.end() ; it++ ){
		main_role.root->collision_detection(*it);
	}
}

void RootScene::create(){
	root = new node ;
	
	//camera
	Vector3 eye(CAMERA_POS),lookat(LOOKAT_POS),up(UP_DIR);
	camera.create( eye, lookat, up );

	//background
	ground_plan.create( GROUND, 1.0f );

	//main_role
	root->push_child( main_role.create() );
	//enemy
	root->push_child( enemy.create() );
}