#include <ctime>
#include "donneesSchrage2.h"

double diffclock(clock_t clock1,clock_t clock2);
bool isSolOptimal(unsigned int *uiSeq);
void LeftInsert(unsigned int * puiSchrage, double *pdMargin, unsigned int *puiStart,unsigned int uiStartPos, double dLmaxRef, double * pdNewLmax, unsigned int *pPos);
void LeftSwap(unsigned int * puiSchrage, double *pdMargin, unsigned int *puiStart,unsigned int uiStartPos, double dLmaxRef, double * pdNewLmax, unsigned int *pPos);
void RightInsert(unsigned int * puiSchrage, double *pdMargin, unsigned int *puiStart,unsigned int uiStartPos, double dLmaxRef, double * pdNewLmax, unsigned int *pPos);
double ComputeLmax(unsigned int *puiSchedule);
double SchrageFJ(unsigned int uiInsertPos, unsigned int uiJob, unsigned int uiDate, unsigned int *puiLoJobs, unsigned int uiNbLoJobs,unsigned int *puiSchedule, unsigned int *puiStartTime);
double SchrageFoJP(unsigned int uiForbidenPos, unsigned int uiJob, unsigned int uiDate, unsigned int *puiLoJobs, unsigned int uiNbLoJobs,unsigned int *puiSchedule, unsigned int *puiStartTime);
void AddInERDOrder(unsigned int puiList[MaxJobs1], unsigned int *uiSize,unsigned int uiJob);
double BlockScheduler(unsigned int * puiSchrage, double dLmaxRef);
double LocalSearch(unsigned int * puiSchrage, double dLmaxRef);
void AddInEDDOrder(unsigned int puiList[MaxJobs1], unsigned int *uiSize,unsigned int uiJob);
double Schrage();
double SchragePre();

