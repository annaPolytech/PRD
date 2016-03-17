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


#include "donnees.h"
#include "donneesSchrage2.h"
#include "1riLmax.h"
using namespace std;

#define ThreadNumber 1
#define TimeLimit 900

#define UbOpt false
// Si false: La upper bound est égale à la valeur retournée par Schrage (ou Schrage+RL le cas échéant)
// Si true: La upper bound est égale à la valeur optimale du MIP (le MIP doit être exécuté avant PreMIP)

#define DEBUG false

#define NbMaxMachine 3



/*
	Execution de Schrage 
*/
double executerSchrage(unsigned int idMachine){
	
	printf("\#\#\#\#\#\# RUNNING SchrageEtRL\n");
	
	clock_t time1,time2;
	double CPUTime,dValSchrage,dValSchragePre;
	FILE *Output;

	// Reading the data from donnees.pb file
	ReadDataSchrage2(idMachine);
	time1= clock();

	dValSchrage=Schrage();


	// This upper bound is the schedule deduced from preemptive Schrage plus a local search
	 //if (dValSchragePre<dValSchrage) dValSchrage=dValSchragePre;

	 time2=clock();
	 CPUTime=diffclock(time2,time1);
	 int ci[NbMaxJob];
	int jobI[NbMaxJob];
	for(int job=0;job<NbJobs;job++){
		ci[job] = CiS(job);
		jobI[job] = jobInit(job);
		setCi(idMachine, jobI[job], ci[job]);
	}
	
	printf ("Le retard sur la machine est de : %lf \n", dValSchrage);
	return dValSchrage;
}



/*
	JobShop
*/
double JobShop()
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
				//printf("Jfirst = %u \n",jFirst);
			}
		}
			
		// ********** Calcul des dates de débuts ********** //
		int valeurRi = 0;
		int valeurDi = 0;
		// pour chaque job
		for(int job=0;job<NbJobs ;job++)
		{
			
			// initialisation de la valeur de ri
			valeurRi = 0;
		
			// pour chaque machine
			int mach1=jFirst;
			int mach2=NbMaxMachine+1;
			int nbIte=0;
			int jGamme = 0;
			int machPrec = 0;
			bool Cexiste=false;
		
			// recherche de la position de la machine dans la gamme du job
			while(mach2 != mach1 && nbIte<NbMaxMachine ){
				for(int mach=0; mach<NbMaxMachine;mach++){
					if(gamme(job,mach)==mach1)
					{
						jGamme = mach;
					//	printf("jGamme = %u \n",jGamme);
					}
				}

				// si c'est la 1ere machine
				if(jGamme == 0){
					valeurRi = valeurRi + riG(job);
					mach1=mach2;
				}else{

					machPrec = gamme(job,jGamme-1);
			
					// si la machine précédente est deja ordonnancée
					if(Ci(machPrec,job)!=0)
					{
						if(nbIte <1){
							valeurRi = Ci(machPrec,job) + delai(job,jGamme-1);
							nbIte =NbMaxMachine ;
						}else{
							valeurRi = valeurRi+ Ci(machPrec,job) + delai(job,jGamme-1);
							nbIte =NbMaxMachine ;
						}
						Cexiste=true;
					}
					else
					{
						// calcul de la valeur de ri
						valeurRi = valeurRi + pi(machPrec,job) + delai(job,jGamme-1);
						mach2=mach1;
						mach1=machPrec;
					}
					nbIte++;
				}
			}
			

			// mise a jour de la valeur du ri
			setri(jFirst ,job,valeurRi);	
				
			// ********** Calcul des dates de fin ********** //

			valeurDi = 0;
			mach1=jFirst;
			mach2=NbMaxMachine+1;
			nbIte=0;
			Cexiste=false;
			int machSuiv=0;


			// recherche de la position de la machine dans la gamme du job
		  

			while(mach2 != mach1 && nbIte<NbMaxMachine ){
				for(int mach=0; mach<NbMaxMachine;mach++){
					if(gamme(job,mach)==mach1)
					{
						jGamme = mach;
						//printf("jGamme = %u \n",jGamme);
					}
				}

				// si c'est la derniere machine
				if(jGamme == NbMaxMachine-1){
					valeurDi = diG(job) + valeurDi;
					mach1=mach2;
				}else{

					machSuiv = gamme(job,jGamme+1);
			
					// si la machine suivante est deja ordonnancée
					if(Ci(machSuiv,job)!=0)
					{
						if(nbIte<1){
							valeurDi = Ci(machSuiv,job) - pi(machSuiv,job) - delai(job,jGamme);
							nbIte =NbMaxMachine ;
						}else{
							valeurDi = Ci(machSuiv,job) - pi(machSuiv,job) -  delai(job,jGamme) + valeurDi;
							nbIte =NbMaxMachine ;
						}
						Cexiste=true;
					}
					else
					{
						// calcul de la valeur de di
						valeurDi = valeurDi - pi(machSuiv,job) - delai(job,jGamme);
						mach2=mach1;
						mach1=machSuiv;
					}
					nbIte++;
				}
			}
			// mise a jour de la valeur du di
			setdi(jFirst ,job,valeurDi);
		}
	/*	printf("avantSchrage\n");
			printf("ri di\n");
		for(int job4=0;job4<NbMaxJob;job4++){
			
			printf("%u %u\n",ri(jFirst,job4), di(jFirst,job4));
		}
		*/
		// ********** Application de Schrage ********** //
		//genererDonneesSchrage(NbJobs, jFirst);
		double retard = executerSchrage(jFirst);
		sommeRetardMax = sommeRetardMax + retard;	
		nombreMachines --;
		machineOrdo[nbMOrdo]=jFirst;
		nbMOrdo ++;
	}
	printf("le retard max total est de: %lf\n", sommeRetardMax);
	return sommeRetardMax;
		
}

