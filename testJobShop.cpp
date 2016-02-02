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
	 
		 //////////////////////////////////////////

		 // création du fichier des jobs par machine

	 fichier=fopen("donnees.pb","wt");
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

	 ////////////////////////////////////////////////////////////

	 
	 ////////////////////////////////////////////////////////////
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

void genererGenerale(long int nbtrav, long int nbmach, double alpha,double beta,double a)
{
	long int i,k,j;
	 long int r,p,d,riG[NbMaxJob], diG[NbMaxJob];
	 int sum_pi;
	// création du fichier général
	 FILE *fichierGenerale;
	 fichierGenerale=fopen("donneesG.pb","wt");


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
		//printf ("ri %ld ", riG[i]);
		//printf ("di %ld \n", diG[i]);
	}
	
	fprintf(fichierGenerale,"%d %d\n",nbtrav,2);
	for (i=0;i<nbtrav;i++)
	{
		fprintf(fichierGenerale,"%ld %ld\n",riG[i],diG[i]);
	}
	fclose(fichierGenerale);

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
	// on prend la machine la plus chargée
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
						ReadData();
						genererGenerale(nbjob,nbmachine,alpha,beta,a);
						ReadData2();

					}
				}
			}
		}

		printf("fin de la generation \n");
		//ReadData();
	
		printf("Lecture des données terminée \n");
		/*for(int j=0;j<NbMachines;j++){
			for(int i = 0;i<NbJobs ;i++){
				printf("id, ri %ld %ld\n",id(j), ri(j,i));
				printf("pi , di %ld %ld\n",pi(j,i), di(j,i));
			}
		}*/
		for(int i = 0;i<NbJobs ;i++){
			printf("riG, diG %ld %ld\n",riG(i), diG(i));
		}
		//JobShop();
		system("PAUSE");
}