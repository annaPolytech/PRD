#include <stdio.h>
#include <iostream>
#include "donnees.h"

unsigned int NbJobs=20;
unsigned int NbMachines =3;
//TData Data; 
TData tabStructure[MaxMachines];
TData2 dataJob;

inline unsigned int id(unsigned int uiMachine){return tabStructure[uiMachine].identifiant;}
inline unsigned int ri(unsigned int uiMachine, unsigned int uiJob) {return tabStructure[uiMachine].ri[uiJob];}
inline unsigned int pi(unsigned int uiMachine, unsigned int uiJob) {return tabStructure[uiMachine].pi[uiJob];}
inline unsigned int di(unsigned int uiMachine, unsigned int uiJob) {return tabStructure[uiMachine].di[uiJob];}
inline unsigned int Ci(unsigned int uiMachine, unsigned int uiJob) {return tabStructure[uiMachine].Ci[uiJob];}
inline unsigned int Di(unsigned int uiMachine, unsigned int uiJob, unsigned int uiMachine2) {return tabStructure[uiMachine].Di[uiJob][uiMachine2];}

inline unsigned int setri(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurri) {return tabStructure[uiMachine].ri[uiJob] = valeurri;}
inline unsigned int setdi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurdi) {return tabStructure[uiMachine].di[uiJob] = valeurdi;}
inline unsigned int setCi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurCi) {return tabStructure[uiMachine].Ci[uiJob] = valeurCi;}

inline unsigned int riG(unsigned int uiJob) {return dataJob.riG[uiJob];}
inline unsigned int diG(unsigned int uiJob) {return dataJob.diG[uiJob];}

void ReadData()
{
 // Reading of the data in the file donnees.pb
 FILE *FInG;
 FILE *FIn;
 unsigned int temp,uiLoop,riG, diG, ri,pi,di,uiLoop2,uiLoop3,j;
 unsigned int Di2;





 // lecture du fichier job / machine

 FIn=fopen("donnees1.pb","rt");
 // pour chaque machine
 for(j=0;j<NbMachines;j++)
 {
	 // on lit la 1ere
	 fscanf(FIn,"%u %u\n",&NbJobs,&temp);
	 for(uiLoop=0;uiLoop<NbJobs;uiLoop++)
	 {
		 fscanf(FIn,"%u %u %u ",&ri,&di,&pi);
		 unsigned int Di [MaxMachines];
		 for (int i=0;i<NbMachines ;i++){
			fscanf(FIn,"%u ",&Di2);
			Di[i]=Di2;
		 }
		 fscanf(FIn,"\n");

		/* uiLoop2=0;
		 while (uiLoop2<uiLoop && (ri>tabStructure[j].ri[uiLoop2] || (ri==tabStructure[j].ri[uiLoop2] && di>tabStructure[j].di[uiLoop2]))) uiLoop2++;
		 for (uiLoop3=uiLoop;uiLoop3>uiLoop2;uiLoop3--)
		 {
			 tabStructure[j].identifiant=temp;
			 tabStructure[j].ri[uiLoop3]=tabStructure[j].ri[uiLoop3-1];
			 tabStructure[j].pi[uiLoop3]=tabStructure[j].pi[uiLoop3-1];
			 tabStructure[j].di[uiLoop3]=tabStructure[j].di[uiLoop3-1];
			 tabStructure[j].Ci[uiLoop3]=tabStructure[j].Ci[uiLoop3-1];
			for (int i=0;i<NbMachines ;i++){
				tabStructure[j].Di[uiLoop3][i]=tabStructure[j].Di[uiLoop3-1][i];
			}
			 //Data.Di[uiLoop3]=Data.Di[uiLoop3-1];
			 
		 }
		 tabStructure[j].identifiant=temp;
		 tabStructure[j].ri[uiLoop2]=ri;
		 tabStructure[j].pi[uiLoop2]=pi;
		 tabStructure[j].di[uiLoop2]=di;
		 tabStructure[j].Ci[uiLoop2]=0;
		 for (int i=0;i<NbMachines ;i++){
			 tabStructure[j].Di[uiLoop2][i]=Di[i];
		}	*/	 

		 tabStructure[j].identifiant=temp;
		 tabStructure[j].ri[uiLoop]=ri;
		 tabStructure[j].pi[uiLoop]=pi;
		 tabStructure[j].di[uiLoop]=di;
		 tabStructure[j].Ci[uiLoop]=0;
		 for (int i=0;i<NbMachines ;i++){
			 tabStructure[j].Di[uiLoop][i]=Di[i];
		}	
	 }
 }
 fclose(FIn);
/* printf ("ri %u \n", ri);
printf ("riDonne %u %u %u %u %u %u \n", tabStructure[1].ri[0], tabStructure[1].pi[0], tabStructure[1].di[0], tabStructure[1].Di[0][0], tabStructure[1].Di[0][1], tabStructure[1].Di[0][2]);
printf ("riDonne %u %u %u %u %u %u \n", tabStructure[1].ri[1], tabStructure[1].pi[1], tabStructure[1].di[1], tabStructure[1].Di[1][0], tabStructure[1].Di[1][1], tabStructure[1].Di[1][2]);
printf ("riDonne %u %u %u %u %u %u \n", tabStructure[1].ri[2], tabStructure[1].pi[2], tabStructure[1].di[2], tabStructure[1].Di[2][0], tabStructure[1].Di[2][1], tabStructure[1].Di[2][2]);
printf ("riDonne %u %u %u %u %u %u \n", tabStructure[1].ri[3], tabStructure[1].pi[3], tabStructure[1].di[3], tabStructure[1].Di[3][0], tabStructure[1].Di[3][1], tabStructure[1].Di[3][2]);
 */// Writting of the data into the format of Carlier's Branch and Bound (file donnees.dat)

}

void ReadData2(){
	// Reading of the data in the file donnees.pb
 FILE *FInG;
 FILE *FIn;
 unsigned int temp,uiLoop,riG, diG, ri,pi,di,uiLoop2,uiLoop3,j;
 unsigned int Di2;

 // Lecture du fichier general
 FInG=fopen("donneesG1.pb","rt");
 // On lit la 1ere
 fscanf(FInG,"%u %u\n",&NbJobs,&temp);
 for(uiLoop=0;uiLoop<NbJobs;uiLoop++)
 {
	 fscanf(FInG,"%u %u\n",&riG,&diG);
	 //uiLoop2=0;
	 //while (uiLoop2<uiLoop && (riG>dataJob.riG[uiLoop2] || (riG==dataJob.riG[uiLoop2] && diG>dataJob.diG[uiLoop2]))) uiLoop2++;
	// for (uiLoop3=uiLoop;uiLoop3>uiLoop2;uiLoop3--)
	 //{
	 	// dataJob.riG[uiLoop3]=dataJob.riG[uiLoop3-1];
		// dataJob.diG[uiLoop3]=dataJob.diG[uiLoop3-1];
	// }
	 //dataJob.riG[uiLoop2]=riG;
	 //dataJob.diG[uiLoop2]=diG;
	 dataJob.riG[uiLoop]=riG;
	 dataJob.diG[uiLoop]=diG;
 }
 fclose(FInG);
}