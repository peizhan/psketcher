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


#include <iostream>
#include "bfgs.h"

using namespace std;

void display_vector(mmcMatrix vector, ostream *out_buf);

/**********************************************************************/
void MeritFunction::MultGold(double lambda_u_init, double f, const mmcMatrix &search_dir,
		          const mmcMatrix &xref, double *lambda_lower_final, double *lambda_upper_final)
/**********************************************************************/
/*  AUTHOR: Michael Greminger
**  DATE: 10/17/00
**
**  PURPOSE
**    The function multgold preforms a golden section optimization of
**    a search direction of a multi-variable merit function.  This function
**    will work with a merit function of any dimension (constrained only by
**    system memory).
** 
**  INPUT
**    lambda_u_init - Initial upper limit on the search vector scale
**         factor.
**    num_dim - Number of independent variables in merit function.  This 
**         indicates the size of search_dir_p and xref_p.
**    search_dir_pp - Pointer to a vector indicating the search direction to 
**         optimized.
**    xref_pp - Pointer to a array indicating the starting point of the 
**         optimization.
**    NOTE:  All of the vectors are represected as an array of pointers
**           to each element of the vector.  This is done to keep the 
**           vectors compatible with the matrix funcitons such as
**           mat_mult.
**    f - Desired fractional reduction in the interval of uncertainty.
**    merit - Pointer to the function that contains the merit function.
**    merit_data_p - void pointer to data needed by merit function.
**
**  OUTPUT 
**    multgold - Error status: 0 = error, 1 = successful 
**    lambda_upper_final - Final upper limit of search vector scale factor.
**    lambda_lower_final - Final lower limit of search vector scale factor.
**         Note that the initial lower limit of the search vector scale 
**         factor is assumed to be zero.
**
**  DESCRIPTION OF LOCAL VARIABLES
**    gold_num - Golden number.  Evaluated only once for efficiency.
**    lambda_l_bnd - Current lower limit of the scale factor.
**    lambda_u_bnd - Current right limit of the scale factor.
**    lambda_m_l - Lower scale factor where merit function is evaluated.
**    lambda_m_u - Upper scale factor where merit function is evaluated.
**    num_additional_computations - Computations of merit function 
**        needed after the two initial computations of merit function.
**    counter - Iteration counter variabel.
**    temp_counter - counter variable used in memory allocation
**    displacement_vector_p - Vector from xref_p to 
**         x_trial_p.
**    x_trial_p - Point where the merit function is being evaluated.
**    
**  FUNCTIONS CALLED
**    merit - Function that returns value of merit function at a point.
**    scale_matrix - Multiplies a matrix by a scalar.
**    mat_addition - Adds two matrices.
*/
{
  /*
  **  LOCAL DECLARATIONS
  */
  double gold_num;
  double lambda_l_bnd, lambda_u_bnd;
  double lambda_l, lambda_u;
  double lambda_m_l, lambda_m_u;
  int num_additional_computations;
  int counter;

  if(VerboseLevel >= 2)
  {
	//Display the value of f that was input
	*out_buf << "\nf = " << f << endl;
  }


  /* 
  ** Will initialize pointers to NULL to prevent coding errors
  ** NOTE:  Vectors are represented as an array of pointers to each element.
  **        This is done to keep the vectors compatible with the matrix 
  **        functions such as mat_mult.
  */
  mmcMatrix displacement_vector_pp;
  mmcMatrix x_trial;
  
  /*
  **  START OF EXECUTABLE CODE
  */
  
  /*
  **  Need to insure that the initial upper bound on lambda is actually 
  **  greater than the lower bound (the lower bound is taken to be zero).
  */
  if(!(lambda_u_init > 0))
    throw MeritFunctionException();
  
  /*
  ** Need to insure that f is positive and non-zero since the 
  ** natural log of f will later be evaluated.
  */
  if(f <= 0)
    throw MeritFunctionException();

  /*
  ** The value of the golden number is evaluated and stored for 
  ** later use in order to increase efficiency.
  */
  gold_num = 0.5 * (sqrt(5.0) - 1);

  /*
  ** Set the current limits on lambda to the initial
  ** limits.
  */
  lambda_l_bnd = 0.0;
  lambda_u_bnd = lambda_u_init;

  /*
  ** Calculate the first two points to evaluate the merit function.
  */
  lambda_l = lambda_u_bnd - gold_num * (lambda_u_bnd - lambda_l_bnd);
  lambda_u = lambda_l_bnd + gold_num *(lambda_u_bnd - lambda_l_bnd);

  /*
  ** Evaluate the actual merit function for the first two points
  */
	x_trial = xref + search_dir.GetScaled(lambda_l);


  /* Can now evaluate merit function at x_trial_p corresponding to the 
      ** lower limit of lambda */
  lambda_m_l = GetMeritValue(x_trial);
  
  /* Will repeat the same procedure for the upper bound of lambda */
	x_trial = xref + search_dir.GetScaled(lambda_u);

  lambda_m_u = GetMeritValue(x_trial);

  /*
  ** Need to calculate the number of additional merit function evaluations
  ** that are required.  Two evaluations of the merit function have already
  ** been done.  From now on each iteration will only need to evaluate the 
  ** merit function once.
  */
  num_additional_computations = (int)ceil(1 - 2.078 * log(f)) - 2;
  
  /* 
  ** If more computations are needed, compute them.  Note that it is 
  ** possible that no more computations are needed if the user chose the 
  ** value of f to be 1 or greater.
  */
  if(num_additional_computations > 0)
    {
    /*
    ** Iteration loop.  Each iteration cuts down the uncertainty interval
    ** and evaluates the merit function at one additional point.
    */
      for(counter = 0; counter < num_additional_computations; counter++)
	  {
	  /*
	  ** Need to determine whether to keep r_bnd or l_bnd depending
	  ** on which of the values m_l or m_r is smaller.  Will then find the 
	  ** next point to evaluate the merit function and evaluate it there.
	  */
	  if (lambda_m_l < lambda_m_u)
	    {
	      
	      lambda_u_bnd = lambda_u;
	      lambda_u = lambda_l;
	      lambda_m_u = lambda_m_l;
	      lambda_l = lambda_u_bnd - gold_num * 
		(lambda_u_bnd - lambda_l_bnd);
	      
	      /* 
	      **  Will now evaluate the merit function with lambda_l
	      */
		  x_trial = xref + search_dir.GetScaled(lambda_l);
	      
	      /* Can now evaluate merit function at x_trial_p corresponding 
	      ** to thelower limit of lambda */
	      lambda_m_l = GetMeritValue(x_trial);
	      
	    }else{
	      
	      lambda_l_bnd = lambda_l;
	      lambda_l = lambda_u;
	      lambda_m_l = lambda_m_u;
	      lambda_u = lambda_l_bnd + gold_num * 
		(lambda_u_bnd - lambda_l_bnd);
	      
	      /* 
	      **  Will now evaluate the merit function with lambda_u
	      **  will first find out where to go from xref_p 
	      ** will first find out where to go from xref_p 
	      */
		  x_trial = xref + search_dir.GetScaled(lambda_u);

	      /* Can now evaluate merit function at x_trial_p corresponding 
	      ** to the uper limit of lambda */
	      lambda_m_u = GetMeritValue(x_trial);
		}
	  }	
  }	
  /*
  **  Will now narrow down the interval of uncertainty one last time.
  **  Will not evaluate the merit function an additional time though.
  **  Will return a value of 1 to indicate a successful computation.
  **  Must also free memory of temporary arrays back to the heap.
  */
  
  /* Now will return results */
  if(lambda_m_l < lambda_m_u)
    {
      *lambda_lower_final = lambda_l_bnd;
      *lambda_upper_final = lambda_u;
    }else{
      *lambda_lower_final = lambda_l;
      *lambda_upper_final = lambda_u_bnd;
    }
}


