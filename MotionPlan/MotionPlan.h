
#pragma once
#ifndef MOTIONPLAN_H_
#define MOTIONPLAN_H_
#include <vector>
using namespace std;

struct PosStruct
{
	double x;				// x���꣬��λmm
	double y;				// y���꣬��λmm
	double z;				// z���꣬��λmm
	double yaw;				// yaw���꣬��λ��
	double pitch;			// pitch���꣬��λ��
	double roll;			// roll���꣬��λ��
	bool config[3]{1,1,1};	// config, ��ʾ��������̬
};

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

	void SetSampleTime(double sampleTime);		                    //���ò���ʱ��
	void SetPlanPoints(PosStruct startPos, PosStruct endPos);		//������ʼ��λ�ͽ�����λ�ĵѿ�������
	void SetProfile(double vel, double acc, double dec);			//�����˶��������ٶȡ����ٶȺͼ��ٶ�
	void SetLine(double v, double a);                               //�������ٶȣ����ٶ�
	void GetPlanPoints();			                                //��ȡ�켣�滮����ɢ��λ
	void Cartesian(PosStruct startPos, PosStruct endPos);
	void CartesianLine(PosStruct startPos, PosStruct endPos);

};

#endif