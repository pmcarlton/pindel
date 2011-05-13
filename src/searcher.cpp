// Pindel
#include "pindel.h"
#include "searcher.h"

using namespace std;
// Searcher.cpp

void CheckLeft_Close(const SPLIT_READ & OneRead,
							const string & TheInput, 
							const string & CurrentReadSeq,
							const vector <unsigned int> Left_PD[],
							const short & BP_Left_Start,
							const short & BP_Left_End,
							const short & CurrentLength,
							vector <UniquePoint> & LeftUP) {
	int Sum;
   if (CurrentLength >= BP_Left_Start && CurrentLength <= BP_Left_End) {
      // put it to LeftUP if unique
		for (short i = 0; i <= OneRead.MAX_SNP_ERROR; i++) {
			if (Left_PD[i].size() == 1 && CurrentLength >= BP_Left_Start + i) {
				Sum = 0;
				if (ADDITIONAL_MISMATCH) 
		   		for (short j = 1; j <= ADDITIONAL_MISMATCH; j++) 
			   		Sum += Left_PD[i + j].size();
				
				if (Sum == 0 && i <= (short)(CurrentLength * Seq_Error_Rate + 1)) {
					UniquePoint TempOne;
					TempOne.LengthStr = CurrentLength;
					TempOne.AbsLoc = Left_PD[i][0]; 
					TempOne.Direction = FORWARD;
					TempOne.Strand = ANTISENSE;
					TempOne.Mismatches = i;
					LeftUP.push_back(TempOne);	
					break;
				}
			}
		}
   }
   if (CurrentLength < BP_Left_End) {
      vector <unsigned int> Left_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED];
		for (int CheckedIndex = 0; CheckedIndex < OneRead.TOTAL_SNP_ERROR_CHECKED; CheckedIndex++) {
			Left_PD_Output[CheckedIndex].reserve(Left_PD[CheckedIndex].size());
		}
      const char CurrentChar = CurrentReadSeq[CurrentLength];
      //const int SizeOfCurrent = Left_PD.size();
      unsigned int pos;
		for (int i = 0; i < OneRead.TOTAL_SNP_ERROR_CHECKED_Minus; i++) {
			int SizeOfCurrent = Left_PD[i].size();
			if (CurrentChar == 'N') {
            //#pragma omp parallel default(shared)
				{
					// #pragma omp for
				   for (int j = 0; j < SizeOfCurrent; j++) {
					   pos =  Left_PD[i][j] + 1;
					   if (Match2N[(short)TheInput[pos]] == 'N')
						   Left_PD_Output[i].push_back(pos);
				   }     
				}
			}
			else {
            //#pragma omp parallel default(shared)
				{
               //#pragma omp for
				   for (int j = 0; j < SizeOfCurrent; j++) {
					   pos =  Left_PD[i][j] + 1;
					   if (TheInput[pos] == CurrentChar)
						   Left_PD_Output[i].push_back(pos);
					   else Left_PD_Output[i + 1].push_back(pos); 
				   }  
				}
			}			
		}
		
		int SizeOfCurrent = Left_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].size();
		if (CurrentChar == 'N') {
         //#pragma omp parallel default(shared)
			{
				// #pragma omp for
			   for (int j = 0; j < SizeOfCurrent; j++) {
				   pos =  Left_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus][j] + 1;
				   if (Match2N[(short)TheInput[pos]] == 'N')
					   Left_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
			   }
			}
		}
		else {
         //#pragma omp parallel default(shared)
			{
				// #pragma omp for
			   for (int j = 0; j < SizeOfCurrent; j++) {
				   pos =  Left_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus][j] + 1;
				   if (TheInput[pos] == CurrentChar)
					   Left_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
					//else Left_PD_Output[i + 1].push_back(pos); 
			   }   
			}
		}	
		Sum = 0;
		for (int i = 0; i <= OneRead.MAX_SNP_ERROR; i++) {
			Sum += Left_PD_Output[i].size();
		}
      if (Sum) {
         const short CurrentLengthOutput = CurrentLength + 1;
         CheckLeft_Close(OneRead, TheInput, CurrentReadSeq, Left_PD_Output,
								 BP_Left_Start, BP_Left_End,
								 CurrentLengthOutput, LeftUP);         
      }
      else return;
   }   
   else return;
}

