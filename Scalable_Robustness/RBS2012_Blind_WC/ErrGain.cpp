#include <iostream>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>


using namespace std;

#define MAX_LENGTH 52        		//This is the length of lists which need to be compared
#define MAX_ERR_PATTERN_LENGTH 2567 	//This is an estimate of the number of comparisons required by the longest running computation
#define ERR_INPUT_NUM 1000			//This the number of inputs and error patterns which will be executed to check for criticallity
#define RATE_DENOM 20			//This is the RATE_DENOM mentioned above (its the thing which divides the rates)
#define KB_UP_BOUND 2567			//This is for measuring the K-bubble sort criticality. 
#define COMP_NUM_BASE 0			//This tells you which comparison to start with.
#define RATE 1


uint64_t Abs(int64_t i){
  if (i >= 0)
    return i;
  else 
    return -i;
}

/**
 * @return: This finds the amount of error in an array
 */
int64_t FindPositionError(uint64_t * array, int64_t length){
  int64_t error = 0;
  for (int64_t i = 0; i < length; i ++){
    error += Abs((int64_t)array[i] - i);
  } 
  return error;
}

/**
 * @return: whether comparison Count fails given some error pattern.
 */
bool Pattern_Func(uint64_t* ErrPattern, uint64_t Count){ 
  uint64_t CountIndex = Count/sizeof(uint64_t);
  uint64_t CountShift = Count % sizeof(uint64_t);
  return ((ErrPattern[CountIndex]) >> CountShift) & 1; 
}


uint64_t Fault_Count(uint64_t* ErrPattern, uint64_t Length){
	uint64_t faults = 0;
	for (uint64_t i = 0; i < Length; i ++)
		if(Pattern_Func(ErrPattern, i))
			faults ++;
	return faults;
}



//Merge Sort Code was taken and modified from http://www.cprogramming.com/tutorial/computersciencetheory/merge.html
/* Helper function for finding the max of two numbers */
uint64_t max(uint64_t x, uint64_t y)
{
  if(x > y)
    {
      return x;
    }
  else
    {
      return y;
    }
}

/* left is the index of the leftmost element of the subarray; right is one
 * past the index of the rightmost element */
uint64_t merge_helper(uint64_t *input, uint64_t left, uint64_t right, uint64_t *scratch, uint64_t * ErrPattern, uint64_t Count, uint64_t ActLength)
{
  /* base case: one element */
  if(right == left + 1)
    {      
      return Count;
    }
  else
    {
      uint64_t i = 0;
      uint64_t length = right - left;
      uint64_t midpoint_distance = length/2;
      /* l and r are to the positions in the left and right subarrays */
      uint64_t l = left, r = left + midpoint_distance;

      /* sort each subarray */
      Count = merge_helper(input, left, left + midpoint_distance, scratch, ErrPattern, Count, ActLength);
      Count = merge_helper(input, left + midpoint_distance, right, scratch, ErrPattern, Count, ActLength);



      /* merge the arrays together using scratch for temporary storage */ 
      for(i = 0; i < length; i++)
        {
	  /* Check to see if any elements remain in the left array; if so,
	   * we check if there are any elements left in the right array; if
	   * so, we compare them.  Otherwise, we know that the merge must
	   * use take the element from the left array */
	  // we also modify the code here using Pattern_Func to create errors in the code at the 
	  // appropriate moment. 
     if (length != ActLength && ((input[l] == ActLength - 1) || (input[r] == ActLength - 1))) {
		ErrPattern[(Count/sizeof(uint64_t))] |= (1 << Count % sizeof(uint64_t));
//		cerr << "InputL! " << input[l] << " and L! " << l << " inputR! " << input[r] << " r " << r << "\n";	
	  }

	  if(l < left + midpoint_distance && 
	     (r == right || ((!Pattern_Func(ErrPattern, Count) && max(input[l], input[r]) == input[r])
			     || (Pattern_Func(ErrPattern, Count) && max(input[l],input[r]) == input[l]))))
            {
	      scratch[i] = input[l];
	      l++;
            }
	  else
            {
	      scratch[i] = input[r];
	      r++;
            }
	  // here I create some code to increment Count at the appropriate moment
	  if ((((l - 1) < (left + midpoint_distance)) && (r < right)) ||
		(l < (left + midpoint_distance) && (r - 1) < right)){
	    Count ++; 

	  }
        }
      /* Copy the sorted subarray back to the input */
      for(i = left; i < right; i++)
        {
	  input[i] = scratch[i - left];
        }
    }
//          cerr << "Num2 Left: " << left << " right: " << right << " ErrPattern: " << hex << ErrPattern << " Count " << Count << "\n";
  return Count;
}

/* mergesort returns true on success.  Note that in C++, you could also
 * replace malloc with new and if memory allocation fails, an exception will
 * be thrown.  If we don't allocate a scratch array here, what happens? 
 *
 * Elements are sorted in reverse order -- greatest to least */

