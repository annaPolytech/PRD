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
#define NbMaxJob 4
#define depart 500   // Min number of jobs
#define taille_max 5000   // Max number of jobs
#define pas 500		// Step in the loop on the number of jobs
#define iterations 2

#define EPSILON 0.0001

#define fixwith 0
// O: use reduced cost and pseudo-cost
// 1: use lp

#define CalMip 0
// 0: we don't calculate the MIP
// 1: we calculate the MIP


/*
	Formattage des données pour Schrage 
*/
void genererDonneesSchrage(long int nbtrav, unsigned int idMachine){ 
	int hello = 0;
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
	int JobI[NbMaxJob];
	double retard;
	if(inFile.is_open()) {
			int foo;
			inFile >> foo; // NBjob
			inFile >> retard; // 40
			for(int job=0; job<NbJobs ; job++){
				inFile >> CiSchrage[job];
				inFile >> JobI[job];
			}
			inFile.close();
	}
	
	for(int job=0; job<NbJobs ; job++){
		setCi(idMachine, JobI[job], CiSchrage[job]);
		//printf ("Ci : %u \n", CiSchrage[job]);
	}

	printf ("Le retard sur la machine est de : %lf \n", retard);
	return retard;
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

		// ********** Application de Schrage ********** //
		genererDonneesSchrage(NbJobs, jFirst);
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
	//printf("\n");
	//printf("fin MAJ machine\n");
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

	/*printf("les ref \n");
	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int i=0;i<NbMaxJob;i++){
			printf("%ld %ld\n", reference[machine][i][0], reference[machine][i][1]);
		}
		printf("\n");
	}
	*/
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

	/*printf("les position \n");
	for(int machine=0;machine<NbMaxMachine;machine++){
		//pour chaque position
		for(int i=0;i<NbMaxJob;i++){
			printf("%ld ", pos[machine][reference[machine][i][0]]);
		}
		printf("\n");
	}*/




	/*printf("Tri des jobs par Ri \n");
	for(int machine=0;machine<NbMaxMachine;machine++){
		for(int i=0;i<NbMaxJob;i++){
			printf("%ld ",triRi[machine][i]);
			for(int job=0;job<NbMaxJob;job++){
				if(reference[machine][job][1]==triRi[machine][i]){
					printf("%ld ", reference[machine][job][0]);
				}
			}
			printf("\n");
		}
		printf("\n");
	}
	*/


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
			//printf("\n job : %ld \n",jobInit);
			//printf("machine : %ld \n",machine);

			// si le job est en retard
			if(Ci(machine,jobInit)>di(machine,jobInit)){
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
						//printf("suivant %ld \n",suivant);
						//printf("DateDeb %ld \n",dateDeb);
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


void priseDeDecision1(){
	int ite = 0;
	int tabCout[NbMaxJob];
	int reference2[NbMaxJob][2];
	int travailCoutRMin = 0;
	int minCoutRejet = 0;
	bool changement = true;
	int nbIte = 0;

	// Tri Cout Rejet
	for(int travail=0;travail<NbMaxJob;travail++){
		tabCout[travail]=coutR(travail);
	}
	
	std::sort(std::begin(tabCout),std::end(tabCout));

	while(changement && nbIte <NbMaxJob){
		int Li=0;
		for(int travail=0;travail<NbMaxJob;travail++){
			if(tabCout[ite]==coutR(travail)){
				minCoutRejet = tabCout[ite];
				travailCoutRMin= travail;
			}
		}

		// calcul du retard cumulé du job
		for(int machine =0; machine<NbMaxMachine; machine++){
			 Li = Li + (Ci(machine,travailCoutRMin)-di(machine, travailCoutRMin));
		}

		printf("Li %u \n", Li);
		// Si le job est en retard
		if(Li>0){
			changement = false;
		}else{
			nbIte++;
			ite++;
		}
	}
}
void main(void)
{

	 // -- GENERATION DES DONNEES -- //

	FILE *output;
	 srand(1);


		ReadData();
		ReadData2();
		for(int i =0; i<NbMaxJob;i++){
			printf("cout %ld \n", coutR(i));
		}
	/*	for(int i = 0;i<NbJobs ;i++){
			printf("riG, diG %ld %ld\n",riG(i), diG(i));
			for(int j = 0;j<NbMachines;j++){
				printf("%ld ",gamme(i,j));
			}
			printf("\n");
			for(int j = 0;j<NbMachines-1 ;j++){
				printf("%ld ",delai(i,j));
			}
			printf("\n");
		}
		printf("\n");*/
		/*for(int j=0;j<NbMachines;j++){
			for(int i = 0;i<NbJobs ;i++){
				printf("mach, job, pi %u %ld %ld\n",j, i, pi(j,i));
			}
		}*/
		double sommeRet = JobShop();
		printf("Job shop \n");
		for(int j=0;j<NbMachines;j++){
			for(int i = 0;i<NbMaxJob ;i++){
				printf("ri, di, pi, Ci %ld | %ld | %ld | %ld |",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
				printf(" \n");
			}
			printf(" \n");
		}

		output=fopen("jobshop.txt","wt");
		fprintf(output,"%lf\n",sommeRet);
		for(int mach = 0;mach<NbMaxMachine;mach++){
			fprintf(output,"%ld ",NbJobs);
			fprintf(output,"%ld\n",id(mach));
			for(int job = 0;job<NbMaxJob;job++){
				fprintf(output,"%ld %ld %ld %ld\n",ri(mach,job), di(mach,job), pi(mach,job), Ci(mach,job));
			}
			fprintf(output,"\n");
		}
		
		fclose(output);

		miseAJour();

		printf("miseAjours \n");
		for(int j=0;j<NbMachines;j++){
			for(int i = 0;i<NbJobs ;i++){
				printf("ri, di, pi, Ci %ld | %ld | %ld | %ld |",ri(j,i), di(j,i), pi(j,i), Ci(j,i));
				printf(" \n");
			}
			printf(" \n");
		}

		priseDeDecision1();
		//system("PAUSE");
}