void miseAJourMachine(int jobActuel, int pos[NbMaxMachine][NbMaxJob], int suivant){
	bool retard2 = true;
	int nbTrav = 0; 
	int positionJobACtu = 0;
	int jobApres = 0;
	int riSuivant = 0;
	int valCi = 0;
	nbTrav = pos[gamme(jobActuel,suivant)][jobActuel];
	while(retard2==true && nbTrav<NbMaxJob){
		// on cherche la position du job sur la nouvelle machine
		positionJobACtu = pos[gamme(jobActuel,suivant)][jobActuel];
		if(positionJobACtu<NbMaxJob-1){
			// on cherche le job dans la position suivante
			for(int mach=0; mach<NbMaxMachine; mach++){
				for(int l=0; l<NbMaxJob; l++){
					// si pos = pos+1
					if(pos[gamme(jobActuel,suivant)][l]==positionJobACtu+1){
						jobApres = l;
					}
				}
			}
								
			// calcul de ri du job suivant sur la machine
			riSuivant = Ci(gamme(jobActuel,suivant),jobApres) - pi(gamme(jobActuel,suivant),jobApres);
			if(riSuivant<Ci(gamme(jobActuel,suivant),jobActuel)){
				// mise a jour du Ci du job d'apres
				valCi = Ci(gamme(jobActuel,suivant),jobActuel) + pi(gamme(jobActuel,suivant),jobApres);
				setCi(gamme(jobActuel,suivant),jobApres,valCi);
				//printf("MAJ mach - > job = %u machine = %u valeurCi = %u", jobApres,gamme(jobActuel,suivant), valCi);

				//printf("mise a jour machine %ld %ld",jobApres, valCi);
				jobActuel = jobApres;
				nbTrav++;

			}else{
				// fin
				retard2=false;
			}
		}else{
			retard2=false;
		}
	}
	
}
 

void miseAJour(){
	int suivant = 0;
	int jGamme = 0;
	int dateDeb = 0;
	int datePossible = 0;
	int machAv = 0;
	int valCi = 0;
	int nbMachine = 0;
	bool retard = true;
	bool retard2=true;
	int nbTrav = 0;
	int riSuivant = 0;

	// pour chaque machine
	int triRi[NbMaxMachine][NbMaxJob];
	int reference[NbMaxMachine][NbMaxJob][2];
	int pos[NbMaxMachine][NbMaxJob];

	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbMaxJob;job++){
			triRi[machine][job] = Ci(machine,job)-pi(machine,job);
			reference[machine][job][1]=triRi[machine][job];
			reference[machine][job][0]=job;
		}
		std::sort(std::begin(triRi[machine]),std::end(triRi[machine]));
	}


	// pour chaque machine
	for(int machine=0;machine<NbMaxMachine;machine++){
		//pour chaque position
		for(int posi=0;posi<NbMaxJob;posi++){
			// pour chaque i
			for(int i=0;i<NbMaxJob;i++){
				if(reference[machine][i][1] == triRi[machine][posi]){
					pos[machine][reference[machine][i][0]] = posi;
				}
			}
		}
	}


	int jobInit = 0;
	// pour chaque  job trié par ri croissant
	for(int i=0; i<NbMaxJob; i++){
		for(int machine=0; machine<NbMaxMachine; machine++){
			// récupération du job
			for(int job=0;job<NbMaxJob;job++){
				if(reference[machine][job][1]==triRi[machine][i]){
					// on trouve le job 
					jobInit = reference[machine][job][0];
				}
			}

			// si le job est en retard
			if(Ci(machine,jobInit)>di(machine,jobInit) || (Ci(machine,jobInit)-pi(machine,jobInit))<ri(machine,jobInit)){
			//if(pi(machine,jobInit)!=0){
				// recherche de la place de la machine dans la gamme
				for(int oper=0; oper<NbMaxMachine ; oper++){
					if(gamme(jobInit,oper)==machine){
						jGamme = oper;
					}
				}
				// on a la machine actuelle 
				machAv = machine;
				// numéro de l'opération dans la gamme du job
				nbMachine = jGamme;
				// numéro suivant de l'opération
				suivant = jGamme + 1;

				// pour les autres machines suivantes dans la gamme du job
				while(nbMachine<NbMaxMachine && suivant<NbMaxMachine && retard == true){
					if(suivant==0){
						// calcul date deb
						dateDeb = Ci(gamme(jobInit,suivant),jobInit) - pi(gamme(jobInit,suivant),jobInit);
						// calcul date possible
						datePossible = Ci(machAv,jobInit);
					}else{
						// calcul date deb
						dateDeb = Ci(gamme(jobInit,suivant),jobInit) - pi(gamme(jobInit,suivant),jobInit);
						// calcul date possible
						datePossible = Ci(machAv,jobInit)+delai(jobInit,suivant-1);  
					}
					

					// si le job sexecute avant de le pouvoir on le décale ainsi que les autres
					if(dateDeb < datePossible){
						// mise a jour de la date de fin
						valCi = datePossible + pi(gamme(jobInit,suivant),jobInit);
						setCi(gamme(jobInit,suivant),jobInit,valCi);
						//printf("mise a jour job %ld - %ld - %ld",gamme(jobInit,suivant), jobInit, valCi);
				
						miseAJourMachine(jobInit, pos, suivant);

						// mise a jour des variable
						machAv = gamme(jobInit,suivant);
						suivant++;
						nbMachine++;
					}
					else{
						retard = false;
					}
				}
			}
		}
	}
}


