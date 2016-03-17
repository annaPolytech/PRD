#include "donneesSchrage2.h"
#include "donnees.h"
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string>
#include <Math.h>
#include <limits>
#include <cstdlib>
#include <climits>
#include <algorithm>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
unsigned int NbJobs1;
//#define NbMaxJob 50
TDataSchrage dataSchrage;
using namespace std;

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

void ReadDataSchrage2(int idMachine)
{
	NbJobs1=nombreJobs(idMachine);
	int tabRi[MaxJobs1];
	for(int x=0;x<MaxJobs1;x++){
		tabRi[x] = 999;
	}
	int ref[MaxJobs1][2];

	// remplir tabRi avec les ri des jobs de cette machine
	for(int job=0;job<NbJobs1;job++)
	{
		tabRi[job]=ri(idMachine,job);
	
		ref[job][0]=job;
		ref[job][1]=ri(idMachine,job);
	}

	// on trie le tableau 
	std::sort(std::begin(tabRi),std::end(tabRi));

	int parcours = 0;
	for(int job1=0; job1<NbJobs1;job1++){

		dataSchrage.riS[job1]=tabRi[job1];

		for(int job2=0 ; job2<NbJobs1; job2++){
			if(ref[job2][1]==tabRi[job1]){
				dataSchrage.piS[job1]=pi(idMachine,job2);
				dataSchrage.diS[job1]=di(idMachine,job2);
				dataSchrage.CiS[job1]=1;
				dataSchrage.jobInit[job1]=job2;
			}
		}
	}

	printf("riS diS piS \n");
	for(int job1=0 ; job1<NbJobs1; job1++){
		printf("%u %u %u \n",dataSchrage.riS[job1],dataSchrage.diS[job1],dataSchrage.piS[job1]);
	}
}