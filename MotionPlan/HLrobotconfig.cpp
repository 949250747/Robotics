#include "HLrobotconfig.h"
#include <cmath>
#include <iostream> 
#include "Eigen/Dense"

using namespace std;
using namespace Eigen;
#define PI 3.141592
#define L1 0.491
#define L2 0.45
#define L3 0.45
#define L4 0.084

namespace HLRobot
{
	Vector4d q0_4(4, 1), q1_4(4, 1), q2_4(4, 1), q3_4(4, 1), q4_4(4, 1); // ���˶�ѧ��ʼ��ѡ��

	Vector3d v1, v2, v3, v4, v5, v6;                                     //�ٶ�

	Vector3d right1, right2, right3, right4, right5, right6;             //ƽ����

	Matrix3d I = MatrixXd::Identity(3, 3);   //3x3��λ��

	Matrix4d g_st0(4, 4);                    //��ʼλ��

	/* ������ */
	bool Subproblem1(double* theta, const int index, const VectorXd& xi, const Vector4d& p_4, const Vector4d& q_4, const Vector4d& r_4);
	bool Subproblem2(double* theta, const int index1, const int index2, const VectorXd& xi1, const VectorXd& xi2, const Vector4d& p_4, const Vector4d& q_4, const Vector4d& r_4);
	bool Subproblem3(double* theta, const int index, const VectorXd& xi, const Vector4d& p_4, const Vector4d& q_4, const Vector4d& r_4, const double delta);

