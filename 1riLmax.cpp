#include <time.h>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <process.h>

// Includes related to the project
#include "donneesSchrage2.h"
#include "1riLmax.h"
//#include "pyramides.h"

#define ThreadNumber 1
#define TimeLimit 900

#define UbOpt false
// Si false: La upper bound est égale à la valeur retournée par Schrage (ou Schrage+RL le cas échéant)
// Si true: La upper bound est égale à la valeur optimale du MIP (le MIP doit être exécuté avant PreMIP)

#define DEBUG false
// Si false: Exécuter le programme sans voir les commentaires de status
// Si true: Exécuter le programme avec les commentaires de status et logs de debug

/**********************************************************************************
Starting variable declaration
**********************************************************************************/

unsigned int puiSchrageBase[MaxJobs1];
unsigned int puiBestSchrage[MaxJobs1];

unsigned long cptLeftInsert=0;
unsigned long cptRightInsert=0;
unsigned long cptLeftSwap=0;

unsigned long amLeftInsert=0;
unsigned long amRightInsert=0;
unsigned long amLeftSwap=0;

/*********************************************************************************
Ending of variable declaration
*********************************************************************************/


using namespace std;

void AddInERDOrder(unsigned int puiList[MaxJobs1], unsigned int *uiSize,unsigned int uiJob);
void AddInEDDOrder(unsigned int puiList[MaxJobs1], unsigned int *uiSize,unsigned int uiJob);
bool isSolOptimal(unsigned int *uiSeq);



/***********************************************************************************
diffclock - Returns the time between two time points (in seconds)
***********************************************************************************/

double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffs=diffticks/(double)CLOCKS_PER_SEC;
	return diffs;
}

/***********************************************************************************
isSolOptimal- Applies the test of Carlier to detect if a feasible schedule is optimal or not
***********************************************************************************/
bool isSolOptimal(unsigned int *uiSeq)
{ 
 unsigned int uiLoop,uiCj,uiPosLmax,uiCjLmax,uiStartTime[10000],uiMinri,uiSumPi,uiMaxdi;
 signed int siLmax,siLoop2;
 bool isOpt=false;
 unsigned int uiTabPos[10000],uiTabCj[10000],uiSizePos,uiSizeCj;


 // We first look for the jobs giving the Lmax value
 uiSizePos=uiSizeCj=0;
 uiCj=riS(uiSeq[0])+piS(uiSeq[0]);
 uiStartTime[uiSeq[0]]=riS(uiSeq[0]);
 siLmax=(signed int)uiCj-(signed int)diS(uiSeq[0]);
 uiPosLmax=0;
 uiCjLmax=uiCj;
 uiTabPos[0]=uiPosLmax;
 uiTabCj[0]=uiCjLmax;
 uiSizePos=uiSizeCj=1;
 for(uiLoop=1;uiLoop<NbJobs1;uiLoop++)
 {
	 if (riS(uiSeq[uiLoop])>uiCj) uiCj=riS(uiSeq[uiLoop]);
	 uiStartTime[uiSeq[uiLoop]-1]=uiCj;
	 uiCj+=piS(uiSeq[uiLoop]);
	 if (((signed int)uiCj-(signed int)diS(uiSeq[uiLoop]))>siLmax) 
		 {
			 siLmax=(signed int)uiCj-(signed int)diS(uiSeq[uiLoop]);
			 uiPosLmax=uiLoop;
			 uiCjLmax=uiCj;
			 uiTabPos[0]=uiPosLmax;
			 uiTabCj[0]=uiCjLmax;
			 uiSizePos=uiSizeCj=1;
	 } else if (((signed int)uiCj-(signed int)diS(uiSeq[uiLoop]))==siLmax) 
	 {
			 uiTabPos[uiSizePos]=uiLoop;
			 uiTabCj[uiSizeCj]=uiCj;
			 uiSizePos++;
			 uiSizeCj++;
	 }
 }

 //printf("Number of Lmax jobs=%ld\n",uiSizePos);
 //printf("Best Lmax=%lf / Calculated Lmax=%ld\n",BestLmax,(unsigned int)siLmax);
 //printf("(Position,Cj) of the job giving the Lmax=(%ld,%ld)\n",uiPosLmax,uiCjLmax);

 // We are now looking backward to determine whether all jobs of the block of uiPosLmax are in EDD order or not
 //if (uiPosLmax==0) isOpt=true;
 if (uiTabPos[0]==0) isOpt=true;
 else {
	for(unsigned int uiLoop3=0;uiLoop3<uiSizePos && !isOpt;uiLoop3++)
	{
	 //siLoop2=uiPosLmax-1;
	 siLoop2=uiTabPos[uiLoop3]-1;
	 //printf("\t ri=%ld, pi=%ld, di=%ld, Ci=%ld, start=%ld\n",ri[uiSeq[uiPosLmax]-1],pi[uiSeq[uiPosLmax]-1],di[uiSeq[uiPosLmax]-1],uiCjLmax,uiStartTime[uiSeq[uiPosLmax]-1]);
	 //uiMinri=ri[uiSeq[uiPosLmax]-1];
	 uiMinri=riS(uiSeq[uiTabPos[uiLoop3]]);
	 //uiCj=uiCjLmax-pi[uiSeq[uiPosLmax]-1];
	 uiCj=uiTabCj[uiLoop3]-piS(uiSeq[uiTabPos[uiLoop3]]);
	 //uiSumPi=pi[uiSeq[uiPosLmax]-1];
	 //uiMaxdi=di[uiSeq[uiPosLmax]-1];
	 uiSumPi=piS(uiSeq[uiTabPos[uiLoop3]]);
	 uiMaxdi=diS(uiSeq[uiTabPos[uiLoop3]]);
	 //while ((siLoop2>=0)&&(	(uiStartTime[uiSeq[siLoop2]-1]+pi[uiSeq[siLoop2]-1])==uiCj)&&(di[uiSeq[siLoop2]-1]<=di[uiSeq[uiPosLmax]-1]))
	 //while ((siLoop2>=0)&&((uiStartTime[uiSeq[siLoop2]-1]+pi[uiSeq[siLoop2]-1])==uiCj)&&((((signed int)uiMinri+(signed int)uiSumPi)-(signed int)uiMaxdi)==siLmax))
	 while ((siLoop2>=0)&&((uiStartTime[uiSeq[siLoop2]-1]+piS(uiSeq[siLoop2]))==uiCj))
	 {
	     //printf("\t ri=%ld, pi=%ld, di=%ld, Ci=%ld, start=%ld\n",ri[uiSeq[siLoop2]-1],pi[uiSeq[siLoop2]-1],di[uiSeq[siLoop2]-1],uiCj,uiStartTime[uiSeq[siLoop2]-1]);
		 uiCj=uiStartTime[uiSeq[siLoop2]-1];
		 if (riS(uiSeq[siLoop2])<uiMinri) uiMinri=riS(uiSeq[siLoop2]);
		 if (diS(uiSeq[siLoop2])>uiMaxdi) uiMaxdi=diS(uiSeq[siLoop2]);
		 uiSumPi+=piS(uiSeq[siLoop2]);
		 //printf("\t\t sumpi=%ld\n",uiSumPi);
		 siLoop2--;
	 }
	// printf("\t (loop=%d) : ri=%ld, pi=%ld, di=%ld, Ci=%ld, start=%ld\n",siLoop2,ri[uiSeq[siLoop2]-1],pi[uiSeq[siLoop2]-1],di[uiSeq[siLoop2]-1],uiStartTime[uiSeq[siLoop2]-1]+pi[uiSeq[siLoop2]-1],uiStartTime[uiSeq[siLoop2]-1]);
	 //printf("Lmax=%d / Minri=%ld / SumPi=%ld / MaxDi=%ld => (ri+pi-di)=%d\n",siLmax,uiMinri,uiSumPi,uiMaxdi,(signed int)uiMinri+(signed int)uiSumPi-(signed int)uiMaxdi);
	 //getch();
	 //if ((siLoop2==-1)||(((uiStartTime[uiSeq[siLoop2]-1]+pi[uiSeq[siLoop2]-1])!=uiCj)))
	 //if ((siLoop2==-1)||(((uiStartTime[uiSeq[siLoop2]-1]+pi[uiSeq[siLoop2]-1])!=uiCj)&&(uiMinri>=ri[uiSeq[siLoop2+1]-1])))
	 //if ((siLoop2==-1)||((((signed int)uiMinri+(signed int)uiSumPi)-(signed int)uiMaxdi)==siLmax))
	 if ((((signed int)uiMinri+(signed int)uiSumPi)-(signed int)uiMaxdi)==siLmax)
		 isOpt=true;
	}
 }

 return isOpt;
}