void CheckRight_Close(const SPLIT_READ & OneRead,
							 const string & TheInput,
							 const string & CurrentReadSeq,
							 const vector <unsigned int> Right_PD[],
							 const short & BP_Right_Start,
							 const short & BP_Right_End,
							 const short & CurrentLength,
							 vector <UniquePoint> & RightUP) {
	//cout << CurrentLength << "\t" << RightUP.size() << "\t" << Right_PD[0].size() << "\t" << Right_PD[1].size() << endl;
   short ReadLengthMinus = CurrentReadSeq.size() - 1;      
	int Sum;
   if (CurrentLength >= BP_Right_Start && CurrentLength <= BP_Right_End) {
	   for (short i = 0; i <= OneRead.MAX_SNP_ERROR; i++) {
		   if (Right_PD[i].size() == 1 && CurrentLength >= BP_Right_Start + i) {
				Sum = 0;
				if (ADDITIONAL_MISMATCH) 
					for (short j = 1; j <= ADDITIONAL_MISMATCH; j++) 
						Sum += Right_PD[i + j].size();
				
				if (Sum == 0 && i <= (short)(CurrentLength * Seq_Error_Rate + 1)) {
					UniquePoint TempOne;
					TempOne.LengthStr = CurrentLength;
					TempOne.AbsLoc = Right_PD[i][0];   
					TempOne.Direction = BACKWARD;
					TempOne.Strand = SENSE;
					TempOne.Mismatches = i;
					RightUP.push_back(TempOne);	
					break;
				}
			}
		}
   }
	
   if (CurrentLength < BP_Right_End) {
      vector <unsigned int> Right_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED];
		for (int CheckedIndex = 0; CheckedIndex < OneRead.TOTAL_SNP_ERROR_CHECKED; CheckedIndex++) {
			Right_PD_Output[CheckedIndex].reserve(Right_PD[CheckedIndex].size());
		}
      const char CurrentChar = CurrentReadSeq[ReadLengthMinus - CurrentLength];
      unsigned int pos;
		
		for (int i = 0; i < OneRead.TOTAL_SNP_ERROR_CHECKED_Minus; i++) {
			int SizeOfCurrent = Right_PD[i].size();
			if (CurrentChar == 'N') {
            //#pragma omp parallel default(shared)
				{
					// #pragma omp for
				   for (int j = 0; j < SizeOfCurrent; j++) {
					   pos =  Right_PD[i][j] - 1;
					   if (Match2N[(short)TheInput[pos]] == 'N')
						   Right_PD_Output[i].push_back(pos);
				   }  
				}
			}
			else {
            //#pragma omp parallel default(shared)
				{
					// #pragma omp for
				   for (int j = 0; j < SizeOfCurrent; j++) {
					   pos =  Right_PD[i][j] - 1;
					   if (TheInput[pos] == CurrentChar)
						   Right_PD_Output[i].push_back(pos);
					   else Right_PD_Output[i + 1].push_back(pos); 
				   }
				}
			}			
		}
		
		int SizeOfCurrent = Right_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].size();
		if (CurrentChar == 'N') {
			for (int j = 0; j < SizeOfCurrent; j++) {
				pos =  Right_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus][j] - 1;
				if (Match2N[(short)TheInput[pos]] == 'N')
					Right_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
			}         
		}
		else {
			for (int j = 0; j < SizeOfCurrent; j++) {
				pos =  Right_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus][j] - 1;
				if (TheInput[pos] == CurrentChar)
					Right_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
				//else Left_PD_Output[i + 1].push_back(pos); 
			}         
		}	
		
		Sum = 0;
		for (int i = 0; i <= OneRead.MAX_SNP_ERROR; i++) {
			Sum += Right_PD_Output[i].size();
		}
      if (Sum) {
         short CurrentLength_output = CurrentLength + 1;
         CheckRight_Close(OneRead, TheInput, CurrentReadSeq, Right_PD_Output,
								  BP_Right_Start, BP_Right_End,
								  CurrentLength_output, RightUP);
      }
      else return;
   }
   else return;
}

