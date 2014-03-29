#include "StdAfx.h"
#include "d3d_control.h"
#include "node.h"
#include "basic_shape.h"
#include <d3d9.h>
#include <d3dx9.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

d3d_control d3d_ctrl;


ID3DXFont *d3dfont=NULL;
WCHAR *s_information;

d3d_control::d3d_control(void)
{
	screen_width = SCREEN_WIDTH;
	screen_height = SCREEN_HEIGHT;
	using_camera = true;
	prev_mouse_pos[0] = mouse_pos[0] = SCREEN_WIDTH/2;
	prev_mouse_pos[1] = mouse_pos[1] = SCREEN_HEIGHT/2;
	mouse_clicking = false;
}


d3d_control::~d3d_control(void)
{
}

bool d3d_control::input_handler( MSG msg ){
	if(msg.message == WM_KEYDOWN ){
		switch (msg.wParam ){
		case VK_ESCAPE:
			return false;
		case VK_F :
			d3d_ctrl.fillmode_changed = true;
			break;
		case VK_R :
			d3d_ctrl.world_reset();
			break;
		case VK_UP :
			main_role.root->translate(0.0f,0.0f,MOVE_SPEED);
			main_role.root->selfspin(ROTATE_SPEED,0.0f,0.0f);
			camera.translate(0.0f,0.0f,MOVE_SPEED);
			break;
		case VK_DOWN :
			main_role.root->translate(0.0f,0.0f,-MOVE_SPEED);
			main_role.root->selfspin(-ROTATE_SPEED,0.0f,0.0f);
			camera.translate(0.0f,0.0f,-MOVE_SPEED);
			break;
		case VK_LEFT :
			main_role.root->rotate(0.0f,ROTATE_SPEED,0.0f);
			camera.rotate(0.0f,ROTATE_SPEED,0.0f);
			break;
		case VK_RIGHT :
			main_role.root->rotate(0.0f,-ROTATE_SPEED,0.0f);
			camera.rotate(0.0f,-ROTATE_SPEED,0.0f);
			break;
		case VK_Q :
			break;
		case VK_A :
			camera.move_right_dir( -MOVE_SPEED );
			break;
		case VK_W :
			camera.move_up_dir( MOVE_SPEED );
			break;
		case VK_S :
			camera.move_up_dir( -MOVE_SPEED );
			break;
		case VK_E :
			break;
		case VK_D :
			camera.move_right_dir( MOVE_SPEED );
			break;
		case VK_P :
			using_camera	= !using_camera;
			break;
		}
	}
	else if((msg.wParam & MK_LBUTTON)&&(msg.message ==  WM_MOUSEMOVE)){
		int x = (short) LOWORD(msg.lParam);
		int y = (short) HIWORD(msg.lParam);
		
		//swprintf_s(s_information,STRING_SIZE, L"%s x:%d y%d\n", s_information, x,y );
		
		if( !mouse_clicking ){
			prev_mouse_pos[0] = mouse_pos[0] = x ; prev_mouse_pos[1] = mouse_pos[1] =y;
			mouse_clicking = true;
		}
		else{
			mouse_pos[0] = x; mouse_pos[1] = y;
			camera.mouse_move( mouse_pos[0]-prev_mouse_pos[0], mouse_pos[1]-prev_mouse_pos[1] );
		}
	}
	else if( msg.message == WM_LBUTTONUP ){
		mouse_clicking = false;
		prev_mouse_pos[0]= mouse_pos[0] = d3d_ctrl.screen_width/2;
		prev_mouse_pos[1]= mouse_pos[1] = d3d_ctrl.screen_height/2;
	}
	return true;
}

void d3d_control::init_axis(){
	d3ddev->CreateVertexBuffer(3*2*sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&line_buffer,
		NULL);

	VOID* pVoid;    // a void pointer
    
	// lock v_buffer and load the vertices into it
    line_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, lines, 3*2*sizeof(CUSTOMVERTEX));
    line_buffer->Unlock();
}

