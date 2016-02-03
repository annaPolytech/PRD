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

#include "donnees.h"
using namespace std;

#define TEST false

#define NbMaxMachine 3
#define NbMaxJob 20
#define depart 500   // Min number of jobs
#define taille_max 5000   // Max number of jobs
#define pas 500		// Step in the loop on the number of jobs
#define iterations 30

#define EPSILON 0.0001

#define fixwith 0
// O: use reduced cost and pseudo-cost
// 1: use lp

#define CalMip 0
// 0: we don't calculate the MIP
// 1: we calculate the MIP


int somme(long int *in,int taille)
{
	int sum=0;
	int i;
	for(i=0;i<taille;i++)
	{
		sum+=in[i];
	}
	return sum;
}

/*
	Génération des données des jobs en fonction des machines 
*/
void generer(long int nbtrav, long int nbmach, double alpha,double beta,double a)
{
	
	long int i,k,j;
	 long int r,p,d,riG[NbMaxJob], diG[NbMaxJob], ri[NbMaxJob],di[NbMaxJob],pi[NbMaxJob],Di[NbMaxJob][NbMaxJob][NbMaxMachine];
	 int sum_pi;
	 FILE *fichier;

	 for(i=0;i<NbMaxMachine;i++)
	 {
		for(j=0;j<NbMaxJob;j++)
		{
			for(k=0;k<NbMaxMachine;k++)
			{
				Di[i][j][k]=0;
			}
		}
	 }

	 for(i=0;i<NbMaxJob;i++){
		ri[i] = 0;
		di[i] = 0;
		pi[i] = 0;
	 }
	
	// création du fichier des jobs par machine

	 fichier=fopen("donneesJobMachine.pb","wt");
	 int km;
	 // pour chaque machine
	 for(km=0;km<nbmach;km++)
	 {
		 // Génération des temps d'execution pi
		 for(i=0;i<nbtrav;i++)
		 {
			p=((float)rand()/(float)RAND_MAX)*49+1;
			pi[i]=p;
		 }
		 sum_pi=somme(pi,nbtrav);
		 // Génération des dates de début ri
		 for(i=0;i<nbtrav;i++)
		 {
			int rlb,rub;
			rlb=0;
			rub=alpha*sum_pi;
			r=((float)rand()/(float)RAND_MAX)*(rub-rlb)+rlb;
			ri[i]=r;
		 }
		 for(i=0;i<nbtrav;i++)
		 {
			int dlb,dub;
			dlb=(1-beta)*a*sum_pi;
			dub=a*sum_pi;
			d=((float)rand()/(float)RAND_MAX)*(dub-dlb)+dlb;
			if(d<ri[i]+pi[i])
			{
				d=ri[i]+sum_pi/nbtrav*2+pi[i];
			}
			di[i]=d;
		 }
		 // Génération des délais Di
		 for(i=0;i<nbtrav;i++)
		 {
			 for(int m=0;m<NbMaxMachine;m++)
			 {
				 if(m==km)
				 {
					Di[km][i][m]=0;
				 }else
				 {
					 bool delai = true;
					 for(int p=0;p<km;p++)
					 {
						 if(Di[p][i][m]!=0)
						 {
							 delai = false;
						 }
					 }
					 if(delai==false)
					 {
						Di[km][i][m] = 0;
					 }else
					 {
						 bool delai2 = true;
						 for(int p1=0;p1<NbMaxMachine ;p1++)
						 {
							 if(Di[km][i][p1]!=0)
							 {
								 
								 delai2 = false;
							 }
						 }
						 if(delai2==false)
						 {
							Di[km][i][m] = 0;
						 }else
						 {
							Di[km][i][m]=1;
						 }
					 }
				 }
			 }
		 }
		 fprintf(fichier,"%d %d\n",nbtrav,km);
		 for (i=0;i<nbtrav;i++)
		 {
			fprintf(fichier,"%ld %ld %ld",ri[i],di[i],pi[i]);
			for (j=0;j<nbmach;j++)
			{
				fprintf(fichier," %ld",Di[km][i][j]);
			}
			fprintf(fichier,"\n");
		 }
	 }
 
	 fclose(fichier);
}