// Backtracking line search function
double MeritFunction::BackTrack(const mmcMatrix &position, const mmcMatrix &gradient, const double &initial_merit, mmcMatrix &search_dir, double max_step,
	                            double step_tol, double alpha, double beta, bool &error, mmcMatrix &new_gradient, double &new_merit)
{
	error = false;
	
	// Calculate the current merit value
	double merit_value = initial_merit;

	// Insure that the length of the search vector is not too long
	double newton_length = search_dir.GetMagnitude();
	
	if(newton_length > max_step)
	{
		if(VerboseLevel >= 1)
			*out_buf << "Back track line search had to shorten search vector." << endl;
		search_dir = search_dir.GetScaled(max_step / newton_length);
		newton_length = max_step;
	}

	// Calculate the initial slope
	double init_slope = gradient.DotProduct(search_dir);

	// Calculated the relative length of the search diriction vector
	double rel_length = search_dir.GetElement(0,0) / position.GetElement(0,0);
	for(int i = 1; i < NumDimensions; i++)
	{
		double temp = search_dir.GetElement(i,0) / position.GetElement(i,0);
		if(temp > rel_length)
			rel_length = temp;
	}

	double min_lambda = step_tol / rel_length;

	double lambda = 1.0;

	mmcMatrix new_position;
	double prev_new_merit_value = 0;
	double lambda_temp, lambda_prev = 0, disc;
	mmcMatrix temp1(2,2);
	mmcMatrix temp2(2,1);
	mmcMatrix result;
	double a,b;

    bool finished_first_backtrack = false;	
	
	while(true) // loop is broken when lambda condition is satisfied by a return statement
	{

	
		if(MaxMeritEvals != 0 && MeritEvals > MaxMeritEvals)
		{
			// Reached maximum number of merit function calls, must return whatever lamba is now
			error = false;
			return lambda;
		}

		new_position = position + search_dir.GetScaled(lambda);
		GetMeritValuePlusGradient(new_position, new_merit, new_gradient);
		MeritEvals++;
		
		// Check to see whether this lambda is acceptable
		if( new_merit <= merit_value + alpha*lambda*init_slope)
		{
			// Satisfactory value of lambda found
			error = false;
			return lambda;

		} else if (lambda < min_lambda) {
			// no satisfactory lambda can be found far enough away from starting position
			error = true;
			return 0.0;
				
		} else {
			// Need to reduce lambda
			if(!finished_first_backtrack)
			{
				// For fist backtrack use quadratic fit
				lambda_temp = -init_slope / (2 * (new_merit - merit_value - init_slope));
				
				finished_first_backtrack = true;

			} else {
				// for subsequent backtracks used cubic fit
				temp2.SetElement(0,0, new_merit - merit_value - lambda*init_slope);
				temp2.SetElement(1,0, prev_new_merit_value - merit_value - lambda_prev * init_slope);

				temp1.SetElement(0,0, 1 / (lambda * lambda));
				temp1.SetElement(0,1, -1 / (lambda_prev * lambda_prev));
				temp1.SetElement(1,0, -lambda_prev / (lambda * lambda));
				temp1.SetElement(1,1, lambda / (lambda_prev * lambda_prev));
				
				result = (temp1 * temp2).GetScaled( 1/ (lambda - lambda_prev));
				
				a = result.GetElement(0,0);
				b = result.GetElement(1,0);

				disc = b*b - 3*a*init_slope;

				if(a == 0)
					// the cubic is a quadratic
					lambda_temp = -init_slope / (2*b);
				else
					// the cubic is a cubic!
					lambda_temp = (-b + sqrt(disc)) / (3.0 * a);

				if(lambda_temp > 0.5 * lambda)
					lambda_temp = 0.5 * lambda;
			}

			lambda_prev = lambda;
			prev_new_merit_value = new_merit;

			if(lambda_temp <= 0.1*lambda)
				lambda = 0.1 * lambda;
			else
				lambda = lambda_temp;
		}
		
	} // End of while loop
}	
	