/***********************************************************************************
LeftInsert- Insert the critical job (given in input), in a position to its left
               trying to improe the Lmax
***********************************************************************************/

void LeftInsert(unsigned int * puiSchrage, double *pdMargin, unsigned int *puiStart,unsigned int uiStartPos, double dLmaxRef, double * pdNewLmax, unsigned int *pPos)
{
	unsigned int uiLoop, uiBestPos, uiLoop2, uiJob;
	double dCmax, dLmaxLoc, dLmaxBest;

	dLmaxBest=dLmaxRef;
	(*pdNewLmax)=dLmaxRef;
	for (uiLoop=uiStartPos-1;uiLoop>=0 && pdMargin[uiLoop]>=piS(puiSchrage[uiStartPos]) && (puiStart[uiLoop]>riS(puiSchrage[uiStartPos]) || (puiStart[uiLoop]>riS(puiSchrage[uiStartPos]) && puiStart[uiLoop+1]>riS(puiSchrage[uiStartPos])));uiLoop--)
	{ // We scan all positions on the left
		dLmaxLoc=-999999.0;
		if (uiLoop==0)
			dCmax=riS(puiSchrage[0]);
		else dCmax=puiStart[uiLoop-1]+piS(puiSchrage[uiLoop-1]);

		// We now schedule previously in position uiStartPos
		if (dCmax<riS(puiSchrage[uiStartPos]))
				dCmax=(double) riS(puiSchrage[uiStartPos])+ (double) piS(puiSchrage[uiStartPos]);
		else dCmax+=(double)piS(puiSchrage[uiStartPos]);
		dLmaxLoc=dCmax-(double) diS(puiSchrage[uiStartPos]);
		// We now schedule the other jobs up to the old position uiStartPos
	    for (uiLoop2=uiLoop;uiLoop2<uiStartPos;uiLoop2++)
		{
			if (dCmax<riS(puiSchrage[uiLoop2]))
				dCmax=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
			else dCmax+=piS(puiSchrage[uiLoop2]);
			if ((dCmax-(double)diS(puiSchrage[uiLoop2]))>dLmaxLoc) dLmaxLoc=(dCmax-(double)diS(puiSchrage[uiLoop2]));
		}

		// We check if the local Lmax improves upon dLmaxRef
		if (dLmaxLoc<dLmaxBest)
		{
			dLmaxBest=dLmaxLoc;
			uiBestPos=uiLoop;
		}
	}

	// We now update the puiSchrage schedule if necessary
	if (dLmaxBest<dLmaxRef)
	{
		uiJob=puiSchrage[uiStartPos];
		for (uiLoop=uiStartPos;uiLoop>uiBestPos;uiLoop--)
			puiSchrage[uiLoop]=puiSchrage[uiLoop-1];
		puiSchrage[uiBestPos]=uiJob;
		(*pdNewLmax)=dLmaxBest;
		(*pPos)=uiBestPos;
	}
}

/***********************************************************************************
LeftSwap- Swap the critical job with a job located to its left in the schedule
***********************************************************************************/

