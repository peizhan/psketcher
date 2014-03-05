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


#include <pthread.h>
#include <iomanip>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <string.h>
#include <gsl/gsl_linalg.h>
#include "mmcMatrix.h"

using namespace std;

pthread_mutex_t mmcLock; // mutex lock

// Default constructor, does not allocate memory
mmcMatrix::mmcMatrix()
{
	NumRows = 0;
	NumColumns = 0;
	MatrixData = NULL;
	OutputFormat = MMC_NATIVE;
	Name[0] = '\0';
}


// Constructor which allocates memory and initializes matrix data
mmcMatrix::mmcMatrix(int rows, int columns, double initial_value)
{
  int row, col;

  OutputFormat = MMC_NATIVE;
  Name[0] = '\0';

  if(MMC_ERROR_CHECK)
  { 
    // check for valid dimensions
    // if dimensions are not valid, throw an exception
    if(rows < 0 || columns < 0 || columns == 0 || rows == 0)
      throw mmcException(CANNOT_CREATE, __LINE__);
  }
    
  // initialize the member variables staring matrix size
  NumRows = rows;
  NumColumns = columns;

  // allocate matrix memory
  MatrixData = new double[rows * columns];

  // initialize all of the elements of the matrix
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      SetElement(row,col,initial_value);
}

// Constructor which allocates memory but does not initialize matrix data
mmcMatrix::mmcMatrix(int rows, int columns)
{
  int row, col;

  OutputFormat = MMC_NATIVE;
  Name[0] = '\0';

  if(MMC_ERROR_CHECK)
  { 
    // check for valid dimensions
    // if dimensions are not valid, throw an exception
    if(rows < 0 || columns < 0 || columns == 0 || rows == 0)
      throw mmcException(CANNOT_CREATE, __LINE__);
  }
    
  // initialize the member variables staring matrix size
  NumRows = rows;
  NumColumns = columns;

  // allocate matrix memory
  MatrixData = new double[rows * columns];
}

// Member function that resizes the matrix and initializes the new elements of the matrix
void mmcMatrix::SetSize(int rows, int columns, double initial_value)
{
  
  if(MMC_ERROR_CHECK)
  {
    // check for valid dimensions
    // if dimensions are not valid, throw an exception
    if(rows < 0 || columns < 0 || columns == 0 || rows == 0)
      throw mmcException(CANNOT_CREATE, __LINE__);
  }

  // Need to remember the original size of the matrix
  int old_num_rows = NumRows;
  int old_num_columns = NumColumns;

  // initialize the member variables staring matrix size
  NumRows = rows;
  NumColumns = columns;

  // if matrix was empty to start out with, allocate new memory and set to initial value
  if(MatrixData == NULL)
  {
    MatrixData = new double[rows * columns];

    // initialize all of the elements of the matrix
    for(int row = 0; row < NumRows; row++)
      for(int col = 0; col < NumColumns; col++)
        SetElement(row,col,initial_value);

  }else {  // Matrix was not empty so it will be resized retaining values of old elements
    double *old_buffer = MatrixData;
    MatrixData = new double[rows * columns];

    // initialize all of the elements of the matrix
    for(int row = 0; row < NumRows; row++)
      for(int col = 0; col < NumColumns; col++)
      {
        if(col < old_num_columns && row < old_num_rows)
          SetElement(row,col,old_buffer[row * old_num_columns + col]);
        else
          SetElement(row,col,initial_value);
      }

    // free the old buffer
    delete [] old_buffer;
  }
}



// Member function that resizes the matrix without initializing the new elements
void mmcMatrix::SetSize(int rows, int columns)
{
  if(MMC_ERROR_CHECK)
  {
    // check for valid dimensions
    // if dimensions are not valid, throw an exception
    if(rows < 0 || columns < 0 || columns == 0 || rows == 0)
      throw mmcException(CANNOT_CREATE, __LINE__);
  }

  // Need to remember the original size of the matrix
  int old_num_rows = NumRows;
  int old_num_columns = NumColumns;

  // initialize the member variables staring matrix size
  NumRows = rows;
  NumColumns = columns;

  // if matrix was empty to start out with, allocate new memory and set to initial value
  if(MatrixData == NULL)
  {
    MatrixData = new double[rows * columns];
  }else {  // Matrix was not empty so it will be resized retaining values of old elements
    double *old_buffer = MatrixData;
    MatrixData = new double[rows * columns];

    // retain the old values in the matrix
    for(int row = 0; row < NumRows; row++)
      for(int col = 0; col < NumColumns; col++)
      {
        if(col < old_num_columns && row < old_num_rows)
          SetElement(row,col,old_buffer[row * old_num_columns + col]);
      }

    // free the old buffer
    delete [] old_buffer;
  }
}



// Destructor which frees memory
mmcMatrix::~mmcMatrix()
{
  NumRows = 0;
  NumColumns = 0;
  if(MatrixData != NULL)
    delete [] MatrixData;
  MatrixData = NULL;
}

// copy constructor
mmcMatrix::mmcMatrix(const mmcMatrix & rhs)
{
  int row, col;

  OutputFormat = MMC_NATIVE;
  Name[0] = '\0';

  // copy the number of rows and columns
  NumRows = rhs.GetNumRows();
  NumColumns = rhs.GetNumColumns();

  // allocate memory for the matrix if rhs has memory allocated
  if(rhs.MatrixData != NULL)
    MatrixData = new double[NumRows * NumColumns];
  else
    MatrixData = NULL;

  double *rhs_matrix_data = rhs.GetMatrixData();
  int rhs_num_columns = rhs.GetNumColumns();  
  
  // copy matrix data element by element
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
	    MatrixData[row*NumColumns + col] =  rhs_matrix_data[row*rhs_num_columns + col];
}