/*********************************************************************/
mmcMatrix MeritFunction::GetNextBfgsSearchDir(const mmcMatrix &current_position, 
											const mmcMatrix &previous_position,
											const mmcMatrix &current_gradient,
											const mmcMatrix &previous_gradient,
											const mmcMatrix &prev_inv_hessian,
											mmcMatrix &new_inv_hessian)const
/*********************************************************************/
/*  AUTHOR: Michael Greminger
**  DATE: 10/22/00
**
**  PURPOSE
**    Calculates the search direction to use using the BFGS method.
**
**  INPUT
**    NOTE: All of the matrices are implemented as an array of pointers
**          to row arrays.  Even the vectors are implemented in this 
**          way, they are simply column matrices.  This simplifies things
**          because the same functions can be used for matrices and vectors.
**    current_position_pp - Current position vector at this point in the 
**          optimization.
**    previous_position_pp - Previous position in the optimization.
**    current_gradient_pp - Gradient at the current position.
**    previos_gradient_pp - Gradient vector at previous position.
**    prev_inv_hessian_pp - Inverse Hessian from previous iteration.
**    num_dim - Number of degrees of freedom in the optimization problem.
**
**  OUTPUT
**    new_search_dir_pp - Vector indicating new search direction.
**    new_inv_hessian_pp - New inversion hessian matrix.
**
**  DESCRIPTION OF LOCAL VARIABLES
**    d_pp - d matrix used in the calculation of the inv Hessian 
**           matrix.  It is of the size n by n.
**    p_pp - p vector used in the calculation of the inv hessian.
**           It is of the size n by 1.
**    p_transpose_pp - transpose of p_pp.
**    sigma - scalar used in the evaluation of the Hessian.
**    tau - scalar used in the evaluation of the Hessian.
**    y_pp - n by 1 vector used to calculate the hessian.
**    y_transpose_pp - transpose of y_pp.
**    success_flag - Set to zero if an error occurrs.  Otherwise has]
**                   a value of one.
**    counter - Iteration counter.
**    temp_vector_1_pp - n by 1 used in intermediate calculations.
**    temp_single_element_matrix_pp - 1 by 1 matrix used in intermediante
**                                    calculations
**    temp_vector_transpose_pp - 1 by n used in intermediate calculations
**    temp_matrix_1_pp - n by n used in intermediate calculations
**    temp_matrix_2_pp - n by n used in intermediate calculations
**    temp_matrix_3_pp - n by n used in intermediate calculations
**    temp_matrix_4_pp - n by n used in intermediate calculations
**    temp_matrix_5_pp - n by n used in intermediate calculations
**    
**  FUNCTIONS CALLED
**    mat_mult - multiplies two matrices
**    mat_addition - adds two matrices
**    scale_matrix - multiplies a matrix by a scalar
**    transpose_matrix - transposes matrix
**    
*/
{
	/*
	**  LOCAL DECLARATIONS
	*/
	mmcMatrix d;
	mmcMatrix p;
	mmcMatrix y;
	mmcMatrix new_search_dir;
	double sigma;
	double tau;

	/* 
	**  Will first calculate p
	** p = current_position - previous_position
	*/
	//p = current_position - previous_position;
  p = current_position;
  p -= previous_position;
  
	/*
	**  Now will evaluate y
	**  y = current_gradient - previous_gradient
	*/
	/*  First need to multiply the previous gradient by -1 */
	//y = current_gradient - previous_gradient;
  y = current_gradient;
  y -= previous_gradient;

	/*
	**  Will now calculate sigma
	**  sigma = p dot y
	*/
	sigma = y.DotProduct(p);

	/*
	**  Will now calculate tau
	**  tau = y_transpose * prev_inv_hessian * y
	*/
	tau = (y.GetTranspose() * prev_inv_hessian * y).GetElement(0,0);

	/* Must insure that sigma is not zero becase it is used in the 
	** denominator of some calculations 
	*/
	if(sigma == 0)
	{
		sigma = 1.0e-100;
		//throw MeritFunctionException();
	}

	/*
	**  will now calculate the d matrix
	** d = (sigma + tau)/(sigma^2) * p * p_transpose -
	**     (1/sigma)(h*y*p_transpose + p * (h*y)transpose)
	*/
	//d = p.GetScaled((sigma + tau) / (pow(sigma,2))) * p.GetTranspose()  - 
	//	((prev_inv_hessian * y.GetScaled(1/sigma)) * p.GetTranspose() + p.GetScaled(1/sigma) * (prev_inv_hessian * y).GetTranspose());
  
  d = p.GetScaled((sigma + tau) / (pow(sigma,2))) * p.GetTranspose();
  d -= (prev_inv_hessian * y.GetScaled(1/sigma)) * p.GetTranspose();
  d -= p.GetScaled(1/sigma) * (prev_inv_hessian * y).GetTranspose();
  
	/* 
	**  Can now calculate the current inv hessian
	** inv_hessian = prev_inv_hessan + d
	*/
	//new_inv_hessian = prev_inv_hessian + d;
  new_inv_hessian = prev_inv_hessian;
  new_inv_hessian += d;
  
	/*
	**  Can now calculate the new search direction
	** search_dir = -1.0*inv_hessian * gradient
	*/
	new_search_dir = new_inv_hessian * current_gradient.GetScaled(-1.0);

  return new_search_dir;
}


