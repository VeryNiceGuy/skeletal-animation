#include "Skeleton.h"
#include <new>
#include <memory>
#include <cstdio>
#include "ErrorHandler.h"      

	SkeletalAnimation::SLERP::SLERP():Keys(),NumKeys(){}; // �������� ��� ���������e - ��� � ��� �������������.
	SkeletalAnimation::SLERP::~SLERP(){};

	void SkeletalAnimation::SLERP::Initialize( Keyframe*const key, size_t Num) // ������ ��������� �� ������ ���� ������������������
	{                                                                          // ������ ���������� ������ � ������������������
		   Keys=key; NumKeys=Num;
	};

	void SkeletalAnimation::SLERP::GetInterpolated(const unsigned long t, POD_Math::Matrix4x3& i) // ������� ���� �� �������������
	{  
	   
			unsigned short Key1=0;   // ������ ������� ������
			unsigned short Key2=0;

			float startFrameTime = 0.0f; // �� �����
			float endFrameTime = 0.0f;

			float timeDifference = 0.0f; // ������� ����� �������
			float Scalar = 0.0f;         // ������, ��� � � ����� ������������

			const float MSPF = 33.3f;    // ����������� � ����, �������� =) FPS30 

			POD_Math::Quaternion SlerpResult = {0};
			POD_Math::Vector3    LerpResult  = {0};
			POD_Math::Quaternion Normal      = {0};

			// ������� ��� ���� ���������� �� �����)))
			Key1=static_cast<unsigned short>(floor((static_cast<float>(t) / MSPF)));
			
			if(Key1 >= NumKeys-1) Key2=Key1; 
			else Key2 = Key1+1;

			startFrameTime = MSPF * Key1;
			endFrameTime =   MSPF * Key2;

			timeDifference = endFrameTime - startFrameTime;
			Scalar = (t - startFrameTime) / timeDifference; // ��������� ����� ������� ����� � ����� �� �������. 
															// ������������ �������, ����� �������)
                                                   
			POD_Math::QuatSlerp(SlerpResult,Keys[Key1].rotation,Keys[Key2].rotation,Scalar);
			POD_Math::QuatNormalize(Normal,SlerpResult);
			POD_Math::Vector3Lerp(LerpResult,Keys[Key1].position,Keys[Key2].position,Scalar);
			POD_Math::QuatToMatrix4x3(i,Normal);

			i._41=LerpResult.x; 
			i._42=LerpResult.y;
			i._43=LerpResult.z;

	};



	SkeletalAnimation::Bone::Bone(): Children(),       //�������� 2 ���������, 2 ���������� � �������� ������� ������� �����
									SharedArray(),
									NumChildren(),
									MaxChildren(),
									Space(){};

	SkeletalAnimation::Bone::~Bone(){ delete[] Children;  }; // ������ ����������� ������ ��� ������ �����

	bool SkeletalAnimation::Bone::Allocate(size_t size)
	{
		  
		Children = new(std::nothrow) Bone*[size];

		if(!Children) 
		{
			ErrorHandler::ShowWarning(L"�������� � ���������� � ���������� �������");

			return false;
		};

		MaxChildren=size;

		return true;
	};


	bool SkeletalAnimation::Bone::Reallocate()
	{
		int Ch=0;
		Bone**tmp = new(std::nothrow) Bone*[MaxChildren*2];
		if(!tmp) 
		{
			ErrorHandler::ShowWarning(L"�������� ��� ����������� ������� ���������� �������� ������ �������");
			return false;
		};

		if(Children)
		{
				for(Ch; Ch < NumChildren; ++Ch)
				{
					tmp[Ch]=Children[Ch];
				};

				MaxChildren*=2;
		};

			delete[]Children;
			Children=tmp;

			return true;
	};


	bool SkeletalAnimation::Bone::AttachBone(Bone*const child) 
	{
		if(NumChildren==MaxChildren) 
		{
			ErrorHandler::ShowWarning(L"������������ ����� � ������ ���������� ����� �� ������ �������");

			return false;
		};

		Children[NumChildren]=child;

		++NumChildren;

		return true;
	};


	void SkeletalAnimation::Bone::UpdateRoot(const unsigned long t)
	{

		POD_Math::Matrix4x3 Interpolated = POD_Math::Matrix4x3();
		POD_Math::Matrix4x3 Complete     = POD_Math::Matrix4x3();

		Interpolator.GetInterpolated(t, Interpolated);
		POD_Math::Matrix4x3Mul(Complete, Space, Interpolated);
		POD_Math::Matrix4x3Transpose(*SharedArray, Complete);
	
			if(NumChildren)
			{

				for(int ch=0; ch < NumChildren; ++ch) 
				Children[ch]->UpdateChild(t, Interpolated);
			};
	};

		// ��������! ��������!
	void SkeletalAnimation::Bone::UpdateChild(const unsigned long t,
												POD_Math::Matrix4x3& p)
	{
			
		POD_Math::Matrix4x3 Interpolated = POD_Math::Matrix4x3();
		POD_Math::Matrix4x3 Complete     = POD_Math::Matrix4x3();
		POD_Math::Matrix4x3 Combined     = POD_Math::Matrix4x3();

			// 3 ������� - ����������������� ���������, 
			// ����������������� ��������������� ������������� �� �������� ������� 1 �����.
			// ����������������� ���������, ������������� �� ����������������� ������� ������������.

		Interpolator.GetInterpolated(t, Interpolated);
		POD_Math::Matrix4x3Mul(Combined, Interpolated,p);
		POD_Math::Matrix4x3Mul(Complete, Space, Combined);

		POD_Math::Matrix4x3Transpose(*SharedArray,Complete);

			if(NumChildren)
			{
				for(int ch=0; ch < NumChildren; ++ch) 
					Children[ch]->UpdateChild(t, Combined);
			};
	};

	SkeletalAnimation::SLERP& SkeletalAnimation::Bone::GetInterpolator()
	{      // ���������� ������ �� ������������ ��� ���������� �������������
		return Interpolator;
	};

	void SkeletalAnimation::Bone::SetSharedArray(POD_Math::Matrix4x3*const p)
	{      // ������ ������ ���������
		SharedArray=p;
	};

	void SkeletalAnimation::Bone::SetSpaceMatrix(POD_Math::Matrix4x3& mat)
	{
			//������ �������� �����
		memcpy(&Space,&mat,sizeof(POD_Math::Matrix4x3));  
	};

	SkeletalAnimation::Skeleton::Skeleton(): Bones(),
											Root(),
											NumBones(),
											SharedArray(),
											AllKeys(){};

	SkeletalAnimation::Skeleton::~Skeleton()
	{
		delete[] SharedArray;
		delete[] Bones;
		delete[] AllKeys;
	};

	// ����������� ��������
	bool SkeletalAnimation::Skeleton::Build(vBone*    const bones,
											Keyframe* const keys,
											const size_t NumB,
											const size_t NumK)
	{
	
		Bones = new(std::nothrow) Bone[NumB]; 
		if(!Bones) 
		{
			ErrorHandler::ShowWarning(L"�������� � ���������� ������ ���������� �������� ������ �������");

			return false;
		};

		NumBones=NumB;

		SharedArray = new(std::nothrow) POD_Math::Matrix4x3[NumB];
		if(!SharedArray) 
		{
			ErrorHandler::ShowWarning(L"�������� � ���������� ������ ������ ������� �������");

			return false;
		}; 

			for(size_t nB=0; nB < NumB; ++nB)
			{
				
				if(!Bones[nB].Allocate(10)) return false;                    // ����� ������ ����� ������ �� 10 ����������

				if(bones[nB].Parent==-1) {                                   // ���� -1 - ��� ��� ������
					Root = &Bones[nB]; }
				else Bones[bones[nB].Parent].AttachBone(&Bones[nB]);         // ���� ���, ����� ������������� �����

				Bones[nB].SetSpaceMatrix(bones[nB].Inverse);                 // �������� ��������
				Bones[nB].SetSharedArray(&SharedArray[nB]);                  // �������� ��������� �� ������� ������ �������
				Bones[nB].GetInterpolator().Initialize(&keys[nB*NumK],NumK); // ����� ������ �������� ����� �������������.
			};

			delete[]bones; // �������� �������������, ��� POD'� ������ �� ����� 
			AllKeys=keys;  // ������ ������, �������� ��������� �������������. �������������� � ����������� �������.

				return true ;
		};


		void SkeletalAnimation::Skeleton::Update(const unsigned long t)
		{
			Root->UpdateRoot(t); // � ������� �������� � ������������ � �� ����������� ������� ������ 
		};

		size_t SkeletalAnimation::Skeleton::GetNumBones()const
		{
			return NumBones;		
		};

		POD_Math::Matrix4x3* SkeletalAnimation::Skeleton::GetSharedArray()const
		{
			return SharedArray;
		};