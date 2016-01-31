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
#define NbMaxJob 5000
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

void generer(long int nbtrav, long int nbmach, double alpha,double beta,double a)
{
	 long int i,k,j;
	 long int r,p,d,ri[NbMaxJob],di[NbMaxJob],pi[NbMaxJob],Di[NbMaxMachine][NbMaxJob][NbMaxMachine];
	 int sum_pi;
	 FILE *fichier;

	 for(i=0;i<NbMaxMachine;i++)
		for(j=0;j<NbMaxJob;j++)
			for(k=0;k<NbMaxMachine;k++)
				Di[i][j][k]=0;

	 // cr�ation du fichier g�n�ral
	 FILE *fichierGenerale;
	 fichierGenerale=fopen("donneesG.pb","wt");

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
		fprintf(fichierGenerale,"%d %d\n",nbtrav,2);
		 for (i=0;i<nbtrav;i++)
		 {
			fprintf(fichierGenerale,"%ld %ld\n",ri[i],di[i]);
		 }
		 fclose(fichierGenerale);
		 //////////////////////////////////////////

		 // cr�ation du fichier des jobs par machine

	 fichier=fopen("donnees.pb","wt");
	 int km;
	 // pour chaque machine
	 for(km=0;km<nbmach;km++)
	 {
		 // G�n�ration des temps d'execution pi
		 for(i=0;i<nbtrav;i++)
		 {
			p=((float)rand()/(float)RAND_MAX)*49+1;
			pi[i]=p;
		 }
		 // G�n�ration des dates de d�but ri
		 for(i=0;i<nbtrav;i++)
		 {
			ri[i]=0;
		 }
		 // G�n�ration des dates de fin di
		 for(i=0;i<nbtrav;i++)
		 {
			di[i]=0;
		 }
		 // G�n�ration des d�lais Di
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

 	filebuf fichier2;
	filebuf *ouvert=fichier2.open("DONNEES.DAT",ios_base::out);
	if(!ouvert)
	{
		cout<<"probleme d'ouverture du fichier "<<"DONNEES.DAT"<<endl;
		exit(1);
	}
	ostream sortie(&fichier2);
	for(int j=0;j<nbmach;j++)
	{
		for(int i=0;i<nbtrav;i++)
		{
			sortie<<j<<" "<<i+1<<" "<<ri[i]<<" "<<pi[i]<<" "<<di[i]<<" "<<Di[i]<<endl;
		}
	}
	fichier2.close();

}

void JobShop()
{
	// pour chaque machine, on calcule la somme des temps d'execution
	unsigned int sommePIJ[NbMaxMachine];
	unsigned int reference[NbMaxMachine][2];
	
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

	//affichage des valeurs
	for(int i=0;i<NbMaxMachine;i++){
		printf("%u \n",sommePIJ[i]);
	}
	for(int i=0;i<NbMaxMachine;i++){
		printf("%u %u\n",reference[i][0], reference[i][1]);
	}

	int jFirst=0;
	// on prend la machine la plus charg�e
	for(int i=0;i<NbMaxMachine;i++){
		if(reference[i][1]==sommePIJ[0])
		{
			jFirst = reference[i][0];
			printf("%u \n",jFirst);
		}
	}

	// pour chaque job
	for(int job=0;job<NbJobs ;job++)
	{
		// pour chaque machine
		for(int i=0;i<NbMaxMachine;i++){
	
			if(Di(jFirst,job,i)!=0)
			{
				
			}
		}
	}


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

		for(alpha=0.2;alpha<=1.0;alpha+=0.2)
		{
			for(beta=0.2;beta<=1.0;beta+=0.2)
			{
				for(a=1.0;a<=1.1;a+=0.1)
				{
					for (it=0;it<iterations;it++)
					{
						generer(nbjob,nbmachine,alpha,beta,a);
					}
				}
			}
		}

		printf("fin de la generation \n");
		ReadData();
		//printf("Lecture des donn�es termin�e \n");
		//JobShop();
		system("PAUSE");
}