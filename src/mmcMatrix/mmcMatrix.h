/*
Copyright (c) 2006-2014, Michael Greminger
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF A
DVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef mmcMatrixH
#define mmcMatrixH

#include <iostream>


// If MMC_ERROR_CHECK is set to 1, mmcMatrix class checks for overruns when accessing matrices.
// If MMC_ERROR_CHECK is set to 0, the class will be much faster and some methods will be inlined.
#define MMC_ERROR_CHECK 1


const double mmcPI = 3.14159265359;

enum mmcECODE {CANNOT_CREATE, ADD_INCOMPAT, SIZE_ZERO, MULT_INCOMPAT, OVERRUN, NOT_SQUARE, NOT_VECTOR, VECTOR_INCOMPAT, SINGULAR, CANNOT_CROSS, CANNOT_COMBINE, CANNOT_SUB, CHILD_ERROR, IP_ERROR, CANNOT_OPEN, DIVIDE_BY_ZERO, NOT_3X3};

const char mmcECODE_STRINGS[][250] = {"Attempt to create or resize a matrix with the number of rows or columns less that or equal to 0.",
                "Matrices not compatible for addition, subraction or element-wise multiplication operations.",
								"Cannot perform this operation with a matrix of size zero.",
								"Matrices not compatible for multiplication.",
								"Attempt to write to or read from a matrix with dimensions that are not within the size of the matrix.",
								"Matrix must be square for this operation.",
								"Matrix must be a vector for this operation.",
								"Matrices are not compatible for distance calculation, dot product, magnitude or cross product calculations.",
								"Matrix is singular.",
								"Matrices are not 3x1 or 1x3 for cross product.",
								"Cannot combine matrices into a row or column matrice.",
								"Cannot get/set sub matrix.",
								"Error occurred in child class.",
                "Result formal paramter can not be the class itself or another formal parameter.",
								"Cannot open file.",
								"Divide by zero.",
								"Matrix must be 3x3 to perform this operation."
};

// format to use when writing the matrix to the output buffer
enum mmcOUTPUT_FORMAT {MMC_NATIVE, MATLAB_ASCII};

// maximum number of charactors that the matlab name for a matrix can have
// this name is only used for outputting files in the MATLAB format
const int mmcMAX_NAME_LENGTH = 25;

// declare constants and data types used for multithreading (set to 1 for a uniprocessor system for maximum efficiency)
const int mmcMaxNumThreads = 1;
const int mmcChunkSize = 100;
const int mmcThreshold = 60000;  // min number of elements a matrix should have in order to use multithreading
typedef struct mmcThreadData_s{  // struct to store data used by threads
  double *rhs_data;
  double *lhs_data;
  double *result_data;
  int rhs_num_rows;
  int rhs_num_columns;
  int lhs_num_rows;
  int lhs_num_columns;
  int result_num_rows;
  int result_num_columns;
  int current_row;                  // Mutex mmcLock must be used to access this value
  int current_column;            // Mutex mmcLock must be used to access this value
}mmcThreadData;
// thread routine used for matrix multiplication
void *mmcThreadMultiply(void *thread_data);

      
class mmcMatrix
{
public:
  // Default constructer, does not allocate memory
  mmcMatrix(); 
 
  // Constructer that allocates memory for a matrix and initializes
  // every element of the matrix.
  mmcMatrix(int rows, int columns, double initial_value);
  mmcMatrix(int rows, int columns);
	mmcMatrix(const char *file_name);          // Read from file

  // matrix destructor
  ~mmcMatrix();
 
  //copy constructer
  mmcMatrix(const mmcMatrix &);

  // methods to set or get matrix elements
#if MMC_ERROR_CHECK==1 // error checking enabled, will used original methods not the inlined ones below
  void SetElement( int row, int col, double new_value);  
  double& operator() (int row, int col);
  double GetElement(int row, int col)const;
  double operator() (int row, int col)const;  
#else // no error checking, so will inline these functions
  void SetElement( int row, int col, double new_value){MatrixData[row * NumColumns + col] = new_value;}  
  double& operator() (int row, int col)               {return MatrixData[row * NumColumns + col];}
  double GetElement(int row, int col)const            {return MatrixData[row * NumColumns + col];}
  double operator() (int row, int col)const           {return MatrixData[row * NumColumns + col];}  
#endif

  void SetSize(int rows, int columns, double initial_value); // only added elements are set to initial_value when resizing, existing elements remain unchanged when resizing
  void SetSize(int rows, int columns);

  void SetOutputFormat(mmcOUTPUT_FORMAT output_format) {OutputFormat = output_format;}

  // overloaded operators:
  //assignment operator (overload of the binary = operator)
  mmcMatrix & operator=(const mmcMatrix & rhs);
  //addition operator (overload of the binary + operator)
  mmcMatrix operator+(const mmcMatrix & rhs)const;
  //minus operator (overload of the binary - operator)
  mmcMatrix operator-(const mmcMatrix & rhs)const;
  //multiplication operator (overload of the binary * operator)
  mmcMatrix operator*(const mmcMatrix & rhs)const;
  mmcMatrix operator*(double scale_factor)const;
  friend mmcMatrix operator*(double scale_factor, const mmcMatrix & rhs);
  const mmcMatrix & operator+=(const mmcMatrix & rhs);
  const mmcMatrix & operator-=(const mmcMatrix & rhs);
  const mmcMatrix & operator*=(double scale_factor);
  
  // accessor functions that do not change the object
  int GetNumRows()const {return NumRows;} 
  int GetNumColumns()const {return NumColumns;}

  void DisplayMatrix()const;  //Prints the matrix to the screen
  
  // functions that return a mmcMatrix object without changing 
  // the original matrix.
  mmcMatrix GetInverse3By3()const;  // return inverse of original a 3x3 matrix
  double GetDeterminate3By3()const; // return the determinate of a 3x3 matrix
  mmcMatrix GetTranspose()const; // return transpose of original matrix
  mmcMatrix GetScaled(double scale_factor)const; // return scaled version of original matrix
  double DotProduct(const mmcMatrix & rhs)const; // calculates dot product
  mmcMatrix CrossProduct(const mmcMatrix & rhs)const;  // calculates dot product
  mmcMatrix ElementWiseMultiplication(const mmcMatrix & rhs)const; 
  mmcMatrix CombineAsColumn(const mmcMatrix & rhs)const;
  mmcMatrix CombineAsRow(const mmcMatrix & rhs)const;
  mmcMatrix GetSubMatrix(int start_row, int start_col,
	                     int end_row, int end_col)const;
  
  // Preforms a Discrete Cosine Transform on the vector or matrix
  mmcMatrix GetDCT()const;

  //functions that return a matrix object and change the original matrix
  const mmcMatrix & SetIdentity(); // sets the matrix to the identity matrix
	const mmcMatrix & SetZero();     // sets the matrix elements to zero
  const mmcMatrix & SetSubmatrix(int start_row, int start_column, const mmcMatrix &input_matrix);
  const mmcMatrix & SetRandom(double min, double max); // sets the matrix to random values between min and max

  // set matrix name used when outputting the matrix to file
  const mmcMatrix & SetName(const char *name);
  
	//function that returns double without changing original matrix
  double GetDistanceTo(const mmcMatrix &input_vector, bool take_sqrt = true)const;

  double GetMagnitude()const;
  mmcMatrix GetNormalized()const;

  double *GetCVector()const; // Allocates memory for and returns a c style vector
                             // Application that calls this function must free the memory that was allocated
  double *GetMatrixData()const{return MatrixData;}

  // overide the indirection operators, used for file I/O
  friend std::ostream& operator<< ( std::ostream &os, const mmcMatrix &rhs );
  friend std::istream& operator>> (std::istream& is, mmcMatrix &rhs);
  
  // file IO
  bool WriteToFile(const char *file_name)const;
  bool ReadFromFile(const char *file_name);

	void Clear(); // deletes all matrix data and sets its size to 0x0

private:
  int NumRows;
  int NumColumns;

  // Pointer to actual matrix data.  Has the value of NULL before memory
  // has been allocated to the matrix.
  double *MatrixData;

  // Flag indicating which output format to use
  mmcOUTPUT_FORMAT OutputFormat;

  // Name to give matrix when output format is MATLAB
  char Name[mmcMAX_NAME_LENGTH];
}; 

// the exception classes are now defined
class mmcException{
public:	
  mmcException(mmcECODE error_code,int line_num) {ErrorCode = error_code;
                                                  std::cerr << "mmcError: " << mmcECODE_STRINGS[error_code] << " at line " << line_num << " in " << "mmcMatrix.cpp." << std::endl;}
  mmcException(mmcECODE error_code,int line_num, const char *source_file) {ErrorCode = error_code;
                                                                           std::cerr << "mmcError: " << mmcECODE_STRINGS[error_code] << " at line " << line_num << " in " << source_file << "." << std::endl;}

private:
  mmcECODE ErrorCode;
}; 



#endif //mmcMatrixH