uint64_t Merge_Sort(uint64_t *input, uint64_t size, uint64_t * ErrPattern)
{
  uint64_t Count = 0;
  uint64_t *scratch = (uint64_t *)malloc(size * sizeof(uint64_t));
  if(scratch != NULL)
    {
      Count = merge_helper(input, 0, size, scratch, ErrPattern, Count, size);
      free(scratch);
      return Count;
    }
  else
    {
      return 0;
    }
}

uint64_t Comp_Count_Sort(uint64_t * array, uint64_t Length, uint64_t* ErrPattern){
	uint64_t Counts[MAX_LENGTH];
	uint64_t Scratch[MAX_LENGTH];
	uint64_t count = 0;	

	for(int i = 0; i < Length; i ++)
		Counts[i] = 0;

	for(uint64_t i = 0; i < Length; i ++){
		for (uint64_t j = 0; j < Length; j ++){
			if(j == i);
			else{ 
				if ((array[i] > array[j] && !Pattern_Func(ErrPattern,count)) 
	  				|| (array[i] < array[j] && Pattern_Func(ErrPattern,count))){
					Counts[i] ++;		
			    	}
			count ++;	
			}
		}
	}

	int q = 0;
	for(uint64_t i = 0; i < Length; i ++){
		for (uint64_t j = 0; j < Length; j ++){
			if (Counts[j] == i){
				Scratch[q] = array[j];
				q++;
			}
		}
	}
	
	for(uint64_t i = 0; i < Length; i ++)
		array[i] = Scratch[i];
	 				
	return count; 
}

//This performs a bubble sort
uint64_t FBubble_Sort(uint64_t * array, uint64_t length, uint64_t * ErrPattern){
  uint64_t Count = 0;
  for(uint64_t i = 0; i < length - 1; i ++){
    for (uint64_t j = 0; j < length - 1; j ++){
      if ((array[j] > array[j+1] && !Pattern_Func(ErrPattern, Count)) 
	  || (array[j] < array[j+1] && Pattern_Func(ErrPattern,Count))){
	uint64_t t = array[j+1];
	array[j+1] = array[j];
	array[j] = t;
      }
      Count ++;
    }
  }
  return Count;
}

/**
 * @return: a legitimate random input i.e. one which has only the numbers 1...Length, each one occuring only once
It relies on an input value called length to do its calculation
 */
void CreateInput(uint64_t * arg, uint64_t Length){
  uint num_taken[MAX_LENGTH];

  for(uint64_t i = 0; i < Length; i ++){
      num_taken[i] = 0;
  }
  for(uint64_t i = 0; i < Length; i++){
    int random;
    int q;
    int hold;
    if((Length - i - 1) > 0)
      random = rand()%(Length - i - 1);
    else 
      random = 0;
    q = 0;
    for(uint64_t j = 0; j < Length; j ++){ 
      if (q == random && !num_taken[j]){
	num_taken[j] = 1;
	hold = j;
	j = Length;
      }
      else{ 
	if (q < random && !num_taken[j]){
	  q++;
	}
      }
    }
    arg[i] = hold;
  }
}


/**
 * 
 * @return: This will take in an ErrPattern array and then return that array filled with a valid ErrPattern
 */
void CreateErrorPattern(uint64_t * ErrPattern, uint64_t Rates, uint64_t Rate_Denom, uint64_t Err_Length){
  
  for(uint64_t i = 0; i < Err_Length; i++){
      ErrPattern[i] = 0;
  }  
  for(uint64_t i = 0; i < Err_Length * sizeof(uint64_t); i++){
      if ((uint64_t)rand() % Rate_Denom < Rates){
	ErrPattern[(i/sizeof(uint64_t))] |= (1 << i % sizeof(uint64_t));
      }
  }
}

//This creates an input list which is sorted backwards
void Reverse_List(uint64_t* arg, uint64_t Length){
	for(uint64_t i = 0; i < Length; i ++)
		arg[i] = Length - i-1;
}

//This creates an input list which is sorted backwards
void Semi_Reverse_List(uint64_t* arg, uint64_t Length){
	for(uint64_t i = 0; i < Length; i ++)
		arg[i] = Length - i-1;
	arg[0] = 0;
	arg[Length - 1] = Length - 1;
}


//This is used to create an error pattern which blocks every nth comparison
//used with full bubble sort.
void N_Blocker(uint64_t * ErrPattern, uint64_t Err_Length, uint64_t Length){
  
  for(uint64_t i = 0; i < Err_Length; i++){
      ErrPattern[i] = 0;
  }  
//	cerr << "Okay here!\n";
  for(uint64_t i = 0; i < Err_Length * sizeof(uint64_t); i++){
	if(Length >1)
      if (i%(Length-1) == (Length-1)/2){
	ErrPattern[(i/sizeof(uint64_t))] |= (1 << i % sizeof(uint64_t));
      }
  }
}