void CheckLeft_Far(SPLIT_READ & OneRead,
						 const string & TheInput, 
						 const string & CurrentReadSeq,
						 const vector <unsigned int> Left_PD[],
						 const short & BP_Left_Start,
						 const short & BP_Left_End,
						 const short & CurrentLength,
						 vector <UniquePoint> & LeftUP) {
	//if (OneRead.MatchedRelPos > 160000 && LeftUP.size())
	//cout << "+ " << TheInput.size() << "\t" << CurrentLength << "\t" << CurrentReadSeq << "\t" << Left_PD[0].size() << "\t" << Left_PD[1].size() << "\t" << Left_PD[2].size() << "\t" << BP_Left_Start << "\t" << BP_Left_End << "\t" << LeftUP.size() << endl;
	
	if (CurrentLength == 20) OneRead.Found = true;
	int Sum;
	//cout << "1" << endl;
   if (CurrentLength >= BP_Left_Start && CurrentLength <= BP_Left_End) {
      // put it to LeftUP if unique
		for (short i = 0; i <= OneRead.MAX_SNP_ERROR; i++) {
			if (Left_PD[i].size() == 1 && CurrentLength >= BP_Left_Start + i) {
				Sum = 0;
				if (ADDITIONAL_MISMATCH) 
		   		for (short j = 1; j <= ADDITIONAL_MISMATCH; j++) 
			   		Sum += Left_PD[i + j].size();
				
				if (Sum == 0 && i <= (short)(CurrentLength * Seq_Error_Rate + 1)) {
					UniquePoint TempOne;
					TempOne.LengthStr = CurrentLength;
					TempOne.AbsLoc = Left_PD[i][0]; 
					TempOne.Direction = FORWARD;
					TempOne.Strand = SENSE;
					TempOne.Mismatches = i;
					LeftUP.push_back(TempOne);	
					break;
				}
			}
		}
   }
	//cout << "2" << endl;
   if (CurrentLength < BP_Left_End) {
      vector <unsigned int> Left_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED];
		for (int CheckedIndex = 0; CheckedIndex < OneRead.TOTAL_SNP_ERROR_CHECKED; CheckedIndex++) {
			Left_PD_Output[CheckedIndex].reserve(Left_PD[CheckedIndex].size());
		}
      const char CurrentChar = CurrentReadSeq[CurrentLength];
      //const int SizeOfCurrent = Left_PD.size();
      unsigned int pos;
		//if (TOTAL_SNP_ERROR_CHECKED_Minus) 
		{
			for (int i = 0; i < OneRead.TOTAL_SNP_ERROR_CHECKED_Minus; i++) {
				int SizeOfCurrent = Left_PD[i].size();
				if (CurrentChar == 'N') {
					for (int j = 0; j < SizeOfCurrent; j++) {
						pos =  Left_PD[i][j] + 1;
						if (Match2N[(short)TheInput[pos]] == 'N')
							Left_PD_Output[i].push_back(pos);
					}         
				}
				else {
					for (int j = 0; j < SizeOfCurrent; j++) {
						pos =  Left_PD[i][j] + 1;
						if (TheInput[pos] == CurrentChar)
							Left_PD_Output[i].push_back(pos);
						else Left_PD_Output[i + 1].push_back(pos); 
					}         
				}			
			}
			
			int SizeOfCurrent = Left_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].size();
			if (CurrentChar == 'N') {
				for (int j = 0; j < SizeOfCurrent; j++) {
					pos =  Left_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus][j] + 1;
					if (Match2N[(short)TheInput[pos]] == 'N')
						Left_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
				}         
			}
			else {
				for (int j = 0; j < SizeOfCurrent; j++) {
					pos =  Left_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus][j] + 1;
					if (TheInput[pos] == CurrentChar)
						Left_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
					//else Left_PD_Output[i + 1].push_back(pos); 
				}         
			}	
			
			Sum = 0;
			for (int i = 0; i <= OneRead.MAX_SNP_ERROR; i++) {
				Sum += Left_PD_Output[i].size();
			}
			//cout << "Sum: " << Sum << endl;
			
			if (Sum) {
				const short CurrentLengthOutput = CurrentLength + 1;
				CheckLeft_Far(OneRead,TheInput, CurrentReadSeq, Left_PD_Output,
								  BP_Left_Start, BP_Left_End,
								  CurrentLengthOutput, LeftUP); 
				//cout << "end0" << endl;
				return;
			}
			else {
				//cout << "end1" << endl;
			   return;	
			}
		}

   }   
   else {
		//cout << "end2" << endl;
		return;
	}
}

