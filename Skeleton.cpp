#include "Skeleton.h"
#include <new>
#include <memory>
#include <cstdio>
#include "ErrorHandler.h"      

	SkeletalAnimation::SLERP::SLERP():Keys(),NumKeys(){}; // Обнуляем две переменныe - вот и вся инициализация.
	SkeletalAnimation::SLERP::~SLERP(){};

	void SkeletalAnimation::SLERP::Initialize( Keyframe*const key, size_t Num) // Кладем указатель на первый кадр последовательности
	{                                                                          // Задаем количество ключей в последовательности
		   Keys=key; NumKeys=Num;
	};

	void SkeletalAnimation::SLERP::GetInterpolated(const unsigned long t, POD_Math::Matrix4x3& i) // Изымаем ключ из интерполятора
	{  
	   
			unsigned short Key1=0;   // Номера будущих ключей
			unsigned short Key2=0;

			float startFrameTime = 0.0f; // Их время
			float endFrameTime = 0.0f;

			float timeDifference = 0.0f; // Разница между ключами
			float Scalar = 0.0f;         // Скаляр, как и в любой интерполяции

			const float MSPF = 33.3f;    // Миллисекунд в кадр, реалтайм =) FPS30 

			POD_Math::Quaternion SlerpResult = {0};
			POD_Math::Vector3    LerpResult  = {0};
			POD_Math::Quaternion Normal      = {0};

			// Горжусь что смог додуматься до этого)))
			Key1=static_cast<unsigned short>(floor((static_cast<float>(t) / MSPF)));
			
			if(Key1 >= NumKeys-1) Key2=Key1; 
			else Key2 = Key1+1;

			startFrameTime = MSPF * Key1;
			endFrameTime =   MSPF * Key2;

			timeDifference = endFrameTime - startFrameTime;
			Scalar = (t - startFrameTime) / timeDifference; // Исключаем время первого ключа и делим на разницу. 
															// Запутывающая формула, потом объясню)
                                                   
			POD_Math::QuatSlerp(SlerpResult,Keys[Key1].rotation,Keys[Key2].rotation,Scalar);
			POD_Math::QuatNormalize(Normal,SlerpResult);
			POD_Math::Vector3Lerp(LerpResult,Keys[Key1].position,Keys[Key2].position,Scalar);
			POD_Math::QuatToMatrix4x3(i,Normal);

			i._41=LerpResult.x; 
			i._42=LerpResult.y;
			i._43=LerpResult.z;

	};



	SkeletalAnimation::Bone::Bone(): Children(),       //Обнуляем 2 указателя, 2 переменные и обратную матрицу первого кадра
									SharedArray(),
									NumChildren(),
									MaxChildren(),
									Space(){};

	SkeletalAnimation::Bone::~Bone(){ delete[] Children;  }; // Просто высвобожаем память под массив детей

	bool SkeletalAnimation::Bone::Allocate(size_t size)
	{
		  
		Children = new(std::nothrow) Bone*[size];

		if(!Children) 
		{
			ErrorHandler::ShowWarning(L"Проблема с аллокацией в экземпляре скелета");

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
			ErrorHandler::ShowWarning(L"Проблема при реаллокации массива внутренних структур костей скелета");
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
			ErrorHandler::ShowWarning(L"Недостаточно места в буфере указателей одной из костей скелета");

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

		// ВНИМАНИЕ! РЕКУРСИЯ!
	void SkeletalAnimation::Bone::UpdateChild(const unsigned long t,
												POD_Math::Matrix4x3& p)
	{
			
		POD_Math::Matrix4x3 Interpolated = POD_Math::Matrix4x3();
		POD_Math::Matrix4x3 Complete     = POD_Math::Matrix4x3();
		POD_Math::Matrix4x3 Combined     = POD_Math::Matrix4x3();

			// 3 матрицы - Интерполированная локальная, 
			// Интерполированная комбинированная перемноженная на обратную матрицу 1 кадра.
			// Интерполированная локальная, перемноженная на интерполированную мировую родительскую.

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
	{      // Возвращаем ссылку на интерполятор для дальнейшей инициализации
		return Interpolator;
	};

	void SkeletalAnimation::Bone::SetSharedArray(POD_Math::Matrix4x3*const p)
	{      // Просто задаем указатель
		SharedArray=p;
	};

	void SkeletalAnimation::Bone::SetSpaceMatrix(POD_Math::Matrix4x3& mat)
	{
			//Быстро копируем байты
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

	// Выстраиваем иерархию
	bool SkeletalAnimation::Skeleton::Build(vBone*    const bones,
											Keyframe* const keys,
											const size_t NumB,
											const size_t NumK)
	{
	
		Bones = new(std::nothrow) Bone[NumB]; 
		if(!Bones) 
		{
			ErrorHandler::ShowWarning(L"Проблемы с аллокацией буфера внутренних структур костей скелета");

			return false;
		};

		NumBones=NumB;

		SharedArray = new(std::nothrow) POD_Math::Matrix4x3[NumB];
		if(!SharedArray) 
		{
			ErrorHandler::ShowWarning(L"Проблемы с аллокацией общего буфера униформ скелета");

			return false;
		}; 

			for(size_t nB=0; nB < NumB; ++nB)
			{
				
				if(!Bones[nB].Allocate(10)) return false;                    // Дадим каждой кости памяти на 10 указателей

				if(bones[nB].Parent==-1) {                                   // Если -1 - это наш корень
					Root = &Bones[nB]; }
				else Bones[bones[nB].Parent].AttachBone(&Bones[nB]);         // Если нет, тогда устанавливаем связь

				Bones[nB].SetSpaceMatrix(bones[nB].Inverse);                 // Копируем инверсию
				Bones[nB].SetSharedArray(&SharedArray[nB]);                  // Передаем указатель на элемент общего массива
				Bones[nB].GetInterpolator().Initialize(&keys[nB*NumK],NumK); // Через ссылку передаем ключи интерполятору.
			};

			delete[]bones; // Иерархия восстановлена, эти POD'ы больше не нужны 
			AllKeys=keys;  // Массив ключей, которыми оперируют интерполяторы. Высвобождается в деструкторе скелета.

				return true ;
		};


		void SkeletalAnimation::Skeleton::Update(const unsigned long t)
		{
			Root->UpdateRoot(t); // С помощью иерархии в соответствии с ПК расчитываем матрицы костей 
		};

		size_t SkeletalAnimation::Skeleton::GetNumBones()const
		{
			return NumBones;		
		};

		POD_Math::Matrix4x3* SkeletalAnimation::Skeleton::GetSharedArray()const
		{
			return SharedArray;
		};