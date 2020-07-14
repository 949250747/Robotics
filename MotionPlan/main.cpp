#include <iostream>
#include "HLrobotconfig.h"
#include "MotionPlan.h"

using namespace std;

int main()
{  
    /************* ʹ��ʾ�� *************/
    //����յ�
    PosStruct startPoint, endPoint;

    //�滮
    CHLMotionPlan trajectory;

    trajectory.Init(0.001, 3);      //�ȳ�ʼ��

    /* �ٵ��ù滮���� */
    trajectory.JointSpacePlan(startPoint, endPoint);     //�ؽڿռ�
    trajectory.CartesianLinePlan(startPoint, endPoint);  //�ѿ����ռ�ֱ�߹滮
    trajectory.CirclePlan(startPoint, endPoint);

    return 0;
}