void CheckRight_Far(SPLIT_READ & OneRead,
						  const string & TheInput,
						  const string & CurrentReadSeq,
						  const vector <unsigned int> Right_PD[],
						  const short & BP_Right_Start,
						  const short & BP_Right_End,
						  const short & CurrentLength,
						  vector <UniquePoint> & RightUP) {
	//if (OneRead.MatchedRelPos > 160000 && RightUP.size())
		//cout << "- " << TheInput.size() << "\t" << CurrentLength << "\t" << CurrentReadSeq << "\t" << Right_PD[0].size() << "\t" << Right_PD[1].size() << "\t" << Right_PD[2].size() << "\t" << BP_Right_Start << "\t" << BP_Right_End << "\t" << RightUP.size() << endl;
	if (CurrentLength == 20) OneRead.Found = true;
   short ReadLengthMinus = CurrentReadSeq.size() - 1;      
	int Sum;
   if (CurrentLength >= BP_Right_Start && CurrentLength <= BP_Right_End) {
	   for (short i = 0; i <= OneRead.MAX_SNP_ERROR; i++) {
		   if (Right_PD[i].size() == 1 && CurrentLength >= BP_Right_Start + i) {
				Sum = 0;
				if (ADDITIONAL_MISMATCH) 
					for (short j = 1; j <= ADDITIONAL_MISMATCH; j++) 
						Sum += Right_PD[i + j].size();
				
				if (Sum == 0 && i <= (short)(CurrentLength * Seq_Error_Rate + 1)) {
					UniquePoint TempOne;
					TempOne.LengthStr = CurrentLength;
					TempOne.AbsLoc = Right_PD[i][0];   
					TempOne.Direction = BACKWARD;
					TempOne.Strand = ANTISENSE;
					TempOne.Mismatches = i;
					RightUP.push_back(TempOne);	
					break;
				}
			}
		}
   }
	
   if (CurrentLength < BP_Right_End) {
      vector <unsigned int> Right_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED];
		for (int CheckedIndex = 0; CheckedIndex < OneRead.TOTAL_SNP_ERROR_CHECKED; CheckedIndex++) {
			Right_PD_Output[CheckedIndex].reserve(Right_PD[CheckedIndex].size());
		}
      const char CurrentChar = CurrentReadSeq[ReadLengthMinus - CurrentLength];
      unsigned int pos;
		
		//if (TOTAL_SNP_ERROR_CHECKED_Minus) 
		{
			for (int i = 0; i < OneRead.TOTAL_SNP_ERROR_CHECKED_Minus; i++) {
				int SizeOfCurrent = Right_PD[i].size();
				if (CurrentChar == 'N') {
					for (int j = 0; j < SizeOfCurrent; j++) {
						pos =  Right_PD[i][j] - 1;
						if (Match2N[(short)TheInput[pos]] == 'N')
							Right_PD_Output[i].push_back(pos);
					}         
				}
				else {
					for (int j = 0; j < SizeOfCurrent; j++) {
						pos =  Right_PD[i][j] - 1;
						if (TheInput[pos] == CurrentChar)
							Right_PD_Output[i].push_back(pos);
						else Right_PD_Output[i + 1].push_back(pos); 
					}         
				}			
			}
			
			int SizeOfCurrent = Right_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].size();
			if (CurrentChar == 'N') {
				for (int j = 0; j < SizeOfCurrent; j++) {
					pos =  Right_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus][j] - 1;
					if (Match2N[(short)TheInput[pos]] == 'N')
						Right_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
				}         
			}
			else {
				for (int j = 0; j < SizeOfCurrent; j++) {
					pos =  Right_PD[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus][j] - 1;
					if (TheInput[pos] == CurrentChar)
						Right_PD_Output[OneRead.TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
					//else Left_PD_Output[i + 1].push_back(pos); 
				}         
			}	
			
			Sum = 0;
			for (int i = 0; i <= OneRead.MAX_SNP_ERROR; i++) {
				Sum += Right_PD_Output[i].size();
			}
			if (Sum) {
				short CurrentLength_output = CurrentLength + 1;
				CheckRight_Far(OneRead, TheInput, CurrentReadSeq, Right_PD_Output,
									BP_Right_Start, BP_Right_End,
									CurrentLength_output, RightUP);
			}
			else return;
		}
		/*
		 else { // TOTAL_SNP_ERROR_CHECKED_Minus
		 
		 int SizeOfCurrent = Right_PD[TOTAL_SNP_ERROR_CHECKED_Minus].size();
		 if (CurrentChar == 'N') {
		 for (int j = 0; j < SizeOfCurrent; j++) {
		 pos =  Right_PD[TOTAL_SNP_ERROR_CHECKED_Minus][j] - 1;
		 if (Match2N[(short)TheInput[pos]] == 'N')
		 Right_PD_Output[TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
		 }         
		 }
		 else {
		 for (int j = 0; j < SizeOfCurrent; j++) {
		 pos =  Right_PD[TOTAL_SNP_ERROR_CHECKED_Minus][j] - 1;
		 if (TheInput[pos] == CurrentChar)
		 Right_PD_Output[TOTAL_SNP_ERROR_CHECKED_Minus].push_back(pos);
		 //else Left_PD_Output[i + 1].push_back(pos); 
		 }         
		 }	
		 
		 if (!Right_PD_Output[TOTAL_SNP_ERROR_CHECKED_Minus].empty()) {
		 short CurrentLength_output = CurrentLength + 1;
		 CheckRight_Far(TheInput, CurrentReadSeq, Right_PD_Output,
		 BP_Right_Start, BP_Right_End,
		 CurrentLength_output, RightUP);
		 }
		 else return;
		 }
		 */
   }
   else return;
}