void d3d_control::init_text(){
   D3DXCreateFont(d3ddev,     //D3D Device
                     22,               //Font height
                     0,                //Font width
                     FW_NORMAL,        //Font Weight
                     1,                //MipLevels
                     false,            //Italic
                     DEFAULT_CHARSET,  //CharSet
                     OUT_DEFAULT_PRECIS, //OutputPrecision
                     ANTIALIASED_QUALITY, //Quality
                     DEFAULT_PITCH|FF_DONTCARE,//PitchAndFamily
                     L"Arial",          //pFacename,
                     &d3dfont);         //ppFont
   s_information = new WCHAR[STRING_SIZE];
   swprintf_s(s_information,STRING_SIZE, L"");
}

void d3d_control::draw_axis( D3DXMATRIX& matLocal ){
	
	d3ddev->SetTransform(D3DTS_WORLD, &(matLocal));

	 d3ddev->SetStreamSource(0, line_buffer, 0, sizeof(CUSTOMVERTEX));
	 d3ddev->DrawPrimitive( D3DPT_LINELIST, 0, 3 );
  }

void d3d_control::draw_text(){
	D3DXMATRIX matLocal;
	D3DXMatrixIdentity(&matLocal);
	
	d3ddev->SetTransform(D3DTS_WORLD, &(matLocal));
	RECT font_rect;
	SetRect(&font_rect,0,0,800,800);

	d3dfont->DrawText(NULL,//pSprite
		s_information,  //pString
		-1,          //Count
		&font_rect,  //pRect
		DT_LEFT|DT_NOCLIP,//Format,
		0xFFFFFFFF); //Color
}

void d3d_control::initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

    D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);
	
	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
    d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // turn off culling
    d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
	d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light
	
	d3ddev->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	init_axis();
	init_text();
	rs.create();
	fillmode_changed=false;
	is_solid = true;
}

void d3d_control::view_transform(){
	// set the view transform    
	if( using_camera )
		camera.set();
	else{
		D3DXMATRIX matView;    // the view transform matrix
		D3DXMatrixLookAtLH(&matView,
			&CAMERA_POSITION,    // the camera position
			&D3DXVECTOR3 (0.0f, 0.0f, 0.0f),      // the look-at position
			&D3DXVECTOR3 (0.0f, 1.0f, 0.0f));    // the up direction
		d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView 
	}
}

void d3d_control::projection_transform(){
	// set the projection transform
    D3DXMATRIX matProjection;    // the projection transform matrix
    D3DXMatrixPerspectiveFovLH(&matProjection,
                               D3DXToRadian(45),    // the horizontal field of view
                               (FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
                               1.0f,   // the near view-plane
                               100.0f);    // the far view-plane
    d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection); // set the projection
}

void d3d_control::setWorldTransform( D3DXMATRIX& mat){
	d3ddev->SetTransform(D3DTS_WORLD, &mat);
}

void d3d_control::setViewTransform( D3DXMATRIX& mat){
	d3ddev->SetTransform(D3DTS_VIEW, &mat );
}

void d3d_control::render_frame(void)
{
    // clear the window to a deep blue
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//change wireframe mode
	if( fillmode_changed ){
		if( !is_solid ){
			d3ddev->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
		}
		else{
			d3ddev->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
		}
		is_solid = !is_solid;
		fillmode_changed=false;
	}

    d3ddev->BeginScene();    // begins the 3D scene

	// select which vertex format we are using
	d3ddev->SetFVF(CUSTOMFVF);

	//set transform
	view_transform();
    projection_transform();

    // set the world transform
	D3DXMATRIX matLocal;
	D3DXMatrixIdentity(&matLocal);
	draw_axis( matLocal );
	draw_text();
	
	rs.draw();

    d3ddev->EndScene();    // ends the 3D scene
	d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}