/**********************************************************************/
double MeritFunction::GetLambdaLimit(const mmcMatrix &x_ref, const mmcMatrix &search_dir)
/**********************************************************************/
/*  AUTHOR:  Michael Greminger
**  DATE:  10/22/00
**
**  PURPOSE
**    Sets upper limit for value of lambda
**
**  INPUT
**    x_ref - current reference point
**    search_dir - current search direction
**
**  OUTPUT
**    lambda_lims - upper limit for one-dimensional search
**
**  DESCRIPTION OF LOCAL VARIABLES
**
**  FUNCTIONS CALLED
*/
{
/*
**  LOCAL DECLARATIONS
*/
/*
**  START OF EXECUTABLE CODE
*/
	double search_dir_mag;

	search_dir_mag = sqrt(search_dir.DotProduct(search_dir));

	if (search_dir_mag != 0.0)
		return((SearchDistance / search_dir_mag));
	else
		return(1.0);

	//return(SearchDistance);
}


// This function calculates a fractional reduction of the uncertainty level for the mult gold function
double  MeritFunction::GetF(const mmcMatrix & search_dir)
{
	double new_f;

	new_f = MultGoldResolution / (sqrt(search_dir.DotProduct(search_dir)) * SearchDistance);

	return(new_f);

	return (new_f);
}


