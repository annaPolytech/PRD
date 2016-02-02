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
	unsigned int Di[MaxJobs][MaxMachines];
} ;

// données d'un job en général
struct TData2{
	unsigned int riG[MaxJobs];
	unsigned int diG[MaxJobs];
} ;

extern TData2 Data;
extern unsigned int NbJobs;
extern unsigned int NbMachines;


void ReadData();
void ReadData2();
// This function reads the data and write them into the format expected by the PSE of Carlier (Rivereau implementation)
// The read data are put into the structure Data and are sorted by increasing values of ri (break ties by increasing di)

extern inline unsigned int id(unsigned int uiMachine);
extern inline unsigned int ri(unsigned int uiMachine, unsigned int uiJob);
extern inline unsigned int pi(unsigned int uiMachine, unsigned int uiJob);
extern inline unsigned int di(unsigned int uiMachine, unsigned int uiJob);
extern inline unsigned int Di(unsigned int uiMachine, unsigned int uiJob, unsigned int uiMachine2);

extern inline unsigned int riG(unsigned int uiJob);
extern inline unsigned int diG(unsigned int uiJob);

#endif