#ifndef SKELETON_H
#define SKELETON_H

#include "Quaternion.h"
#include "Matrix4x3.h"
#include "Vector3.h"

	namespace SkeletalAnimation
	{

		#pragma pack(push,1)

		struct Keyframe
		{
			POD_Math::Quaternion rotation;
			POD_Math::Vector3    position;
		};

		struct vBone
		{
			char                Name[31];
			char                Parent;
			POD_Math::Matrix4x3 Inverse;
		};

		#pragma pack(pop)


		class SLERP
		{

		public:

			SLERP();
			~SLERP();

			void Initialize( Keyframe *const key,
							size_t    Num);

			void GetInterpolated(const unsigned long t,
				             POD_Math::Matrix4x3& i);

			Keyframe *Keys;
			size_t NumKeys;
		};


		class Bone
		{

		public:

			Bone();
			~Bone();

			bool Allocate(size_t size);

			bool Reallocate();

			bool AttachBone(Bone*const);

			void UpdateRoot(const unsigned long t);

			void UpdateChild(const unsigned long t,
				            POD_Math::Matrix4x3& p);

			SLERP& GetInterpolator();
			void   SetSharedArray(POD_Math::Matrix4x3*const);
			void   SetSpaceMatrix(POD_Math::Matrix4x3& mat);

		private:

			SLERP              Interpolator;

			Bone               ** Children; 
			POD_Math::Matrix4x3 * SharedArray;
			POD_Math::Matrix4x3   Space;

			size_t MaxChildren;
			size_t NumChildren;
		};


		class Skeleton
		{

			public:

				Skeleton();
				~Skeleton();

				bool Build(vBone*    const bones,
							Keyframe* const keys,
							const size_t NumB,
							const size_t NumK);

				POD_Math::Matrix4x3* GetSharedArray()const;
				void Update(const unsigned long t);
				size_t GetNumBones()const;

				Bone				*Bones;
				size_t				NumBones;
				Bone				*Root; 
				POD_Math::Matrix4x3	*SharedArray;
				Keyframe            *AllKeys;
		};
	};

#endif