// Assignment operator overload function (overloads binary =)
mmcMatrix & mmcMatrix::operator=(const mmcMatrix & rhs)
{
  int row, col;

  // check to see if the objects on the left hand side and the rignt
  // hand side are equal
  if (this == &rhs)
    return *this;

  // deallocate the memory already allocated for this matrix object
  if(MatrixData != NULL)
    delete [] MatrixData;
  MatrixData = NULL;

  // copy the number of rows and columns
  NumRows = rhs.GetNumRows();
  NumColumns = rhs.GetNumColumns();

  // allocate memory for the matrix if rhs has memory allocated
  if(rhs.MatrixData != NULL)
    MatrixData = new double[NumRows * NumColumns];
  else
    MatrixData = NULL;

  double *rhs_matrix_data = rhs.GetMatrixData();
  int rhs_num_columns = rhs.GetNumColumns();
  
  // copy matrix data element by element
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      MatrixData[row*NumColumns + col] =  rhs_matrix_data[row*rhs_num_columns + col];

  return *this;
}

//  Addition operator overload function (overlaods binary +)
mmcMatrix mmcMatrix::operator+(const mmcMatrix & rhs)const
{
  int row, col;

  
  if(MMC_ERROR_CHECK)
  {
    // insure that both matrices have the same dimensions
    if(NumRows != rhs.GetNumRows() || NumColumns != rhs.GetNumColumns())
      throw mmcException(ADD_INCOMPAT, __LINE__);
  
    // insure matrices have at least one element
    if((NumRows == 0 && NumColumns == 0) || (rhs.GetNumRows() == 0 &&
               rhs.GetNumColumns() == 0))
      throw mmcException(SIZE_ZERO, __LINE__);
  
     // insure that there is memory allocated for the left hand side matrix
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that there is memory allocated for the right had side matrix
    if(rhs.MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  // create a new mmcMatrix object in order to store the result
  mmcMatrix result(NumRows, NumColumns);

  double *result_matrix_data = result.GetMatrixData();
  int result_num_columns = result.GetNumColumns();
  double *rhs_matrix_data = rhs.GetMatrixData();
  int rhs_num_columns = rhs.GetNumColumns();  
  
  // Add the two matrices element by element
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
        result_matrix_data[row*result_num_columns + col] = MatrixData[row*NumColumns + col] + rhs_matrix_data[row*rhs_num_columns + col];

  // return the result
  return result;
}


const mmcMatrix & mmcMatrix::operator+=(const mmcMatrix & rhs)
{
  int row, col;
  
  if(MMC_ERROR_CHECK)
  {
    // insure that both matrices have the same dimensions
    if(NumRows != rhs.GetNumRows() || NumColumns != rhs.GetNumColumns())
      throw mmcException(ADD_INCOMPAT, __LINE__);
  
    // insure matrices have at least one element
    if((NumRows == 0 && NumColumns == 0) || (rhs.GetNumRows() == 0 &&
               rhs.GetNumColumns() == 0))
      throw mmcException(SIZE_ZERO, __LINE__);
  
     // insure that there is memory allocated for the left hand side matrix
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that there is memory allocated for the right had side matrix
    if(rhs.MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  double *rhs_matrix_data = rhs.GetMatrixData();
  int rhs_num_columns = rhs.GetNumColumns();  
  
  // Add the two matrices element by element
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
        MatrixData[row*NumColumns + col] = MatrixData[row*NumColumns + col] + rhs_matrix_data[row*rhs_num_columns + col];

  // return the result
  return (*this);
}

const mmcMatrix & mmcMatrix::operator-=(const mmcMatrix & rhs)
{
  int row, col;
  
  if(MMC_ERROR_CHECK)
  {
    // insure that both matrices have the same dimensions
    if(NumRows != rhs.GetNumRows() || NumColumns != rhs.GetNumColumns())
      throw mmcException(ADD_INCOMPAT, __LINE__);
  
    // insure matrices have at least one element
    if((NumRows == 0 && NumColumns == 0) || (rhs.GetNumRows() == 0 &&
               rhs.GetNumColumns() == 0))
      throw mmcException(SIZE_ZERO, __LINE__);
  
     // insure that there is memory allocated for the left hand side matrix
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that there is memory allocated for the right had side matrix
    if(rhs.MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  double *rhs_matrix_data = rhs.GetMatrixData();
  int rhs_num_columns = rhs.GetNumColumns();  
  
  // subtract the two matrices element by element
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
        MatrixData[row*NumColumns + col] = MatrixData[row*NumColumns + col] - rhs_matrix_data[row*rhs_num_columns + col];

  // return the result
  return (*this);
}


//  Subtraction operator overload function (overlaods binary -)
mmcMatrix mmcMatrix::operator-(const mmcMatrix & rhs)const
{
  double difference;  // Variable to store temporary results
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // insure that both matrices have the same dimensions
    if(NumRows != rhs.GetNumRows() || NumColumns != rhs.GetNumColumns())
      throw mmcException(ADD_INCOMPAT, __LINE__);
  
    // insure matrices have at least one element
    if((NumRows == 0 && NumColumns == 0) || (rhs.GetNumRows() == 0 &&
            rhs.GetNumColumns() == 0))
      throw mmcException(SIZE_ZERO, __LINE__);
  
     // insure that there is memory allocated for the left hand side matrix
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that there is memory allocated for the right had side matrix
    if(rhs.MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  // create a new mmcMatrix object in order to store the result
  mmcMatrix result(NumRows, NumColumns);

  double *result_matrix_data = result.GetMatrixData();
  int result_num_columns = result.GetNumColumns();
  double *rhs_matrix_data = rhs.GetMatrixData();
  int rhs_num_columns = rhs.GetNumColumns();  
  
  // Add the two matrices element by element
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
        result_matrix_data[row*result_num_columns + col] = MatrixData[row*NumColumns + col] - rhs_matrix_data[row*rhs_num_columns + col];

  // return the result
  return result;
}

//  Multiplication operator overload function (overlaods binary *)
mmcMatrix mmcMatrix::operator*(const mmcMatrix & rhs)const
{
  double sum;  // Variable to store temporary results
  int row, col, index;

  if(MMC_ERROR_CHECK)
  {
    // insure that the number of columns in the left hand side matrix
    // matches the number of rows in the right hand matrix
    if(NumColumns != rhs.GetNumRows())
      throw mmcException(MULT_INCOMPAT, __LINE__);
  
    // insure matrices have at least one element
    if((NumRows == 0 && NumColumns == 0) || (rhs.GetNumRows() == 0 &&
            rhs.GetNumColumns() == 0))
      throw mmcException(SIZE_ZERO, __LINE__);
  
     // insure that there is memory allocated for the left hand side matrix
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that there is memory allocated for the right had side matrix
    if(rhs.MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  // create a new mmcMatrix object of the correct dimensions
  // in order to store the result
  mmcMatrix result(NumRows, rhs.GetNumColumns());

  if(mmcMaxNumThreads > 1 && NumColumns*NumRows > mmcThreshold) // if more that one thread is allowed, used multiple threads. Otherwise use serial code
  {  
    bool done = false;
    int error, status;

    //cout << "starting new multiplication" << endl;
    
    // Define data structure that will passed to all of the threads
    mmcThreadData thread_data;
    thread_data.lhs_data = MatrixData;
    thread_data.lhs_num_rows = NumRows;
    thread_data.lhs_num_columns = NumColumns;        
    thread_data.result_data = result.GetMatrixData();
    thread_data.result_num_columns = result.GetNumColumns();
    thread_data.result_num_rows = result.GetNumRows();
    thread_data.rhs_data = rhs.GetMatrixData();
    thread_data.rhs_num_columns = rhs.GetNumColumns();
    thread_data.rhs_num_rows = rhs.GetNumRows();
    thread_data.current_column = 0;
    thread_data.current_row = 0;  

    mmcThreadData *data = &thread_data;
    
    // create all of the threads
    pthread_t threads[mmcMaxNumThreads-1];
    for(int i = 0; i < mmcMaxNumThreads-1; i++)
    {
        error = pthread_create(&threads[i], 0, mmcThreadMultiply, (void *)&thread_data);
        if (error)
          cout << "Error occurred while creating thread number: " << i << endl;
    }

    // join in on the work
    while(!done)
    {
     // obtain the mutex lock and determine current row and column
     pthread_mutex_lock(&mmcLock);
     row = (data->current_row);
     col = (data->current_column);
     data->current_column += mmcChunkSize;
     while(data->current_column >= data->rhs_num_columns)
     {
         data->current_column = data->current_column - data->rhs_num_columns;
         (data->current_row)++;
      }
     pthread_mutex_unlock(&mmcLock); // release the lock

     for(int i = 0; i < mmcChunkSize && !done; i++)
     {
       // if there is work left to be done, do it!
       if(row < data->lhs_num_rows && col < data->rhs_num_columns)
       {
         //cout << "(" << row << ", " << col << ")" << endl;
         // Preform sum of row in left hand matrix times column in right hand
         // matrix.
         sum = 0.0;

         for(index = 0; index < data->lhs_num_columns; index ++)
           sum += data->lhs_data[row*data->lhs_num_columns + index] * data->rhs_data[index*data->rhs_num_columns + col];

         data->result_data[row*data->result_num_columns + col] = sum;

         col++;
         if(col == data->rhs_num_columns)
         {
             col = 0;
             row++;
          }

       } else {
         // there is nothing else left to be done
         done = true;
       }
     }
    }  // while(!done) 

    // wait for the other threads to finish
    for(int i =  0; i < mmcMaxNumThreads-1; i++)
    {
        error = pthread_join(threads[i], (void **)&status);
        if (error)
          cout << "Error joining thread." << endl;            
    }
  } else { // Only one thread allowed, using serial code
   double *result_matrix_data = result.GetMatrixData();
   int result_num_columns = result.GetNumColumns();
   double *rhs_matrix_data = rhs.GetMatrixData();
   int rhs_num_columns = rhs.GetNumColumns();

   // Preform the actual multiplication
   for(row = 0; row < NumRows; row++)
     for(col = 0; col < rhs.GetNumColumns(); col++)
       {
         // Preform sum of row in left hand matrix times column in right hand
         // matrix.
         sum = 0.0;

         for(index = 0; index < NumColumns; index ++)
           sum += MatrixData[row*NumColumns + index] * rhs_matrix_data[index*rhs_num_columns + col];

         result_matrix_data[row*result_num_columns + col] = sum;
       }
   }
  
  // return the result
  return result;
}

// thread routine for matrix multiply
void *mmcThreadMultiply(void *thread_data)
{
  int row, col, index;
  bool done = false;
  double sum;
  
  mmcThreadData *data;
  data = (mmcThreadData *)thread_data;

  while(!done)
  {
   // obtain the mutex lock and determine current row and column
   pthread_mutex_lock(&mmcLock);
   row = (data->current_row);
   col = (data->current_column);
   data->current_column += mmcChunkSize;
   while(data->current_column >= data->rhs_num_columns)
   {
       data->current_column = data->current_column - data->rhs_num_columns;
       (data->current_row)++;
    }
   pthread_mutex_unlock(&mmcLock); // release the lock

   for(int i = 0; i < mmcChunkSize && !done; i++)
   {
     // if there is work left to be done, do it!
     if(row < data->lhs_num_rows && col < data->rhs_num_columns)
     {
       //cout << "(" << row << ", " << col << ")" << endl;
       // Preform sum of row in left hand matrix times column in right hand
       // matrix.
       sum = 0.0;

       for(index = 0; index < data->lhs_num_columns; index ++)
         sum += data->lhs_data[row*data->lhs_num_columns + index] * data->rhs_data[index*data->rhs_num_columns + col];

       data->result_data[row*data->result_num_columns + col] = sum;

       col++;
       if(col == data->rhs_num_columns)
       {
           col = 0;
           row++;
        }       
       
     } else {
       // there is nothing else left to be done
       done = true;
     }
   }
  }  // while(!done) 
  
  pthread_exit((void*)0);
}

#if MMC_ERROR_CHECK==1 // Only define these methods if error checking is on, otherwise they are inlined
  // returns an element of the matrix
  double mmcMatrix::GetElement(int row, int col)const
  {
    if(MMC_ERROR_CHECK)
    {
      // insure that the element does not exceed the bounds of the matrix
      if(row < 0 || col < 0 || row >= NumRows || col >= NumColumns || MatrixData == NULL)
        throw mmcException(OVERRUN, __LINE__);
    }
  
    // return the element
    return(MatrixData[row * NumColumns + col]);
  }
  
  double mmcMatrix::operator() (int row, int col)const
  {
    if(MMC_ERROR_CHECK)
    {
      // insure that the element does not exceed the bounds of the matrix
      if(row < 0 || col < 0 || row >= NumRows || col >= NumColumns || MatrixData == NULL)
        throw mmcException(OVERRUN, __LINE__);
    }
  
    // return the element
    return(MatrixData[row * NumColumns + col]);
  }
  
  
  // sets the value of a matrix element
  void mmcMatrix::SetElement(int row, int col, double new_value)
  {
    if(MMC_ERROR_CHECK)
    {
      // insure that the element does not exceed the bounds of the matrix
      if(row < 0 || col < 0 || row >= NumRows || col >= NumColumns || MatrixData == NULL)
        throw mmcException(OVERRUN, __LINE__);
    }
  
    // Set the new value of the matrix element
    MatrixData[row * NumColumns + col] = new_value;
  }
  
  double& mmcMatrix::operator() (int row, int col)
  {
    if(MMC_ERROR_CHECK)
    {
      // insure that the element does not exceed the bounds of the matrix
      if(row < 0 || col < 0 || row >= NumRows || col >= NumColumns || MatrixData == NULL)
        throw mmcException(OVERRUN, __LINE__);
    }
  
    // Set the new value of the matrix element
    return MatrixData[row * NumColumns + col];
  }
#endif

// returns matrix transpose
mmcMatrix mmcMatrix::GetTranspose()const
{
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // insure that the matrix has at least one element
    if(NumRows == 0 && NumColumns == 0)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that the matrix data pointer is not NULL
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  // create a matrix object to store the transpose
  mmcMatrix result(NumColumns, NumRows);
  
  double *result_matrix_data = result.GetMatrixData();
  int result_num_columns = result.GetNumColumns();  

  // transpose the matrix
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      result_matrix_data[col*result_num_columns +row] = MatrixData[row*NumColumns + col];

  return(result);
}

// returns matrix scaled by a constant
mmcMatrix mmcMatrix::GetScaled(double scale_factor)const
{
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // insure that the matrix has at least one element
    if(NumRows == 0 && NumColumns == 0)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that the matrix data pointer is not NULL
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  // create a matrix object to store the transpose
  mmcMatrix result(NumRows, NumColumns);

  double *result_matrix_data = result.GetMatrixData();
  int result_num_columns = result.GetNumColumns();
  
  // Multiply each element of the matrix by the scale factor
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      result_matrix_data[row*result_num_columns + col] = scale_factor * MatrixData[row*NumColumns + col];

  return(result);
}

mmcMatrix mmcMatrix::operator*(double scale_factor)const
{
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // insure that the matrix has at least one element
    if(NumRows == 0 && NumColumns == 0)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that the matrix data pointer is not NULL
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  // create a matrix object to store the transpose
  mmcMatrix result(NumRows, NumColumns);

  double *result_matrix_data = result.GetMatrixData();
  int result_num_columns = result.GetNumColumns();
  
  // Multiply each element of the matrix by the scale factor
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      result_matrix_data[row*result_num_columns + col] = scale_factor * MatrixData[row*NumColumns + col];

  return(result);
}


mmcMatrix operator*(double scale_factor, const mmcMatrix & rhs)
{
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // insure that the matrix has at least one element
    if(rhs.GetNumRows() == 0 && rhs.GetNumColumns() == 0)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that the matrix data pointer is not NULL
    if(rhs.GetMatrixData() == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  double *rhs_matrix_data = rhs.GetMatrixData();
  int rhs_num_columns = rhs.GetNumColumns();  
  
  // create a matrix object to store the transpose
  mmcMatrix result(rhs.GetNumRows(), rhs.GetNumColumns());

  double *result_matrix_data = result.GetMatrixData();
  int result_num_columns = result.GetNumColumns();
  
  // Multiply each element of the matrix by the scale factor
  for(row = 0; row < rhs.GetNumRows(); row++)
    for(col = 0; col < rhs.GetNumColumns(); col++)
      result_matrix_data[row*result_num_columns + col] = scale_factor * rhs_matrix_data[row*rhs_num_columns + col];

  return(result);
}

// returns matrix scaled by a constant
const mmcMatrix & mmcMatrix::operator*=(double scale_factor)
{
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // insure that the matrix has at least one element
    if(NumRows == 0 && NumColumns == 0)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // insure that the matrix data pointer is not NULL
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }
  
  // Multiply each element of the matrix by the scale factor
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      MatrixData[row*NumColumns + col] = scale_factor * MatrixData[row*NumColumns + col];

  return (*this);
}

// Sets the matrix to the identity matrix
const mmcMatrix & mmcMatrix::SetIdentity()
{
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // Check to ensure that there is memory allocated to this matrix
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  
    // Check to ensure that the number of rows equals the number of columns
    if(NumRows != NumColumns)
      throw mmcException(NOT_SQUARE, __LINE__);
  }

  // Set the matrix elements to those of the identity matrix
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      {
	if (row == col)
	  SetElement(row,col,1.0);
	else
	  SetElement(row,col,0.0);
      }

  return (*this);
}

// Sets all elements of the matrix to zero
const mmcMatrix & mmcMatrix::SetZero()
{
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // Check to ensure that there is memory allocated to this matrix
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  // Set the matrix elements to those of the identity matrix
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      {
				SetElement(row,col,0.0);
      }

  return (*this);
}

const mmcMatrix & mmcMatrix::SetRandom(double min, double max) // sets the matrix to random values between min and max
{
  int row, col;

  if(MMC_ERROR_CHECK)
  {
    // Check to ensure that there is memory allocated to this matrix
    if(MatrixData == NULL)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

  // Set the matrix elements to those of the identity matrix
  for(row = 0; row < NumRows; row++)
    for(col = 0; col < NumColumns; col++)
      {
         SetElement(row, col, min + (double)rand() * ((max - min) / (double)RAND_MAX));
      }

  return (*this);
}  

// Sets the name for the string used in matlab formatted output
const mmcMatrix & mmcMatrix::SetName(const char *name)
{
  strncpy(Name, name, mmcMAX_NAME_LENGTH);

  Name[mmcMAX_NAME_LENGTH - 1] = '\0';

  return (*this);
}

//Calculates the distance between two points represented by matrices
//if take_sqrt is true, then the distance will be returned and if
//take_sqrt is false, the distance sqaured will be returned
double mmcMatrix::GetDistanceTo(const mmcMatrix &input_vector, bool take_sqrt)const
{
	double distance = 0.0;
	bool col_vector;
	int row, col;

  if(MMC_ERROR_CHECK)
  {
    //Check to insure that the two vectors are of the same dimensions
    if(NumColumns != input_vector.GetNumColumns() || NumRows != input_vector.GetNumRows())
      throw mmcException(VECTOR_INCOMPAT, __LINE__);
  
    //insure that the vectors are both either column vectors or row vectors
    if(NumColumns != 1 && NumRows != 1)
      throw mmcException(NOT_VECTOR, __LINE__);
  }

	//determine whether they are row or column vectors
	if(NumColumns == 1)
		col_vector = true;
	else
		col_vector = false;
	
	//Calculate the distance between the vectors
	if(col_vector)
	{
		//The vectors are column vectors
		col = 0;

		for(row = 0; row < NumRows; row++)
		{
			distance += pow(GetElement(row,col) - input_vector.GetElement(row,col), 2);
		}
	} else {
		//the vectors are row vectors
		row = 0;

		for(col = 0; col < NumColumns; col++)
		{
			distance += pow(GetElement(row,col) - input_vector.GetElement(row,col), 2);
		}
	}


	if(take_sqrt)
		return (sqrt(distance));
	else
		return (distance);
}


//Calculates the dot product between *this and rhs
double mmcMatrix::DotProduct(const mmcMatrix & rhs)const
{
  if(MMC_ERROR_CHECK)
  {
    //first must check to insure that the two matrices are of the same dimension
    if(GetNumRows() != rhs.GetNumRows() || GetNumColumns() != rhs.GetNumColumns())
      throw mmcException(VECTOR_INCOMPAT, __LINE__);
  }

	bool column_vector;

	//Check to insure that the matrices are either column or row vectors
	if(GetNumRows() == 1)
		column_vector = false;
	else if(GetNumColumns() == 1)
		column_vector = true;
	else
		throw mmcException(NOT_VECTOR, __LINE__);

	double result = 0.0;

	//Preform the dot product calculation
	if(column_vector)
		for(int counter = 0; counter < GetNumRows(); counter++)
			result += GetElement(counter, 0) * rhs.GetElement(counter, 0);
	else
		for(int counter = 0; counter < GetNumColumns(); counter++)
			result += GetElement(0,counter) * rhs.GetElement(0,counter);
	
	return(result);
}


// Perform an Element wise multiplication of two matrices
mmcMatrix mmcMatrix::ElementWiseMultiplication(const mmcMatrix & rhs)const
{
  if(MMC_ERROR_CHECK)
  {
    //first must check to insure that the two matrices are of the same dimension
    if(GetNumRows() != rhs.GetNumRows() || GetNumColumns() != rhs.GetNumColumns())
      throw mmcException(ADD_INCOMPAT, __LINE__);
  }


	mmcMatrix result(GetNumRows(), GetNumColumns());

	for(int row = 0; row < GetNumRows(); row++)
		for(int col = 0; col < GetNumColumns(); col++)
			result.SetElement(row, col, GetElement(row, col) * rhs.GetElement(row,col));
	
	return(result);
}

//Calculates the inverse of a 3x3 matrix
mmcMatrix mmcMatrix::GetInverse3By3()const
{
	if(MMC_ERROR_CHECK)
	{
	  //Check to insure that the matrix is a 3x3 matrix
	  if(GetNumRows() != 3 && GetNumColumns() != 3)
		throw mmcException(NOT_3X3, __LINE__);

	  //Insure that memory is allocated
	  if(MatrixData == NULL)
		throw mmcException(SIZE_ZERO, __LINE__);
	}

	mmcMatrix solution(3, 3);

	double determinate = GetDeterminate3By3();
	
	if(determinate == 0.0)
	  throw mmcException(SINGULAR, __LINE__);

	solution(0,0) = GetElement(1,1)*GetElement(2,2) - GetElement(1,2)*GetElement(2,1);
	solution(0,1) = GetElement(0,2)*GetElement(2,1) - GetElement(0,1)*GetElement(2,2);
	solution(0,2) = GetElement(0,1)*GetElement(1,2) - GetElement(0,2)*GetElement(1,1);
	
	solution(1,0) = GetElement(1,2)*GetElement(2,0) - GetElement(1,0)*GetElement(2,2);
	solution(1,1) = GetElement(0,0)*GetElement(2,2) - GetElement(0,2)*GetElement(2,0);
	solution(1,2) = GetElement(0,2)*GetElement(1,0) - GetElement(0,0)*GetElement(1,2);
	
	solution(2,0) = GetElement(1,0)*GetElement(2,1) - GetElement(1,1)*GetElement(2,0);
	solution(2,1) = GetElement(0,1)*GetElement(2,0) - GetElement(0,0)*GetElement(2,1);
	solution(2,2) = GetElement(0,0)*GetElement(1,1) - GetElement(0,1)*GetElement(1,0);
	
	solution = (1.0/determinate)*solution;
	
	return(solution);
}

//Calculates the determinate of a 3x3 matrix
double mmcMatrix::GetDeterminate3By3()const
{
	if(MMC_ERROR_CHECK)
	{
	  //Check to insure that the matrix is a 3x3 matrix
	  if(GetNumRows() != 3 && GetNumColumns() != 3)
		throw mmcException(NOT_3X3, __LINE__);

	  //Insure that memory is allocated
	  if(MatrixData == NULL)
		throw mmcException(SIZE_ZERO, __LINE__);
	}

	double solution;
	
	solution = GetElement(0,0)*(GetElement(1,1)*GetElement(2,2)-GetElement(1,2)*GetElement(2,1)) 
			 - GetElement(0,1)*(GetElement(1,0)*GetElement(2,2)-GetElement(1,2)*GetElement(2,0)) 
			 + GetElement(0,2)*(GetElement(1,0)*GetElement(2,1)-GetElement(1,1)*GetElement(2,0));
		
	return(solution);
}


void mmcMatrix::DisplayMatrix()const
{
	for(int row = 0; row < GetNumRows(); row++)
	{
		for(int col = 0; col < GetNumColumns(); col++)
		{	
			
			cout << GetElement(row,col) << "\t";
		}

		cout << "\n";
	}
}


double mmcMatrix::GetMagnitude()const
{
  if(MMC_ERROR_CHECK)
  {
    // Must be a vector
    if(NumRows != 1 && NumColumns != 1)
      throw mmcException(NOT_VECTOR, __LINE__);
  }

  double magnitude = 0.0;

  if (NumRows > 1)
    // Column vector
    for(int i = 0; i < NumRows; i++)
      magnitude += pow(GetElement(i,0),2);
  else
    // Row Vector
     for(int i = 0; i < NumColumns; i++)
      magnitude += pow(GetElement(0,i),2);

  return sqrt(magnitude);

}

double *mmcMatrix::GetCVector()const
{
  if(MMC_ERROR_CHECK)
  {
    // Must be a vector
    if(NumRows != 1 && NumColumns != 1)
      throw mmcException(NOT_VECTOR, __LINE__);
  }

  double *c_vector;

  if (NumRows > 1)
  {
    c_vector = new double[NumRows];
    // Column vector
    for(int i = 0; i < NumRows; i++)
      c_vector[i] = GetElement(i,0);
  }else{
    c_vector = new double[NumColumns];
    // Row Vector
     for(int i = 0; i < NumColumns; i++)
      c_vector[i] = GetElement(0,i);
   }

  return c_vector;
}

// Normalizes a vector
mmcMatrix mmcMatrix::GetNormalized()const
{
  if(MMC_ERROR_CHECK)
  {
    // check to insure the magnitude is not zero
    double magnitude = GetMagnitude();
	if(magnitude == 0)
		throw mmcException(DIVIDE_BY_ZERO, __LINE__);
	else
		return GetScaled(1.0/magnitude);
  }

  return GetScaled(1.0 / GetMagnitude());
}


// Calculate the cross products of two 3x1 or 1X3 vectors
mmcMatrix mmcMatrix::CrossProduct(const mmcMatrix & rhs)const
{
  if (NumRows == 1 && NumColumns == 3 && rhs.GetNumRows() == 1 && rhs.GetNumColumns() == 3)
    {
      // Both vectors are row vectors
      mmcMatrix result(1,3);

      result.SetElement(0,0,
			GetElement(0,1)*rhs.GetElement(0,2) - GetElement(0,2)*rhs.GetElement(0,1));

      result.SetElement(0,1,
			GetElement(0,2)*rhs.GetElement(0,0) - GetElement(0,0)*rhs.GetElement(0,2));

      result.SetElement(0,2,
			GetElement(0,0)*rhs.GetElement(0,1) - GetElement(0,1)*rhs.GetElement(0,0));

      return result;

    } else if (NumRows == 3 && NumColumns == 1 && rhs.GetNumRows() == 3 && rhs.GetNumColumns() == 1){

      // Both vectors are column vectors

      mmcMatrix result(3,1);

      result.SetElement(0,0,
			GetElement(1,0)*rhs.GetElement(2,0) - GetElement(2,0)*rhs.GetElement(1,0));

      result.SetElement(1,0,
			GetElement(2,0)*rhs.GetElement(0,0) - GetElement(0,0)*rhs.GetElement(2,0));

      result.SetElement(2,0,
			GetElement(0,0)*rhs.GetElement(1,0) - GetElement(1,0)*rhs.GetElement(0,0));

      return result;

    } else {
      // Matrices are not in either both 3x1 or both 1x3
      throw mmcException(CANNOT_CROSS, __LINE__);

    }

}


mmcMatrix mmcMatrix::CombineAsColumn(const mmcMatrix & rhs)const
{
	if(NumColumns == 0 && NumRows == 0)
	{
		// special case of a empty matrix
		// simply return the rhs
		return rhs;
	}else if(GetNumColumns() == rhs.GetNumColumns()) {
		mmcMatrix result = *this;

		result.SetSize(GetNumRows() + rhs.GetNumRows(), GetNumColumns());

		for(int row = GetNumRows(); row < GetNumRows() + rhs.GetNumRows(); row++)
      for(int col = 0; col < GetNumColumns(); col++)  
      {
        result.SetElement(row, col, rhs.GetElement(row - GetNumRows(), col));
      }

		return result;

	} else{
		// The matrices were not compatible to combine as a column
		throw mmcException(CANNOT_COMBINE, __LINE__);
	}
}

mmcMatrix mmcMatrix::CombineAsRow(const mmcMatrix & rhs)const
{
	if(NumColumns == 0 && NumRows == 0)
	{
		// special case of a empty matrix
		// simply return the rhs
		return rhs;
	}else if( GetNumRows() == rhs.GetNumRows() ) {
		mmcMatrix result = *this;

		result.SetSize(GetNumRows(), GetNumColumns() + rhs.GetNumColumns());

		for(int col = GetNumColumns(); col < GetNumColumns() + rhs.GetNumColumns(); col++)
		  for(int row = 0; row < GetNumRows(); row++)
      {
        result.SetElement(row, col, rhs.GetElement(row, col - GetNumColumns()));
      }
    
		return result;
    
	} else {
		// The matrices were not compatible to combine as a row
		throw mmcException(CANNOT_COMBINE, __LINE__);
	}
}

mmcMatrix mmcMatrix::GetSubMatrix(int start_row, int start_col,
	                              int end_row, int end_col)const
{
  if(MMC_ERROR_CHECK)
  {
    // Check for valid input arguements
    if(end_col < start_col || end_row < start_row)
      throw mmcException(CANNOT_SUB, __LINE__);
  
    if(start_col < 0 || start_row < 0)
      throw mmcException(CANNOT_SUB, __LINE__);
  
    if(end_col + 1 > NumColumns || end_row + 1 > NumRows)
      throw mmcException(CANNOT_SUB, __LINE__);
  }

	mmcMatrix result((end_row - start_row) + 1, (end_col - start_col) + 1);

	for(int row = start_row; row <= end_row; row++)
		for(int col = start_col; col <= end_col; col++)
		{
			result.SetElement(row - start_row ,col - start_col , GetElement(row, col));
		
		}

	return result;
}

ostream& operator<< (ostream& os, const mmcMatrix &rhs)
{	
	// Check to see if the matrix has any data to output
	if(rhs.GetNumRows() == 0 || rhs.GetNumColumns() == 0)
		return os;

    switch(rhs.OutputFormat){
    case MMC_NATIVE:
    	// First output the num rows and num of columns on one line
    	os << rhs.GetNumRows() << "   " << rhs.GetNumColumns() << "\n";

    	// Set the precision of the data output
    	//os << setw(30);    
      os.precision(__DBL_DIG__);

    	// Now write all of the data in a row-wise fashion
    	for(int row = 0; row < rhs.GetNumRows(); row++)
    		for(int col = 0; col < rhs.GetNumColumns(); col++)
    		{
    			os << rhs.GetElement(row,col) << "\n";
    		}
    	break;
    		
    case MATLAB_ASCII:
			os << "# Created with mmcMatrix\n";   // Comment
			os << "# name: " << rhs.Name << "\n";       // Matlab name for the matrix
			os << "# type: matrix\n";                      // Matlab variable types
			os << "# rows: " << rhs.GetNumRows() << "\n";
			os << "# columns: " << rhs.GetNumColumns() << "\n";
			
			// Set the precision of the data output   
			os.precision(20);
				
    	// Now write all of the data in a row-wise fashion
    	for(int row = 0; row < rhs.GetNumRows(); row++)
    		for(int col = 0; col < rhs.GetNumColumns(); col++)
    		{
    			os << rhs.GetElement(row,col) << "  ";
    		}

    	os << "\n";
    			
        break;
    } // End Switch

	return os;
}

istream& operator>> (istream& is, mmcMatrix &rhs)
{
	if(is.good() != true)
		return is;
	
	int num_rows, num_columns;

	//First input the number of rows and columns
	is >> num_rows >> num_columns;

	rhs.SetSize(num_rows, num_columns);

	double element;

	// Now input all of the data in a row-wise fashion
	for(int row = 0; row < num_rows; row++)
		for(int col = 0; col < num_columns; col++)
		{
			is >> element;
			rhs.SetElement(row, col, element);
		}

	return is;
}


// Performs a discrete cosine transform
mmcMatrix mmcMatrix::GetDCT()const
{
  if(MMC_ERROR_CHECK)
  {
    // Check to see if a DCT can be performed
    if(NumRows == 0 || NumColumns == 0)
      throw mmcException(SIZE_ZERO, __LINE__);
  }

	mmcMatrix result(NumRows, NumColumns);

	if(NumRows == 1 && NumColumns == 1)
	{
		int size;

		if(NumRows > NumColumns)
			size = NumRows;
		else
			size = NumColumns;
			
		double constant1 = 1.0 / sqrt((double)size);
		double constant2 = sqrt(2.0 / size);

		// Perform a 1D DCT
		for(int i = 0; i < size; i++)
		{
			double sum = 0;

			for(int j = 0; j < size; j++)
			{
				if(i == 0)
				{
					if(NumColumns == 1)
						sum += constant1 * GetElement(j, 0) * cos(( 2*j + 1)*i * mmcPI/2.0/size);
					else
						sum += constant1 * GetElement(0, j) * cos(( 2*j + 1)*i * mmcPI/2.0/size);
				} else {
					if(NumColumns == 1)
						sum += constant2 * GetElement(j, 0) * cos(( 2*j + 1)*i * mmcPI/2.0/size);
					else
						sum += constant2 * GetElement(0, j) * cos(( 2*j + 1)*i * mmcPI/2.0/size);					
				}
			}

			if(NumColumns == 1)
				result.SetElement(i,0, sum);
			else
				result.SetElement(0,i, sum);
		}
		
	} else {
		// perform a 2D DCT
	
		double constant1 = sqrt(1.0 / (double)(NumRows * NumColumns));
		double constant2 = sqrt(4.0 / (double)(NumRows * NumColumns));
		double constant3 = sqrt(2.0 / (double)(NumRows * NumColumns));
		double sum;

		for(int i = 0; i < NumRows; i++)
			for(int j = 0; j < NumColumns; j++)
			{
				sum = 0;

				for(int y = 0; y < NumRows; y++)
					for(int x = 0; x < NumColumns; x++)
					{
						if( i != 0 && j != 0)
						{
							sum += constant2 * GetElement(y,x) *
								   cos( ((2 * y + 1)*i*mmcPI) / (double)(2 * NumRows)) *
								   cos( ((2 * x + 1)*j*mmcPI) / (double)(2 * NumColumns));
						} else if (i == 0 && j == 0) {
							sum += constant1 * GetElement(y,x) *
								   cos( ((2 * y + 1)*i*mmcPI) / (double)(2 * NumRows)) *
								   cos( ((2 * x + 1)*j*mmcPI) / (double)(2 * NumColumns));
						} else {
							sum += constant3 * GetElement(y,x) *
								   cos( ((2 * y + 1)*i*mmcPI) / (double)(2 * NumRows)) *
								   cos( ((2 * x + 1)*j*mmcPI) / (double)(2 * NumColumns));
						}
					}
				
				result.SetElement(i,j,sum);	
			}
	}

	return result;
}

// replaces the elements of *this with the elements of input_matrix starting at start_row and start_column
// input_matrix must fit in this space or an error will occur
const mmcMatrix & mmcMatrix::SetSubmatrix(int start_row, int start_column, const mmcMatrix &input_matrix)
{
  if(MMC_ERROR_CHECK)
  {
    // Check to see if input_matrix fits into the allotted space and that valid start_row and start_column are selected
    if(start_column < 0 || start_row < 0)
      throw mmcException(CANNOT_SUB, __LINE__);
  
    if(start_column + input_matrix.GetNumColumns() > NumColumns || start_row + input_matrix.GetNumRows() > NumRows)
      throw mmcException(CANNOT_SUB, __LINE__);
  }

  for(int row = start_row; row < start_row + input_matrix.GetNumRows(); row++)
     for(int col = start_column; col < start_column + input_matrix.GetNumColumns(); col++)
       SetElement(row,col, input_matrix.GetElement(row - start_row, col - start_column));

  return (*this);
}


bool mmcMatrix::ReadFromFile(const char *file_name)
{	
	//open the file for input, the object fin will point to the file
	ifstream fin(file_name);

	//check to see of the file has been openned successfully
	if(fin.good() != true)
		return true;
	
	//Network form file
	fin >> *this;

	//close the file
	fin.close();

	return false;
}

// Create a matrix from file
mmcMatrix::mmcMatrix(const char *file_name)
{	
	NumRows = 0;
	NumColumns = 0;
	MatrixData = NULL;
	OutputFormat = MMC_NATIVE;
	Name[0] = '\0';

	bool error = ReadFromFile(file_name);

	if(error)
	{
		cerr << "Failed to open file: " << file_name << endl;
		throw mmcException(CANNOT_OPEN, __LINE__);
	}
}

bool mmcMatrix::WriteToFile(const char *file_name)const
{
	// Open the file	
	ofstream fout(file_name, ios::out);

	if(!fout)
	{
		// Error opening the file
		cerr << "Error creating the file: " << file_name << endl;
		return true;
		
	} else {
		fout << *this;
	}

	// Close the file
	fout.close();	

	return false;
}

// deletes matrix data and sets its size to 0x0
void mmcMatrix::Clear()
{
  NumRows = 0;
  NumColumns = 0;
  if(MatrixData != NULL)
    delete [] MatrixData;
  MatrixData = NULL;
}
