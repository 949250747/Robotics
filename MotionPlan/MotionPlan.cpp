#include <iostream>
#include <fstream>
#include "MotionPlan.h"
#include "HLrobotconfig.h"
#include <algorithm>
#include <Windows.h>
#include "Eigen/Dense"

using namespace std;
using namespace HLRobot;
using namespace Eigen;

/********************************************************************
ABSTRACT:	���캯��

INPUTS:		<none>

OUTPUTS:	<none>

RETURN:		<none>
***********************************************************************/
CHLMotionPlan::CHLMotionPlan()
{
	for (int i = 0; i < 6; i++)
	{
		mJointAngleBegin[i] = 0;
		mJointAngleEnd[i] = 0;
	}

	for (int i = 0; i < 16; i++)
	{
		mStartMatrixData[i] = 0;
		mEndMatrixData[i] = 0;
	}
	for (int i = 0; i < 3; i++)
	{
		mConfig[i] = 0;
	}

	mSampleTime = 0.001;
	mVel = 0;
	mAcc = 0;
	mDec = 0;
}

/********************************************************************
ABSTRACT:	��������

INPUTS:		<none>

OUTPUTS:	<none>

RETURN:		<none>
***********************************************************************/
CHLMotionPlan::~CHLMotionPlan()
{

}

/********************************************************************
ABSTRACT:	��ʼ��

INPUTS:		sampleTime			����ʱ�䣬��λS
			vel			�ٶȣ���λm/s
			acc			���ٶȣ���λm/s/s
OUTPUTS:	<none>

RETURN:		<none>
***********************************************************************/
void CHLMotionPlan::Init(double sampleTime, int VLevel)
{
	if (sampleTime < 0.001) {
		mSampleTime = 0.001;
	}
	else {
		mSampleTime = sampleTime;
	}
	if (VLevel == 1) {
		mVel = 5;
		mAcc = 5;
		mDec = 5;
		LVel = 0.05;
		LAcc = 0.05;
	}
	else if (VLevel == 3) {
		mVel = 20;
		mAcc = 20;
		mDec = 20;
		LVel = 0.3;
		LAcc = 0.3;
	}
	else {
		mVel = 10;
		mAcc = 10;
		mDec = 10;
		LVel = 0.1;
		LAcc = 0.1;
	}
}

