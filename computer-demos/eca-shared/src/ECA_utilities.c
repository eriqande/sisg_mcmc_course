/*  headers: */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ECA_MemAlloc.h"
#include "ECA_utilities.h"



/*  Memory Allocation Functions */

/*  allocate to an array of doubles subscripted from 0 to High.   */
/*  Dummy must be zero. */
double *
dvector(int Lo, int High)  
{
	double *temp;
	
	temp = (double *)ECA_MALLOC((size_t)  (High-Lo + 2) * sizeof(double) );
	
	if(temp==NULL)  {
		printf("\n\ndvector failed to allocate to array of length %d",High-Lo+2);
		exit(1);
	}	
		
	return(temp-Lo +1);
}



/*  free the memory allocated to a double vector */
void 
free_dvector(double *Array, int Lo, int High)
{
	int ArrayLength;
	/*  I don't really use ArrayLength, but have it here so that  */
	/*  Dummy and High are used and so we don't get a compile warning about them */
	ArrayLength = High - Lo + 1;
	
	free( (char *) (Array + Lo - 1) );
}

/*  allocate to an array of ints subscripted from 0 to High.   */
/*  Dummy must be zero. */
int *
ivector(int Lo, int High)  
{
	int *temp;
	
	temp = (int *)ECA_MALLOC( (size_t) ((High - Lo + 2) * sizeof(int)) );

	if(temp==NULL)  {
		printf("\n\nivector failed to allocate to array of length %d",High-Lo+2);
		exit(1);
	}	
		
	return (temp-Lo+1);
}

/*  free the memory allocated to an int vector */
void 
free_ivector(int *Array, int Lo, int High)
{
	int ArrayLength;
	/*  I don't really use ArrayLength, but have it here so that  */
	/*  Dummy and High are used and so we don't get a compile warning about them */
	ArrayLength = High - Lo + 1;
	
	free( (char *) (Array+Lo-1));
}


/*  allocate to a matrix of doubles subscripted from 0 to HiRows and 0 to HiCols */
double **
dmatrix(int LoRows, int HiRows, int LoCols, int HiCols)
{
	int i,NumRows,NumCols;
	double **temp;
	
	NumRows = HiRows - LoRows + 1;
	NumCols = HiCols - LoCols + 1;
		
	temp = (double **)ECA_MALLOC( (size_t) ((NumRows+1) * sizeof(double *) ) );
	
	if(temp==NULL)  {
		printf("\n\ndmatrix failed to allocate to array of pointers of length %d",HiRows+1);
		exit(1);
	}
	
	temp += 1;
	temp -= LoRows;
	
	temp[LoRows] = (double *)ECA_MALLOC( (size_t)( (NumRows*NumCols+1)*sizeof(double)));
	
	if(temp[LoRows]==NULL)  {
		printf("\n\ndmatrix failed to allocate to array of doubles of length %d",HiRows+1);
		exit(1);
	}
	temp[LoRows] += 1;
	temp[LoRows] -= LoCols;
	
	for(i=LoRows+1;i<=HiRows;i++)  {
		temp[i] = temp[i-1] + NumCols;
	}
	
	return(temp);
}

void
free_dmatrix(double **Matrix, int LoRows, int HiRows, int LoCols, int HiCols)
{
	int silly;
	
	silly = HiCols - HiRows;
	
	free( (char *) (Matrix[LoRows] + LoCols - 1));
	free( (char *) (Matrix + LoRows -1));
}	


/*  allocate to a matrix of ints subscripted from 0 to NumRows - 1 and 0 to NumCols - 1 */
int **
imatrix(int NumRows, int NumCols)
{
	int i;
	int **temp;
		
	temp = (int **)ECA_CALLOC( (size_t)NumRows, sizeof(int *));
	
	if(temp==NULL)  {
		printf("\n\nimatrix failed to allocate to array of pointers of length %d",NumRows);
		exit(1);
	}
	
	for(i=0;i<NumRows;i++)  {
		temp[i] = (int *)ECA_CALLOC( (size_t)NumCols, sizeof(int));
		if(temp[i]==NULL)  {
			printf("\n\nimatrix failed to allocate to array of ints of length %d",NumCols);
			exit(1);
		}
	}
	
	return(temp);
}

void
free_imatrix(int **Matrix, int NumRows)
{
	int i;
	
	for(i=0;i<NumRows;i++)  {
		free(Matrix[i]);
	}
	free(Matrix);
}	


