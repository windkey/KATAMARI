﻿#include "StdAfx.h"
#include "webcamera.h"

CameraClass webcm;

//-----------------------------------------------------------------------------
// Desc: 构造函数
//-----------------------------------------------------------------------------
CameraClass::CameraClass(IDirect3DDevice9 *pd3dDevice)
{
    m_pd3dDevice = pd3dDevice;
    m_vRightVector  = D3DXVECTOR3(1.0f, 0.0f, 0.0f);   // 默认右向量与X正半轴重合
    m_vUpVector     = D3DXVECTOR3(0.0f, 1.0f, 0.0f);   // 默认上向量与Y正半轴重合
    m_vLookVector   = D3DXVECTOR3(0.0f, 0.0f, 1.0f);   // 默认观察向量与Z正半轴重合
    m_vCameraPosition  = D3DXVECTOR3(-5.0f, 30.0f, -30.0f);   // 默认摄像机坐标为(-5.0f, 30.0f, -30.0f)
    m_vTargetPosition    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//默认观察目标位置为(0.0f, 0.0f, 0.0f);

}


//-----------------------------------------------------------------------------
// Name：CameraClass::CalculateViewMatrix( )
// Desc: 根据给定的矩阵计算出取景变换矩阵
//-----------------------------------------------------------------------------
VOID CameraClass::CalculateViewMatrix(D3DXMATRIX *pMatrix) 
{
    //1.先把3个向量都规范化并使其相互垂直，成为一组正交矩阵
    D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);  //规范化观察分量
    D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);    // 上向量与观察向量垂直
    D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);                // 规范化上向量
    D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);    // 右向量与上向量垂直
    D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);          // 规范化右向量


    // 2.创建出取景变换矩阵
    //依次写出取景变换矩阵的第一行
    pMatrix->_11 = m_vRightVector.x;           // Rx
    pMatrix->_12 = m_vUpVector.x;              // Ux
    pMatrix->_13 = m_vLookVector.x;            // Lx
    pMatrix->_14 = 0.0f;
    //依次写出取景变换矩阵的第二行
    pMatrix->_21 = m_vRightVector.y;           // Ry
    pMatrix->_22 = m_vUpVector.y;              // Uy
    pMatrix->_23 = m_vLookVector.y;            // Ly
    pMatrix->_24 = 0.0f;
    //依次写出取景变换矩阵的第三行
    pMatrix->_31 = m_vRightVector.z;           // Rz
    pMatrix->_32 = m_vUpVector.z;              // Uz
    pMatrix->_33 = m_vLookVector.z;            // Lz
    pMatrix->_34 = 0.0f;
    //依次写出取景变换矩阵的第四行
    pMatrix->_41 = -D3DXVec3Dot(&m_vRightVector, &m_vCameraPosition);    // -P*R
    pMatrix->_42 = -D3DXVec3Dot(&m_vUpVector, &m_vCameraPosition);       // -P*U
    pMatrix->_43 = -D3DXVec3Dot(&m_vLookVector, &m_vCameraPosition);     // -P*L
    pMatrix->_44 = 1.0f;
}


//-----------------------------------------------------------------------------
// Name：CameraClass::SetTargetPosition( )
// Desc: 设置摄像机的观察位置
//-----------------------------------------------------------------------------
VOID CameraClass::SetTargetPosition(D3DXVECTOR3 *pLookat) 
{
    //先看看pLookat是否为默认值NULL
    if (pLookat != NULL)  m_vTargetPosition = (*pLookat);
    else m_vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    m_vLookVector = m_vTargetPosition - m_vCameraPosition;//观察点位置减摄像机位置，得到观察方向向量
    D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);//规范化m_vLookVector向量

    //正交并规范化m_vUpVector和m_vRightVector
    D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);
    D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);
    D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);
    D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);
}

