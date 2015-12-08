
#ifndef MATRIX_H
#define MATRIX_H

typedef float number;

#define SIZE_CHECK


namespace Maths {

/*! \class Matrix
 * \brief Base class for all n by m matrix description
 */
class Matrix {

private:


protected:

	/*!
     *  \brief allocate memory for an n*m matrix
     */
	virtual void allocateMemory(int n, int m);

	/*!
     *  \brief free all allocated memory
     */
	virtual void freeMemory();

	/*!
     *  \brief copy the matrix content
     */
	virtual void copy(const Matrix& src);


	number* data;		/*!< Pointer to the data of the matrix */

	int n;				/*!< number of lines of the matrix */
	int m;				/*!< number of columns of the matrix */
	int nb;				/*!< number of elements into the matrix */

public:

	/*!
     *  \brief allocate memory for an n*m matrix
     */
	Matrix(int n, int m);
	Matrix(const Matrix& src);
	~Matrix();

	/*!
     *  \brief set an element of the matrix at (i:line, j:column)
     */
	void set(int i, int j, number val);

	/*!
     *  \brief return the value of element at (i:line, j:column)
     */
	number get(int i, int j) const;

	/*!
     *  \brief asignment operator
     */
	Matrix& operator=(const Matrix& src);

};

}

#endif