short CompareTwoReads(const SPLIT_READ & First, const SPLIT_READ & Second) {
   //if (First.MatchedSeqID < Second.MatchedSeqID) return 0;
   //else if (First.MatchedSeqID > Second.MatchedSeqID) return 1;
   short FirstDISize = First.NT_str.size();
   short SecondDISize = Second.NT_str.size();
   short FirstSISize = First.InsertedStr.size();
   short SecondSISize = Second.InsertedStr.size();
	
   if (First.BPLeft > Second.BPLeft) return 1;
   else if (First.BPLeft < Second.BPLeft) return 0;
   else {
      {
         if (First.IndelSize > Second.IndelSize) return 1; 
         else if (First.IndelSize < Second.IndelSize) return 0;
         else
         {
            if (First.Tag.size() < Second.Tag.size()) return 0;
            else if (First.Tag.size() > Second.Tag.size()) return 1;
            else if (First.Tag.size() == Second.Tag.size()) {
               for (unsigned posindex = 0; posindex < First.Tag.size(); posindex++) {
                  if ((short)First.Tag[posindex] < (short)Second.Tag[posindex]) return 0;
                  else if ((short)First.Tag[posindex] > (short)Second.Tag[posindex]) return 1; 
               }
               if (First.MatchedRelPos < Second.MatchedRelPos)  return 0;
               else if (First.MatchedRelPos > Second.MatchedRelPos) return 1;
               else {
                  if (FirstDISize > SecondDISize) return 1;
						else if (FirstDISize < SecondDISize) return 0;
						else if (FirstDISize != 0) {
                     for (int i = 0; i < FirstDISize; i++) 
                        if ((short)First.NT_str[i] > (short)Second.NT_str[i]) return 1;
                        else if ((short)First.NT_str[i] < (short)Second.NT_str[i]) return 0;
                  }
                  else {
                     if (FirstSISize > SecondSISize) return 1;
                     else if (FirstSISize < SecondSISize) return 0;
                     else if (FirstSISize != 0) {
                        for (int i = 0; i < FirstSISize; i++) 
                           if ((short)First.InsertedStr[i] > (short)Second.InsertedStr[i]) return 1;
                           else if ((short)First.InsertedStr[i] < (short)Second.InsertedStr[i]) return 0;
                     } 
                     else return 2; 
                  } 
               }
               //return 2;                     
            }   
         }    
      } 
   } 
	return 0;
}

