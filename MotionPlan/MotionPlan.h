#ifndef MOTIONPLAN_H_
#define MOTIONPLAN_H_
#include <vector>

using namespace std;

typedef struct
{
	double x;				// x���꣬��λmm
	double y;				// y���꣬��λmm
	double z;				// z���꣬��λmm
	double yaw;				// yaw���꣬��λ��
	double pitch;			// pitch���꣬��λ��
	double roll;			// roll���꣬��λ��
}PosStruct;

class CHLMotionPlan
{
private:
	double mJointAngleBegin[6];					//��ʼ��λ�ĹؽڽǶ�,��λ��
	double mJointAngleEnd[6];					//������λ�ĹؽڽǶȣ���λ��
	double mStartMatrixData[16];				//��ʼ��λ��ת����������
	double mEndMatrixData[16];					//������λ��ת����������
	double mSampleTime;							//������λ����λS
	double mVel;								//���ٶȣ���λ��/s
	double mAcc;								//���ٶȣ���λ��/s/s
	double mDec;								//���ٶȣ���λ��/ s / s
	double LVel;                                //���ٶ� ����λm/s
	double LAcc;                                //�߼��ٶȣ���λm/s/s
	bool   mConfig[3];							//��������̬

public:
	CHLMotionPlan();
	virtual ~CHLMotionPlan();
	void Init(double sampleTime, int VLevel);		//��ʼ��
	void JointSpacePlan(PosStruct startPos, PosStruct endPos);		//�ؽڿռ�滮											 
	void CartesianLinePlan(PosStruct startPos, PosStruct endPos);	    //�ѿ����ռ�滮
	void CirclePlan(PosStruct startPos, PosStruct endPos);   //���滮
};

#endif