	/********************************************************************
	ABSTRACT:	���������

	INPUTS:		T[16]:	λ�˾������г��Ⱦ���Ϊ��
				config[3]����̬����������˶�Ӧ��8����̬��Ϊ�˰�ȫ��
				ʵ����������ֻ����һ�ּ��ɣ�config�����˶�ѧ�н����˳�ʼ�������Բ���Ҫ���и��ġ�
				Tool:���Ժ���
				Turns[6]��ÿ���ؽڶ�Ӧ��Ȧ��

	OUTPUTS:    theta[6] 6���ؽڽ�, ��λΪ����

	RETURN:		<none>
	***********************************************************************/
	void robotBackwardHJQ(const double* T, bool* config, double* Tool, bool* Turns, double* theta)
	{
		VectorXd xi1(6, 1), xi2(6, 1), xi3(6, 1), xi4(6, 1), xi5(6, 1), xi6(6, 1);                  //�˶�����
		Vector3d omega1(3, 1), omega2(3, 1), omega3(3, 1), omega4(3, 1), omega5(3, 1), omega6(3, 1);//ת��
		Matrix4d g_st(4, 4);

		// ���˶�ѧ��ʼ�㸳ֵ
		q0_4 << 0, 0, 0, 1;
		q3_4 << 0, 0, L1 + L2 + L3, 1;
		q1_4 << 0, 0, L1, 1;
		q2_4 << 0, 0, L1 + L2, 1;
		q4_4 << 0, 0, L1 + L2 + L3 + L4, 1;

		// ���˶�ѧ��ʼ��ѡ��
		Vector3d q0(3, 1), q1(3, 1), q2(3, 1), q3(3, 1);
		q0 << 0, 0, 0;
		q1 << 0, 0, L1;
		q2 << 0, 0, L1 + L2;
		q3 << 0, 0, L1 + L2 + L3;

		// �任����ֵ
		int k = 0;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				g_st(i, j) = T[k];
				k++;
			}
		}

		//��ʼλ�͸�ֵ
		g_st0 << -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 1.475, 0, 0, 0, 1;

		//ת��
		omega1 << 0, 0, 1;
		omega2 << 0, 1, 0;
		omega3 << 0, 1, 0;
		omega4 << 0, 0, 1;
		omega5 << 0, 1, 0;
		omega6 << 0, 0, 1;

		//�ٶ�
		v1 << -omega1.cross(q0);
		v2 << -omega2.cross(q1);
		v3 << -omega3.cross(q2);
		v4 << -omega4.cross(q0);
		v5 << -omega5.cross(q3);
		v6 << -omega6.cross(q0);

		// �˶�������ֵ
		xi1.block(0, 0, 3, 1) << v1;
		xi1.block(3, 0, 3, 1) << omega1;
		xi2.block(0, 0, 3, 1) << v2;
		xi2.block(3, 0, 3, 1) << omega2;
		xi3.block(0, 0, 3, 1) << v3;
		xi3.block(3, 0, 3, 1) << omega3;
		xi4.block(0, 0, 3, 1) << v4;
		xi4.block(3, 0, 3, 1) << omega4;
		xi5.block(0, 0, 3, 1) << v5;
		xi5.block(3, 0, 3, 1) << omega5;
		xi6.block(0, 0, 3, 1) << v6;
		xi6.block(3, 0, 3, 1) << omega6;

		/********************************** ��� *******************************/

		Matrix4d G1(4, 4), G2(4, 4), G3(4, 4);
		G1 << g_st * g_st0.inverse();
		double delta;
		Vector4d p, q, r;

		// ������3���Theta3
		delta = (G1 * q3_4 - q1_4).norm();
		p << q3_4;
		q << q1_4;
		r << q2_4;
		Subproblem3(theta, 2, xi3, p, q, r, delta);

		// ������2���Theta1��Theta2
		Isometry3d g3 = Isometry3d::Identity();
		AngleAxisd Omega3(theta[2], omega3);
		g3.rotate(Omega3);
		right3 << (I - Omega3.matrix()) * (omega3.cross(v3)) + omega3 * omega3.transpose() * v3 * theta[2];
		g3.pretranslate(right3);

		p << g3 * q3_4;
		q << G1 * q3_4;
		r << q1_4;
		Subproblem2(theta, 0, 1, xi1, xi2, p, q, r);

		//������3��theta5
		Isometry3d g1 = Isometry3d::Identity();
		Isometry3d g2 = Isometry3d::Identity();
		AngleAxisd Omega1(theta[0], omega1);
		AngleAxisd Omega2(theta[1], omega2);
		g1.rotate(Omega1);
		g2.rotate(Omega2);
		right1 << (I - Omega1.matrix()) * (omega1.cross(v1)) + omega1 * omega1.transpose() * v1 * theta[0];
		right2 << (I - Omega2.matrix()) * (omega2.cross(v2)) + omega2 * omega2.transpose() * v2 * theta[1];
		g1.pretranslate(right1);
		g2.pretranslate(right2);

		G2 << g3.inverse() * g2.inverse() * g1.inverse() * G1;

		delta = (G2 * q0_4 - q4_4).norm();
		p << q0_4;
		q << q4_4;
		r << q3_4;
		Subproblem3(theta, 4, xi5, p, q, r, delta);

		//������1���theta4
		Isometry3d g5 = Isometry3d::Identity();
		AngleAxisd Omega5(theta[4], omega5);
		g5.rotate(Omega5);
		right5 << (I - Omega5.matrix()) * (omega5.cross(v5)) + omega5 * omega5.transpose() * v5 * theta[4];
		g5.pretranslate(right5);

		p << g5 * q1_4;
		q << G2 * q1_4;
		r << q0_4;
		Subproblem1(theta, 3, xi4, p, q, r);

		//������1���theta6
		Isometry3d g4 = Eigen::Isometry3d::Identity();
		AngleAxisd Omega4(theta[3], omega4);
		g4.rotate(Omega4);
		right4 << (I - Omega4.matrix()) * (omega4.cross(v4)) + omega4 * omega4.transpose() * v4 * theta[3];
		g4.pretranslate(right4);

		G3 << g5.inverse() * g4.inverse() * G2;

		p << 100, 0, 0, 1;
		q << G3 * p;
		r << q2_4;
		Subproblem1(theta, 5, xi6, p, q, r);
	}

	/********************************************************************
	ABSTRACT:	������ǰ���˶�ѧ

	INPUTS:		q[6]: 6���ؽڽ�, ��λΪ����
				Tool:���Ժ���

	OUTPUTS:	config[3]����̬����������˶�Ӧ��8����̬��Ϊ�˰�ȫ��
				ʵ����������ֻ����һ�ּ��ɣ�config�����˶�ѧ�н����˳�ʼ�������Բ���Ҫ���и��ġ�
				Turns[6]��ÿ���ؽڶ�Ӧ��Ȧ��
				TransVector[16] : ����任����Ҳ����ĩ�˵�λ�����������г��Ⱦ���Ϊ��

	RETURN:		<none>
	***********************************************************************/
	void robotForwardHJQ(const double* q, const double* Tool, double* TransVector, bool* config, bool* turns)
	{
		config[0] = 1; config[1] = 1; config[2] = 1;
		turns[0] = 1; turns[1] = 1; turns[2] = 1; turns[3] = 1; turns[4] = 1; turns[5] = 1;

		Vector3d q1, q2, q3, q4, q5, q6;                         //���ϵ�
		Vector3d omega1, omega2, omega3, omega4, omega5, omega6; //ת��
		MatrixXd Gst0(4, 4), Gst(4, 4);                          //ʼĩλ��

		omega1 << 0, 0, 1;
		omega2 << 0, 1, 0;
		omega3 << 0, 1, 0;
		omega4 << 0, 0, 1;
		omega5 << 0, 1, 0;
		omega6 << 0, 0, 1;

		q1 << 0, 0, 0;
		q2 << 0, 0, 0.491;
		q3 << 0, 0, 0.941;
		q4 << 0, 0, 0.941;
		q5 << 0, 0, 1.391;
		q6 << 0, 0, 1.475;

		v1 = -omega1.cross(q1);
		v2 = -omega2.cross(q2);
		v3 = -omega3.cross(q3);
		v4 = -omega4.cross(q4);
		v5 = -omega5.cross(q5);
		v6 = -omega6.cross(q6);

		AngleAxisd Eomega1(q[0], Vector3d(0, 0, 1));
		AngleAxisd Eomega2(q[1], Vector3d(0, 1, 0));
		AngleAxisd Eomega3(q[2], Vector3d(0, 1, 0));
		AngleAxisd Eomega4(q[3], Vector3d(0, 0, 1));
		AngleAxisd Eomega5(q[4], Vector3d(0, 1, 0));
		AngleAxisd Eomega6(q[5], Vector3d(0, 0, 1));

		// ָ���������Ͻ�
		right1 = (I - Eomega1.matrix()) * omega1.cross(v1) + omega1 * omega1.transpose() * v1 * q[0];
		right2 = (I - Eomega2.matrix()) * omega2.cross(v2) + omega2 * omega2.transpose() * v2 * q[1];
		right3 = (I - Eomega3.matrix()) * omega3.cross(v3) + omega3 * omega3.transpose() * v3 * q[2];
		right4 = (I - Eomega4.matrix()) * omega4.cross(v4) + omega4 * omega4.transpose() * v4 * q[3];
		right5 = (I - Eomega5.matrix()) * omega5.cross(v5) + omega5 * omega5.transpose() * v5 * q[4];
		right6 = (I - Eomega6.matrix()) * omega6.cross(v6) + omega6 * omega6.transpose() * v6 * q[5];

		// ָ��������
		Isometry3d e_xi1 = Isometry3d::Identity(), e_xi2 = Isometry3d::Identity(), e_xi3 = Isometry3d::Identity(), e_xi4 = Isometry3d::Identity(), e_xi5 = Isometry3d::Identity(), e_xi6 = Isometry3d::Identity();
		e_xi1.rotate(Eomega1.matrix());
		e_xi1.pretranslate(right1);
		e_xi2.rotate(Eomega2.matrix());
		e_xi2.pretranslate(right2);
		e_xi3.rotate(Eomega3.matrix());
		e_xi3.pretranslate(right3);
		e_xi4.rotate(Eomega4.matrix());
		e_xi4.pretranslate(right4);
		e_xi5.rotate(Eomega5.matrix());
		e_xi5.pretranslate(right5);
		e_xi6.rotate(Eomega6.matrix());
		e_xi6.pretranslate(right6);

		Gst0 << -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 1.475, 0, 0, 0, 1;

		//����λ��
		Gst = e_xi1.matrix() * e_xi2.matrix() * e_xi3.matrix() * e_xi4.matrix() * e_xi5.matrix() * e_xi6.matrix() * Gst0;

		//��ֵ���������Tran[]
		int count = 0;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				TransVector[count] = Gst(i, j);
				count++;
			}
		}
	}

	/*
	 * ������1
	 */
	bool Subproblem1(double* theta, const int index, const VectorXd& xi, const Vector4d& p_4, const Vector4d& q_4, const Vector4d& r_4)
	{
		// omega,��p,q,r����ĳ�ʼ��
		Vector3d omega(3, 1), p(3, 1), q(3, 1), r(3, 1);
		omega << xi.block(3, 0, 3, 1);
		p << p_4.block(0, 0, 3, 1);
		q << q_4.block(0, 0, 3, 1);
		r << r_4.block(0, 0, 3, 1);

		// u,v,t,u_prime,v_prime ��������ĳ�ʼ��
		Vector3d u(3, 1), v(3, 1), t(3, 1), u_prime(3, 1), v_prime(3, 1);
		u << p - r;
		v << q - r;
		t << u.transpose() * omega * omega;
		u_prime << u - t;
		v_prime << v - t;

		// �������
		if (1)
		{
			if (u_prime.norm() == 0)
			{
				theta[index] = 0;
				cout << "There are Infinite numbers of solutions for Subproblem1 !!!\t\t\t The angle is theta" << index + 1 << " !" << endl;
			}
			else
			{
				theta[index] = atan2(omega.transpose() * (u_prime.cross(v_prime)), u_prime.transpose() * v_prime);
			}
			return true;
		}
		else
		{
			cout << "There is no solution for Subproblem1 !!!\t\t\t The angle is theta" << index + 1 << " !" << endl;
			theta[index] = 0;
			return false;
		}


	}

	/*
	 * ������2
	*/
	bool Subproblem2(double* theta, const int index1, const int index2, const VectorXd& xi1, const VectorXd& xi2, const Vector4d& p_4, const Vector4d& q_4, const Vector4d& r_4)
	{
		// omega1, omega2 ��p,q,r����ĳ�ʼ��
		Vector3d omega1(3, 1), omega2(3, 1), p(3, 1), q(3, 1), r(3, 1);
		omega1 << xi1.block(3, 0, 3, 1);
		omega2 << xi2.block(3, 0, 3, 1);
		p << p_4.block(0, 0, 3, 1);
		q << q_4.block(0, 0, 3, 1);
		r << r_4.block(0, 0, 3, 1);

		// u,v ���������Լ�alpha,beta�ĳ�ʼ��
		Vector3d u(3, 1), v(3, 1);
		double alpha, beta, temp, temp1, temp4, temp3;             // temp,temp1,temp2,temp3Ϊ�м��������
		u << p - r;
		v << q - r;
		temp = omega1.transpose() * omega2;
		temp4 = omega2.transpose() * u;
		temp3 = omega1.transpose() * v;
		alpha = (temp * temp4 - temp3) / (temp * temp - 1);
		beta = (temp * temp3 - temp4) / (temp * temp - 1);
		temp1 = u.squaredNorm() - alpha * alpha - beta * beta - 2 * alpha * beta * temp;

		// �������
		if (temp1 < 0)
		{
			cout << "There is no solution for Subproblem2 !!!\t\t\t The angles are theta" << index1 + 1 << " and theta" << index2 + 1 << " !" << endl;
			theta[index1] = 0;
			theta[index2] = 0;
			return false;
		}
		else if (temp1 == 0)
		{
			Vector4d z, c;
			z.block(0, 0, 3, 1) << alpha * omega1 + beta * omega2;
			c.block(0, 0, 3, 1) << z.block(0, 0, 3, 1) + r;
			Subproblem1(theta, index2, xi2, p_4, c, r_4);
			Subproblem1(theta, index1, xi1, c, q_4, r_4);
			return true;
		}
		else
		{
			Vector4d temp2, z1, c1;
			double gamma1;
			temp2.block(0, 0, 3, 1) << omega1.cross(omega2);
			gamma1 = -sqrt((temp1 / (temp2.block(0, 0, 3, 1)).squaredNorm()));
			z1.block(0, 0, 3, 1) << alpha * omega1 + beta * omega2 + gamma1 * temp2.block(0, 0, 3, 1);
			c1.block(0, 0, 3, 1) << z1.block(0, 0, 3, 1) + r;
			Subproblem1(theta, index2, xi2, p_4, c1, r_4);
			Subproblem1(theta, index1, xi1, c1, q_4, r_4);
			return true;
		}
	}

	/*
	* ������3
	*/
	bool Subproblem3(double* theta, const int index, const VectorXd& xi, const Vector4d& p_4, const Vector4d& q_4, const Vector4d& r_4, const double delta)
	{
		// omega,��p,q,r����ĳ�ʼ��
		Vector3d omega(3, 1), p(3, 1), q(3, 1), r(3, 1);
		omega << xi.block(3, 0, 3, 1);
		p << p_4.block(0, 0, 3, 1);
		q << q_4.block(0, 0, 3, 1);
		r << r_4.block(0, 0, 3, 1);

		// u,v,u_prime,v_prime �ĸ������ĳ�ʼ��
		Vector3d u(3, 1), v(3, 1), t(3, 1), u_prime(3, 1), v_prime(3, 1);
		u << p - r;
		v << q - r;
		u_prime << u - u.transpose() * omega * omega;
		v_prime << v - v.transpose() * omega * omega;

		// theta0,delta_prime_squ�����ĳ�ʼ��
		double theta0, delta_prime_squ;
		theta0 = atan2(omega.transpose() * (u_prime.cross(v_prime)), u_prime.transpose() * v_prime);
		delta_prime_squ = delta * delta - (omega.transpose() * (p - q)).squaredNorm();

		// �м����temp
		double temp = u_prime.squaredNorm() + v_prime.squaredNorm() - delta_prime_squ;
		temp = temp / (2 * u_prime.norm() * v_prime.norm());

		// �������
		if (abs(temp) > 1)
		{
			cout << "There is no solution for Subproblem3 !!!\t\t\t The angle is theta" << index + 1 << " !" << endl;
			theta[index] = 0;
			return false;
		}
		else
		{
			double alpha = acos(temp);
			theta[index] = theta0 - alpha;
			return true;
		}

	}

	/*
	 * brief��Ĭ������µ����
	 * param��*EMatrix ����任����
	 *        *angle   ��õĹؽڽ�
	 */
	void GetJointAngles(double* EMatrix, double* angle)
	{
		bool config[3] = { 1,1,1 };
		bool Turns[6] = { 1,1,1,1,1,1 };
		double Tool[16]{ 0 };
		double theta[6] = { 0 };
		HLRobot::robotBackwardHJQ(EMatrix, config, Tool, Turns, theta);
		for (int i = 0; i < 6; i++)
		{
			angle[i] = theta[i] * 180 / PI;
		}
	}
}