bool CheckMismatches(const string & TheInput, 
                     const string & InputReadSeq, 
                     //const unsigned int & Start,
							const UniquePoint & UP
							) {
	//return true; 	short LengthStr;
	//unsigned int AbsLoc; 
	//cout << "CheckMismatches1" << endl;
	string CurrentReadSeq;
	if (UP.Strand == SENSE) CurrentReadSeq = InputReadSeq;
	else CurrentReadSeq = ReverseComplement(InputReadSeq);
	short CurrentReadLength = CurrentReadSeq.size();
	unsigned int Start = 0;
	//cout << "CheckMismatches2" << endl;
	string BP_On_Read, BP_On_Ref;
	if (UP.Direction == FORWARD) {
		//cout << "+s" << endl;
		//cout << "UP.AbsLoc: " << UP.AbsLoc << "\t" << "UP.LengthStr: " << UP.LengthStr << endl;
		
		Start = UP.AbsLoc - UP.LengthStr + 1; 
		//cout << "Start: " << Start << endl;
		//cout << "Min_Perfect_Match_Around_BP: " << Min_Perfect_Match_Around_BP << endl;
		if (UP.LengthStr <= Min_Perfect_Match_Around_BP) return false;
		BP_On_Read = CurrentReadSeq.substr(UP.LengthStr - Min_Perfect_Match_Around_BP, Min_Perfect_Match_Around_BP);
		//cout << "BP_On_Read: " << BP_On_Read << endl;
		//if (UP.AbsLoc < Min_Perfect_Match_Around_BP) return false;
		BP_On_Ref = TheInput.substr(UP.AbsLoc - Min_Perfect_Match_Around_BP + 1, Min_Perfect_Match_Around_BP);
		//cout << "BP_On_Ref: " << BP_On_Ref << endl;
		if (BP_On_Read != BP_On_Ref) return false;
		//cout << "+e" << endl;
	}
	else if (UP.Direction == BACKWARD) {
		//cout << "-s" << endl;
		Start = UP.AbsLoc + UP.LengthStr - CurrentReadLength;
		if (CurrentReadLength < UP.LengthStr) return false;
		BP_On_Read = CurrentReadSeq.substr(CurrentReadLength - UP.LengthStr, Min_Perfect_Match_Around_BP);
		BP_On_Ref = TheInput.substr(UP.AbsLoc, Min_Perfect_Match_Around_BP);
		if (BP_On_Read != BP_On_Ref) return false;
		//cout << "-e" << endl;
	}
	//cout << "CheckMismatches3" << endl;
	short MAX_ALLOWED_MISMATCHES = (short)(CurrentReadSeq.size() * MaximumAllowedMismatchRate + 1); // 
	
   short NumMismatches = 0;                 // Match2N[(short)'A'] = 'N';    
   
   for (short i = 0; i < CurrentReadLength; i++) {
      if (CurrentReadSeq[i] == N_char) {
         if (Match2N[(short)TheInput[Start + i]] != N_char)  
            NumMismatches++;
      }
      else {
         if (TheInput[Start + i] != CurrentReadSeq[i])
            NumMismatches++;
      }   
   }   
	//cout << "CheckMismatches4" << endl;
   if (NumMismatches > MAX_ALLOWED_MISMATCHES) return true;
   else return false;                     
}

