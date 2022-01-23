#include "Camera.h"
#include<memory>

	Camera::Camera():   QYaw(),
						QPitch(),
						QRoll(),
						Projection(),
						View()
	{
		Look.x=0.0f;
		Look.y=0.0f;
		Look.z=1.0f;

		Right.x=1.0f;
		Right.y=0.0f;
		Right.z=0.0f;

		Up.x=0.0f;
		Up.y=1.0f;
		Up.z=0.0f;

		Position.x = 0.0f;
		Position.y = 0.0f;
		Position.z = -50.0f;
	};


	Camera::~Camera(){};

	void Camera::SetProjectionMatrix(const POD_Math::Matrix4x4& proj)
	{
		memcpy(&Projection, &proj,sizeof(POD_Math::Matrix4x4));
	};

	POD_Math::Matrix4x4& Camera::GetProjectionMatrix()
	{
		return Projection;
	};

	POD_Math::Matrix4x4& Camera::GetViewMatrix()
	{
		View._11 = Right.x;  View._12 = Up.x;  View._13 = Look.x;   View._14 = 0.0f;
		View._21 = Right.y;  View._22 = Up.y;  View._23 = Look.y;   View._24 = 0.0f;
		View._31 = Right.z;  View._32 = Up.z;  View._33 = Look.z;   View._34 = 0.0f;

		View._41 = -POD_Math::Vector3Dot(Position,Right);
		View._42 = -POD_Math::Vector3Dot(Position,Up);
		View._43 = -POD_Math::Vector3Dot(Position,Look);
		View._44 = 1.0f;

		return View;
	}; 


	void Camera::Yaw(const float step)   
	{
		POD_Math::Vector3 v= { 0.0f, 1.0f, 0.0f };
		POD_Math::QuatAxis(QYaw,v,step);

		POD_Math::QuatMul(Look,QYaw,Look);
		POD_Math::QuatMul(Right,QYaw,Right);
		POD_Math::QuatMul(Up,QYaw,Up);
	};

	void Camera::Pitch(const float step)
	{
		POD_Math::QuatAxis(QPitch, Right, step);
		POD_Math::QuatMul(Look,QPitch,Look);
		POD_Math::QuatMul(Up,QPitch,Up);
	};

	void Camera::Roll(const float step)
	{
		POD_Math::Vector3 v={ 0.0f, 0.0f, 1.0f};
		POD_Math::QuatAxis(QRoll, v, step);

		POD_Math::QuatMul(Look,QRoll,Look);
		POD_Math::QuatMul(Right,QRoll,Right);
		POD_Math::QuatMul(Up,QRoll,Up);
	};

	void Camera::Forward(const float step)
	{
		POD_Math::Vector3 GO={0.0f, 0.0f, 0.0f};
		POD_Math::Vector3Mul(GO,Look,step);
		POD_Math::Vector3Add(Position,Position,GO);
	};

	void Camera::Backward(const float step)
	{
		POD_Math::Vector3 GO={0.0f, 0.0f, 0.0f};
		POD_Math::Vector3Mul(GO,Look,step);
		POD_Math::Vector3Sub(Position,Position,GO);
	};

	void Camera::Leftward(const float step)
	{
		POD_Math::Vector3 GO={0.0f, 0.0f, 0.0f};
		POD_Math::Vector3Mul(GO,Right,step);
		POD_Math::Vector3Sub(Position,Position,GO);
	};

	void Camera::Rightward(const float step)
	{
		POD_Math::Vector3 GO={0.0f, 0.0f, 0.0f};
		POD_Math::Vector3Mul(GO,Right,step);
		POD_Math::Vector3Add(Position,Position,GO);
	};

	void Camera::Upward(const float step)
	{
		POD_Math::Vector3 GO={0.0f, 0.0f, 0.0f};
		POD_Math::Vector3Mul(GO,Up,step);
		POD_Math::Vector3Add(Position,Position,GO);
	};

	void Camera::Downward(const float step)
	{
		POD_Math::Vector3 GO={0.0f, 0.0f, 0.0f};
		POD_Math::Vector3Mul(GO,Up,step);
		POD_Math::Vector3Add(Position,Position,GO);
	};
