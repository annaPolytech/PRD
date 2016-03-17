#pragma once
#ifndef FDonnees
#define FDonnees 10

#define MaxMachines 3
#define NbMaxJob 50

// données d'un job pour une machine
struct TData{
	unsigned int identifiant;
	unsigned int pi[NbMaxJob];
	unsigned int ri[NbMaxJob];
	unsigned int di[NbMaxJob];
	unsigned int Ci[NbMaxJob];
	unsigned int nombreJobs;
} ;

// données d'un job en général
struct TDataG{
	unsigned int riG[NbMaxJob];
	unsigned int diG[NbMaxJob];
	unsigned int gamme[NbMaxJob][MaxMachines];
	unsigned int delai[NbMaxJob][MaxMachines-1];
	unsigned int coutR[NbMaxJob];
};

extern TDataG Data;
extern unsigned int NbJobs;
extern unsigned int NbMachines;


void ReadData();
void ReadData2();
// This function reads the data and write them into the format expected by the PSE of Carlier (Rivereau implementation)
// The read data are put into the structure Data and are sorted by increasing values of ri (break ties by increasing di)

// getteur
extern inline unsigned int id(unsigned int uiMachine);
extern inline unsigned int ri(unsigned int uiMachine, unsigned int uiJob);
extern inline unsigned int pi(unsigned int uiMachine, unsigned int uiJob);
extern inline unsigned int di(unsigned int uiMachine, unsigned int uiJob);
extern inline unsigned int Ci(unsigned int uiMachine, unsigned int uiJob);
extern inline unsigned int nombreJobs(unsigned int uiMachine);
//extern inline unsigned int Di(unsigned int uiMachine, unsigned int uiJob, unsigned int uiMachine2);


extern inline unsigned int riG(unsigned int uiJob);
extern inline unsigned int diG(unsigned int uiJob);
extern inline unsigned int gamme(unsigned int uiJob, unsigned int uiOperation);
extern inline unsigned int delai(unsigned int uiJob, unsigned int uiOperation);
extern inline unsigned int coutR(unsigned int uiJob);

// setteur
extern inline unsigned int setri(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurri);
extern inline unsigned int setdi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurdi);
extern inline unsigned int setpi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurpi);
extern inline unsigned int setCi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurCi);
extern inline unsigned int setCoutRejet(unsigned int uiJob, unsigned int valeurCoutR);
#endif