/********************************************************************
ABSTRACT:	�����˹ؽڿռ������ٶȹ滮

INPUTS:		startPos-��ʼ��λ��(x,y,z,yaw,pitch,roll)��endPos-��ֹ��λ��

OUTPUTS:	<none>

RETURN:		<none>
***********************************************************************/
void CHLMotionPlan::JointSpacePlan(PosStruct startPos, PosStruct endPos)
{
	double startAngle[3], endAngle[3];

	startAngle[0] = startPos.yaw * PI / 180;
	startAngle[1] = startPos.pitch * PI / 180;
	startAngle[2] = startPos.roll * PI / 180;

	endAngle[0] = endPos.yaw * PI / 180;
	endAngle[1] = endPos.pitch * PI / 180;
	endAngle[2] = endPos.roll * PI / 180;

	mStartMatrixData[0] = cos(startAngle[0]) * cos(startAngle[1]) * cos(startAngle[2]) - sin(startAngle[0]) * sin(startAngle[2]);
	mStartMatrixData[1] = -cos(startAngle[0]) * cos(startAngle[1]) * sin(startAngle[2]) - sin(startAngle[0]) * cos(startAngle[2]);
	mStartMatrixData[2] = cos(startAngle[0]) * sin(startAngle[1]);
	mStartMatrixData[3] = startPos.x / 1000;

	mStartMatrixData[4] = sin(startAngle[0]) * cos(startAngle[1]) * cos(startAngle[2]) + cos(startAngle[0]) * sin(startAngle[2]);
	mStartMatrixData[5] = -sin(startAngle[0]) * cos(startAngle[1]) * sin(startAngle[2]) + cos(startAngle[0]) * cos(startAngle[2]);
	mStartMatrixData[6] = sin(startAngle[0]) * sin(startAngle[1]);
	mStartMatrixData[7] = startPos.y / 1000;

	mStartMatrixData[8] = -sin(startAngle[1]) * cos(startAngle[2]);
	mStartMatrixData[9] = sin(startAngle[1]) * sin(startAngle[2]);
	mStartMatrixData[10] = cos(startAngle[1]);
	mStartMatrixData[11] = startPos.z / 1000;

	mStartMatrixData[12] = 0;
	mStartMatrixData[13] = 0;
	mStartMatrixData[14] = 0;
	mStartMatrixData[15] = 1;

	mEndMatrixData[0] = cos(endAngle[0]) * cos(endAngle[1]) * cos(endAngle[2]) - sin(endAngle[0]) * sin(endAngle[2]);
	mEndMatrixData[1] = -cos(endAngle[0]) * cos(endAngle[1]) * sin(endAngle[2]) - sin(endAngle[0]) * cos(endAngle[2]);
	mEndMatrixData[2] = cos(endAngle[0]) * sin(endAngle[1]);
	mEndMatrixData[3] = endPos.x / 1000;

	mEndMatrixData[4] = sin(endAngle[0]) * cos(endAngle[1]) * cos(endAngle[2]) + cos(endAngle[0]) * sin(endAngle[2]);
	mEndMatrixData[5] = -sin(endAngle[0]) * cos(endAngle[1]) * sin(endAngle[2]) + cos(endAngle[0]) * cos(endAngle[2]);
	mEndMatrixData[6] = sin(endAngle[0]) * sin(endAngle[1]);
	mEndMatrixData[7] = endPos.y / 1000;

	mEndMatrixData[8] = -sin(endAngle[1]) * cos(endAngle[2]);
	mEndMatrixData[9] = sin(endAngle[1]) * sin(endAngle[2]);
	mEndMatrixData[10] = cos(endAngle[1]);
	mEndMatrixData[11] = endPos.z / 1000;

	mEndMatrixData[12] = 0;
	mEndMatrixData[13] = 0;
	mEndMatrixData[14] = 0;
	mEndMatrixData[15] = 1;

	double angle[6] = { 0 };
	HLRobot::GetJointAngles(mStartMatrixData, angle);

	mJointAngleBegin[0] = angle[0];
	mJointAngleBegin[1] = angle[1];
	mJointAngleBegin[2] = angle[2];
	mJointAngleBegin[3] = angle[3];
	mJointAngleBegin[4] = angle[4];
	mJointAngleBegin[5] = angle[5];

	HLRobot::GetJointAngles(mEndMatrixData, angle);
	mJointAngleEnd[0] = angle[0];
	mJointAngleEnd[1] = angle[1];
	mJointAngleEnd[2] = angle[2];
	mJointAngleEnd[3] = angle[3];
	mJointAngleEnd[4] = angle[4];
	mJointAngleEnd[5] = angle[5];

	/* ����1�������ļ���д���ʼ�Ƕ� */
	ofstream outfile;               			//�����ļ�
	outfile.open("data.txt");
	outfile << mJointAngleBegin[0] << "  "
		<< mJointAngleBegin[1] << "  "
		<< mJointAngleBegin[2] << "  "
		<< mJointAngleBegin[3] << "  "
		<< mJointAngleBegin[4] << "  "
		<< mJointAngleBegin[5] << "  ";
	outfile << endl;                           //�����ʼ��ʱ�䡢�����ؽڽǶ�

	double Tour[6] = { 0 };     //ÿ���ؽ���Ҫ�ߵĽǶ�
	double Time[6] = { 0 };     //ÿ���ؽ��ߵ�ʱ�� 
	int PointNum[6] = { 0 };     //��ɢ��λ��
	double Acc[6] = { 0 };      //���ؽڵļ��ٶȷ���
	for (int i = 0; i < 6; i++)
	{
		/* ����2������ÿ������ת�ĽǶ� */

		Tour[i] = fabs(mJointAngleBegin[i] - mJointAngleEnd[i]);

		if (mJointAngleBegin[i] > mJointAngleEnd[i])
		{
			Acc[i] = -mAcc;
		}
		else
		{
			Acc[i] = mAcc;
		}

		/* ����3������ÿ�����ƶ�����ֹ������Ҫʱ�� */
		//ת��̫С���˻�Ϊ������
		if (Tour[i] <= mVel * mVel / mAcc)
		{
			Time[i] = 2 * sqrt(Tour[i] / mAcc);
		}
		//ת���㹻�����ι滮
		else
		{
			Time[i] = mVel / mAcc + Tour[i] / mVel;
		}

		/* ����4�����ݲ���ʱ�������ɢ��λ�� */
		PointNum[i] = Time[i] / mSampleTime;
	}

	for (int k = 0; k < 6; k++)
	{
		cout << mJointAngleBegin[k] << "   ";
	}
	cout << endl;
	for (int k = 0; k < 6; k++)
	{
		cout << mJointAngleEnd[k] << "   ";
	}
	cout << endl;
	for (int k = 0; k < 6; k++)
	{
		cout << PointNum[k] << "   ";
	}


	/* �������ؽڵĵ�������� */
	int MaxNum = 0;
	for (int i = 0; i < 6; i++)
	{
		if (PointNum[i] > MaxNum)
			MaxNum = PointNum[i];
	}


	/************************ ������ؽڽ� ***********************/

	double TempPos[6] = { 0 };  //ÿ���ᵱǰ�ؽڽ�
	for (int i = 0; i < MaxNum; i++)
	{//ʱ��
		//��
		for (int j = 0; j < 6; j++)
		{

			//��ת��̫С���������ι滮
			if (Tour[j] <= mVel * mVel / mAcc)
			{
				if (i < 1000 * sqrt(Tour[j] / mAcc))
				{
					//0.5at^2
					TempPos[j] = mJointAngleBegin[j] + 0.5 * Acc[j] * i * i * 0.001 * 0.001;
				}
				else if (i >= 1000 * sqrt(Tour[j] / mAcc) && i < PointNum[j])
				{
					TempPos[j] = mJointAngleEnd[j] - 0.5 * Acc[j] * (PointNum[j] - i) * (PointNum[j] - i) * 0.001 * 0.001;
				}
				else
				{
					TempPos[j] = TempPos[j];
				}
			}
			//ת���㹻���������ι滮
			else
			{
				if (i < 1000 * mVel / mAcc)
				{
					TempPos[j] = mJointAngleBegin[j] + 0.5 * Acc[j] * i * i * 0.001 * 0.001;
				}
				else if (i >= 1000 * mVel / mAcc && i < PointNum[j] - 1000 * mVel / mAcc)
				{
					TempPos[j] = mJointAngleBegin[j] + Acc[j] * (mVel / mAcc) * (i * 0.001 - 0.5 * mVel / mAcc);
				}
				else if (i >= PointNum[j] - 1000 * mVel / mAcc && i < PointNum[j])
				{
					TempPos[j] = mJointAngleEnd[j] - 0.5 * Acc[j] * (PointNum[j] - i) * (PointNum[j] - i) * 0.001 * 0.001;
				}
				else
				{
					TempPos[j] = TempPos[j];
				}
			}
		}

		//������ļ�
		outfile << TempPos[0] << "  "
			<< TempPos[1] << "  "
			<< TempPos[2] << "  "
			<< TempPos[3] << "  "
			<< TempPos[4] << "  "
			<< TempPos[5] << "  ";
		outfile << endl;
	}
	outfile.close();
}