//-----------------------------------------------------------------------------
// Name：CameraClass::SetCameraPosition( )
// Desc: 设置摄像机所在的位置
//-----------------------------------------------------------------------------
VOID CameraClass::SetCameraPosition(D3DXVECTOR3 *pVector) 
{
    D3DXVECTOR3 V = D3DXVECTOR3(-5.0f, 30.0f, -30.0f);
    m_vCameraPosition = pVector ? (*pVector) : V;//三目运算符，如果pVector为真的话，
                                                                        //返回*pVector的值（即m_vCameraPosition=*pVector），
                                                                        //否则返回V的值（即m_vCameraPosition=V）
    //m_vLookVector=m_vTargetPosition-m_vCameraPosition;
    //D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);  //规范化观察分量
}

//-----------------------------------------------------------------------------
// Name：CameraClass::SetViewMatrix( )
// Desc: 设置取景变换矩阵
//-----------------------------------------------------------------------------
VOID CameraClass::SetViewMatrix(D3DXMATRIX *pMatrix) 
{
    //根据pMatrix的值先做一下判断
    if (pMatrix) m_matView = *pMatrix;
    else CalculateViewMatrix(&m_matView);
    m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);
    //把取景变换矩阵的值分下来分别给右分量，上分量，和观察分量
    m_vRightVector = D3DXVECTOR3(m_matView._11, m_matView._12, m_matView._13);
    m_vUpVector    = D3DXVECTOR3(m_matView._21, m_matView._22, m_matView._23);
    m_vLookVector  = D3DXVECTOR3(m_matView._31, m_matView._32, m_matView._33);
}

//-----------------------------------------------------------------------------
// Name：CameraClass::MoveAlongRightVec( )
// Desc: 沿右向量平移fUnits个单位
//-----------------------------------------------------------------------------
VOID CameraClass::MoveAlongRightVec(FLOAT fUnits) 
{
    //直接乘以fUnits的量来累加就行了
    m_vCameraPosition += m_vRightVector * fUnits;
    m_vTargetPosition   += m_vRightVector * fUnits;
}

//-----------------------------------------------------------------------------
// Name：CameraClass::MoveAlongUpVec( )
// Desc:  沿上向量平移fUnits个单位
//-----------------------------------------------------------------------------
VOID CameraClass::MoveAlongUpVec(FLOAT fUnits) 
{
    //直接乘以fUnits的量来累加就行了
    m_vCameraPosition += m_vUpVector * fUnits;
    m_vTargetPosition   += m_vUpVector * fUnits;
}

//-----------------------------------------------------------------------------
// Name：CameraClass::MoveAlongLookVec( )
// Desc:  沿观察向量平移fUnits个单位
//-----------------------------------------------------------------------------
VOID CameraClass::MoveAlongLookVec(FLOAT fUnits) 
{
    //直接乘以fUnits的量来累加就行了
    m_vCameraPosition += m_vLookVector * fUnits;
    m_vTargetPosition   += m_vLookVector * fUnits;
}

//-----------------------------------------------------------------------------
// Name：CameraClass::RotationRightVec( )
// Desc:  沿右向量旋转fAngle个弧度单位的角度
//　第一人称
//-----------------------------------------------------------------------------
VOID CameraClass::RotationRightVec(FLOAT fAngle) 
{
    D3DXMATRIX R;
    D3DXMatrixRotationAxis(&R, &m_vRightVector, fAngle);//创建出绕m_vRightVector旋转fAngle个角度的R矩阵
    D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);//让m_vUpVector向量绕m_vRightVector旋转fAngle个角度
    D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);//让m_vLookVector向量绕m_vRightVector旋转fAngle个角度

    m_vTargetPosition = m_vLookVector * D3DXVec3Length(&m_vCameraPosition);//更新一下观察点的新位置（方向乘以模=向量）
}

