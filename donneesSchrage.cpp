#include <stdio.h>
#include "donneesSchrage2.h"

unsigned int NbJobs1;
TDataSchrage dataSchrage;

inline unsigned int riS(unsigned int uiJob) {return dataSchrage.riS[uiJob];}
inline unsigned int piS(unsigned int uiJob) {return dataSchrage.piS[uiJob];}
inline unsigned int diS(unsigned int uiJob) {return dataSchrage.diS[uiJob];}
inline unsigned int CiS(unsigned int uiJob) {return dataSchrage.CiS[uiJob];}
inline unsigned int jobInit(unsigned int uiJob) {return dataSchrage.jobInit[uiJob];}
inline unsigned int setCiS(unsigned int uiJob, unsigned int valeurCi) {return dataSchrage.CiS[uiJob]=valeurCi;}

void ReadDataSchrage()
{
 // Reading of the data in the file donnees.pb
 FILE *FIn;
 unsigned int temp,uiLoop,ri,pi,di,ci,uiLoop2,uiLoop3;
 FIn=fopen("donnees.pb","rt");
 fscanf(FIn,"%d %d\n",&NbJobs1,&temp);
 for(uiLoop=0;uiLoop<NbJobs1;uiLoop++)
 {
	 fscanf(FIn,"%d %d %d\n",&ri,&di,&pi);
	 uiLoop2=0;
	 while (uiLoop2<uiLoop && (ri>dataSchrage.riS[uiLoop2] || (ri==dataSchrage.riS[uiLoop2] && di>dataSchrage.diS[uiLoop2]))) uiLoop2++;
	 for (uiLoop3=uiLoop;uiLoop3>uiLoop2;uiLoop3--)
	 {
		 dataSchrage.riS[uiLoop3]=dataSchrage.riS[uiLoop3-1];
		 dataSchrage.piS[uiLoop3]=dataSchrage.piS[uiLoop3-1];
		 dataSchrage.diS[uiLoop3]=dataSchrage.diS[uiLoop3-1];
		 dataSchrage.CiS[uiLoop3]=dataSchrage.CiS[uiLoop3-1];
		 dataSchrage.jobInit[uiLoop3]=dataSchrage.jobInit[uiLoop3-1];
	 }
	 dataSchrage.riS[uiLoop2]=ri;
	 dataSchrage.piS[uiLoop2]=pi;
	 dataSchrage.diS[uiLoop2]=di;
	 dataSchrage.CiS[uiLoop2]=1;
	 dataSchrage.jobInit[uiLoop2]=uiLoop;
 }
 fclose(FIn);

 // Writting of the data into the format of Carlier's Branch and Bound (file donnees.dat)
 FILE *FOut;
 FOut=fopen("donnees.dat","wt");
 for(uiLoop=0;uiLoop<NbJobs1;uiLoop++)
	 	 fprintf(FOut,"%d %d %d %d %d\n",dataSchrage.riS[uiLoop],dataSchrage.piS[uiLoop],dataSchrage.diS[uiLoop],dataSchrage.CiS[uiLoop],dataSchrage.jobInit[uiLoop]);
 fclose(FOut);
}