/**********************************************************************/
mmcMatrix MeritFunction::MinimizeMeritFunction(const mmcMatrix &x_init, double search_distance, double tolerance, double mult_gold_resolution, int maxit, int verbose_level, ostream *output_buffer, int max_merit_evals)
/**********************************************************************/
/*  AUTHOR: Michael Greminger
**  DATE:  10/22/00
**
**  PURPOSE
**    Implementation of the BFGS multi-d optimization method.  Uses 
**    multgold to handle all of the 1-d optimizations.
**
**  INPUT
**    NOTE:  All of the matrices are implemented as arrays of pointers
**           to row arrays.  Even the vectors are implemented like this
**           so that vectors and matrices can be manipulated by the same
**           functions.  The _pp suffix on the variable name indicates that
**           it is this type of matrix.
**    x_init_pp - starting point for the optimization.
**    merit - pointer to the function that evaluates to the merit function.
**    grad - pointer to the function that evaluates to the gradient of the 
**           merit function.
**    lambda_lims - pointre to function to set upper limits for the 1-d
**                  optimizations.
**    num_dim - Number of degrees of freedom to be optimized.
**    tolerance - convergence criterea.
**    maxit - maximum number of iterations allowed.
**    printprogress - flag indicating wheather to print status reports
**                    during the calculation.
**    f - fractional reduction in the interval of uncertainty
**    merit_data_p - void* pointer to data needed by the merit function and 
**                   gradient function.
**
**  OUTPUT
**    x_best_pp - optimized design variables
**    bfgs_optimization - successful completion flag
**
**  DESCRIPTION OF LOCAL VARIABLES
**    prev_gradient_pp - gradient from last iteration
**    current_gradient_pp - gradient at current position
**    prev_inv_hessian_pp - inv hessian from previous iteration
**    current_inv_hessian_pp - current inv hessian matrix
**    displacement_vector_pp - vector from previous x position to current 
**                             x position
**    x_previous_pp - previous x position
**    search_dir_pp - current search direction
**    count - counter variable
**    success_flag - set to zero if an error occurs, otherwise has a value 
**                   of one
**    slope - used to insure that search_direction is acceptable.
**    row - row counter variable
**    col - col counter variable
**    temp_vector_pp - vector used for imtermediate calculations
**    lambda_upper_limit - the upper limit of lambda for the 1-d optimization
**    lambda_u_final - upper limit of lambda returned by multgold
**    lambda_l_final - lower limit of lambda returned by multgold
**    lambda_mid - average of lambda_u_final and labda_l_final
**    search_vector_mag - magnitude of the search vector
**    temp_vector_pp - vector used in intermediate calculations
**
**  FUNCTIONS CALLED
**    get_bfgs_search_dir - gets the next search direction using the 
**                          bfgs method.
**    mat_addition - adds two matrices
**    scale_matrix - multiplies a matrix by a scalar.
**    dot_product - computes that dot product of two vectors
**    copy_matrix - copies the contents of one matrix to another of equal size
*/
{
  MaxMeritEvals = max_merit_evals;
  MeritEvals = 0;
	
  out_buf = output_buffer;
	
  SearchDistance = search_distance;  
  MultGoldResolution  = mult_gold_resolution;
  VerboseLevel = verbose_level;

  /*
  **  LOCAL DECLARATIONS
  */
  mmcMatrix prev_gradient;
  double prev_merit, new_merit;
  mmcMatrix current_gradient;
  mmcMatrix prev_inv_hessian(NumDimensions, NumDimensions);
  mmcMatrix current_inv_hessian;
  mmcMatrix displacement_vector;
  mmcMatrix x_previous;
  mmcMatrix search_dir;
  mmcMatrix temp_vector;
  mmcMatrix x_best;
  int count;
  double slope;
  double lambda_upper_limit;
  double lambda_l_final, lambda_u_final, lambda_mid;
  double search_vector_mag;

  if(VerboseLevel >= 2)
    {
      *out_buf << "Initial x position = \n";
      display_vector(x_init, out_buf);
    }

  /*
  **  START OF EXECUTABLE CODE
  */

  /*  Insure that num_dim is valid */
  if(NumDimensions <= 0 )
    throw MeritFunctionException();
    
      /*
      **  assign prev_inv_hessian_pp to the identity matrix
      */
	  prev_inv_hessian.SetIdentity();

      /* 
      **  set the current x equal to the initial x
      */
      x_previous = x_init;

      /*
      **  Calculate the gradient at the initial position
      */
	  if(LineSearch == BACK_TRACK)
	  {
		  GetMeritValuePlusGradient(x_init, prev_merit, prev_gradient);
		  MeritEvals++;
	  }
	  else
	  {
		  prev_gradient = GetMeritGradient(x_init);
	  }

	  if(VerboseLevel >= 1)
	  {
		//display the initial merit function value
	    *out_buf << "Initial Merit Value = " <<  GetMeritValue(x_init) << "\n" << flush;

	  }

      if(VerboseLevel >= 2)
		{
		  //Display the initial gradient
		  *out_buf << "Initial Gradient =\n";
		  display_vector(prev_gradient, out_buf);
		}
      
      /*
      **  Set the initial search dir to the negative of the initial gradient
      */
      /*  Must first multiply the gradient by -1.0 */
      search_dir = prev_gradient.GetScaled(-1.0);

      /*
      **  Now will enter iteration loop to complete optimization
      */
      for (count = 0; count < maxit; count++)
	{


	  if(LineSearch == GOLDEN_SECTION)
	  {
		  // Use golden section line search

		  /*
		  **  Get the lambda upper limit
		  */
		  //lambda_upper_limit = GetLambdaLimit(x_previous,search_dir);
		  lambda_upper_limit = SearchDistance;

		  /*
		  ** Call multgold to do the 1-d optimization
		  */
		  MultGold(lambda_upper_limit, GetF(search_dir) ,search_dir,x_previous, &lambda_l_final, &lambda_u_final);

		  /*
		  **  get the value for lambda mid in order to get a better
		  **  x position.
		  */
		  lambda_mid = 0.5 * (lambda_u_final + lambda_l_final);
	  } else if(LineSearch == BACK_TRACK) {
		  // Use backtracking line search

		  bool back_track_error;

		  lambda_mid = BackTrack(x_previous, prev_gradient, prev_merit, search_dir, search_distance, MultGoldResolution, 1e-4, 0.9, back_track_error, current_gradient, new_merit); 
	  
		  if(back_track_error && VerboseLevel >= 1)
		  {
			*out_buf << "Back tracking line search could not find a satisfactory lambda value." << endl;
		  }

	  } else {
	    cout << "bfgsError: No valid LineSearch selected" << endl;
	    lambda_mid = 0.0;
	  }

	  /* 
	  **  get the value for the current x position
	  */
	  x_best = x_previous + search_dir.GetScaled(lambda_mid);

	  /*  if print progress is on, print the current x value */
	    if(VerboseLevel >= 1)
	      {
					*out_buf << "Iteration =" << count << ", lambda_mid = " <<
							lambda_mid << ", Merit = " << GetMeritValue(x_best) << "\n" << flush;

    			if(VerboseLevel >= 2)
    			{
    				*out_buf << "Current x = \n";
    				display_vector(x_best, out_buf);
    				*out_buf << "Current search dir = \n";
    				display_vector(search_dir, out_buf);
    			}
    			
	      }

	  /*
	  **  Must check for convergence
	  */
	  /*  Need to first get the magnitude of the search vector */	  
		search_vector_mag = sqrt(search_dir.DotProduct(search_dir));

	  /* Now the actual convergence check is preformed */
	    if((lambda_mid * search_vector_mag) < tolerance)
	      {/* solution has converged!! */
			   /* return the solution*/
			   return x_best;
	      }

	  /* 
	  **  calculate the gradient at the new x_position
	  */
	  if(LineSearch != BACK_TRACK)
		  current_gradient = GetMeritGradient(x_best);

	  /*
	  **  Calculate the next search direction using bfgs method
	  */
	  search_dir = GetNextBfgsSearchDir(x_best, x_previous, current_gradient, prev_gradient,
		                                prev_inv_hessian, current_inv_hessian);

	  /*
	  **  Check that search direction is still pointing in a 
	  **  direction to decrease the merit function.
	  */
		slope = search_dir.DotProduct(current_gradient);
	    if(slope > 0)
	      {
    			/*
    			**  started going in the wrong direction
    			**  need to reset the search direction to opposite the
    			**  gradient
    			*/
    		  search_dir = current_gradient.GetScaled(-1.0);
    			current_inv_hessian.SetIdentity();

    			if(VerboseLevel >= 1)
 			  		*out_buf << "Started going the wrong way!!!  Now going opposite of gradient.\n"; 					
 			  }


	  /*
	  **  Need to reset prev_gradient, prev_inv_hessian and 
	  **  x_previous
	  */
	  prev_gradient = current_gradient;
	  prev_merit = new_merit;
	  prev_inv_hessian = current_inv_hessian;
	  x_previous = x_best; 

      // Check to see if the maximum number of evaluations of the merit funciton has been reached
	  if(MaxMeritEvals != 0 && MeritEvals > MaxMeritEvals)
	  {
		 cout << "Time limited" << endl;
		 break;
	  }

	} 

  /* optimization went to the max number of iterations */
  return(x_best);
}


