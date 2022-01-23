#ifndef CAMERA_H
#define CAMERA_H

	// No frustum :(
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4x4.h"


	class Camera
	{

	public:

		Camera();
		~Camera();

		void SetProjectionMatrix(const POD_Math::Matrix4x4&);
		POD_Math::Matrix4x4& GetProjectionMatrix();
		POD_Math::Matrix4x4&  GetViewMatrix(); 
		
		void Yaw(const float);
		void Pitch(const float);
		void Roll(const float);

		void Forward(const float);
		void Backward(const float);
		void Leftward(const float);
		void Rightward(const float);
		void Upward(const float);
		void Downward(const float);

	protected:

		POD_Math::Vector3 Position;
		POD_Math::Vector3 Right;
		POD_Math::Vector3 Up;
		POD_Math::Vector3 Look;

		POD_Math::Quaternion QYaw;
		POD_Math::Quaternion QPitch;
		POD_Math::Quaternion QRoll;

		POD_Math::Matrix4x4 Projection;
		POD_Math::Matrix4x4 View;
	};

#endif