/*
	Génération des données générales des jobs 
*/
void genererGenerale(long int nbtrav, long int nbmach, double alpha,double beta,double a)
{
	long int i,k,j;
	 long int r,p,d,riG[NbMaxJob], diG[NbMaxJob];
	 int sum_pi;
	// création du fichier général
	 FILE *fichierGenerale;
	 fichierGenerale=fopen("donneesJob.pb","wt");


	 for(i=0;i<nbtrav;i++)
	{
		riG[i]=99999999999999999;
		diG[i]=0;
		for(j=0;j<nbmach;j++)
		{
			if(ri(j,i)<riG[i]){
				riG[i] = ri(j,i);
			}
			if(di(j,i)>diG[i]){
				diG[i] = di(j,i);
			}
		}
	}
	
	fprintf(fichierGenerale,"%d %d\n",nbtrav,2);
	for (i=0;i<nbtrav;i++)
	{
		fprintf(fichierGenerale,"%ld %ld\n",riG[i],diG[i]);
	}
	fclose(fichierGenerale);
}

/*
	Formattage des données pour Schrage 
*/
void genererDonneesSchrage(long int nbtrav, unsigned int idMachine){
	FILE *fichierSchrage;
	fichierSchrage=fopen("donnees.pb","wt");

	fprintf(fichierSchrage,"%d %d\n",nbtrav,idMachine);

	for (int i=0;i<nbtrav;i++)
	{
 		 fprintf(fichierSchrage,"%ld %ld %ld\n",ri(idMachine,i),di(idMachine,i),pi(idMachine,i));
	}
	fclose(fichierSchrage);
}


/*
	Execution de Schrage 
*/
double executerSchrage(unsigned int idMachine){
	printf("\#\#\#\#\#\# RUNNING SchrageEtRL\n");
	spawnl(P_WAIT,"SchrageEtRL.exe","SchrageEtRL.exe",NULL);

	// On ouvre en lecture le fichier IPPrepro.txt
	ifstream inFile("SchrageEtRL.txt");
	int CiSchrage[NbMaxJob];
	double retard;
	if(inFile.is_open()) {
			int foo;
			inFile >> foo; // NBjob
			inFile >> retard; // 40
			for(int job=0; job<NbJobs ; job++){
				inFile >> CiSchrage[job];
			}
			inFile.close();
	}
	
	for(int job=0; job<NbJobs ; job++){
		setCi(idMachine, job, CiSchrage[job]);
		//printf ("Ci : %u \n", CiSchrage[job]);
	}

	printf ("Le retard sur la machine est de : %lf \n", retard);
	return retard;
}