/*  The following function is used to print a vector to the screen */
void display_vector(mmcMatrix vector, ostream *out_buf)
{
  int counter;
  int num_dim = vector.GetNumRows();

  for(counter = 0; counter < num_dim; counter++)
    {
      *out_buf << "\tx" << counter + 1 << " =\t" << vector.GetElement(counter,0) << "\n";
    }

  *out_buf << "\n";
}

//Calculates the merit function gradient numerically
mmcMatrix MeritFunction::GetNumericalGradient(const mmcMatrix & x, double tolerance)
{
	mmcMatrix upper_x;
	mmcMatrix lower_x;
	double upper_y;
	double lower_y;

	mmcMatrix result(NumDimensions,1); 
  
	//insure that tolerance is a positive non-zero value
	if(tolerance <= 0.0)
		throw MeritFunctionException();

	//Check to insure that x is of the proper dimensions
	if(x.GetNumColumns() != 1 || x.GetNumRows() != NumDimensions)
		throw MeritFunctionException();
  
	for(int counter = 0; counter < NumDimensions; counter++)
	{
		if(counter == 0)
      upper_x = x;
    else
      upper_x(counter-1,0) = x(counter-1,0);
    
		upper_x(counter,0) += 0.5 * tolerance;

		if(counter == 0)
      lower_x = x;
    else
      lower_x(counter-1,0) = x(counter-1,0);
    
		lower_x(counter,0) += -0.5 * tolerance;

		upper_y = GetMeritValue(upper_x);
		lower_y = GetMeritValue(lower_x);

		result.SetElement(counter,0, (upper_y - lower_y) / tolerance);
	}

	return result;
}


