
#ifndef MY_MATH_H
#define MY_MATH_H

#define M3_11		0
#define M3_12		1
#define M3_13		2
#define M3_21		3
#define M3_22		4
#define M3_23		5
#define M3_31		6
#define M3_32		7
#define M3_33		8

#define M4_11		0
#define M4_12		1
#define M4_13		2
#define M4_14		3
#define M4_21		4
#define M4_22		5
#define M4_23		6
#define M4_24		7
#define M4_31		8
#define M4_32		9
#define M4_33		10
#define M4_34		11
#define M4_41		12
#define M4_42		13
#define M4_43		14
#define M4_44		15

#define Q_X			0
#define Q_Y			1
#define Q_Z			2
#define Q_W			3

#define V_X			0
#define V_Y			1
#define V_Z			2

#define M_EPSILON	0.00001

/*! \class MyMath
 * \brief Basic math definitions
 *
 */
class MyMath {

private:

public:

	static float* matrixToQuaternion(float* mat_src, float* quat_dest);

	static float* getOrientationFrom4x4Matrix(float* mat_src, float* mat_dest);
	static float* getTranslationFrom4x4Matrix(float* mat_src, float* vect_dest);

	static float* crossMatrix(float* vect_src, float* mat_dest);

	static float* matrix3x3Mult(float* mat_src1, float* mat_src2, float* mat_dest);
	static float* matrix3x3Invert(float* mat_src, float* mat_dest);
	static float* matrix3x3Transpose(float* mat_src, float* mat_dest);
	static float* matrix3x3Add(float* mat_src1, float* mat_src2, float* mat_dest);
	static float* matrix3x3Scalar(float* mat_src, float scalar, float* mat_dest);

	static float* vectAdd(float* vect_src1, float* vect_src2, float* vect_dest);
	static float* vectScalar(float* vect_src1, float scalar, float* vect_dest);

	static float* matrixVectMult(float* mat_src, float* vect_src, float* vect_dest);

};

#endif