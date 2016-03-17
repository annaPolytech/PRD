#pragma once
#ifndef FDonnees1
#define FDonnees1 10

#define MaxMachines1 3
#define MaxJobs1 50

// données d'un job pour une machine
struct TDataSchrage{
	unsigned int identifiant;
	unsigned int piS[MaxJobs1];
	unsigned int riS[MaxJobs1];
	unsigned int diS[MaxJobs1];
	unsigned int CiS[MaxJobs1];
	unsigned int jobInit[MaxJobs1];
	
} ;

extern TDataSchrage DataS;
extern unsigned int NbJobs1;

void ReadDataSchrage();
void ReadDataSchrage2(int idMachine);
// This function reads the data and write them into the format expected by the PSE of Carlier (Rivereau implementation)
// The read data are put into the structure Data and are sorted by increasing values of ri (break ties by increasing di)

extern inline unsigned int riS(unsigned int uiJob);
extern inline unsigned int piS(unsigned int uiJob);
extern inline unsigned int diS(unsigned int uiJob);
extern inline unsigned int CiS(unsigned int uiJob);
extern inline unsigned int jobInit(unsigned int uiJob);
extern inline unsigned int setCiS(unsigned int uiJob, unsigned int valeurCi);

#endif