int ordoSansMinCr(int jMinCr, int *jobsRetires,int nbJobRet){
	int nbRet = 0;
	/** Récuperation de la position des jobs sur les machines **/

	int triRi[NbMaxMachine][NbMaxJob];
	int reference[NbMaxMachine][NbMaxJob][2];
	int pos[NbMaxMachine][NbMaxJob];

	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			triRi[machine][job] = Ci(machine,job)-pi(machine,job);
			reference[machine][job][1]=triRi[machine][job];
			reference[machine][job][0]=job;
		}
		std::sort(std::begin(triRi[machine]),std::end(triRi[machine]));
	}

	// pour chaque machine
	for(int machine=0;machine<NbMaxMachine;machine++){
		//pour chaque position
		for(int posi=0;posi<NbJobs;posi++){
			// pour chaque i
			for(int i=0;i<NbJobs;i++){
				if(reference[machine][i][1] == triRi[machine][posi]){
					pos[machine][reference[machine][i][0]] = posi;
				}
			}
		}
	}

	/** Décalage à gauche **/
	int position = 0;
	int jobSuivant = 0;
	int operation = 0;
	int ordoOk = true;
	int posi = 0;
	int riSuivant = 0;
	int CiJobSuivant = 0;
	int CiJobAvant = 0;

	for(int machine=0;machine<NbMaxMachine;machine++){
		ordoOk = true;
	
		// recherche de la position du job 
		position = pos[machine][jMinCr];

		//printf("position = %u\n", position);
		posi = position +1;
		

		// pour chaque job suivant s'il y en a
		while(posi<NbJobs && ordoOk==true){
			
			/** recherche du job correspondant à la position **/
			for(int job = 0; job<NbJobs; job++){
				if(pos[machine][job]==posi){
					jobSuivant = job;
				}
			}
			

			/** recherche de la position du job dans sa gamme **/
			for(int ope = 0; ope<NbMaxMachine; ope++){
				if(gamme(jobSuivant,ope)==machine){
					operation = ope;
				}
			}
			//printf("posi = %u job = %u ope = %u\n", posi, jobSuivant, operation);

			/** recherche du job avant correspondant à la position **/
			if(posi == position+1){
				if(posi == 1){
					CiJobAvant = riG(jobSuivant);
				}else{
					for(int job = 0; job<NbJobs; job++){
						if(pos[machine][job]==posi-2){
							CiJobAvant = Ci(machine, job);
						}
					}
				}
			}else{
				for(int job = 0; job<NbJobs; job++){
					if(pos[machine][job]==posi-1){
						CiJobAvant = Ci(machine, job);
					}
				}
			}
			
			CiJobSuivant= CiJobAvant + pi(machine,jobSuivant);

			if(operation == 0){
				// si riSuiv - piMin > riG
				if(CiJobAvant > riG(jobSuivant)){
					setCi(machine,jobSuivant,CiJobSuivant);
					posi++;
				}else{
					CiJobSuivant = riG(jobSuivant) + pi(machine, jobSuivant);
					setCi(machine,jobSuivant,CiJobSuivant);
					posi++;
				}
			}else{
				setCi(machine,jobSuivant,CiJobSuivant);
				posi++;
			}
		}
		// mise a 0 des donnees du job min Cr 
		setri(machine,jMinCr,999);
		setdi(machine,jMinCr,999);
		setpi(machine,jMinCr,0);
		setCi(machine,jMinCr,999);
		setCoutRejet(jMinCr,999);
	}
	//printf("nombre de job retirés OrdoSansMin= %u", nbJobRet);
	/** Affichage jobs retires**/
	/*printf("jobs retires\n");
	for(int jobRet = 0; jobRet<NbJobs; jobRet++){
		printf("%u\n",jobsRetires[jobRet]);
	}*/

	jobsRetires[nbJobRet]=jMinCr;
	//nbJobRet=nbJobRet+1;
	nbRet = nbJobRet+1;
	return nbRet;
}