//-----------------------------------------------------------------------------
// Name：CameraClass::RotationUpVec( )
// Desc:  沿上向量旋转fAngle个弧度单位的角度
// 第一人称
//-----------------------------------------------------------------------------
VOID CameraClass::RotationUpVec(FLOAT fAngle) 
{
    D3DXMATRIX R;
    D3DXVECTOR3 m_matup= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXMatrixRotationAxis(&R, &m_matup, fAngle);//创建出绕上向量旋转fAngle个角度的R矩阵   ，为了保持视野平衡对称
    D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);//让m_vRightVector向量绕m_vUpVector旋转fAngle个角度
    D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);//让m_vLookVector向量绕m_vUpVector旋转fAngle个角度

    m_vTargetPosition = m_vLookVector * D3DXVec3Length(&m_vCameraPosition);//更新一下观察点的新位置（方向乘以模=向量）
}

//-----------------------------------------------------------------------------
// Name：CameraClass::RotationLookVec( )
// Desc:  沿观察向量旋转fAngle个弧度单位的角度
// 第一人称
//-----------------------------------------------------------------------------
VOID CameraClass::RotationLookVec(FLOAT fAngle) 
{
    D3DXMATRIX R;
    D3DXMatrixRotationAxis(&R, &m_vLookVector, fAngle);//创建出绕m_vLookVector旋转fAngle个角度的R矩阵
    D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);//让m_vRightVector向量绕m_vLookVector旋转fAngle个角度
    D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);//让m_vUpVector向量绕m_vLookVector旋转fAngle个角度

    m_vTargetPosition = m_vLookVector * D3DXVec3Length(&m_vCameraPosition);//更新一下观察点的新位置（方向乘以模=向量）
}

//---------------------------------------------------------------------------------
//第3人称摄象机旋转      角色跟随，需要setTargetePosition和setCameraPosition实现左右平移
//---------------------------------------------------------------------------------
void CameraClass::ThirdPersonRotationX(float angle)
{
    D3DXMATRIX matRotationX;
    D3DXVECTOR3 vNewEye,vTemp;

    //D3DXVec3Normalize(&m_vNewZAxis,&(m_vLookat-m_vEye));
	//叉乘求得right向量

    D3DXVec3Cross(&m_vNewXAxis, &(m_vTargetPosition-m_vCameraPosition), &m_vUpVector);

    D3DXMatrixRotationAxis(&matRotationX, &m_vNewXAxis, angle);//求得绕right旋转angle角度的矩阵
    D3DXVec3TransformCoord(&vNewEye,&(m_vCameraPosition-m_vTargetPosition),&matRotationX);
    
    D3DXVec3TransformCoord(&m_vLookVector,&m_vLookVector,&matRotationX);
    D3DXVec3TransformCoord(&m_vUpVector,&m_vUpVector,&matRotationX);
    vNewEye += m_vTargetPosition;

    D3DXVec3Normalize(&vTemp,&(m_vTargetPosition - vNewEye));
    float fAngle = D3DXVec3Dot(&vTemp,&m_vUpVector);
    if(fAngle < (-0.95f) || fAngle > 0.95f)
    {
        return ;
    }

    m_vCameraPosition = vNewEye;
}

void CameraClass::ThirdPersonRotationY(float angle)
{
    D3DXMATRIX matRotationY;
    D3DXVECTOR3 vtemp;
        D3DXVECTOR3 m_matup= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXMatrixRotationAxis(&matRotationY, &m_matup, angle);//求得绕标准上向量旋转angle的矩阵，保持法线向上
    D3DXVec3TransformCoord(&vtemp,&(m_vCameraPosition-m_vTargetPosition),&matRotationY);

    D3DXVec3TransformCoord(&m_vRightVector,&m_vRightVector,&matRotationY);
    D3DXVec3TransformCoord(&m_vLookVector,&m_vLookVector,&matRotationY);

    m_vCameraPosition= m_vTargetPosition+vtemp;

}
//-----------------------------------------------------------------------------
// Desc: 析构函数
//-----------------------------------------------------------------------------
CameraClass::~CameraClass(void)
{
}