void LeftSwap(unsigned int * puiSchrage, double *pdMargin, unsigned int *puiStart,unsigned int uiStartPos, double dLmaxRef, double * pdNewLmax, unsigned int *pPos)
{
	unsigned int  uiBestPos, uiLoop2, uiJob;
	signed int uiLoop;
	double dCmax, dLmaxLoc, dLmaxBest;

	dLmaxBest=dLmaxRef;
	(*pdNewLmax)=dLmaxRef;
	for (uiLoop=uiStartPos-1;uiLoop>=0 && (puiStart[uiLoop]>riS(puiSchrage[uiStartPos]) || (puiStart[uiLoop]>riS(puiSchrage[uiStartPos]) && puiStart[uiLoop+1]>riS(puiSchrage[uiStartPos])));uiLoop--)
	if (diS(puiSchrage[uiLoop])>diS(puiSchrage[uiStartPos]))
	{ // We try to do a swap with the job into position uiLoop
		dLmaxLoc=-999999.0;
		if (uiLoop==0)
			dCmax=riS(puiSchrage[0]);
		else dCmax=puiStart[uiLoop-1]+piS(puiSchrage[uiLoop-1]);

		// We now schedule previously in position uiStartPos
		if (dCmax<riS(puiSchrage[uiStartPos]))
				dCmax=(double) riS(puiSchrage[uiStartPos])+ (double) piS(puiSchrage[uiStartPos]);
		else dCmax+=(double)piS(puiSchrage[uiStartPos]);
		dLmaxLoc=dCmax-(double) diS(puiSchrage[uiStartPos]);
		// We now schedule the other jobs up to the old position uiStartPos (except the job in position uiLoop)
	    for (uiLoop2=uiLoop+1;uiLoop2<uiStartPos;uiLoop2++)
		{
			if (dCmax<riS(puiSchrage[uiLoop2]))
				dCmax=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
			else dCmax+=piS(puiSchrage[uiLoop2]);
			if ((dCmax-(double)diS(puiSchrage[uiLoop2]))>dLmaxLoc) dLmaxLoc=(dCmax-(double)diS(puiSchrage[uiLoop2]));
		}
		// We now schedule the job previously in position uiLoop
		if (dCmax<riS(puiSchrage[uiLoop]))
				dCmax=riS(puiSchrage[uiLoop])+piS(puiSchrage[uiLoop]);
			else dCmax+=piS(puiSchrage[uiLoop]);
			if ((dCmax-(double)diS(puiSchrage[uiLoop]))>dLmaxLoc) dLmaxLoc=(dCmax-(double)diS(puiSchrage[uiLoop]));

		// We now propagate the schedule up to the end of the block
		uiLoop2=uiStartPos+1;
		while (uiLoop2<NbJobs1 && dCmax>=riS(puiSchrage[uiLoop2]))
		{
			if (dCmax<riS(puiSchrage[uiLoop2]))
				dCmax=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
			else dCmax+=piS(puiSchrage[uiLoop2]);
			if ((dCmax-(double)diS(puiSchrage[uiLoop2]))>dLmaxLoc) dLmaxLoc=(dCmax-(double)diS(puiSchrage[uiLoop2]));
			uiLoop2++;
		}

		// We check if the local Lmax improves upon dLmaxRef
		if (dLmaxLoc<dLmaxBest)
		{
			dLmaxBest=dLmaxLoc;
			uiBestPos=uiLoop;
		}
	}

	// We now update the puiSchrage schedule if necessary
	if (dLmaxBest<dLmaxRef)
	{
		uiJob=puiSchrage[uiStartPos];
		puiSchrage[uiStartPos]=puiSchrage[uiBestPos];
		puiSchrage[uiBestPos]=uiJob;
		(*pdNewLmax)=dLmaxBest;
		(*pPos)=uiBestPos;
	}	
}

/***********************************************************************************
RightInsert- Insert a job before the critical job (given in input), in a position to 
               the right of that of the critical job, trying to improe the Lmax
***********************************************************************************/

void RightInsert(unsigned int * puiSchrage, double *pdMargin, unsigned int *puiStart,unsigned int uiStartPos, double dLmaxRef, double * pdNewLmax, unsigned int *pPos)
{
	unsigned int uiBestPos, uiLoop2, uiLoop3, uiJob, uiBestInsertPos;
	signed int uiLoop;
	double dCmax, dCmaxBis, dLmaxLocBis,dLmaxLoc, dLmaxBest;

	dLmaxBest=dLmaxRef;
	(*pdNewLmax)=dLmaxRef;
	for (uiLoop=uiStartPos-1;uiLoop>=0 && (riS(puiSchrage[uiLoop])<puiStart[uiLoop] || (riS(puiSchrage[uiLoop])==puiStart[uiLoop] && uiLoop>0 && puiStart[uiLoop-1]+piS(puiSchrage[uiLoop-1])==puiStart[uiLoop]));uiLoop--)
	if (diS(puiSchrage[uiLoop])>diS(puiSchrage[uiStartPos]))
	{ // We try to right insert the current job uiLoop to a position on the right of StartPos
		dLmaxLoc=-999999.0;
		if (uiLoop==0)
			dCmax=riS(puiSchrage[1]);
		else dCmax=puiStart[uiLoop];
		for (uiLoop2=uiLoop+1;uiLoop2<=uiStartPos;uiLoop2++)
		{ // We schedule the jobs between uiLoop and uiStartPos
			if (dCmax<riS(puiSchrage[uiLoop2]))
				dCmax=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
			else dCmax+=piS(puiSchrage[uiLoop2]);
			if ((dCmax-(double)diS(puiSchrage[uiLoop2]))>dLmaxLoc) dLmaxLoc=(dCmax-(double)diS(puiSchrage[uiLoop2]));
		}
		dLmaxLocBis=dLmaxLoc;
		for (uiLoop3=uiStartPos+1;uiLoop3<NbJobs1 && dLmaxLocBis<=dLmaxBest;uiLoop3++)
		{ // uiLoop3 is the position of insertion in Schrage's schedule
			dCmaxBis=dCmax;
			dLmaxLocBis=dLmaxLoc;
			for (uiLoop2=uiStartPos+1;uiLoop2<uiLoop3;uiLoop2++)
			{
			 if (dCmaxBis<riS(puiSchrage[uiLoop2]))
				dCmaxBis=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
			 else dCmaxBis+=piS(puiSchrage[uiLoop2]);
			 if ((dCmaxBis-(double)diS(puiSchrage[uiLoop2]))>dLmaxLocBis) dLmaxLocBis=(dCmaxBis-(double)diS(puiSchrage[uiLoop2]));
			}
			// We insert now the job in the current position
			if (dCmaxBis<riS(puiSchrage[uiLoop]))
				dCmaxBis=(double) riS(puiSchrage[uiLoop])+ (double) piS(puiSchrage[uiLoop]);
		    else dCmaxBis+=(double)piS(puiSchrage[uiLoop]);
		    if ((dCmaxBis-(double)diS(puiSchrage[uiLoop]))>dLmaxLocBis) dLmaxLocBis=dCmaxBis-(double) diS(puiSchrage[uiLoop]);
			// We now continue with the other jobs in the schedule
			for (uiLoop2=uiLoop3;uiLoop2<NbJobs1;uiLoop2++)
			{
			 if (dCmaxBis<riS(puiSchrage[uiLoop2]))
				dCmaxBis=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
			 else dCmaxBis+=piS(puiSchrage[uiLoop2]);
			 if ((dCmaxBis-(double)diS(puiSchrage[uiLoop2]))>dLmaxLocBis) dLmaxLocBis=(dCmaxBis-(double)diS(puiSchrage[uiLoop2]));
			}
			// We now check wether we get a lower Lmax value or not
		    if (dLmaxLocBis<dLmaxBest)
		    {
			  dLmaxBest=dLmaxLocBis;
			  uiBestPos=uiLoop;
			  uiBestInsertPos=uiLoop3;
		    }
		}
	}

	// We now update the puiSchrage schedule if necessary
	if (dLmaxBest<dLmaxRef)
	{
		uiJob=puiSchrage[uiBestPos];
		for (uiLoop=uiBestPos;uiLoop<uiBestInsertPos-1;uiLoop++)
			puiSchrage[uiLoop]=puiSchrage[uiLoop+1];
		puiSchrage[uiBestInsertPos-1]=uiJob;
		(*pdNewLmax)=dLmaxBest;
		(*pPos)=uiBestPos;
	}
}