void calculDate(int jMinCr, int *jobsRetires, int nbJobRet){
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
				//printf("Jfirst = %u \n",jFirst);
			}
		}
			
		// ********** Calcul des dates de débuts ********** //
		int valeurRi = 0;
		int valeurDi = 0;
		bool dejaRet = false;
		// pour chaque job
		for(int job=0;job<NbJobs ;job++)
		{
			dejaRet = false;
			// verification que le job ne soit pas retire
			for(int jobR=0; jobR<nbJobRet; jobR++){
				if(job == jobsRetires[jobR]){
					dejaRet = true;
				}
			}
			if(dejaRet==false){
			// initialisation de la valeur de ri
			valeurRi = 0;
		
			// pour chaque machine
			int mach1=jFirst;
			int mach2=NbMaxMachine+1;
			int nbIte=0;
			int jGamme = 0;
			int machPrec = 0;
			bool Cexiste=false;
		
			// recherche de la position de la machine dans la gamme du job
			while(mach2 != mach1 && nbIte<NbMaxMachine ){
				for(int mach=0; mach<NbMaxMachine;mach++){
					if(gamme(job,mach)==mach1)
					{
						jGamme = mach;
						//printf("jGamme = %u \n",jGamme);
					}
				}

				// si c'est la 1ere machine
				if(jGamme == 0){
					valeurRi = valeurRi + riG(job);
					mach1=mach2;
				}else{

					machPrec = gamme(job,jGamme-1);
			
					// si la machine précédente est deja ordonnancée
				/*	if(Ci(machPrec,job)!=0)
					{
						if(nbIte <1){
							valeurRi = Ci(machPrec,job) + delai(job,jGamme-1);
							nbIte =NbMaxMachine ;
						}else{
							valeurRi = valeurRi+ Ci(machPrec,job) + delai(job,jGamme-1);
							nbIte =NbMaxMachine ;
						}
						Cexiste=true;
					}
					else
					{*/
						// calcul de la valeur de ri
					valeurRi = valeurRi + pi(machPrec,job) + delai(job,jGamme-1);
					mach2=mach1;
					mach1=machPrec;
					//}
					nbIte++;
				}
			}
			

			// mise a jour de la valeur du ri
			setri(jFirst ,job,valeurRi);	
				
			// ********** Calcul des dates de fin ********** //

			valeurDi = 0;
			mach1=jFirst;
			mach2=NbMaxMachine+1;
			nbIte=0;
			Cexiste=false;
			int machSuiv=0;


			// recherche de la position de la machine dans la gamme du job
		  

			while(mach2 != mach1 && nbIte<NbMaxMachine ){
				for(int mach=0; mach<NbMaxMachine;mach++){
					if(gamme(job,mach)==mach1)
					{
						jGamme = mach;
						//printf("jGamme = %u \n",jGamme);
					}
				}

				// si c'est la derniere machine
				if(jGamme == NbMaxMachine-1){
					valeurDi = diG(job) + valeurDi;
					mach1=mach2;
				}else{

					machSuiv = gamme(job,jGamme+1);
			
					// si la machine suivante est deja ordonnancée
					/*if(Ci(machSuiv,job)!=0)
					{
						if(nbIte<1){
							valeurDi = Ci(machSuiv,job) - pi(machSuiv,job) - delai(job,jGamme);
							nbIte =NbMaxMachine ;
						}else{
							valeurDi = Ci(machSuiv,job) - pi(machSuiv,job) -  delai(job,jGamme) + valeurDi;
							nbIte =NbMaxMachine ;
						}
						Cexiste=true;
					}
					else
					{*/
						// calcul de la valeur de di
						valeurDi = valeurDi - pi(machSuiv,job) - delai(job,jGamme);
						mach2=mach1;
						mach1=machSuiv;
					//}
					nbIte++;
				}
			}
			// mise a jour de la valeur du di
			setdi(jFirst ,job,valeurDi);
			}
		}

		// ********** Application de Schrage ********** //
		//genererDonneesSchrage(NbJobs, jFirst);
		//double retard = executerSchrage(jFirst);
		//sommeRetardMax = sommeRetardMax + retard;	
		nombreMachines --;
		machineOrdo[nbMOrdo]=jFirst;
		nbMOrdo ++;
	}
/*	printf("calculDate \n");
	for(int j=0;j<NbMachines;j++){
		for(int i = 0;i<NbJobs ;i++){
			printf("ri, di, pi, Ci %ld | %ld | %ld | %ld |",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
			printf(" \n");
		}
		printf(" \n");
	}*/


	//printf("nombre de job retirés calculDate = %u", nbJobRet);
	/** Affichage jobs retires**/
	/*printf("jobs retires\n");
	for(int jobRet = 0; jobRet<NbJobs; jobRet++){
		printf("%u\n",jobsRetires[jobRet]);
	}*/

}