/********************************************************************
ABSTRACT:	�滮�ѿ����ռ�������ֱ���˶�

INPUTS:		startPos			��ʼ��λ�ѿ�������
			endPos				������λ�ѿ�������

OUTPUTS:	�㼣�ļ� "data.txt"

RETURN:		<none>
***********************************************************************/
void CHLMotionPlan::CartesianLinePlan(PosStruct startPos, PosStruct endPos)
{
	ofstream outfile;               			//�����ļ�
	outfile.open("data.txt");

	double Tour[3] = { 0 };     //x,y,z��������������λm
	double direction[3] = { 0 };//����λ����

	int PointNum = 0;          //����

	Tour[0] = (endPos.x - startPos.x) / 1000;
	Tour[1] = (endPos.y - startPos.y) / 1000;
	Tour[2] = (endPos.z - startPos.z) / 1000;

	double distance = sqrt(Tour[0] * Tour[0] + Tour[1] * Tour[1] + Tour[2] * Tour[2]);   //�˶�����
	double Time = 0;           //ʱ��
	double delta = 0;          //ÿһС�ε�����

	direction[0] = Tour[0] / distance;
	direction[1] = Tour[1] / distance;
	direction[2] = Tour[2] / distance;

	/* ����ʱ�� */
	if (distance < LVel * LVel / LAcc)
	{
		Time = 2 * sqrt(LVel / LAcc);
	}
	else
	{
		Time = LVel / LAcc + distance / LVel;
	}

	PointNum = Time / mSampleTime;

	cout << endl << PointNum;

	for (int i = 0; i < PointNum; i++)
	{
		//cout << i << endl;
		if (distance <= LVel * LVel / LAcc)
		{
			if (i < 1000 * sqrt(distance / LAcc))
			{
				//0.5at^2
				delta = 0.5 * LAcc * i * i * 0.001 * 0.001;
				mStartMatrixData[3] = startPos.x / 1000 + delta * direction[0];
				mStartMatrixData[7] = startPos.y / 1000 + delta * direction[1];
				mStartMatrixData[11] = startPos.z / 1000 + delta * direction[2];
			}
			else if (i >= 1000 * sqrt(distance / LAcc) && i < PointNum)
			{
				delta = distance - 0.5 * LAcc * (PointNum - i) * (PointNum - i) * 0.001 * 0.001;
				mStartMatrixData[3] = startPos.x / 1000 + delta * direction[0];
				mStartMatrixData[7] = startPos.y / 1000 + delta * direction[1];
				mStartMatrixData[11] = startPos.z / 1000 + delta * direction[2];
			}
		}
		else
		{
			if (i < 1000 * LVel / LAcc)
			{
				delta = 0.5 * LAcc * i * i * 0.001 * 0.001;
				mStartMatrixData[3] = startPos.x / 1000 + delta * direction[0];
				mStartMatrixData[7] = startPos.y / 1000 + delta * direction[1];
				mStartMatrixData[11] = startPos.z / 1000 + delta * direction[2];
			}
			else if (i >= 1000 * LVel / LAcc && i < PointNum - 1000 * LVel / LAcc)
			{
				delta = LAcc * (LVel / LAcc) * (i * 0.001 - 0.5 * LVel / LAcc);
				mStartMatrixData[3] = startPos.x / 1000 + delta * direction[0];
				mStartMatrixData[7] = startPos.y / 1000 + delta * direction[1];
				mStartMatrixData[11] = startPos.z / 1000 + delta * direction[2];
			}
			else if (i >= PointNum - 1000 * LVel / LAcc && i < PointNum)
			{
				delta = distance - 0.5 * LAcc * (PointNum - i) * (PointNum - i) * 0.001 * 0.001;
				mStartMatrixData[3] = startPos.x / 1000 + delta * direction[0];
				mStartMatrixData[7] = startPos.y / 1000 + delta * direction[1];
				mStartMatrixData[11] = startPos.z / 1000 + delta * direction[2];
			}
		}

		double startAngle[3], endAngle[3];

		startAngle[0] = startPos.yaw * PI / 180;
		startAngle[1] = startPos.pitch * PI / 180;
		startAngle[2] = startPos.roll * PI / 180;

		mStartMatrixData[0] = cos(startAngle[0]) * cos(startAngle[1]) * cos(startAngle[2]) - sin(startAngle[0]) * sin(startAngle[2]);
		mStartMatrixData[1] = -cos(startAngle[0]) * cos(startAngle[1]) * sin(startAngle[2]) - sin(startAngle[0]) * cos(startAngle[2]);
		mStartMatrixData[2] = cos(startAngle[0]) * sin(startAngle[1]);

		mStartMatrixData[4] = sin(startAngle[0]) * cos(startAngle[1]) * cos(startAngle[2]) + cos(startAngle[0]) * sin(startAngle[2]);
		mStartMatrixData[5] = -sin(startAngle[0]) * cos(startAngle[1]) * sin(startAngle[2]) + cos(startAngle[0]) * cos(startAngle[2]);
		mStartMatrixData[6] = sin(startAngle[0]) * sin(startAngle[1]);

		mStartMatrixData[8] = -sin(startAngle[1]) * cos(startAngle[2]);
		mStartMatrixData[9] = sin(startAngle[1]) * sin(startAngle[2]);
		mStartMatrixData[10] = cos(startAngle[1]);

		mStartMatrixData[12] = 0;
		mStartMatrixData[13] = 0;
		mStartMatrixData[14] = 0;
		mStartMatrixData[15] = 1;

		double angle[6] = { 0 };
		HLRobot::GetJointAngles(mStartMatrixData, angle);

		outfile << angle[0] << "  "
			<< angle[1] << "  "
			<< angle[2] << "  "
			<< angle[3] << "  "
			<< angle[4] << "  "
			<< angle[5] << "  ";
		outfile << endl;
	}
	outfile.close();
}