double GetRandomNumber(double lower_limit, double upper_limit)
{
	

  return ((double)rand() * ((upper_limit - lower_limit) / (double) RAND_MAX)) + lower_limit;
}




mmcMatrix MeritFunction::MonteCarloOptimization(const mmcMatrix &x_init, const mmcMatrix &x_delta, int number_iterations, int verbose_level)
{
	VerboseLevel = verbose_level;

	mmcMatrix current_x_min(x_init);
	mmcMatrix x_temp(NumDimensions, 1);

	double temp_merit;
	double current_merit_min;
	
	current_merit_min = GetMeritValue(x_init);
	
	if (VerboseLevel >= 1)
		cout << "Current merit value = " << current_merit_min << "\n";

	for(int counter = 0; counter < number_iterations; counter++)
	{
		for(int dim_counter = 0; dim_counter < NumDimensions; dim_counter++)
		{		
			x_temp.SetElement(dim_counter, 0, GetRandomNumber(x_init.GetElement(dim_counter, 0) - x_delta.GetElement(dim_counter, 0),
							  x_init.GetElement(dim_counter, 0) + x_delta.GetElement(dim_counter, 0)));
		}

		temp_merit = GetMeritValue(x_temp);

		if (temp_merit < current_merit_min)
		{
			current_merit_min = temp_merit;
			current_x_min = x_temp;
		}

		if ( VerboseLevel >=1 )
			cout << "Current merit value = " << current_merit_min << "\n";
	}


	return current_x_min;
}