void CCS_WC_ERR(uint64_t * ErrPattern, uint64_t Err_Length, uint64_t Length){
  
  for(uint64_t i = 0; i < Err_Length; i++){
      ErrPattern[i] = 0;
  }  



  for(uint64_t i = 0; i < Err_Length * sizeof(uint64_t); i++){
	if(Length >1){
      			if (i%(Length-1) >= (i/(Length-1)) && (i%(Length-1)) <= (Length/2)){
				ErrPattern[(i/sizeof(uint64_t))] |= (1 << i % sizeof(uint64_t));
      			}
			if (i%(Length-1) <= (i/(Length-1)) && (i%(Length-1)) >= (Length/2)){
				ErrPattern[(i/sizeof(uint64_t))] |= (1 << i % sizeof(uint64_t));
      			}
	}
  }
}


void MakeO(uint64_t * ErrPattern, uint64_t Err_Length, uint64_t Length){
  for(uint64_t i = 0; i < Err_Length; i++){
      ErrPattern[i] = 0;
  }  
}

  	
int main() {
  uint64_t arg[MAX_LENGTH];  
  uint64_t arg2[MAX_LENGTH];
  uint64_t ErrPattern[MAX_ERR_PATTERN_LENGTH];
  double MaximumGainFB[MAX_LENGTH];  
  double MaximumGainMS[MAX_LENGTH];
  double MaximumGainCCS[MAX_LENGTH]; 




  for(uint64_t i = 0; i < MAX_LENGTH; i ++){
	MaximumGainFB[i]=0;  
 	MaximumGainMS[i]=0;
  	MaximumGainCCS[i]=0; 
  }

  srand (time(NULL));


      //This is the Lengths loop, determines the length under comparison...
      for(uint64_t Length = 1; Length < MAX_LENGTH; Length ++){
	//Here we calculate the maximum positional error.
	uint64_t Max_Pos_Err;
	Reverse_List(arg2, Length);
	Max_Pos_Err = FindPositionError(arg2, Length);

	//This is the input loop      	
	for(uint64_t InsAndErrs = 0; InsAndErrs < ERR_INPUT_NUM; InsAndErrs ++){
		uint64_t GetCount = 0;
		double NormPE,FaultRatio;
	
//		N_Blocker(ErrPattern, MAX_ERR_PATTERN_LENGTH, Length);
	  	CreateErrorPattern(ErrPattern, RATE, Length, MAX_ERR_PATTERN_LENGTH);
//		Reverse_List(arg, Length);
	  	CreateInput(arg, Length); 
	  	GetCount = FBubble_Sort(arg, Length, ErrPattern);
		NormPE = (double)FindPositionError(arg, Length)/(double)Max_Pos_Err;
		FaultRatio = (double)Fault_Count(ErrPattern, GetCount)/(double)GetCount;
		if(FaultRatio > 0)
			if(MaximumGainFB[Length] < (NormPE/FaultRatio))
				MaximumGainFB[Length] = (NormPE/FaultRatio);



//		MakeO(ErrPattern, MAX_ERR_PATTERN_LENGTH, Length);
	  	CreateErrorPattern(ErrPattern, RATE, Length, MAX_ERR_PATTERN_LENGTH);
//		Semi_Reverse_List(arg, Length);
	  	CreateInput(arg, Length); 
	  	GetCount = Merge_Sort(arg, Length, ErrPattern);
		NormPE = (double)FindPositionError(arg, Length)/(double)Max_Pos_Err;
		FaultRatio = (double)Fault_Count(ErrPattern, GetCount)/(double)GetCount;
		if(FaultRatio > 0)		
			if(MaximumGainMS[Length] < (NormPE/FaultRatio))
				MaximumGainMS[Length] = (NormPE/FaultRatio);

//		MakeO(ErrPattern, MAX_ERR_PATTERN_LENGTH, Length);
//		CCS_WC_ERR(ErrPattern, MAX_ERR_PATTERN_LENGTH, Length);
	  	CreateErrorPattern(ErrPattern, RATE, Length, MAX_ERR_PATTERN_LENGTH);
//		Reverse_List(arg, Length);
	  	CreateInput(arg, Length); 
	  	GetCount = Comp_Count_Sort(arg, Length, ErrPattern);
		NormPE = (double)FindPositionError(arg, Length)/(double)Max_Pos_Err;
		FaultRatio = (double)Fault_Count(ErrPattern, GetCount)/(double)GetCount;
		if(FaultRatio > 0)
			if(MaximumGainCCS[Length] < (NormPE/FaultRatio))
				MaximumGainCCS[Length] = (NormPE/FaultRatio);

		if((InsAndErrs % 100) == 0)		
			cerr << "InsErrCount: " << InsAndErrs << "\n";

    	}
		cerr << "Length: " << Length << " Finished!\n";
     }

	cout << "#Count FB MS CCS\n";

	for (uint64_t i = 0; i < MAX_LENGTH; i++)
		cout << i << " " << MaximumGainFB[i] << " " << MaximumGainMS[i]  << " " << MaximumGainCCS[i] << "\n";

  

	return 0;
}