void d3d_control::world_reset(){
	rs.reset();
}

// this is the function that cleans up Direct3D and COM
void d3d_control::cleanD3D(void)
{
	rs.release();
    d3ddev->Release();    // close and release the 3D device
    d3d->Release();    // close and release Direct3D
	line_buffer->Release();
}

Vector3::Vector3(){
}

Vector3::Vector3( float a, float b, float c){
	x = a; y = b; z = c;
}

Vector3 Vector3::operator-( Vector3& r){
	return Vector3(this->x-r.x,this->y-r.y,this->z-r.z);
}

Camera::Camera(){
	init_camera_pos = Vector3(CAMERA_POS);
	init_lookat_pos = Vector3(LOOKAT_POS);
	init_up_dir = Vector3(UP_DIR);
}

float vector_distance( Vector3 &n ){
	return sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
}
void normalize( Vector3 &n ){
	float d;
	d = vector_distance( n );
	n.x = n.x/d;
	n.y = n.y/d;
	n.z = n.z/d;
}

void crossproduct( Vector3& output, Vector3 a, Vector3 b ){
	//ay*bz-az*by
	output.x = a.y*b.z-a.z*b.y;
	//az*bx-ax*az
	output.y = a.z*b.x-a.x*b.z;
	//ax*by-ay*bx
	output.z = a.x*b.y-a.y*b.x;
}