/***********************************************************************************
ComputeLmax- Compute the Lmax value of a given schedule
***********************************************************************************/

double ComputeLmax(unsigned int *puiSchedule)
{
	unsigned int uiLoop;
	double dCmax,dLmax;

	dCmax=0;
	dLmax=-999999.0;
	for (uiLoop=0;uiLoop<NbJobs1;uiLoop++)
	{
		if (dCmax<riS(puiSchedule[uiLoop]))
			dCmax=riS(puiSchedule[uiLoop])+piS(puiSchedule[uiLoop]);
		else dCmax+=piS(puiSchedule[uiLoop]);

		if (dLmax<(dCmax-diS(puiSchedule[uiLoop])))
			dLmax=(dCmax-diS(puiSchedule[uiLoop]));
	}
	return dLmax;
}


/***********************************************************************************
SchrageFJ  - Apply Schrage on a partial schedule and we fix a job in a position
             Note: we assume that jobs in puiLoJobs are sorted by increasing value of ri
			       also, uiJob does not belong to puiLoJobs
***********************************************************************************/

double SchrageFJ(unsigned int uiInsertPos, unsigned int uiJob, unsigned int uiDate, unsigned int *puiLoJobs, unsigned int uiNbLoJobs,unsigned int *puiSchedule, unsigned int *puiStartTime)
{
	unsigned int uiLoop,uiLoop2;
	unsigned int uiSize,uiNbAvJobs,puiAvailJobs[MaxJobs1];
	double dLmax=-9999999.0;
	
	uiLoop=0;
	dLmax=-9999999.0;
	uiNbAvJobs=0;
	uiSize=0;
	while (uiLoop<uiNbLoJobs)
	{ // There are still unscheduled jobs to schedule
		if (uiSize!=uiInsertPos)
		{ // We will schedule an available job
		 if (riS(puiLoJobs[uiLoop])>uiDate && uiNbAvJobs==0)
			uiDate=riS(puiLoJobs[uiLoop]);
		 // We now retrieve the available jobs
		 while (uiLoop<uiNbLoJobs && riS(puiLoJobs[uiLoop])<=uiDate)
		 { // We add the current job into the list of available jobs
			AddInEDDOrder(puiAvailJobs,&uiNbAvJobs,puiLoJobs[uiLoop]);
			uiLoop++;
		 }
		 // We schedule the first job of list puiAvailJobs
		 uiDate+=piS(puiAvailJobs[0]);
		 if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		 puiSchedule[uiSize]=puiAvailJobs[0];
		 puiStartTime[uiSize]=uiDate-piS(puiAvailJobs[0]);
		 uiSize++;
		 for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		 uiNbAvJobs--;
		} else
		{ // We currently have to schedule the imposed job
			if (riS(uiJob)>uiDate) uiDate=riS(uiJob)+piS(uiJob);
			else uiDate+=piS(uiJob);
			if (((double)uiDate-(double)diS(uiJob))>dLmax) dLmax=((double)uiDate-(double)diS(uiJob));
			puiSchedule[uiSize]=uiJob;
		    puiStartTime[uiSize]=uiDate-piS(uiJob);
			uiSize++;
		}
	}

	// We now only need to schedule jobs which are available
	while (uiNbAvJobs>0)
	{
		if (uiSize!=uiInsertPos)
		{ // We will schedule an available job
		 // We schedule the first job of list puiAvailJobs
		 uiDate+=piS(puiAvailJobs[0]);
		 if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		 puiSchedule[uiSize]=puiAvailJobs[0];
		 puiStartTime[uiSize]=uiDate-piS(puiAvailJobs[0]);
		 uiSize++;
		 for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		 uiNbAvJobs--;
		} else
		{// We currently have to schedule the imposed job
			if (riS(uiJob)>uiDate) uiDate=riS(uiJob)+piS(uiJob);
			else uiDate+=piS(uiJob);
			if (((double)uiDate-(double)diS(uiJob))>dLmax) dLmax=((double)uiDate-(double)diS(uiJob));
			puiSchedule[uiSize]=uiJob;
		    puiStartTime[uiSize]=uiDate-piS(uiJob);
			uiSize++;
		}
	}
	return dLmax;
}

/***********************************************************************************
SchrageFoJP  - Apply Schrage on a partial schedule and we forbid a job in a position
             Note: we assume that jobs in puiLoJobs are sorted by increasing value of ri
			       also, uiJob BELONGS to puiLoJobs
***********************************************************************************/