void miseAJourDecision(int jMinCr,int *jobsRetires, int nbJobRet){
	int jGammeSuivant1 = 0;
	int jGamme1 = 0;
	int dateDeb1 = 0;
	int datePossible1 = 0;
	int machActu1 = 0;
	int valCi1 = 0;
	int nbMachine1 = 0;
	bool retard1 = true;
	bool retard3=true;
	int nbTrav1 = 0;
	int riSuivant1 = 0;

	// pour chaque machine
	int triRi1[NbMaxMachine][NbMaxJob];
	int reference1[NbMaxMachine][NbMaxJob][2];
	int pos1[NbMaxMachine][NbMaxJob];

	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			triRi1[machine][job] = Ci(machine,job)-pi(machine,job);
			reference1[machine][job][1]=triRi1[machine][job];
			reference1[machine][job][0]=job;
		}
		std::sort(std::begin(triRi1[machine]),std::end(triRi1[machine]));
	}


	// pour chaque machine
	for(int machine=0;machine<NbMaxMachine;machine++){
		//pour chaque position
		for(int posi1=0;posi1<NbJobs;posi1++){
			// pour chaque i
			for(int i=0;i<NbJobs;i++){
				if(reference1[machine][i][1] == triRi1[machine][posi1]){
					pos1[machine][reference1[machine][i][0]] = posi1;
				}
			}
		}
	}


	int jobInit = 0;
	bool dejaRet = false;
	// pour chaque  job trié par ri croissant
	for(int position1=0; position1<NbJobs; position1++){
		for(int machine=0; machine<NbMaxMachine; machine++){
		//	printf("machine = %u\n", machine);
			// récupération du job
			for(int job=0;job<NbJobs;job++){
				dejaRet = false;
				// verification que le job ne soit pas retire
				for(int jobR=0; jobR<nbJobRet; jobR++){
					if(job == jobsRetires[jobR]){
						dejaRet = true;
					}
				}
				if(dejaRet==false){
					if(reference1[machine][job][1]==triRi1[machine][position1]){
						// on trouve le job 
						jobInit = reference1[machine][job][0];
					}
				}
			}
		
	
			//printf("job = %u machine = %u\n",jobInit, machine);
			// recherche de la place de la machine dans la gamme
			for(int oper=0; oper<NbMaxMachine ; oper++){
				if(gamme(jobInit,oper)==machine){
					jGamme1 = oper;
				}
			}

				
			// on a la machine actuelle 
			machActu1 = machine;
			// numéro de l'opération dans la gamme du job
			nbMachine1 = jGamme1;
			// numéro suivant de l'opération
			jGammeSuivant1 = jGamme1 + 1;
			//printf("nbMach = %u jGammeSuiv = %u \n",nbMachine1,jGammeSuivant1 );
			retard1 = true;
			// pour les autres machines suivantes dans la gamme du job
			while(nbMachine1<NbMaxMachine && jGammeSuivant1<NbMaxMachine && retard1 == true){
			//	printf("JGammeSuivant = %u\n",jGammeSuivant1);
			//	printf("job = %u machine = %u\n",jobInit, gamme(jobInit,jGammeSuivant1));
			
				// calcul date deb
				dateDeb1 = Ci(gamme(jobInit,jGammeSuivant1),jobInit) - pi(gamme(jobInit,jGammeSuivant1),jobInit);
				// calcul date possible
				datePossible1 = Ci(machActu1,jobInit)+delai(jobInit,jGammeSuivant1-1);  
			//	printf("if dateDeb = %u - %u = %u \n",Ci(gamme(jobInit,jGammeSuivant1),jobInit), pi(gamme(jobInit,jGammeSuivant1),jobInit), dateDeb1);
			//	printf("if datePossible = %u + %u = %u \n",Ci(machActu1,jobInit), delai(jobInit,jGammeSuivant1-1), datePossible1);					

				// si le job sexecute avant de le pouvoir on le décale ainsi que les autres
				if(dateDeb1 < datePossible1){
					// mise a jour de la date de fin
					valCi1 = datePossible1 + pi(gamme(jobInit,jGammeSuivant1),jobInit);
				//	printf("valeurCi = %u\n",valCi1);
					setCi(gamme(jobInit,jGammeSuivant1),jobInit,valCi1);
					//printf("mise a jour job %ld - %ld - %ld",gamme(jobInit,suivant), jobInit, valCi);
				
					miseAJourMachine(jobInit, pos1, jGammeSuivant1);

					// mise a jour des variable
					machActu1 = gamme(jobInit,jGammeSuivant1);
					jGammeSuivant1++;
					nbMachine1++;
				}
				else{
					retard1 = false;
				}
			}
			
		}
	}
}


//int *** priseDeDecision1(){
void priseDeDecision1(int *jobsRetires, int nbJobRet){
	
	/*** CREATION DES CONTRAINTES DE PRECEDENCES ***/
	int listePrecedent[NbMaxJob][NbMaxMachine][6];
	int jOp = 0;
	
	// Contraintes de gamme
	for(int job=0; job<NbJobs; job++){
		for(int machine=0; machine<NbMaxMachine; machine++){
			for(int op=0; op<NbMaxMachine; op++){
				if(gamme(job,op)==machine){
					jOp = op;
					
					// si c'est pas la 1ere machine
					if(jOp!=0){
						if(jOp<NbMaxMachine){
							listePrecedent[job][machine][0]= job;
							listePrecedent[job][machine][1]= gamme(job,op-1);
							listePrecedent[job][machine][2]= pi(gamme(job,op-1),job)+delai(job,op-1);
						}else{
							listePrecedent[job][machine][0]= job;
							listePrecedent[job][machine][1]= gamme(job,op-1);
							listePrecedent[job][machine][2]= pi(gamme(job,op-1),job);

						}
					// si la machine est en 1ere place
					}else{
						listePrecedent[job][machine][0]= job;
						listePrecedent[job][machine][1]= machine;
						listePrecedent[job][machine][2]= riG(job);
					}
				}
			}
		}
	}

	// Contrainte d'ordonnancement

	// Recherche de la position du job sur la machine
	int triRiMachine[NbMaxMachine][NbMaxJob];
	int referenceMachine[NbMaxMachine][NbMaxJob][2];
	int posMachine[NbMaxMachine][NbMaxJob];

	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			triRiMachine[machine][job] = Ci(machine,job)-pi(machine,job);
			referenceMachine[machine][job][0]=job;
			referenceMachine[machine][job][1]=triRiMachine[machine][job];
		}
		std::sort(std::begin(triRiMachine[machine]),std::end(triRiMachine[machine]));
	}


	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int posi=0;posi<NbJobs;posi++){
			for(int i=0;i<NbJobs;i++){
				if(referenceMachine[machine][i][1] == triRiMachine[machine][posi]){
					posMachine[machine][referenceMachine[machine][i][0]] = posi;
				}
			}
		}
	}


	int position = 0;
	int jobPrec = 0;
	int duree = 0;

	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			duree = 0;
			// on récupere la position du job 
			position = posMachine[machine][job];

			if(position!=0){
				for(int j = 0; j<NbJobs; j++){
					if(posMachine[machine][j]==position-1){
						jobPrec = j;
					}
				}
				// calcul de la distance entre les 2
				duree =  pi(machine, jobPrec);
				listePrecedent[job][machine][3]= jobPrec;
				listePrecedent[job][machine][4]= machine;
				listePrecedent[job][machine][5]= duree;
			}else{
				listePrecedent[job][machine][3]= job;
				listePrecedent[job][machine][4]= machine;
				listePrecedent[job][machine][5]= duree;
			}
		}
	}

	// AFFICHAGE
