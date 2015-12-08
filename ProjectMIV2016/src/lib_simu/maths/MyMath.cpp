
#include "MyMath.h"
#include <math.h>

/**
 * Matrix to quaternion 
 * src: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm 
 */
float* MyMath::matrixToQuaternion(float* mat_src, float* quat_dest) {
	float trace = mat_src[M3_11] + mat_src[M3_22] + mat_src[M3_33] + 1.0f;
	if(trace > M_EPSILON) {
		float s = 0.5f / sqrtf(trace);
		quat_dest[Q_W] = 0.25f / s;
		quat_dest[Q_X] = ( mat_src[M3_32] - mat_src[M3_23] ) * s;
		quat_dest[Q_Y] = ( mat_src[M3_13] - mat_src[M3_31] ) * s;
		quat_dest[Q_Z] = ( mat_src[M3_21] - mat_src[M3_12] ) * s;
	} else {
		if ( mat_src[M3_11] > mat_src[M3_22] && mat_src[M3_11] > mat_src[M3_33] ) {
			float s = 2.0f * sqrtf( 1.0f + mat_src[M3_11] - mat_src[M3_22] - mat_src[M3_33]);
			quat_dest[Q_W] = (mat_src[M3_32] - mat_src[M3_23] ) / s;
			quat_dest[Q_X] = 0.25f * s;
			quat_dest[Q_Y] = (mat_src[M3_12] + mat_src[M3_21] ) / s;
			quat_dest[Q_Z] = (mat_src[M3_13] + mat_src[M3_31] ) / s;
		} else if (mat_src[M3_22] > mat_src[M3_33]) {
			float s = 2.0f * sqrtf( 1.0f + mat_src[M3_22] - mat_src[M3_11] - mat_src[M3_33]);
			quat_dest[Q_W] = (mat_src[M3_13] - mat_src[M3_31] ) / s;
			quat_dest[Q_X] = (mat_src[M3_12] + mat_src[M3_21] ) / s;
			quat_dest[Q_Y] = 0.25f * s;
			quat_dest[Q_Z] = (mat_src[M3_23] + mat_src[M3_32] ) / s;
		} else {
			float s = 2.0f * sqrtf( 1.0f + mat_src[M3_33] - mat_src[M3_11] - mat_src[M3_22] );
			quat_dest[Q_W] = (mat_src[M3_21] - mat_src[M3_12] ) / s;
			quat_dest[Q_X] = (mat_src[M3_13] + mat_src[M3_31] ) / s;
			quat_dest[Q_Y] = (mat_src[M3_23] + mat_src[M3_32] ) / s;
			quat_dest[Q_Z] = 0.25f * s;
		}
	}

	return quat_dest;
}

/**
 * Extract orientation from 4x4 transormation matrix 
 *
 */
float* MyMath::getOrientationFrom4x4Matrix(float* mat_src, float* mat_dest) {
	mat_dest[M3_11] = mat_src[M4_11];
	mat_dest[M3_12] = mat_src[M4_12];
	mat_dest[M3_13] = mat_src[M4_13];
	mat_dest[M3_21] = mat_src[M4_21];
	mat_dest[M3_22] = mat_src[M4_22];
	mat_dest[M3_23] = mat_src[M4_23];
	mat_dest[M3_31] = mat_src[M4_31];
	mat_dest[M3_32] = mat_src[M4_32];
	mat_dest[M3_33] = mat_src[M4_33];

	return mat_dest;
}

/**
 * Extract translation from 4x4 transormation matrix 
 *
 */
float* MyMath::getTranslationFrom4x4Matrix(float* mat_src, float* vect_dest) {
	vect_dest[V_X] = mat_src[M4_41];
	vect_dest[V_Y] = mat_src[M4_42];
	vect_dest[V_Z] = mat_src[M4_43];
	
	return vect_dest;
}

/**
 * Construct a cross product matrix from a 3 component vector
 *
 */
float* MyMath::crossMatrix(float* vect_src, float* mat_dest) {
	
	mat_dest[M3_11] = 0.0f;
	mat_dest[M3_12] = -vect_src[V_Z];
	mat_dest[M3_13] = vect_src[V_Y];

	mat_dest[M3_21] = vect_src[V_Z];
	mat_dest[M3_22] = 0.0f;
	mat_dest[M3_23] = -vect_src[V_X];

	mat_dest[M3_31] = -vect_src[V_Y];
	mat_dest[M3_32] = vect_src[V_X];
	mat_dest[M3_33] = 0.0f;

	return mat_dest;
}

/**
 * Matrix 3x3 multiplication
 *
 */