/*
	JobShop
*/
void JobShop()
{
	unsigned int sommePIJ[NbMaxMachine];
	unsigned int reference[NbMaxMachine][2];
	
	// pour chaque machine, on calcule la somme des temps d'execution
	for(int i=0;i<NbMaxMachine;i++)
	{
		sommePIJ[i]=0;

		for(int j=0;j<NbJobs;j++)
		{
			sommePIJ[i] = sommePIJ[i] + pi(i,j);
		}
		reference[i][0]=i;
		reference[i][1]=sommePIJ[i];
	}

	// tri du tableau
	std::sort(std::begin(sommePIJ),std::end(sommePIJ));

	int nombreMachines = NbMachines;
	int nbMOrdo = 0;
	int machineOrdo[NbMaxMachine];
	double sommeRetardMax = 0;
	while(nbMOrdo<NbMachines)
	{
		int jFirst=0;

		// ********** Recherche de la machine la plus chargée et non ordonnancée ********** //
		for(int i=0;i<NbMachines;i++)
		{
			bool ordo = false;
			// on regarde si la machine a déja été ordo
			for(int j=0; j<NbMaxMachine;j++)
			{
				if(machineOrdo[j]==reference[i][0])
				{
					ordo = true;
				}
			}
			// on regarde si la machine est la plus chargée
			if(reference[i][1]==sommePIJ[nombreMachines-1] && ordo==false)
			{
				jFirst = reference[i][0];
				printf("Jfirst = %u \n",jFirst);
			}
		}
			
		// ********** Calcul des dates de débuts ********** //
		int valeurRi = 0;
		int valeurDi = 0;
		// pour chaque job
		for(int job=0;job<NbJobs ;job++)
		{
			// initialisation de la valeur de ri
			valeurRi = riG(job);
		
			// pour chaque machine
			int mach1=jFirst;
			int mach2=NbMaxMachine+1;
			int nbIte=0;
		
			// calcul des dates de débuts
			while(mach2 != mach1 && nbIte<NbMaxMachine ){
				for(int mach=0;mach<NbMaxMachine;mach++){
					// s'il y a un délai avec une autre machine
					if(Di(mach,job,mach1)!=0)
					{
						if(Ci(mach,job)!=0)
						{
							valeurRi = Ci(mach,job) + Di(mach,job,mach1);
							//printf ("calcul  %u %u %u \n", valeurRi , Ci(mach,job) ,  Di(mach,job,mach1));
							mach2=mach1;
							nbIte =NbMaxMachine ;
						}
						else
						{
							//printf("%u %u %u %u\n", job, mach1, mach, Di(mach,job,mach1));
							// calcul de la valeur de ri
							valeurRi = valeurRi + pi(mach,job) + Di(mach,job,mach1);
							//printf ("calcul  %u %u %u \n", valeurRi , pi(mach,job) ,  Di(mach,job,mach1));
							//printf ("ri %u \n", ri(mach1, job));
							mach2=mach1;
							mach1=mach;
							//printf ("\n");
						}
					}
				}
				nbIte++;
			}
			// mise a jour de la valeur du ri
			setri(jFirst ,job,valeurRi);	
			//printf("hello\n");
				
			// ********** Calcul des dates de fin ********** //

			valeurDi = diG(job);
			mach1=jFirst;
			mach2=NbMaxMachine+1;
			nbIte=0;
			while(mach2 != mach1 && nbIte<NbMaxMachine ){
				for(int mach=0;mach<NbMaxMachine;mach++){
					// s'il y a un délai avec une autre machine
					if(Di(mach1,job,mach)!=0)
					{
						if(Ci(mach,job)!=0)
						{
							valeurDi = Ci(mach,job) - Di(mach,job,mach1);
							//printf ("calcul  %u %u %u \n", valeurRi , Ci(mach,job) ,  Di(mach,job,mach1));
							mach2=mach1;
							nbIte =NbMaxMachine ;
						}
						else
						{
							//printf("%u %u %u %u\n", job, mach1, mach, Di(mach,job,mach1));
							// calcul de la valeur de ri
							valeurDi = valeurDi - pi(mach,job) - Di(mach1,job,mach);
							//printf ("calcul  %u %u %u \n", valeurRi , pi(mach,job) ,  Di(mach,job,mach1));
							//printf ("ri %u \n", ri(mach1, job));
							mach2=mach1;
							mach1=mach;
							//printf ("\n");
						}
					}
				}
				nbIte++;
			}
			// mise a jour de la valeur du ri
			setdi(jFirst ,job,valeurDi);
		}
		// ********** Application de Schrage ********** //
		genererDonneesSchrage(NbJobs, jFirst);
		double retard = executerSchrage(jFirst);
		sommeRetardMax = sommeRetardMax + retard;	
		nombreMachines --;
		machineOrdo[nbMOrdo]=jFirst;
		nbMOrdo ++;
	}
	
	printf("le retard max total est de: %lf\n", sommeRetardMax);
	
}


void main(void)
{

	 // -- GENERATION DES DONNEES -- //
	int nbjob = 20;
	int nbmachine = 3;
	double alpha,beta,a;									// The parameters to generate the instances according to the Cyril's paper
	int iStart,iEnd,iStep;
	int it;
	double dBest;

	 srand(1);

	/*for(alpha=0.2;alpha<=1.0;alpha+=0.2)
		{
			for(beta=0.2;beta<=1.0;beta+=0.2)
			{
				for(a=1.0;a<=1.1;a+=0.1)
				{
					for (it=0;it<iterations;it++)
					{
						generer(nbjob,nbmachine,alpha,beta,a);
						ReadData();
						genererGenerale(nbjob,nbmachine,alpha,beta,a);
						ReadData2();

					}
				}
			}
		}*/
		printf("fin de la generation \n");
		// Test sur l'exemple
		ReadData();
		ReadData2();
		for(int i = 0;i<NbJobs ;i++){
			printf("riG, diG %ld %ld\n",riG(i), diG(i));
		}
		printf("\n");
		for(int j=0;j<NbMachines;j++){
			for(int i = 0;i<NbJobs ;i++){
				//printf("ri, di %ld %ld\n",ri(j,i), di(j,i));
				printf("mach, job, pi %u %ld %ld\n",j, i, pi(j,i));
			}
		}
		JobShop();
		printf("Job shop \n");
		for(int j=0;j<NbMachines;j++){
			for(int i = 0;i<NbJobs ;i++){
				printf("ri, di, pi, Ci %ld %ld %ld %ld\n",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
			}
		}
		system("PAUSE");
}