/*	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			printf("job : %u machine : %u\n",job, machine); 
			printf("pred g : %u ; %u ; %u\n",listePrecedent[job][machine][0], listePrecedent[job][machine][1], listePrecedent[job][machine][2]); 
			printf("pred o : %u ; %u ; %u\n",listePrecedent[job][machine][3], listePrecedent[job][machine][4], listePrecedent[job][machine][5]); 

		}
	}*/


	/**** CALCUL DES RANGS ****/

	int rang[NbMaxJob][NbMaxMachine];
	int listeJobDejaRange[NbMaxMachine][NbMaxJob];

	// initialisation de la liste des jobs déja rangés
	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			listeJobDejaRange[machine][job]=0;
			rang[machine][job]=0;
		}
	}
	int ite = 0;
	int i1=0;
	int j1=0;
	int i2=0;
	int j2=0;
	int ran = 1;
	int maxRang = 1;
	int listeRang[NbMaxJob][2]; 
	int i; 

	while(ite<NbJobs*NbMaxMachine){
		// INITIALISATION
		i = 0;
		// initialisation listeRang
		for(int i=0;i<NbMaxMachine;i++){
			listeRang[i][0]=0;
			listeRang[i][1]=0;
		}

		// CALCUL
		for(int machine=0;machine<NbMaxMachine;machine++){
			for(int job=0;job<NbJobs;job++){ 
				// si le job n'a pas déja un rang
				if(listeJobDejaRange[machine][job]==0){
					// si pas de prédecesseur rang = 0
					if(listePrecedent[job][machine][0] == job &&  listePrecedent[job][machine][1]== machine && listePrecedent[job][machine][3] == job &&  listePrecedent[job][machine][4]== machine){
						rang[job][machine]=1;
						listeJobDejaRange[machine][job] = 1;
						i++;
					// si il a au moins 1 predecesseur
					}else{
						i1 = listePrecedent[job][machine][0];
						j1 = listePrecedent[job][machine][1];
						i2 = listePrecedent[job][machine][3];
						j2 = listePrecedent[job][machine][4];

						// si les jobs sont deja rangés
						int nbDeja = 0;

						// si son pred gamme a un rang ou si c'est le job lui-meme
						if(listeJobDejaRange[j1][i1] == 1 || (j1 == machine && i1==job)){
							nbDeja ++;
						}
						// si son pred gamme a un rang ou si c'est le job lui-meme
						if(listeJobDejaRange[j2][i2] == 1 || (j2 == machine && i2==job)){
							nbDeja ++;
						}

						// si c'est deux  predecesseurs ont un rang connu
						if(nbDeja == 2){
							// mise à jour du max des rangs
							if(ran > maxRang){
								maxRang = ran; 
							}
							// ajout du job à la liste des jobs auxquels il faut affecter ce rang
							rang[job][machine]=ran;
							listeRang[i][0] = job;
							listeRang[i][1] = machine;
							i++;							
						}
					}
				}
			}
		}
		// on met à jour la liste des jobs rangés
		if(ite!=0){
			for(int k=0; k<i;k++){
				listeJobDejaRange[listeRang[k][1]][listeRang[k][0]] = 1;
			}
		}
		ran= ran + 1;
		ite = ite + i;
		
	}
	
	 // AFFICHAGE
	/*printf("jo || ma || rang  \n");
	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			printf(" %u || %u || %u \n",job, machine, rang[job][machine]);
		}
		printf("\n");
	}*/

	/*** CALCUL DES DATES EST ***/
	int EST[NbMaxJob][NbMaxMachine]; 
	int prec[NbMaxJob][NbMaxMachine][2]; 
	
	//int* prec[NbMaxJob][NbMaxMachine][2] = new int[NbMaxJob][NbMaxMachine][2];
	//int ***prec; 

	//INITIALISATION
	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			EST[job][machine] = 0;
		}
	}
	
	//printf("max = %u\n",maxRang);
	// Parcours par rang croissant
	for(int r=1; r<maxRang+1; r++){
		//printf("r = %u\n",r);
		for(int machine=0;machine<NbMaxMachine;machine++){
			//printf("machine = %u\n",machine);
			for(int job=0;job<NbJobs;job++){
				//printf("job = %u \n", job);
				// si le job est de ce rang on calcule EST
				if(rang[job][machine]==r){
					//printf("Rang \n"); 
					if(r==1){
						EST[job][machine] = riG(job);
						prec[job][machine][0] = job;
						prec[job][machine][1] = machine; 
						//printf("EST \n");
					// si le job a des predecesseurs
					}else{
						//printf("EST2 \n");
						// si la gamme engendre la date de début
						if(EST[job][machine] < EST[listePrecedent[job][machine][0]][listePrecedent[job][machine][1]] + listePrecedent[job][machine][2]){
							EST[job][machine] = EST[listePrecedent[job][machine][0]][listePrecedent[job][machine][1]]+ listePrecedent[job][machine][2];
							prec[job][machine][0] = listePrecedent[job][machine][0];
							prec[job][machine][1] = listePrecedent[job][machine][1]; 
						}
						// si l'ordo engendre la date de début
						if(EST[job][machine] < EST[listePrecedent[job][machine][3]][listePrecedent[job][machine][4]] + listePrecedent[job][machine][5]){
							EST[job][machine] = EST[listePrecedent[job][machine][3]][listePrecedent[job][machine][4]]+ listePrecedent[job][machine][5];
							prec[job][machine][0] = listePrecedent[job][machine][3];
							prec[job][machine][1] = listePrecedent[job][machine][4]; 
						}
					}
				}
			}
		}
	}