double SchrageFoJP(unsigned int uiForbidenPos, unsigned int uiJob, unsigned int uiDate, unsigned int *puiLoJobs, unsigned int uiNbLoJobs,unsigned int *puiSchedule, unsigned int *puiStartTime)
{
	unsigned int uiLoop,uiLoop2;
	unsigned int uiSize,uiNbAvJobs,puiAvailJobs[MaxJobs1];
	double dLmax=-9999999.0;
	
	uiLoop=0;
	dLmax=-9999999.0;
	uiNbAvJobs=0;
	uiSize=0;
	while (uiLoop<uiNbLoJobs)
	{ // There are still unscheduled jobs to schedule
		if (riS(puiLoJobs[uiLoop])>uiDate && uiNbAvJobs==0)
			uiDate=riS(puiLoJobs[uiLoop]);
		// We now retrieve the available jobs
		while (uiLoop<uiNbLoJobs && riS(puiLoJobs[uiLoop])<=uiDate)
		 { // We add the current job into the list of available jobs
			AddInEDDOrder(puiAvailJobs,&uiNbAvJobs,puiLoJobs[uiLoop]);
			uiLoop++;
		 }
		 // We schedule the first job of list puiAvailJobs
		if (uiSize!=uiForbidenPos || uiJob!=puiAvailJobs[0])
		{ // We will schedule an available job
		 uiDate+=piS(puiAvailJobs[0]);
		 if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		 puiSchedule[uiSize]=puiAvailJobs[0];
		 puiStartTime[uiSize]=uiDate-piS(puiAvailJobs[0]);
		 uiSize++;
		 for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		 uiNbAvJobs--;
		} else
		{ // We are expecting to schedule uiJob to uiForbidenPos which is prohibited, so we schedule the next avalaible job (if possible)
		  if (uiNbAvJobs==1 && uiLoop<uiNbLoJobs)
		  {
		    if (riS(puiLoJobs[uiLoop])>uiDate) uiDate=riS(puiLoJobs[uiLoop]);
		    // We now retrieve the available jobs
		    while (uiLoop<uiNbLoJobs && riS(puiLoJobs[uiLoop])<=uiDate)
		    { // We add the current job into the list of available jobs
			 AddInEDDOrder(puiAvailJobs,&uiNbAvJobs,puiLoJobs[uiLoop]);
			 uiLoop++;
		    }
		  }
		  // We now check which job we need to schedule
		  if (uiNbAvJobs==1 || puiAvailJobs[0]!=uiJob)
			{ // We schedule puiAvailJobs[0]
		     uiDate+=piS(puiAvailJobs[0]);
		     if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		     puiSchedule[uiSize]=puiAvailJobs[0];
		     puiStartTime[uiSize]=uiDate-piS(puiAvailJobs[0]);
		     uiSize++;
		     for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		     uiNbAvJobs--;
			}else
			{ // We schedule puiAvailJobs[1]
		     uiDate+=piS(puiAvailJobs[1]);
		     if (((double)uiDate-(double)diS(puiAvailJobs[1]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[1]));
		     puiSchedule[uiSize]=puiAvailJobs[1];
		     puiStartTime[uiSize]=uiDate-piS(puiAvailJobs[1]);
		     uiSize++;
		     for (uiLoop2=1;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		     uiNbAvJobs--;
			}
		  }
		}
	
	// We now only need to schedule jobs which are available
	while (uiNbAvJobs>0)
	{
		if (uiSize!=uiForbidenPos || uiJob!=puiAvailJobs[0] || (uiSize==uiForbidenPos && uiJob==puiAvailJobs[0] && uiNbAvJobs==1))
		{ // We will schedule an available job
		 // We schedule the first job of list puiAvailJobs
		 uiDate+=piS(puiAvailJobs[0]);
		 if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		 puiSchedule[uiSize]=puiAvailJobs[0];
		 puiStartTime[uiSize]=uiDate-piS(puiAvailJobs[0]);
		 uiSize++;
		 for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		 uiNbAvJobs--;
		} else
		{// We schedule puiAvailJobs[1]
		     uiDate+=piS(puiAvailJobs[1]);
		     if (((double)uiDate-(double)diS(puiAvailJobs[1]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[1]));
		     puiSchedule[uiSize]=puiAvailJobs[1];
		     puiStartTime[uiSize]=uiDate-piS(puiAvailJobs[1]);
		     uiSize++;
		     for (uiLoop2=1;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		     uiNbAvJobs--;
		}
	}
	return dLmax;
}

/***********************************************************************************
AddInERDOrder - Add a job in ERDorder into the puiList given as an input
***********************************************************************************/
void AddInERDOrder(unsigned int puiList[MaxJobs1], unsigned int *uiSize,unsigned int uiJob)
{
 unsigned int uiLoop,uiLoop2;

 uiLoop=0;
 while (uiLoop<*uiSize && (riS(puiList[uiLoop])<riS(uiJob) ||(riS(puiList[uiLoop])==riS(uiJob) && diS(puiList[uiLoop])<diS(uiJob))))
	 uiLoop++;
 for (uiLoop2=*uiSize;uiLoop2>uiLoop;uiLoop2--)
	 puiList[uiLoop2]=puiList[uiLoop2-1];
 puiList[uiLoop]=uiJob;

 (*uiSize)=(*uiSize)+1;
}

/***********************************************************************************
BlockScheduler - This is a kind of local search which takes upon entry a schedule
                 and for each block containing a critical job, forbid the first scheduled job of that block 
				 to occupy that position. 
***********************************************************************************/

double BlockScheduler(unsigned int * puiSchrage, double dLmaxRef)
{
 unsigned int uiLoop,uiLoop2,uiLoop3;
 double dMargin[MaxJobs1], Cmax, dNewLmax, dLmaxtmp;
 unsigned int uiStart[MaxJobs1],uiBlockBeg[MaxJobs1], uiBeg;
 unsigned int uiParSched[MaxJobs1],uiNbPS,uiCmaxPS, puiAvailJobs[MaxJobs1],uiNbAvJobs, puiSched[MaxJobs1],puiStart[MaxJobs1];

 // Calcul des marges des travaux par rapport au dLmaxRef
 Cmax=0;
 uiBeg=riS(puiSchrage[0]);
 for (uiLoop=0;uiLoop<NbJobs1;uiLoop++)
 {
	 if (Cmax<riS(puiSchrage[uiLoop]))
	 {
		 Cmax=riS(puiSchrage[uiLoop])+piS(puiSchrage[uiLoop]);
		 uiBeg=uiLoop;
	 }
	 else Cmax+=piS(puiSchrage[uiLoop]);
	 dMargin[uiLoop]=(Cmax-diS(puiSchrage[uiLoop]))-dLmaxRef;
	 uiStart[uiLoop]=Cmax-piS(puiSchrage[uiLoop]);
	 uiBlockBeg[uiLoop]=uiBeg;
 }

 // We are looking for the first critical job
 uiLoop=0;
 while (uiLoop<NbJobs1)
 {
	 if (dMargin[uiLoop]==0.0)
	 {
	  // We now rebuild the partial Schrage's schedule, up to position uiLoop2-1
	  uiNbPS=0;
	  uiCmaxPS=0;
	  for (uiLoop3=0;uiLoop3<uiBlockBeg[uiLoop];uiLoop3++)
	  {
		  uiParSched[uiNbPS]=puiSchrage[uiLoop3];
		  if (uiCmaxPS<riS(uiParSched[uiNbPS])) uiCmaxPS=riS(uiParSched[uiNbPS])+piS(uiParSched[uiNbPS]);
		   else uiCmaxPS+=piS(uiParSched[uiNbPS]);
		  uiNbPS++;
	  }

	  // We build the list of available jobs sorted by increasing value of ri
	  uiNbAvJobs=0;
	  for (uiLoop3=uiBlockBeg[uiLoop];uiLoop3<NbJobs1;uiLoop3++)
			AddInERDOrder(puiAvailJobs, &uiNbAvJobs,puiSchrage[uiLoop3]);

	  // We forbid the job puiSchrage[uiBlockBeg[uiLoop]] to be in position 0 (uiBlockBeg[uiLoop] in the full schedule)
	  dLmaxtmp=SchrageFoJP(0,puiSchrage[uiBlockBeg[uiLoop]],uiCmaxPS,puiAvailJobs,uiNbAvJobs,puiSched,puiStart);
	  if (dLmaxtmp<dLmaxRef)
	  {
		  dLmaxRef=dLmaxtmp;
		  for (uiLoop3=uiBlockBeg[uiLoop];uiLoop3<NbJobs1;uiLoop3++)
		  {
			  puiSchrage[uiLoop3]=puiSched[uiLoop3-uiBlockBeg[uiLoop]];
			  uiStart[uiLoop3]=puiStart[uiLoop3-uiBlockBeg[uiLoop]];
		  }
		  dLmaxRef=ComputeLmax(puiSchrage);
		  for (uiLoop3=0;uiLoop3<NbJobs1;uiLoop3++)
			dMargin[uiLoop3]=(uiStart[uiLoop3]+piS(puiSchrage[uiLoop3])-diS(puiSchrage[uiLoop3]))-dLmaxRef;
		  uiLoop=0;

	  } else uiLoop++;
	  } else uiLoop++;
 }

 return dLmaxRef;

 // Il faut encore faire attention dans la fonction car il ne faudrait réordonnancer que le block courant et pas toute la fin de l'ordonnancement....


}

/***********************************************************************************
LocalSearch - Local Search to improve Schrage's schedule
***********************************************************************************/

double LocalSearch(unsigned int * puiSchrage, double dLmaxRef)
{
 unsigned int uiLoop,uiLoop3, uiLoop4;
 double dMargin[MaxJobs1],Cmax, dNewLmax, dLmaxtmp;
 unsigned int uiStart[MaxJobs1],Pos, uiLoop2;
 unsigned int uiParSched[MaxJobs1],uiNbPS,uiCmaxPS, puiAvailJobs[MaxJobs1],uiNbAvJobs,puiStart[MaxJobs1], puiSched[MaxJobs1];
 unsigned int puiBestSched[MaxJobs1],puiBestStart[MaxJobs1];


 // Calcul des marges des travaux par rapport au dLmaxRef
 Cmax=0;

 for (uiLoop=0;uiLoop<NbJobs1;uiLoop++)
 {
	 if (Cmax<riS(puiSchrage[uiLoop]))
	 {
		 Cmax=riS(puiSchrage[uiLoop])+piS(puiSchrage[uiLoop]);
		 setCiS(puiSchrage[uiLoop], Cmax);
	 }
	 else 
	 {
		Cmax+=piS(puiSchrage[uiLoop]);
		setCiS(puiSchrage[uiLoop],Cmax);
		//printf("Cmax \n");
		//printf("%u %u\n", Ci(puiSchrage[uiLoop]), Cmax);
	 }
	 dMargin[uiLoop]=(Cmax-diS(puiSchrage[uiLoop]))-dLmaxRef;
	 uiStart[uiLoop]=Cmax-piS(puiSchrage[uiLoop]);
 }

 // We are looking for the first critical job: neighbors are built by insert and swap operatorrs (1st pass)
 uiLoop=0;
 while (uiLoop<NbJobs1)
 {
	 if (dMargin[uiLoop]==0.0)
	 {
		 if (uiLoop>0 && diS(puiSchrage[uiLoop-1])>diS(puiSchrage[uiLoop]))
		 {
			 //LEFT INSERT
			 LeftInsert(puiSchrage,dMargin,uiStart,uiLoop,dLmaxRef, & dNewLmax, &Pos); // It can be usefull to perform a left insert of the critical job
			// cptLeftInsert++;
			 // si ça change rien
			 if (dNewLmax==dLmaxRef)
			 { // RIGHT INSERT
				 RightInsert(puiSchrage,dMargin,uiStart,uiLoop,dLmaxRef, & dNewLmax, &Pos);
				// cptRightInsert++;
				 if (dNewLmax<dLmaxRef)
				 {
				  dLmaxRef=ComputeLmax(puiSchrage);
				  Cmax=0;
				  //amRightInsert++;
				  for (uiLoop2=0;uiLoop2<NbJobs1;uiLoop2++)
				  {
					 if (Cmax<riS(puiSchrage[uiLoop2])) 
					{	 Cmax=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
						setCiS(puiSchrage[uiLoop2], Cmax);
					 }
					 else 
					{
						 Cmax+=piS(puiSchrage[uiLoop2]);
						 setCiS(puiSchrage[uiLoop2], Cmax);
					 }
					 dMargin[uiLoop2]=(Cmax-diS(puiSchrage[uiLoop2]))-dLmaxRef;
					 uiStart[uiLoop2]=Cmax-piS(puiSchrage[uiLoop2]);
				  }
				  uiLoop=0;
				 } else uiLoop++;
			 } else if (dNewLmax<dLmaxRef) 
			 {
				 // The Left operators have succeeded in improving the best known Lmax, so we update the starting times and the margin
				 //amLeftInsert++;
				 dLmaxRef=ComputeLmax(puiSchrage);
				 Cmax=0;
				 for (uiLoop2=0;uiLoop2<NbJobs1;uiLoop2++)
				 {
					 if (Cmax<riS(puiSchrage[uiLoop2])) 
					 {
						 Cmax=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
						 setCiS(puiSchrage[uiLoop2], Cmax);
					 }
					 else
					{
						Cmax+=piS(puiSchrage[uiLoop2]);
						setCiS(puiSchrage[uiLoop2], Cmax);
					 }
					 dMargin[uiLoop2]=(Cmax-diS(puiSchrage[uiLoop2]))-dLmaxRef;
					 uiStart[uiLoop2]=Cmax-piS(puiSchrage[uiLoop2]);
				 }
				 uiLoop=0;
			 } else uiLoop++; // No improvement with LeftInsert
		 }
		 else 
		 {
			 // LEFT SWAP
			 LeftSwap(puiSchrage,dMargin,uiStart,uiLoop,dLmaxRef, & dNewLmax, &Pos); // We try to perform a left swap of the critical job
			 //cptLeftSwap++;
			 // si ça change rien
			 if (dNewLmax==dLmaxRef)
			 { // RIGHT INSERT
				 RightInsert(puiSchrage,dMargin,uiStart,uiLoop,dLmaxRef, & dNewLmax, &Pos);
				// cptRightInsert++;
				 if (dNewLmax<dLmaxRef)
				 {
				  dLmaxRef=ComputeLmax(puiSchrage);
				  Cmax=0;
				 // amRightInsert++;
				  for (uiLoop2=0;uiLoop2<NbJobs1;uiLoop2++)
				  {
					 if (Cmax<riS(puiSchrage[uiLoop2])) 
					 {
						 Cmax=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
						 setCiS(puiSchrage[uiLoop2], Cmax);
					 }
					 else 
					{
						Cmax+=piS(puiSchrage[uiLoop2]);
						setCiS(puiSchrage[uiLoop2], Cmax);
					 }
					 dMargin[uiLoop2]=(Cmax-diS(puiSchrage[uiLoop2]))-dLmaxRef;
					 uiStart[uiLoop2]=Cmax-piS(puiSchrage[uiLoop2]);
				  }
				  uiLoop=0;
				 } else uiLoop++;
			 } else if (dNewLmax<dLmaxRef) 
			 {
				 //amLeftSwap++;
				 // The Left operators have succeeded in improving the best known Lmax, so we update the starting times and the margin
				 dLmaxRef=ComputeLmax(puiSchrage);
				 Cmax=0;
				 for (uiLoop2=0;uiLoop2<NbJobs1;uiLoop2++)
				 {
					 if (Cmax<riS(puiSchrage[uiLoop2])) 
					 {
						 Cmax=riS(puiSchrage[uiLoop2])+piS(puiSchrage[uiLoop2]);
						 setCiS(puiSchrage[uiLoop2], Cmax);
					 }
					 else
					{
						Cmax+=piS(puiSchrage[uiLoop2]);
						 setCiS(puiSchrage[uiLoop2], Cmax);
					 }
					 dMargin[uiLoop2]=(Cmax-diS(puiSchrage[uiLoop2]))-dLmaxRef;
					 uiStart[uiLoop2]=Cmax-piS(puiSchrage[uiLoop2]);
				 }
				 uiLoop=0;
			 } else uiLoop++; // No improvement with LeftInsert
		 }

		 
	 } else uiLoop++;
 }

 dNewLmax=BlockScheduler(puiSchrage, dLmaxRef);
 if (dNewLmax<=dLmaxRef) dLmaxRef=dNewLmax;


 return dLmaxRef;
}

/***********************************************************************************
AddInEDDOrder - Add a job in EDD order into the puiList given as an input
***********************************************************************************/
void AddInEDDOrder(unsigned int puiList[MaxJobs1], unsigned int *uiSize,unsigned int uiJob)
{
 unsigned int uiLoop,uiLoop2;

 uiLoop=0;
 while (uiLoop<*uiSize && (diS(puiList[uiLoop])<diS(uiJob) ||(diS(puiList[uiLoop])==diS(uiJob) && piS(puiList[uiLoop])<piS(uiJob))))
	 uiLoop++;
 for (uiLoop2=*uiSize;uiLoop2>uiLoop;uiLoop2--)
	 puiList[uiLoop2]=puiList[uiLoop2-1];
 puiList[uiLoop]=uiJob;

 (*uiSize)=(*uiSize)+1;
}

/***********************************************************************************
Shrage- compute an upper bound to the Lmax by Schrage's heuristic
Nb: it is assumed that jobs are indexed by increasing order of their ri's
***********************************************************************************/
double Schrage()
{
	unsigned int uiLoop,uiLoop2, uiDate,uiCmax;
	unsigned int puiLoJobs[MaxJobs1], uiNbLoJobs;
	unsigned int puiAvailJobs[MaxJobs1], uiNbAvJobs;
	unsigned int uiSchedule[MaxJobs1],uiStartTime[MaxJobs1],uiSize;
	double dLmax,dPrevLmax;

	// We set the list of candidate jobs 
	for (uiLoop=0;uiLoop<NbJobs1;uiLoop++)
		puiLoJobs[uiLoop]=uiLoop;
	uiNbLoJobs=NbJobs1;

	uiDate=0;
	uiLoop=0;
	dLmax=-9999999.0;
	uiNbAvJobs=0;
	uiSize=0;
	while (uiLoop<uiNbLoJobs)
	{ // There are still unscheduled jobs to schedule
		if (riS(puiLoJobs[uiLoop])>uiDate && uiNbAvJobs==0)
			uiDate=riS(puiLoJobs[uiLoop]);
		// We now retrieve the available jobs
		while (uiLoop<uiNbLoJobs && riS(puiLoJobs[uiLoop])<=uiDate)
		{ // We add the current job into the list of available jobs
			AddInEDDOrder(puiAvailJobs,&uiNbAvJobs,uiLoop);
			uiLoop++;
		}
		// We schedule the first job of list puiAvailJobs
		uiDate+=piS(puiAvailJobs[0]);
		if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		uiSchedule[uiSize]=puiAvailJobs[0];
		uiStartTime[uiSize]=uiDate-piS(puiAvailJobs[0]);
		uiSize++;
		for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		uiNbAvJobs--;
	}

	// We now only need to schedule jobs which are available
	while (uiNbAvJobs>0)
	{
		// We schedule the first job of list puiAvailJobs
		uiDate+=piS(puiAvailJobs[0]);
		if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		uiSchedule[uiSize]=puiAvailJobs[0];
		uiStartTime[uiSize]=uiDate-piS(puiAvailJobs[0]);
		uiSize++;
		for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		uiNbAvJobs--;
	}

	///printf("Value of Schrage's schedule %lf\n",dLmax);
	//printf("Schrage's schedule : ");
	//for (uiLoop=0;uiLoop<NbJobs;uiLoop++)
	//	printf("%ld ",uiSchedule[uiLoop]);
	//printf("\n");
	//printf("Start times : ");
	//for (uiLoop=0;uiLoop<NbJobs;uiLoop++)
	//	printf("%ld ",uiStartTime[uiLoop]);
	//printf("\n");

	for (uiLoop=0;uiLoop<NbJobs1;uiLoop++)
		puiSchrageBase[uiLoop]=uiSchedule[uiLoop];

	dPrevLmax=99999999.0;
	
	dLmax=LocalSearch(uiSchedule,dLmax);
	printf("Value of Schrage's schedule after LS %lf\n",dLmax);

	while (dPrevLmax!=dLmax)
	{
		dPrevLmax=dLmax;
		dLmax=LocalSearch(uiSchedule,dLmax);
	    printf("Value of Schrage's schedule after LS %lf\n",dLmax);
	}

	for (uiLoop=0;uiLoop<NbJobs1;uiLoop++)
		puiBestSchrage[uiLoop]=uiSchedule[uiLoop];


	return dLmax;
}

/***********************************************************************************
ShragePre- compute an upper bound to the Lmax by starting with Schrage preemptive schedule
Nb: it is assumed that jobs are indexed by increasing order of their ri's
***********************************************************************************/
double SchragePre()
{
	unsigned int uiLoop,uiLoop2, uiDate,uiCmax;
	unsigned int puiLoJobs[MaxJobs1], uiNbLoJobs;
	unsigned int puiAvailJobs[MaxJobs1], uiNbAvJobs;
	unsigned int uiSchedule[MaxJobs1],uiStartTime[MaxJobs1],uiSize;
	unsigned int uipi[MaxJobs1];
	double dLmax,dPrevLmax;
	bool bisPreempted;

	// We set the list of candidate jobs 
	for (uiLoop=0;uiLoop<NbJobs1;uiLoop++)
	{
		puiLoJobs[uiLoop]=uiLoop;
		uipi[uiLoop]=piS(uiLoop);
	}
	uiNbLoJobs=NbJobs1;

	uiDate=0;
	uiLoop=0;
	dLmax=-9999999.0;
	uiNbAvJobs=0;
	uiSize=0;
	while (uiLoop<uiNbLoJobs)
	{ // There are still unscheduled jobs to schedule
		if (riS(puiLoJobs[uiLoop])>uiDate && uiNbAvJobs==0)
			uiDate=riS(puiLoJobs[uiLoop]);
		// We now retrieve the available jobs
		while (uiLoop<uiNbLoJobs && riS(puiLoJobs[uiLoop])<=uiDate)
		{ // We add the current job into the list of available jobs
			AddInEDDOrder(puiAvailJobs,&uiNbAvJobs,uiLoop);
			uiLoop++;
		}
		// We schedule the first job of list puiAvailJobs... checking whether a future available job may preempt it or not
		bisPreempted=false;
		uiLoop2=uiLoop;
		while (uiLoop2<uiNbLoJobs && (uiDate+uipi[puiAvailJobs[0]])>riS(puiLoJobs[uiLoop2]) && !bisPreempted) 
		{
			if (diS(puiAvailJobs[0])>diS(puiLoJobs[uiLoop2])) bisPreempted=true;
			else uiLoop2++;
		}
		if (bisPreempted)
		{ // The job puiAvailJobs[0] is preempted by the job puiAvailJobs[uiLoop2]
		  uipi[puiAvailJobs[0]]-=(riS(puiLoJobs[uiLoop2])-uiDate);
		  uiDate=riS(puiLoJobs[uiLoop2]);
		} else
		{
		 uiDate+=uipi[puiAvailJobs[0]];
		 if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		 uiSchedule[uiSize]=puiAvailJobs[0];
		 uiSize++;
		 for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		 uiNbAvJobs--;
		}
	}

	// We now only need to schedule jobs which are available
	while (uiNbAvJobs>0)
	{
		// We schedule the first job of list puiAvailJobs
		uiDate+=uipi[puiAvailJobs[0]];
		if (((double)uiDate-(double)diS(puiAvailJobs[0]))>dLmax) dLmax=((double)uiDate-(double)diS(puiAvailJobs[0]));
		uiSchedule[uiSize]=puiAvailJobs[0];
		uiSize++;
		for (uiLoop2=0;uiLoop2<uiNbAvJobs-1;uiLoop2++) puiAvailJobs[uiLoop2]=puiAvailJobs[uiLoop2+1];
		uiNbAvJobs--;
	}

   uiDate=0;
   dLmax=-9999999.0;
   for (uiLoop=0;uiLoop<NbJobs1;uiLoop++)
   {
	   if (riS(uiSchedule[uiLoop])>uiDate) uiDate=riS(uiSchedule[uiLoop]);
	   uiStartTime[uiLoop]=uiDate;
	   uiDate+=piS(uiSchedule[uiLoop]);
 	   if (((double)uiDate-(double)diS(uiSchedule[uiLoop]))>dLmax) dLmax=((double)uiDate-(double)diS(uiSchedule[uiLoop]));
  }
	
	///printf("Value of the schedule obtained by Preemptive Schrage : %lf\n",dLmax);
	

	// We test if the current schedule is equal to the Schrage's schedule : if so, we don't need to do a localrch
	for (uiLoop=0;uiLoop<NbJobs1 && uiSchedule[uiLoop]==puiSchrageBase[uiLoop];uiLoop++);

	if (uiLoop!=NbJobs1)
	{

	 dPrevLmax=99999999.0;
	
	 //dLmax=LocalSearch(uiSchedule,dLmax);
	/// printf("Value of Preemptive Schrage's schedule after LS %lf\n",dLmax);

	 while (dPrevLmax!=dLmax)
	 {
		dPrevLmax=dLmax;
		//dLmax=LocalSearch(uiSchedule,dLmax);
	  ///  printf("Value of Preemptive Schrage's schedule after LS %lf\n",dLmax);
	 }
	}


	return dLmax;
}



/***********************************************************************************
main - the main function of the program
***********************************************************************************/
/*void main()
{
  clock_t time1,time2;
  double CPUTime,dValSchrage,dValSchragePre;
  FILE *Output;

 // Reading the data from donnees.pb file


 ReadDataSchrage();
 time1= clock();

 dValSchrage=Schrage();
 if (!isSolOptimal(puiBestSchrage))
     dValSchragePre=SchragePre();
 else dValSchragePre=99999999.0;
 // This upper bound is the schedule deduced from preemptive Schrage plus a local search

 //if (dValSchragePre<dValSchrage) dValSchrage=dValSchragePre;

 time2=clock();
 CPUTime=diffclock(time2,time1);
 int ci[MaxJobs];
 int jobI[MaxJobs];
 for(int job=0;job<NbJobs;job++){
	ci[job] = CiS(job);
	jobI[job] = jobInit(job);
 }
 // Writing the results into a file
 Output=fopen("SchrageEtRL.txt","wt");
 fprintf(Output,"%ld\n",NbJobs);
 fprintf(Output,"%lf\n",dValSchrage);
 //fprintf(Output,"ci\n");
  for(int job=0;job<NbJobs;job++){
	fprintf(Output,"%u\n",ci[job]);
	fprintf(Output,"%u\n",jobI[job]);
 }
 fprintf(Output,"%lf\n",CPUTime);
 fprintf(Output,"%lf\n",CPUTime);
 fprintf(Output,"0\n");
 fprintf(Output,"0.0000\n");
 fprintf(Output,"diffSchrageRL\n0.0000\n");
 fclose(Output);

}
*/
