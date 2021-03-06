#include <stdio.h>
#include <iostream>
#include "donnees.h"

unsigned int NbJobs;
unsigned int NbMachines=3;

TData tabStructure[MaxMachines];
TDataG dataJob;



// Fonction de la structure job / machine
inline unsigned int id(unsigned int uiMachine){return tabStructure[uiMachine].identifiant;}
inline unsigned int ri(unsigned int uiMachine, unsigned int uiJob) {return tabStructure[uiMachine].ri[uiJob];}
inline unsigned int pi(unsigned int uiMachine, unsigned int uiJob) {return tabStructure[uiMachine].pi[uiJob];}
inline unsigned int di(unsigned int uiMachine, unsigned int uiJob) {return tabStructure[uiMachine].di[uiJob];}
inline unsigned int Ci(unsigned int uiMachine, unsigned int uiJob) {return tabStructure[uiMachine].Ci[uiJob];}
inline unsigned int nombreJobs(unsigned int uiMachine){return tabStructure[uiMachine].nombreJobs;}


inline unsigned int setri(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurri) {return tabStructure[uiMachine].ri[uiJob] = valeurri;}
inline unsigned int setdi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurdi) {return tabStructure[uiMachine].di[uiJob] = valeurdi;}
inline unsigned int setpi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurpi) {return tabStructure[uiMachine].pi[uiJob] = valeurpi;}
inline unsigned int setCi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurCi) {return tabStructure[uiMachine].Ci[uiJob] = valeurCi;}
inline unsigned int setCoutRejet(unsigned int uiJob, unsigned int valeurCoutR){return dataJob.coutR[uiJob] = valeurCoutR;}

// Fonction de la structure job general
inline unsigned int riG(unsigned int uiJob) {return dataJob.riG[uiJob];}
inline unsigned int diG(unsigned int uiJob) {return dataJob.diG[uiJob];}
inline unsigned int gamme(unsigned int uiJob, unsigned int uiOperation) {return dataJob.gamme[uiJob][uiOperation];}
inline unsigned int delai(unsigned int uiJob, unsigned int uiOperation) {return dataJob.delai[uiJob][uiOperation];}
inline unsigned int coutR(unsigned int uiJob) {return dataJob.coutR[uiJob];}


void ReadData()
{
 // Reading of the data in the file donnees.pb
 FILE *FInG;
 FILE *FIn;
 unsigned int temp,uiLoop,riG, diG, ri,pi,di,uiLoop2,uiLoop3,j;
 unsigned int Di2;

 // lecture du fichier job / machine

 //FIn=fopen("donneesJobMachine.pb","rt");
 // FIn=fopen("donnees1.pb","rt");
  FIn=fopen("donnees2.pb","rt");
 // pour chaque machine
 for(j=0;j<NbMachines;j++)
 {
	 // on lit la 1ere
	 fscanf(FIn,"%u %u\n",&NbJobs,&temp);
	 for(uiLoop=0;uiLoop<NbJobs;uiLoop++)
	 {
		 fscanf(FIn,"%u %u %u ",&ri,&di,&pi);
		 fscanf(FIn,"\n");

		 tabStructure[j].identifiant=temp;
		 tabStructure[j].nombreJobs=NbJobs;
		 tabStructure[j].ri[uiLoop]=ri;
		 tabStructure[j].pi[uiLoop]=pi;
		 tabStructure[j].di[uiLoop]=di;
		 tabStructure[j].Ci[uiLoop]=0;
	 }
 }
 fclose(FIn);


}

void ReadData2(){
	// Reading of the data in the file donnees.pb
 FILE *FInG;
 FILE *FIn;
 unsigned int temp,uiLoop, uiMach,riG, diG, coutR, ri,pi,di,uiLoop2,uiLoop3,j,i;
 unsigned int Gi2, Di2;

 //Lecture du fichier general
// FInG=fopen("donneesG1.pb","rt");
 FInG=fopen("donneesG2.pb","rt");
 //FInG=fopen("donneesJob.pb","rt");
 // On lit la 1ere
 fscanf(FInG,"%u %u\n",&NbJobs,&temp);
 for(uiLoop=0;uiLoop<NbJobs;uiLoop++)
 {
	 fscanf(FInG,"%u %u %u",&riG,&diG,&coutR);
	 dataJob.riG[uiLoop]=riG;
	 dataJob.diG[uiLoop]=diG;
	 dataJob.coutR[uiLoop]=coutR;

	 
	unsigned int Gam [MaxMachines];
	for (i=0;i<NbMachines ;i++){
		fscanf(FInG," %u",&Gi2);
		Gam[i]=Gi2;
	}
	unsigned int Del [MaxMachines-1];
	for (i=0;i<NbMachines-1 ;i++){
		fscanf(FInG," %u",&Di2);
		Del[i]=Di2;
	}
	fscanf(FInG,"\n");

	 
	 for(uiMach=0;uiMach<NbMachines;uiMach++)
	  {
		  dataJob.gamme[uiLoop][uiMach]=Gam[uiMach];
		  dataJob.delai[uiLoop][uiMach]=Del[uiMach];
	  }

 }
 fclose(FInG);

}