float dot( Vector3 a, Vector3 b ){
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

void Camera::LookAtLH(Vector3 eye, Vector3 lookat, Vector3 up ){
	Vector3 xaxis, yaxis, zaxis;

	normalize( up );
	zaxis.x = lookat.x-eye.x; zaxis.y = lookat.y-eye.y; zaxis.z = lookat.z-eye.z;
	normalize( zaxis );
	crossproduct( xaxis, up, zaxis );
	normalize( xaxis );
	crossproduct( yaxis, zaxis, xaxis );

	matViewMatrix._11 = xaxis.x;	matViewMatrix._12 = yaxis.x;	
	matViewMatrix._13 = zaxis.x;	matViewMatrix._14 = 0.0f;

	matViewMatrix._21 = xaxis.y;	matViewMatrix._22 = yaxis.y;	
	matViewMatrix._23 = zaxis.y;	matViewMatrix._24 = 0.0f;

	matViewMatrix._31 = xaxis.z;	matViewMatrix._32 = yaxis.z;	
	matViewMatrix._33 = zaxis.z;	matViewMatrix._34 = 0.0f;

	matViewMatrix._41 = -dot(xaxis,eye);	 matViewMatrix._42 = -dot(yaxis,eye); 
	matViewMatrix._43 = -dot(zaxis,eye); matViewMatrix._44 = 1.0f;

	up_dir = yaxis;
	right_dir = xaxis;
	forward_dir = zaxis;
}

void Camera::create( Vector3 eye, Vector3 lookat, Vector3 up ){
	D3DXMatrixIdentity(&matViewMatrix);
	//init
	init_camera_pos.x = camera_pos.x = eye.x;	
	init_camera_pos.y = camera_pos.y = eye.y;
	init_camera_pos.z = camera_pos.z = eye.z;
	init_lookat_pos.x = lookat_pos.x = lookat.x;	
	init_lookat_pos.y = lookat_pos.y = lookat.y;	
	init_lookat_pos.z = lookat_pos.z = lookat.z;
	init_up_dir.x = up_dir.x = up.x; 
	init_up_dir.x = up_dir.y = up.y; 
	init_up_dir.x = up_dir.z = up.z;

	LookAtLH(camera_pos,lookat_pos,up_dir);
	matInit = matViewMatrix;
}

void Camera::set(){
	LookAtLH(camera_pos,lookat_pos,init_up_dir);
    d3d_ctrl.setViewTransform( matViewMatrix );    // set the view transform to matView 
}

void Camera::reset(){
	camera_pos = init_camera_pos;
	lookat_pos = init_lookat_pos;
	up_dir = init_up_dir;

	LookAtLH(init_camera_pos,init_lookat_pos,init_up_dir);
    d3d_ctrl.setViewTransform( matViewMatrix );    // set the view transform to matView 
}

void Camera::vectorTranslate( Vector3 ts, D3DXMATRIX matMove ){
	D3DXMATRIX tmp;
	D3DXMatrixIdentity( &tmp );
	D3DXMatrixTranslation(&tmp, ts.x, ts.y, ts.z);
	matMove._41 = 0.0f; matMove._42 = 0.0f; matMove._43 = 0.0f; matMove._44 = 1.0f;
	tmp = tmp * matMove;
	camera_pos.x += tmp._41;
	camera_pos.y += tmp._42;
	camera_pos.z += tmp._43;
}

// move the main role from lookat to 0,0,0
// recalculate the camera position relative to main role
// rotate the axis accroading to 0,0,0
// move back the main role from 0,0,0 to lookat
// move back the camera position to the same coordinate
void Camera::vectorRotate( Vector3 rs ){
	D3DXMATRIX tmp, tmp1;
	D3DXMatrixIdentity( &tmp1 );
	D3DXMatrixIdentity( &tmp );
	
	D3DXMatrixTranslation(&tmp1, lookat_pos.x, lookat_pos.y, lookat_pos.z);
	tmp = tmp1;

	// rotate rs angle belong to the center of the main_role (lookat point)
	D3DXMatrixRotationX(&tmp1, rs.x);
	tmp = tmp1 * tmp;
	D3DXMatrixRotationY(&tmp1, rs.y);
	tmp = tmp1 * tmp;
	D3DXMatrixRotationZ(&tmp1, rs.z);
	tmp = tmp1 * tmp;
	
	D3DXMatrixTranslation(&tmp1, camera_pos.x-lookat_pos.x, camera_pos.y-lookat_pos.y, camera_pos.z-lookat_pos.z);
	tmp = tmp1 * tmp;

	camera_pos.x = tmp._41;
	camera_pos.y = tmp._42;
	camera_pos.z = tmp._43;
}

void Camera::rotate( float x, float y, float z){
	float max_angle;
	max_angle = (float)M_PI/2.0f;

	lookat_pos.x = main_role.root->matLocal._41;
	lookat_pos.y = main_role.root->matLocal._42;
	lookat_pos.z = main_role.root->matLocal._43;

	vectorRotate( Vector3(x,y,z) );
}

void Camera::translate( float x, float y, float z){
	
	vectorTranslate( Vector3(x,y,z), main_role.root->matModelView*main_role.root->matParent );
	
	lookat_pos.x = main_role.root->matLocal._41;
	lookat_pos.y = main_role.root->matLocal._42;
	lookat_pos.z = main_role.root->matLocal._43;

}

void Camera::move_up_dir( float move ){
	//get up_dir
	//camera_pos += up_dir;
	camera_pos.x += (up_dir.x*move);
	camera_pos.y += (up_dir.y*move);
	camera_pos.z += (up_dir.z*move);
}

void Camera::move_right_dir( float move ){
	//get right_dir
	//camera_pos += right_dir;
	camera_pos.x += (right_dir.x*move);
	camera_pos.y += (right_dir.y*move);
	camera_pos.z += (right_dir.z*move);
}

void Camera::mouse_move( int diffx, int diffy ){
	float x,y;
	x = MAX_ROTATE* (((float)diffx)/(d3d_ctrl.screen_width/2.0f));
	y = MAX_ROTATE* (((float)diffy)/(d3d_ctrl.screen_width/2.0f));
	//swprintf_s(s_information,STRING_SIZE, L"%s mouse_move x:%.3f y%.3f\n", s_information, x,y );
	move_right_dir( x );
	move_up_dir( y );
}