mmcMatrix MeritFunction::ConjugateGradient(const mmcMatrix &x_init, double search_distance, double tolerance, double mult_gold_resolution, int maxit, int verbose_level, ostream *output_buffer)
{
  out_buf = output_buffer;
	
  SearchDistance = search_distance;  
  MultGoldResolution  = mult_gold_resolution;
  VerboseLevel = verbose_level;

  /*
  **  LOCAL DECLARATIONS
  */

  mmcMatrix current_gradient;
  mmcMatrix displacement_vector;
  mmcMatrix x_previous;
  mmcMatrix search_dir;
  mmcMatrix temp_vector;
  mmcMatrix x_best;
  int count;
  double slope;
  double lambda_upper_limit;
  double lambda_l_final, lambda_u_final, lambda_mid;
  double search_vector_mag;
  double a, b;

  if(VerboseLevel >= 2)
    {
      *out_buf << "Initial x position = \n";
      display_vector(x_init, out_buf);
    }

  /*
  **  START OF EXECUTABLE CODE
  */

  /*  Insure that num_dim is valid */
  if(NumDimensions <= 0 )
    throw MeritFunctionException();
    
      /* 
      **  set the current x equal to the initial x
      */
      x_previous = x_init;

      /*
      **  Calculate the gradient at the initial position
      */
	  current_gradient = GetMeritGradient(x_init);


	  if(VerboseLevel >= 1)
	  {
		//display the initial merit function value
	    *out_buf << "Initial Merit Value = " <<  GetMeritValue(x_init) << "\n" << flush;

	  }

      if(VerboseLevel >= 2)
		{
		  //Display the initial gradient
		  *out_buf << "Initial Gradient =\n";
		  display_vector(current_gradient, out_buf);
		}
      
      /*
      **  Set the initial search dir to the negative of the initial gradient
      */
      /*  Must first multiply the gradient by -1.0 */
      search_dir = current_gradient.GetScaled(-1.0);

	  search_dir = search_dir.GetNormalized();

	  a = current_gradient.DotProduct(current_gradient);

      /*
      **  Now will enter iteration loop to complete optimization
      */
      for (count = 0; count < maxit; count++)
	{
	  /*
	  **  Get the lambda upper limit
	  */
	  //lambda_upper_limit = GetLambdaLimit(x_previous,search_dir);
	  lambda_upper_limit = SearchDistance;

	  /*
	  ** Call multgold to do the 1-d optimization
	  */
	  MultGold(lambda_upper_limit, mult_gold_resolution ,search_dir,x_previous, &lambda_l_final, &lambda_u_final);

	  /*
	  **  get the value for lambda mid in order to get a better
	  **  x position.
	  */

	  lambda_mid = 0.5 * (lambda_u_final + lambda_l_final);

	  /* 
	  **  get the value for the current x position
	  */
	  x_best = x_previous + search_dir.GetScaled(lambda_mid);

	  /*  if print progress is on, print the current x value */
	    if(VerboseLevel >= 1)
	      {
			*out_buf << "Iteration=" << count << ", lambda_mid=" << 
							lambda_mid << ", Search Mag="<< search_dir.GetMagnitude() << ", Merit =" << GetMeritValue(x_best) << "\n" << flush;
			if(VerboseLevel >= 2)
			{
				*out_buf << "Current x = \n";
				display_vector(x_best, out_buf);
				*out_buf << "Current search dir = \n";
				display_vector(search_dir, out_buf);
			}
	      }

	  /*
	  **  Must check for convergence
	  */
	  /*  Need to first get the magnitude of the search vector */	  
		search_vector_mag = sqrt(search_dir.DotProduct(search_dir));

	  /* Now the actual convergence check is preformed */
	    if((lambda_mid * search_vector_mag) < tolerance)
	      {/* solution has converged!! */
			/* return the solution*/
			return x_best;
	      }

	  /* 
	  **  calculate the gradient at the new x_position
	  */
	  current_gradient = GetMeritGradient(x_best);

	  b = current_gradient.DotProduct(current_gradient);
		
	  search_dir = current_gradient.GetScaled(-1.0) + search_dir.GetScaled(b / a);
	  //search_dir = current_gradient.GetScaled(-1.0);

	  search_dir = search_dir.GetNormalized();

	  a = b;


	  /*
	  **  Check that search direction is still pointing in a 
	  **  direction to decrease the merit function.
	  */
		slope = search_dir.DotProduct(current_gradient);
	    if(slope > 0)
	      {
			/*
			**  started going in the wrong direction 
			**  need to reset the search direction to opposite the
			**  gradient
			*/
		    search_dir = current_gradient.GetScaled(-1.0);
     
			if(VerboseLevel >= 1)
			  *out_buf << "Started going the wrong way!!!  Now going opposite of gradient.\n" << flush;
	      }

	  /*
	  **  Need to reset prev_gradient, prev_inv_hessian and 
	  **  x_previous
	  */
	  x_previous = x_best; 
	} 

  /* optimization went to the max number of iterations */
  return(x_best);
}