//	printf(" fin boucle \n");

	 // AFFICHAGE
	/*printf("jo || ma || gam || ord || rang || EST || predecesseur \n");
	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbMaxJob;job++){
			printf(" %u || %u || %u %u || %u %u || %u || %u ",job, machine, listePrecedent[job][machine][0], listePrecedent[job][machine][1],listePrecedent[job][machine][3], listePrecedent[job][machine][4],rang[job][machine], EST[job][machine]);
			printf("|| %u %u\n",prec[job][machine][0],prec[job][machine][1]);
		}
		printf("\n");
	}
	*/
	
	
	/*** Recherche du job de cout min et de celui qui a le plus d'impact ***/
	int ciJob = 0;
	int diJob = 0;
	bool retire = false; 
	// Pour tout les jobs
	for(int job=0;job<NbJobs;job++){
		retire = false; 

		ciJob = Ci(gamme(job,NbMaxMachine-1),job);
		diJob = diG(job);
		//printf("job = %u et Ci = %u\n",job, ciJob);
		
		// Verifier que le job n'est pas déja supprimer
		for(int jobR=0; jobR<nbJobRet; jobR++){
		printf("job : %u et TabjobR: %u \n", job, jobsRetires[jobR]);
			if(job == jobsRetires[jobR]){
				printf("job ret \n\n");
				retire = true; 
			}
		}
		printf("jobs retires deb priseDec\n");
		for(int jobRet = 0; jobRet<NbJobs; jobRet++){
			printf("%u\n",jobsRetires[jobRet]);
		}
		printf("nombre de job retirés = %u \n", nbJobRet);
		// si le job est en retard
		if(ciJob>diJob && retire == false){
			printf("job en retard = %u\n",job);
			int tabSommeDuree[NbMaxJob];
			int machine1 = gamme(job,NbMaxMachine-1);
			int machine2 = 0;
			int minCoutR = 99999;
			int job1 = job;
			int job2 = 0;
			tabSommeDuree[job] = pi(machine1, job);
			// tant que le precedent n'est pas le job lui meme
			while((prec[job1][machine1][0] != job1) || (prec[job1][machine1][1] != machine1)){
				//printf(" job1 : %u machine : %u\n", job1, machine1);
				//printf(" prec %u %u\n", prec[job1][machine1][0], prec[job1][machine1][1]);
				tabSommeDuree[prec[job1][machine1][0]] = tabSommeDuree[prec[job1][machine1][0]] +  pi(prec[job1][machine1][1], prec[job1][machine1][0]);
				job2 = prec[job1][machine1][0];
				machine2 = prec[job1][machine1][1];
				job1 = job2;
				machine1 = machine2;
			}
			int jobMaxSommeDi = 0;
			for(int jobTab=0;jobTab<NbJobs;jobTab++){
				if(tabSommeDuree[jobTab]>tabSommeDuree[jobMaxSommeDi]){
					jobMaxSommeDi = jobTab;
				}
				//printf(" sommeDi du job %u = %u\n", jobTab, tabSommeDuree[jobTab]);
			}
			int jobMinCr = 0;
			for(int jobTab=0;jobTab<NbJobs;jobTab++){
				if(coutR(jobTab)<coutR(jobMinCr)){
					jobMinCr = jobTab;
				}
			}
			printf(" job min cout rejet = %u CR = %u\n", jobMinCr, coutR(jobMinCr));
			printf(" job max somme Di = %u Di = %u\n",jobMaxSommeDi, tabSommeDuree[jobMaxSommeDi]);
			int nbJobSupp = ordoSansMinCr(jobMinCr,jobsRetires,nbJobRet);
			printf("resultat ordoSansMin = %u\n",nbJobSupp); 
			nbJobRet = nbJobSupp;
			printf("resultat nbJobRet  = %u\n\n",nbJobRet); 

			/** Affichage de résultat la prise de décision **/
			/*printf("nouvelleOrdo \n");
			for(int j=0;j<NbMachines;j++){
				for(int i = 0;i<NbJobs ;i++){
					printf("ri, di, pi, Ci %ld | %ld | %ld | %ld |",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
					printf(" \n");
				}
				printf(" \n");
			}*/

			/** Calcule des dates **/
			calculDate(jobMinCr,jobsRetires,nbJobRet);
			printf("resultat nbJobRetCal  = %u\n\n",nbJobRet); 
			/** Mise a jour du résultat de la prise de décision **/
			miseAJourDecision(jobMinCr,jobsRetires,nbJobRet);
			printf("resultat nbJobRetMAJ  = %u\n\n",nbJobRet); 
			/** Affichage de la mise a jour du résultat de la prise de décision **/
			/*printf("Apres mise a jour \n");
			for(int j=0;j<NbMachines;j++){
				for(int i = 0;i<NbJobs ;i++){
					printf("ri, di, pi, Ci %ld | %ld | %ld | %ld |",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
					printf(" \n");
				}
				printf(" \n");
			}*/
		}
	}
}

