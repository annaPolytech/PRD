#pragma once
#ifndef FDonnees
#define FDonnees 10

#define MaxMachines 3
#define MaxJobs 10000

// données d'un job pour une machine
struct TData{
	unsigned int identifiant;
	unsigned int pi[MaxJobs];
	unsigned int ri[MaxJobs];
	unsigned int di[MaxJobs];
	unsigned int Ci[MaxJobs];
} ;

// données d'un job en général
struct TData2{
	unsigned int riG[MaxJobs];
	unsigned int diG[MaxJobs];
	unsigned int gamme[MaxJobs][MaxMachines];
	unsigned int delai[MaxJobs][MaxMachines-1];
	unsigned int coutR[MaxJobs];
};

extern TData2 Data;
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
//extern inline unsigned int Di(unsigned int uiMachine, unsigned int uiJob, unsigned int uiMachine2);


extern inline unsigned int riG(unsigned int uiJob);
extern inline unsigned int diG(unsigned int uiJob);
extern inline unsigned int gamme(unsigned int uiJob, unsigned int uiOperation);
extern inline unsigned int delai(unsigned int uiJob, unsigned int uiOperation);
extern inline unsigned int coutR(unsigned int uiJob);

// setteur
extern inline unsigned int setri(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurri);
extern inline unsigned int setdi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurdi);
extern inline unsigned int setCi(unsigned int uiMachine, unsigned int uiJob, unsigned int valeurCi);
#endif