float* MyMath::matrix3x3Mult(float* mat_src1, float* mat_src2, float* mat_dest) {

	mat_dest[M3_11] = mat_src1[M3_11]*mat_src2[M3_11] + mat_src1[M3_12]*mat_src2[M3_21] + mat_src1[M3_13]*mat_src2[M3_31];
	mat_dest[M3_12] = mat_src1[M3_11]*mat_src2[M3_12] + mat_src1[M3_12]*mat_src2[M3_22] + mat_src1[M3_13]*mat_src2[M3_32];
	mat_dest[M3_13] = mat_src1[M3_11]*mat_src2[M3_13] + mat_src1[M3_12]*mat_src2[M3_23] + mat_src1[M3_13]*mat_src2[M3_33];

	mat_dest[M3_21] = mat_src1[M3_21]*mat_src2[M3_11] + mat_src1[M3_22]*mat_src2[M3_21] + mat_src1[M3_23]*mat_src2[M3_31];
	mat_dest[M3_22] = mat_src1[M3_21]*mat_src2[M3_12] + mat_src1[M3_22]*mat_src2[M3_22] + mat_src1[M3_23]*mat_src2[M3_32];
	mat_dest[M3_23] = mat_src1[M3_21]*mat_src2[M3_13] + mat_src1[M3_22]*mat_src2[M3_23] + mat_src1[M3_23]*mat_src2[M3_33];

	mat_dest[M3_31] = mat_src1[M3_31]*mat_src2[M3_11] + mat_src1[M3_32]*mat_src2[M3_21] + mat_src1[M3_33]*mat_src2[M3_31];
	mat_dest[M3_32] = mat_src1[M3_31]*mat_src2[M3_12] + mat_src1[M3_32]*mat_src2[M3_22] + mat_src1[M3_33]*mat_src2[M3_32];
	mat_dest[M3_33] = mat_src1[M3_31]*mat_src2[M3_13] + mat_src1[M3_32]*mat_src2[M3_23] + mat_src1[M3_33]*mat_src2[M3_33];

	return mat_dest;
}

/**
 * Matrix 3x3 inversion
 *
 */
float* MyMath::matrix3x3Invert(float* mat_src, float* mat_dest) {

	return mat_dest;
}

/**
 * Matrix 3x3 transposition
 *
 */
float* MyMath::matrix3x3Transpose(float* mat_src, float* mat_dest) {

	mat_dest[M3_11] = mat_src[M3_11];
	mat_dest[M3_12] = mat_src[M3_21];
	mat_dest[M3_13] = mat_src[M3_31];
	mat_dest[M3_21] = mat_src[M3_12];
	mat_dest[M3_22] = mat_src[M3_22];
	mat_dest[M3_23] = mat_src[M3_32];
	mat_dest[M3_31] = mat_src[M3_13];
	mat_dest[M3_32] = mat_src[M3_23];
	mat_dest[M3_33] = mat_src[M3_33];

	return mat_dest;
}

/**
 * Matrix 3x3 vector multiplication
 *
 */
float* MyMath::matrixVectMult(float* mat_src, float* vect_src, float* vect_dest) {
	vect_dest[V_X] = mat_src[M3_11]*vect_src[V_X] + mat_src[M3_12]*vect_src[V_Y] + mat_src[M3_13]*vect_src[V_Z];
	vect_dest[V_Y] = mat_src[M3_21]*vect_src[V_X] + mat_src[M3_22]*vect_src[V_Y] + mat_src[M3_23]*vect_src[V_Z];
	vect_dest[V_Z] = mat_src[M3_31]*vect_src[V_X] + mat_src[M3_32]*vect_src[V_Y] + mat_src[M3_33]*vect_src[V_Z];

	return vect_dest;
}

/**
 * Matrix 3x3 addition
 *
 */
float* MyMath::matrix3x3Add(float* mat_src1, float* mat_src2, float* mat_dest) {

	mat_dest[M3_11] = mat_src1[M3_11] + mat_src2[M3_11];
	mat_dest[M3_12] = mat_src1[M3_12] + mat_src2[M3_12];
	mat_dest[M3_13] = mat_src1[M3_13] + mat_src2[M3_13];
	mat_dest[M3_21] = mat_src1[M3_21] + mat_src2[M3_21];
	mat_dest[M3_22] = mat_src1[M3_22] + mat_src2[M3_22];
	mat_dest[M3_23] = mat_src1[M3_23] + mat_src2[M3_23];
	mat_dest[M3_31] = mat_src1[M3_31] + mat_src2[M3_31];
	mat_dest[M3_32] = mat_src1[M3_32] + mat_src2[M3_32];
	mat_dest[M3_33] = mat_src1[M3_33] + mat_src2[M3_33];

	return mat_dest;

}

/**
 * Matrix 3x3 scalar multiplication
 *
 */
float* MyMath::matrix3x3Scalar(float* mat_src, float scalar, float* mat_dest) {

	mat_dest[M3_11] = mat_src[M3_11]*scalar;
	mat_dest[M3_12] = mat_src[M3_12]*scalar;
	mat_dest[M3_13] = mat_src[M3_13]*scalar;
	mat_dest[M3_21] = mat_src[M3_21]*scalar;
	mat_dest[M3_22] = mat_src[M3_22]*scalar;
	mat_dest[M3_23] = mat_src[M3_23]*scalar;
	mat_dest[M3_31] = mat_src[M3_31]*scalar;
	mat_dest[M3_32] = mat_src[M3_32]*scalar;
	mat_dest[M3_33] = mat_src[M3_33]*scalar;

	return mat_dest;
}

/**
 * Vector addition
 *
 */
float* MyMath::vectAdd(float* vect_src1, float* vect_src2, float* vect_dest) {

	vect_dest[V_X] = vect_src1[V_X] + vect_src2[V_X];
	vect_dest[V_Y] = vect_src1[V_Y] + vect_src2[V_Y];
	vect_dest[V_Z] = vect_src1[V_Z] + vect_src2[V_Z];

	return vect_dest;
}

/**
 * Vector scalar multiplication
 *
 */
float* MyMath::vectScalar(float* vect_src, float scalar, float* vect_dest) {

	vect_dest[V_X] = vect_src[V_X]*scalar;
	vect_dest[V_Y] = vect_src[V_Y]*scalar;
	vect_dest[V_Z] = vect_src[V_Z]*scalar;

	return vect_dest;
}