void recherche(int *** prec){
	 // AFFICHAGE
	printf("RECHERCHE \n\n");
	printf("jo || ma || predecesseur \n");
	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int job=0;job<NbJobs;job++){
			printf(" %u || %u",job, machine);
			printf(" || %u %u\n",prec[job][machine][0],prec[job][machine][1]);
		}
		printf("\n");
	}
}



void main(void)
{
	clock_t time1,time2;
	double CPUTime;
	// -- GENERATION DES DONNEES -- //
	//printf("ca commence");
	FILE *output;
	srand(1);
	int ciJob = 0;
	int diJob = 0;
	/** Lecture des données **/
	ReadData();
	ReadData2();

	/*for(int i=0; i<NbMaxJob;i++){
		printf("riG :  %ld \n", riG(i));
	}*/

	time1= clock();

	/** Execution de Jobshop **/
	double sommeRet = JobShop();
		
	/** Affichage résultat de Jobshop **/
	/*printf("Job shop \n");
	for(int j=0;j<NbMachines;j++){
		for(int i = 0;i<NbMaxJob ;i++){
			printf("ri, di, pi, Ci %ld | %ld | %ld | %ld |",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
			printf(" \n");
		}
		printf(" \n");
	}*/
	

	/** Execution de mise a jour résultat jobshop **/
	miseAJour();
	time2=clock();
	CPUTime=diffclock(time2,time1);

	printf("nbJob %u\n",NbJobs);
	/** Remplir jobshopTXT**/
	int nbRetard = 0;
	int maxCi = 0;
	for(int job=0;job<NbMaxJob;job++){
		maxCi = 0;
		for(int mach=0;mach<NbMaxMachine;mach++){
			if(Ci(mach,job)>maxCi){
				maxCi = Ci(mach,job);
			}
		}
		//printf("Cmax : %u \n", maxCi);
		if(maxCi >diG(job)){
			nbRetard++;
		}
	}
	output=fopen("jobshop.txt","wt");
	fprintf(output,"%u\n",nbRetard);
	fprintf(output,"%lf\n",CPUTime);
	//fprintf(output,"%lf\n",sommeRet);
	for(int mach = 0;mach<NbMaxMachine;mach++){
		fprintf(output,"%ld ",NbJobs);
		fprintf(output,"%ld\n",id(mach));
		for(int job = 0;job<NbMaxJob;job++){
			fprintf(output,"%ld %ld %ld %ld %ld\n",ri(mach,job), di(mach,job), pi(mach,job), Ci(mach,job), diG(job));
		}
		fprintf(output,"\n");
	}
		
	fclose(output);

	
	//printf("fin jobshop !!!");
	/* Affichage de mise a jour résultat jobshop **/
/*	printf("Mise a jour résultat jobshop \n");
	for(int j=0;j<NbMachines;j++){
		for(int i = 0;i<NbJobs ;i++){
			printf("ri, di, pi, Ci %ld | %ld | %ld | %ld |",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
			printf(" \n");
		}
		printf(" \n");
	}*/
	
	int* jobRetires;
	jobRetires = (int*) malloc(sizeof(int) * NbJobs);

	// init tab jobs retires
	for(int job=0;job<NbJobs;job++){
		jobRetires[job] =999;
	}

	int nbJobRetire = 0;
		
	bool enRetard = false;
	
	/** Recherche si 1 job en retard **/
	for(int job=0;job<NbJobs;job++){
		ciJob = Ci(gamme(job,NbMaxMachine-1),job);
		diJob = diG(job);
		// si le job est en retard
		if(ciJob>diJob){
			enRetard = true;
			printf("job %u en retard", job);
		}
	}

	int nbIte = 0;
	bool dejaRetire = false;
	while(nbIte<NbJobs && enRetard == true){
		/** Execution prise de décision **/
		printf("entree dans la boucle");
		priseDeDecision1(jobRetires,nbJobRetire);

		/** Affichage de résultat la prise de décision **/
		printf("nouvelleOrdo \n");
		for(int j=0;j<NbMachines;j++){
			for(int i = 0;i<NbJobs ;i++){
				printf("ri, di, pi, Ci %ld | %ld | %ld | %ld |",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
				printf(" \n");
			}
			printf(" \n");
		}
		
		/** Affichage jobs retires**/
		printf("jobs retires : ");
		for(int jobRet = 0; jobRet<NbJobs; jobRet++){
			printf("%u\n",jobRetires[jobRet]);
		}
		printf("\n");

		enRetard = false;
		dejaRetire = false;
		for(int job=0;job<NbJobs;job++){
			// on verifie si le job est déja supprimé
			for(int jobR=0;jobR<NbJobs;jobR++){
				if(job == jobRetires[jobR]){
					dejaRetire = true;
				}
			}
			
			ciJob = Ci(gamme(job,NbMaxMachine-1),job);
			diJob = diG(job);
			// si le job est en retard
			if(ciJob>diJob && dejaRetire==false){
				enRetard = true;
			}
		}
		nbIte++;
	}
	//system("PAUSE");
}