/********************************************************************
ABSTRACT:	�滮�ѿ����ռ������ȸߵ��İ�Բ�˶�

INPUTS:		startPos			��ʼ��λ�ѿ�������
			endPos				������λ�ѿ�������

OUTPUTS:	�㼣�ļ� "data.txt"

RETURN:		<none>
***********************************************************************/
void CHLMotionPlan::CirclePlan(PosStruct startPos, PosStruct endPos)
{
	ofstream outfile;           //�����ļ�
	outfile.open("data.txt");	//���ļ�

	double startAngle[3], endAngle[3];
	double Theta, Diameter, Tour, Time, delta = 0;				//Theta��m����x��нǣ�Diameter��ʼĩ����룬Ҳ��Բֱ������λ��m��
																//Tour��·�̣�Ҳ�ǰ�Բ������λm��Time��ʱ�䣻delta���������ʼ��·������
	int PointNum;			//����

	Theta = atan2(endPos.y - startPos.y, endPos.x - startPos.x);
	Diameter = sqrt(pow(endPos.x - startPos.x, 2) + pow(endPos.y - startPos.y, 2)) / 1000;
	Tour = 0.5 * PI * Diameter;

	/* ����ʱ�� */
	if (Tour < LVel * LVel / LAcc)
	{
		Time = 2 * sqrt(Tour / LAcc);
	}
	else
	{
		Time = LVel / LAcc + Tour / LVel;
	}

	/* ������� */
	PointNum = Time / mSampleTime;

	cout << Theta << "      " << Diameter << "       " << Tour << "        " << Time << "       " << PointNum << endl;

	startAngle[0] = startPos.yaw * PI / 180;
	startAngle[1] = startPos.pitch * PI / 180;

	for (int i = 0; i < PointNum; i++)
	{
		//cout << i << endl;
		if (Tour <= LVel * LVel / LAcc)
		{
			if (i < 1000 * sqrt(Tour / LAcc))
			{
				delta = 0.5 * LAcc * i * i * 0.001 * 0.001;
				mStartMatrixData[3] = startPos.x / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * cos(Theta);
				mStartMatrixData[7] = startPos.y / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * sin(Theta);
				mStartMatrixData[11] = startPos.z / 1000 + 0.5 * Diameter * sin(2 * delta / Diameter);
				startAngle[2] = startPos.roll * PI / 180 - 2 * delta / Diameter;
			}
			else if (i >= 1000 * sqrt(Tour / LAcc) && i < PointNum)
			{
				delta = Tour - 0.5 * LAcc * (PointNum - i) * (PointNum - i) * 0.001 * 0.001;
				mStartMatrixData[3] = startPos.x / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * cos(Theta);
				mStartMatrixData[7] = startPos.y / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * sin(Theta);
				mStartMatrixData[11] = startPos.z / 1000 + 0.5 * Diameter * sin(2 * delta / Diameter);
				startAngle[2] = startPos.roll * PI / 180 - 2 * delta / Diameter;
			}
		}
		else
		{
			if (i < 1000 * LVel / LAcc)
			{
				delta = 0.5 * LAcc * i * i * 0.001 * 0.001;
				mStartMatrixData[3] = startPos.x / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * cos(Theta);
				mStartMatrixData[7] = startPos.y / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * sin(Theta);
				mStartMatrixData[11] = startPos.z / 1000 + 0.5 * Diameter * sin(2 * delta / Diameter);
				startAngle[2] = startPos.roll * PI / 180 - 2 * delta / Diameter;
			}
			else if (i >= 1000 * LVel / LAcc && i < PointNum - 1000 * LVel / LAcc)
			{
				delta = LAcc * (LVel / LAcc) * (i * 0.001 - 0.5 * LVel / LAcc);
				mStartMatrixData[3] = startPos.x / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * cos(Theta);
				mStartMatrixData[7] = startPos.y / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * sin(Theta);
				mStartMatrixData[11] = startPos.z / 1000 + 0.5 * Diameter * sin(2 * delta / Diameter);
				startAngle[2] = startPos.roll * PI / 180 - 2 * delta / Diameter;
			}
			else if (i >= PointNum - 1000 * LVel / LAcc && i < PointNum)
			{
				delta = Tour - 0.5 * LAcc * (PointNum - i) * (PointNum - i) * 0.001 * 0.001;
				mStartMatrixData[3] = startPos.x / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * cos(Theta);
				mStartMatrixData[7] = startPos.y / 1000 + 0.5 * Diameter * (1 - cos(2 * delta / Diameter)) * sin(Theta);
				mStartMatrixData[11] = startPos.z / 1000 + 0.5 * Diameter * sin(2 * delta / Diameter);
				startAngle[2] = startPos.roll * PI / 180 - 2 * delta / Diameter;
			}
		}

		mStartMatrixData[0] = cos(startAngle[0]) * cos(startAngle[1]) * cos(startAngle[2]) - sin(startAngle[0]) * sin(startAngle[2]);
		mStartMatrixData[1] = -cos(startAngle[0]) * cos(startAngle[1]) * sin(startAngle[2]) - sin(startAngle[0]) * cos(startAngle[2]);
		mStartMatrixData[2] = cos(startAngle[0]) * sin(startAngle[1]);

		mStartMatrixData[4] = sin(startAngle[0]) * cos(startAngle[1]) * cos(startAngle[2]) + cos(startAngle[0]) * sin(startAngle[2]);
		mStartMatrixData[5] = -sin(startAngle[0]) * cos(startAngle[1]) * sin(startAngle[2]) + cos(startAngle[0]) * cos(startAngle[2]);
		mStartMatrixData[6] = sin(startAngle[0]) * sin(startAngle[1]);

		mStartMatrixData[8] = -sin(startAngle[1]) * cos(startAngle[2]);
		mStartMatrixData[9] = sin(startAngle[1]) * sin(startAngle[2]);
		mStartMatrixData[10] = cos(startAngle[1]);

		mStartMatrixData[12] = 0;
		mStartMatrixData[13] = 0;
		mStartMatrixData[14] = 0;
		mStartMatrixData[15] = 1;

		double angle[6] = { 0 };
		HLRobot::GetJointAngles(mStartMatrixData, angle);

		outfile << angle[0] << "  "
			<< angle[1] << "  "
			<< angle[2] << "  "
			<< angle[3] << "  "
			<< angle[4] << "  "
			<< angle[5] << "  ";
		outfile << endl;
	}

	outfile.close();
}