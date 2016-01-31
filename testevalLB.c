#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#define depart 50 // Min number of jobs
#define taille_max 1000 // Max number of jobs
#define pas 50 // Step in the loop on the number of jobs

unsigned long int iterations=50;

float tpsbsr,tempsP;
unsigned long int ubbsr,sumri;
signed long int imp,lb,op,optchu,lbchu;
float alpha;
unsigned int range;
int save_mode=0; // Set it to 1 if you want to save all the data files

//--------------------------------------------------------------------------
// This function generates the data : first parameter jobnumber, second parameter the value K in the generation scheme
void generer(long int nbtrav, long int K)
{
 long int i,k,j,nbre_travaux=0;
 long int r,p,d,ri[taille_max],di[taille_max],pi[taille_max],maxdi=0,maxri=0,maxpi=0;
 FILE *fichier;
 
 fichier=fopen("donnees.dat","wt");
 for (i=0;i<nbtrav;i++)
 {
  p=((float)rand()/(float)RAND_MAX)*49+1;
  if (p>maxpi) maxpi=p;
  d=((float)rand()/(float)RAND_MAX)*(float)K*(float)nbtrav;
  if (d>maxdi) maxdi=d;
  r=((float)rand()/(float)RAND_MAX)*(float)K*(float)nbtrav;
  if (r>maxri) maxri=r;
  
  j=0;
  // Les travaus sont classés par di croissants (et en cas d'égalit?de di, par pi croissant)
  while ((j<nbre_travaux)
	  &&((di[j]>d)||((di[j]==d)&&(pi[j]<p)))) j++;
  // Je décale
  for (k=nbre_travaux;k>j;k--)
  {
	ri[k]=ri[k-1];
	pi[k]=pi[k-1];
	di[k]=di[k-1];
  };
  // J'insère
  di[j]=d;
  ri[j]=r;
  pi[j]=p;  
  nbre_travaux++;
 }
 /*printf("Max ri = %ld\n", maxri);
 printf("Max pi = %ld\n",maxpi);
 printf("Max di = %ld\n",maxdi);*/
 for (i=0;i<nbtrav;i++)
 {
  fprintf(fichier,"%ld %ld %ld %ld\n",i+1,ri[i],pi[i],(long int)((signed int)maxri+(signed int)maxpi+(signed int)maxdi-(signed int)di[i]));
 }
 //Ecriture de la valeur de K
 //fprintf(fichier, "%ld\n",K);
 fclose(fichier);
}
//--------------------------------------------------------------------------
void main(void)
{
  unsigned int uiRes,uiSumRes,uiResL1,uiResL2,uiResL3,uiResL4,uiNbVal,uiNbValImp,uiAgNbValImp, uiAgNbVal, uiNbDiff;
 long int i,j,lbchu,optchu,k;
 signed int Gap1,Gap2,uiSLB;
 float tpschu;
 int t,iInstNum=4201;
 FILE *fichier;
 char name[100];
 int tcc;
 signed int impPos;

 double permin,permoy,permax,ndc_moy,ndp_moy,tpc_moy,tpp_moy,tpc_min,tpc_max,tpp_min,tpp_max;
 long int ndc_min, ndc_max, ndp_min,ndp_max,ndc,ndp;
 double agpermin,agpermoy,agpermax;
 double perminS,permoyS,permaxS,agperminS,agpermoyS,agpermaxS;
 long int nbS_best, nbU_best, nbEqual,agnbvalS;

 int nbval,agnbval;

 fichier=fopen("perimp.txt","wt");
 fprintf(fichier,"Lower bound improvement over JPS\n");
 fprintf(fichier,"-----------------------------------------------------\n\n"); 
 fprintf(fichier,"These tests are done on a PC Pentium XXXXXXXX\n"); 
 fprintf(fichier,"Number of iterations for a fixed value of n : %d\n\n",iterations); 
 fprintf(fichier,"Algo  n   K  Imp_min(%%) Imp_moy(%%) Imp_max(%%)  NbInst  NbInstImprov\n");
 fprintf(fichier,"----------------------------------------------------------------------\n"); 
 fclose(fichier);

 fichier=fopen("perimpaggreg.txt","wt");
 fprintf(fichier,"Lower bound improvement over JPS\n");
 fprintf(fichier,"-----------------------------------------------------\n\n"); 
 fprintf(fichier,"These tests are done on a PC Pentium XXXXXXXX\n"); 
 fprintf(fichier,"Number of iterations for a fixed value of n : %d\n\n",iterations); 
 fprintf(fichier,"Algo  n   Imp_min(%%) Imp_moy(%%) Imp_max(%%) NbInst  NbInstImprov\n");
 fprintf(fichier,"------------------------------------------------------------------\n"); 
 fclose(fichier);

 fichier=fopen("perimpaggregSLB.txt","wt");
 fprintf(fichier,"Lower bound improvement over SLB\n");
 fprintf(fichier,"-----------------------------------------------------\n\n"); 
 fprintf(fichier,"These tests are done on a PC Pentium XXXXXXXX\n"); 
 fprintf(fichier,"Number of iterations for a fixed value of n : %d\n\n",iterations); 
 fprintf(fichier,"Algo  n   Imp_min(%%) Imp_moy(%%) Imp_max(%%) NbInstSLBBest  NbInstUsBest NBInstEqual\n");
 fprintf(fichier,"-------------------------------------------------------------------------------------\n"); 
 fclose(fichier);

 fichier=fopen("LengthCC.txt","wt");
 fprintf(fichier,"Length of the critical Path in Jackson's Schedule\n");
 fprintf(fichier,"-----------------------------------------------------\n\n"); 
 fprintf(fichier,"n   K  Length\n");
 fprintf(fichier,"-------------\n"); 
 fclose(fichier);

 /*fichier=fopen("ListeStats.txt","wt");
 fprintf(fichier,"Opt  L1   L2   L3   L4\n\n");
 fclose(fichier);*/


 for (i=depart;i<=taille_max;i+=pas)
 {
  srand(1);
  agpermin=agperminS=99999999.9;
  agpermoy=agpermax=agpermoyS=agpermaxS=0.0;
  agnbval=agnbvalS=0;
  uiAgNbValImp=0;
  uiAgNbVal=0;
  uiSumRes=0;
  nbS_best=nbU_best=nbEqual=0;

 // Range values from 0.2 to 1.0
 for (range=1;range<=25;range+=1)
 {
  printf("Nombre de travaux : %ld\n",i);
  permin=999999;
  permoy=0;
  permax=0;
  perminS=999999;
  permoyS=0;
  permaxS=0;
  uiNbDiff=0;
  ndc_min=999999;
  ndc_max=0;
  ndp_min=999999;
  ndp_max=0;
  ndc_moy=ndp_moy=0;
  tpc_min=tpp_min=99999999999.0;
  tpc_moy=tpc_max=tpp_moy=tpp_max=0;
  nbval=0;
  uiNbValImp=0;
  // Initialisation des variables pour les statistiques
  for (j=0;j<iterations;j++)
  {
   printf("Jeux n?ld\n",j+1);
   generer(i,range);


   // Exécution du MIP
   //printf("MIP is running...\n");
   printf("Carlier is running...\n");
   spawnl(P_WAIT,"psecarlier.exe","psecarlier.exe",NULL);
   fichier=fopen("carlier.txt","rt");
   //spawnl(P_WAIT,"cplex1riLmax.exe","cplex1riLmax.exe",NULL);
   //fichier=fopen("HeurILP1riCsum.txt","rt");
   /*spawnl(P_WAIT,"schrage.exe","schrage.exe",NULL);
   fichier=fopen("schrage.txt","rt");*/
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&optchu);
   fclose(fichier);
   //fscanf(fichier,"%ld\n",&lbchu);
   if (tempsP<tpc_min) tpc_min=tempsP;
   if (tempsP>tpc_max) tpc_max=tempsP;
   tpc_moy+=tempsP;

   if (save_mode==1)
   {
	itoa(iInstNum,name,10);
	strcat(name,".res");
    rename("carlier.txt",name);
   }


   // Exécution de JPS
   printf("JPS is running...\n");
   spawnl(P_WAIT,"jps.exe","jps.exe",NULL);
   //spawnl(P_WAIT,"cplex1riLmax.exe","cplex1riLmax.exe",NULL);
   //fichier=fopen("HeurILP1riCsum.txt","rt");
   fichier=fopen("jps.txt","rt");
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&lbchu);
   fclose(fichier);
   /*if (lbchu!=optchu)
	   getch();*/
   


   // Exécution de JPS amélior?
   printf("JPS improved is running...\n");
   spawnl(P_WAIT,"jpsimproved.exe","jpsimproved.exe",NULL);
   fichier=fopen("jpsimproved.txt","rt");
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&imp);
   fscanf(fichier,"%ld\n",&tcc);
   fscanf(fichier,"%ld\n",&uiSLB);
   fclose(fichier);

   fichier=fopen("LengthCC.txt","at");
   fprintf(fichier,"%ld %ld %ld\n",i, range,tcc);
   fclose(fichier);


    if (save_mode==1)
   {
	itoa(iInstNum,name,10);
	strcat(name,".dat");
    rename("donnees.dat",name);
	iInstNum++;
   }

   /*fichier=fopen("Stats.txt","rt");
   //fscanf(fichier,"%ld\n",&uiRes);
   fscanf(fichier,"%ld %ld %ld %ld\n",&uiResL1,&uiResL2,&uiResL3,&uiResL4);
   fclose(fichier);*/
 
   //getch();

   /*if (imp>optchu) 
   {
	printf("Erreur !!!!\n");
	getch();
	exit(0);
   }*/

   /*if (imp<0) imp=imp*-1;
   if (optchu<0) optchu=optchu*-1;
   if (lbchu<0) lbchu=lbchu*-1;*/

   if (uiSLB>imp) imp=uiSLB;

   Gap1=optchu-lbchu;
   if (Gap1<0) Gap1=Gap1*-1.0;
   Gap2=optchu-imp;
   if (Gap2<0) Gap2=Gap2*-1.0;
   Gap2=Gap1-Gap2;

   if ((optchu!=lbchu)&&((double)Gap2/(double)Gap1<permin)) permin=(double)Gap2/(double)Gap1;
   if ((optchu!=lbchu)&&(double)Gap2/(double)Gap1>permax) permax=(double)Gap2/(double)Gap1;


   /*   if (optchu==lbchu) 
   {
	   permax=1;
	   permoy+=1;
   }else  permoy+=(double)Gap2/(double)Gap1;*/
   if (optchu!=lbchu) 
   {
    permoy+=(double)Gap2/(double)Gap1;
    nbval++;
    if (imp>lbchu) uiNbValImp++;
	//uiSumRes+=uiRes;
	/*fichier=fopen("ListeStats.txt","at");
	fprintf(fichier,"%4ld %4ld %4ld %4ld %4ld\n",optchu,uiResL1,uiResL2,uiResL3,uiResL4);
	fclose(fichier);*/
   }

   /*if ((optchu!=lbchu)&&((double)imp/((double)optchu-(double)lbchu)<permin)) permin=(double)imp/((double)optchu-(double)lbchu);
   if ((optchu!=lbchu)&&(double)imp/((double)optchu-(double)lbchu)>permax) permax=(double)imp/((double)optchu-(double)lbchu);
   if (optchu==lbchu) 
   {
	   permax=1;
	   permoy+=1;
   }else  permoy+=(double)imp/((double)optchu-(double)lbchu);*/

   //if (ndp>ndc+1) exit(1);
    printf("Gap1=%d\n",Gap1);
    printf("Gap2=%d\n",Gap2);
	printf("JPS : %ld / Opt : %ld / JPS imp : %ld\n", lbchu,optchu,imp);
	printf("Nbdiff : %ld / Nb imp : %ld\n",nbval,uiNbValImp);

   if (uiSLB<lbchu) uiSLB=lbchu;

   Gap1=optchu-uiSLB;
   if (Gap1<0) Gap1=Gap1*-1.0;
   Gap2=optchu-lbchu;
   if (Gap2<0) Gap2=Gap2*-1.0;
   Gap1=Gap2-Gap1;

   //if (Gap1<0) Gap1=Gap1*-1.0;
   //if (imp<0) impPos=-1*imp;
	//else impPos=imp;
   if ((optchu!=lbchu)&&((double)Gap1/(double)Gap2<perminS)) perminS=(double)Gap1/(double)Gap2;
   if ((optchu!=lbchu)&&(double)Gap1/(double)Gap2>permaxS) permaxS=(double)Gap1/(double)Gap2;


  // if ((imp!=uiSLB)&&((double)Gap1/(double)impPos<perminS)) perminS=(double)Gap1/(double)impPos;
   //if ((imp!=uiSLB)&&((double)Gap1/(double)impPos>permaxS)) permaxS=(double)Gap1/(double)impPos;
   if (optchu!=lbchu) 
   //if (imp!=uiSLB) 
   {
	   permoyS+=(double)Gap1/(double)Gap2;
	   uiNbDiff++;
   }
   if (imp==uiSLB) nbEqual++;
   if (imp>uiSLB) nbU_best++;
   if (imp<uiSLB) nbS_best++;

	//if (Gap2!=0) getch();
  }
   
 fichier=fopen("perimp.txt","at");
 if (nbval==0)
	 fprintf(fichier,"LB+ %d  %ld JPS always gives the optimal solution value\n",i,range);
 else  fprintf(fichier,"LB+  %d  %ld %3.2lf  %3.2lf  %3.2lf %3ld %3ld\n", i,range,permin*100.0,permoy*100.0/(double)nbval,permax*100.0,nbval,uiNbValImp); 
 fclose(fichier);

 if (permin<agpermin) agpermin=permin;
 if (permax>agpermax) agpermax=permax;
 if (nbval!=0) 
 {
	 agpermoy+=permoy/(double)nbval;
	 agnbval++;
	 uiAgNbValImp+=uiNbValImp;
	 uiAgNbVal+=nbval;
 }

 if (perminS<agperminS) agperminS=perminS;
 if (permaxS>agpermaxS) agpermaxS=permaxS;
 if (uiNbDiff!=0) 
 {
	 agpermoyS+=permoyS/(double)uiNbDiff;
	 agnbvalS++;
 }
 }

  // Range values from 0.2 to 1.0
 for (range=30;range<=100;range+=5)
 {
  printf("Nombre de travaux : %ld\n",i);
  permin=999999;
  permoy=0;
  permax=0;
  perminS=999999;
  permoyS=0;
  permaxS=0;
  uiNbDiff=0;
  ndc_min=999999;
  ndc_max=0;
  ndp_min=999999;
  ndp_max=0;
  ndc_moy=ndp_moy=0;
  tpc_min=tpp_min=99999999999.0;
  tpc_moy=tpc_max=tpp_moy=tpp_max=0;
  nbval=0;
  uiNbValImp=0;
  // Initialisation des variables pour les statistiques
  for (j=0;j<iterations;j++)
  {
   printf("Jeux n?ld\n",j+1);
   generer(i,range);

   // Exécution du MIP
   // Exécution du MIP
   //printf("MIP is running...\n");
   printf("Carlier is running...\n");
   spawnl(P_WAIT,"psecarlier.exe","psecarlier.exe",NULL);
   fichier=fopen("carlier.txt","rt");
   //spawnl(P_WAIT,"cplex1riLmax.exe","cplex1riLmax.exe",NULL);
   //fichier=fopen("HeurILP1riCsum.txt","rt");
   /*spawnl(P_WAIT,"schrage.exe","schrage.exe",NULL);
   fichier=fopen("schrage.txt","rt");*/
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&optchu);
   fclose(fichier);
   //fscanf(fichier,"%ld\n",&lbchu);
   if (tempsP<tpc_min) tpc_min=tempsP;
   if (tempsP>tpc_max) tpc_max=tempsP;
   tpc_moy+=tempsP;

   if (save_mode==1)
   {
	itoa(iInstNum,name,10);
	strcat(name,".res");
    rename("carlier.txt",name);
   }

   // Exécution de JPS
   printf("JPS is running...\n");
   spawnl(P_WAIT,"jps.exe","jps.exe",NULL);
   //spawnl(P_WAIT,"cplex1riLmax.exe","cplex1riLmax.exe",NULL);
   //fichier=fopen("HeurILP1riCsum.txt","rt");
   fichier=fopen("jps.txt","rt");
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&lbchu);
   fclose(fichier);
   /*if (lbchu!=optchu)
	   getch();*/
   


   // Exécution de JPS amélior?
   printf("JPS improved is running...\n");
   spawnl(P_WAIT,"jpsimproved.exe","jpsimproved.exe",NULL);
   fichier=fopen("jpsimproved.txt","rt");
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&imp);
   fscanf(fichier,"%ld\n",&tcc);
   fscanf(fichier,"%ld\n",&uiSLB);
   fclose(fichier);

   fichier=fopen("LengthCC.txt","at");
   fprintf(fichier,"%ld %ld %ld\n",i, range,tcc);
   fclose(fichier);


   if (save_mode==1)
   {
	itoa(iInstNum,name,10);
	strcat(name,".dat");
    rename("donnees.dat",name);
	iInstNum++;
   }

  /*fichier=fopen("Stats.txt","rt");
   //fscanf(fichier,"%ld\n",&uiRes);
   fscanf(fichier,"%ld %ld %ld %ld\n",&uiResL1,&uiResL2,&uiResL3,&uiResL4);
   fclose(fichier);*/
 
   //getch();

  /* if (imp>optchu) 
   {
	printf("Erreur !!!!\n");
	getch();
	exit(0);
   }*/

   /*if (imp<0) imp=imp*-1;
   if (optchu<0) optchu=optchu*-1;
   if (lbchu<0) lbchu=lbchu*-1;*/

      if (uiSLB>imp) imp=uiSLB;

   Gap1=optchu-lbchu;
   if (Gap1<0) Gap1=Gap1*-1.0;
   Gap2=optchu-imp;
   if (Gap2<0) Gap2=Gap2*-1.0;
   Gap2=Gap1-Gap2;

   if ((optchu!=lbchu)&&((double)Gap2/(double)Gap1<permin)) permin=(double)Gap2/(double)Gap1;
   if ((optchu!=lbchu)&&(double)Gap2/(double)Gap1>permax) permax=(double)Gap2/(double)Gap1;
/*   if (optchu==lbchu) 
   {
	   permax=1;
	   permoy+=1;
   }else  permoy+=(double)Gap2/(double)Gap1;*/
   if (optchu!=lbchu) 
   {
    permoy+=(double)Gap2/(double)Gap1;
    nbval++;
    if (imp>lbchu) uiNbValImp++;
	//uiSumRes+=uiRes;
	/*fichier=fopen("ListeStats.txt","at");
	fprintf(fichier,"%4ld %4ld %4ld %4ld %4ld\n",optchu,uiResL1,uiResL2,uiResL3,uiResL4);
	fclose(fichier);*/
   }

   /*if ((optchu!=lbchu)&&((double)imp/((double)optchu-(double)lbchu)<permin)) permin=(double)imp/((double)optchu-(double)lbchu);
   if ((optchu!=lbchu)&&(double)imp/((double)optchu-(double)lbchu)>permax) permax=(double)imp/((double)optchu-(double)lbchu);
   if (optchu==lbchu) 
   {
	   permax=1;
	   permoy+=1;
   }else  permoy+=(double)imp/((double)optchu-(double)lbchu);*/

   //if (ndp>ndc+1) exit(1);
    printf("Gap1=%d\n",Gap1);
    printf("Gap2=%d\n",Gap2);
	printf("JPS : %ld / Opt : %ld / JPS imp : %ld\n", lbchu,optchu,imp);
	printf("Nbdiff : %ld / Nb imp : %ld\n",nbval,uiNbValImp);

   if (uiSLB<lbchu) uiSLB=lbchu;

   Gap1=optchu-uiSLB;
   if (Gap1<0) Gap1=Gap1*-1.0;
   Gap2=optchu-lbchu;
   if (Gap2<0) Gap2=Gap2*-1.0;
   Gap1=Gap2-Gap1;

   //if (Gap1<0) Gap1=Gap1*-1.0;
   //if (imp<0) impPos=-1*imp;
	//else impPos=imp;
   if ((optchu!=lbchu)&&((double)Gap1/(double)Gap2<perminS)) perminS=(double)Gap1/(double)Gap2;
   if ((optchu!=lbchu)&&(double)Gap1/(double)Gap2>permaxS) permaxS=(double)Gap1/(double)Gap2;


  // if ((imp!=uiSLB)&&((double)Gap1/(double)impPos<perminS)) perminS=(double)Gap1/(double)impPos;
   //if ((imp!=uiSLB)&&((double)Gap1/(double)impPos>permaxS)) permaxS=(double)Gap1/(double)impPos;
   if (optchu!=lbchu) 
   //if (imp!=uiSLB) 
   {
	   permoyS+=(double)Gap1/(double)Gap2;
	   uiNbDiff++;
   }
   if (imp==uiSLB) nbEqual++;
   if (imp>uiSLB) nbU_best++;
   if (imp<uiSLB) nbS_best++;

	//if (Gap2!=0) getch();
  }
 fichier=fopen("perimp.txt","at");
 if (nbval==0)
	 fprintf(fichier,"LB+ %d  %ld JPS always gives the optimal solution value\n",i,range);
 else  fprintf(fichier,"LB+  %d  %ld %3.2lf  %3.2lf  %3.2lf %3ld %3ld\n", i,range,permin*100.0,permoy*100.0/(double)nbval,permax*100.0,nbval,uiNbValImp); 
 fclose(fichier);

 if (permin<agpermin) agpermin=permin;
 if (permax>agpermax) agpermax=permax;
 if (nbval!=0) 
 {
	 agpermoy+=permoy/(double)nbval;
	 agnbval++;
	 uiAgNbValImp+=uiNbValImp;
	 uiAgNbVal+=nbval;
 }
 if (perminS<agperminS) agperminS=perminS;
 if (permaxS>agpermaxS) agpermaxS=permaxS;
 if (uiNbDiff!=0) 
 {
	 agpermoyS+=permoyS/(double)uiNbDiff;
	 agnbvalS++;
 }

 }

 // Range values from 0.2 to 1.0
 for (range=110;range<=200;range+=10)
 {
  printf("Nombre de travaux : %ld\n",i);
  permin=999999;
  permoy=0;
  permax=0;
  perminS=999999;
  permoyS=0;
  permaxS=0;
  uiNbDiff=0;
  ndc_min=999999;
  ndc_max=0;
  ndp_min=999999;
  ndp_max=0;
  ndc_moy=ndp_moy=0;
  tpc_min=tpp_min=99999999999.0;
  tpc_moy=tpc_max=tpp_moy=tpp_max=0;
  nbval=0;
  uiNbValImp=0;
  // Initialisation des variables pour les statistiques
  for (j=0;j<iterations;j++)
  {
   printf("Jeux n?ld\n",j+1);
   generer(i,range);

   // Exécution du MIP
   // Exécution du MIP
   //printf("MIP is running...\n");
   printf("Carlier is running...\n");
   spawnl(P_WAIT,"psecarlier.exe","psecarlier.exe",NULL);
   fichier=fopen("carlier.txt","rt");
   //spawnl(P_WAIT,"cplex1riLmax.exe","cplex1riLmax.exe",NULL);
   //fichier=fopen("HeurILP1riCsum.txt","rt");
   /*spawnl(P_WAIT,"schrage.exe","schrage.exe",NULL);
   fichier=fopen("schrage.txt","rt");*/
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&optchu);
   fclose(fichier);
   //fscanf(fichier,"%ld\n",&lbchu);
   if (tempsP<tpc_min) tpc_min=tempsP;
   if (tempsP>tpc_max) tpc_max=tempsP;
   tpc_moy+=tempsP;
 
   if (save_mode==1)
   {
	itoa(iInstNum,name,10);
	strcat(name,".res");
    rename("carlier.txt",name);
   }

   // Exécution de JPS
   printf("JPS is running...\n");
   spawnl(P_WAIT,"jps.exe","jps.exe",NULL);
   //spawnl(P_WAIT,"cplex1riLmax.exe","cplex1riLmax.exe",NULL);
   //fichier=fopen("HeurILP1riCsum.txt","rt");
   fichier=fopen("jps.txt","rt");
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&lbchu);
   fclose(fichier);
   /*if (lbchu!=optchu)
	   getch();*/
   


   // Exécution de JPS amélior?
   printf("JPS improved is running...\n");
   spawnl(P_WAIT,"jpsimproved.exe","jpsimproved.exe",NULL);
   fichier=fopen("jpsimproved.txt","rt");
   fscanf(fichier,"%f\n",&tempsP);
   fscanf(fichier,"%ld\n",&imp);
   fscanf(fichier,"%ld\n",&tcc);
   fscanf(fichier,"%ld\n",&uiSLB);
   fclose(fichier);

   fichier=fopen("LengthCC.txt","at");
   fprintf(fichier,"%ld %ld %ld\n",i, range,tcc);
   fclose(fichier);

 
   if (save_mode==1)
   {
	itoa(iInstNum,name,10);
	strcat(name,".dat");
    rename("donnees.dat",name);
	iInstNum++;
   }


   /*fichier=fopen("Stats.txt","rt");
   //fscanf(fichier,"%ld\n",&uiRes);
   fscanf(fichier,"%ld %ld %ld %ld\n",&uiResL1,&uiResL2,&uiResL3,&uiResL4);
   fclose(fichier);*/
 
   //getch();

  /* if (imp>optchu) 
   {
	printf("Erreur !!!!\n");
	getch();
	exit(0);
   }*/

   /*if (imp<0) imp=imp*-1;
   if (optchu<0) optchu=optchu*-1;
   if (lbchu<0) lbchu=lbchu*-1;*/

      if (uiSLB>imp) imp=uiSLB;

   Gap1=optchu-lbchu;
   if (Gap1<0) Gap1=Gap1*-1.0;
   Gap2=optchu-imp;
   if (Gap2<0) Gap2=Gap2*-1.0;
   Gap2=Gap1-Gap2;

   if ((optchu!=lbchu)&&((double)Gap2/(double)Gap1<permin)) permin=(double)Gap2/(double)Gap1;
   if ((optchu!=lbchu)&&(double)Gap2/(double)Gap1>permax) permax=(double)Gap2/(double)Gap1;
/*   if (optchu==lbchu) 
   {
	   permax=1;
	   permoy+=1;
   }else  permoy+=(double)Gap2/(double)Gap1;*/
   if (optchu!=lbchu) 
   {
    permoy+=(double)Gap2/(double)Gap1;
    nbval++;
    if (imp>lbchu) uiNbValImp++;
	//uiSumRes+=uiRes;
	/*fichier=fopen("ListeStats.txt","at");
	fprintf(fichier,"%4ld %4ld %4ld %4ld %4ld\n",optchu,uiResL1,uiResL2,uiResL3,uiResL4);
	fclose(fichier);*/
   }

   /*if ((optchu!=lbchu)&&((double)imp/((double)optchu-(double)lbchu)<permin)) permin=(double)imp/((double)optchu-(double)lbchu);
   if ((optchu!=lbchu)&&(double)imp/((double)optchu-(double)lbchu)>permax) permax=(double)imp/((double)optchu-(double)lbchu);
   if (optchu==lbchu) 
   {
	   permax=1;
	   permoy+=1;
   }else  permoy+=(double)imp/((double)optchu-(double)lbchu);*/

   //if (ndp>ndc+1) exit(1);
    printf("Gap1=%d\n",Gap1);
    printf("Gap2=%d\n",Gap2);
	printf("JPS : %ld / Opt : %ld / JPS imp : %ld\n", lbchu,optchu,imp);
	printf("Nbdiff : %ld / Nb imp : %ld\n",nbval,uiNbValImp);

   if (uiSLB<lbchu) uiSLB=lbchu;

   Gap1=optchu-uiSLB;
   if (Gap1<0) Gap1=Gap1*-1.0;
   Gap2=optchu-lbchu;
   if (Gap2<0) Gap2=Gap2*-1.0;
   Gap1=Gap2-Gap1;

   //if (Gap1<0) Gap1=Gap1*-1.0;
   //if (imp<0) impPos=-1*imp;
	//else impPos=imp;
   if ((optchu!=lbchu)&&((double)Gap1/(double)Gap2<perminS)) perminS=(double)Gap1/(double)Gap2;
   if ((optchu!=lbchu)&&(double)Gap1/(double)Gap2>permaxS) permaxS=(double)Gap1/(double)Gap2;


  // if ((imp!=uiSLB)&&((double)Gap1/(double)impPos<perminS)) perminS=(double)Gap1/(double)impPos;
   //if ((imp!=uiSLB)&&((double)Gap1/(double)impPos>permaxS)) permaxS=(double)Gap1/(double)impPos;
   if (optchu!=lbchu) 
   //if (imp!=uiSLB) 
   {
	   permoyS+=(double)Gap1/(double)Gap2;
	   uiNbDiff++;
   }
   if (imp==uiSLB) nbEqual++;
   if (imp>uiSLB) nbU_best++;
   if (imp<uiSLB) nbS_best++;

	//if (Gap2!=0) getch();
  }
 fichier=fopen("perimp.txt","at");
 if (nbval==0)
	 fprintf(fichier,"LB+ %d  %ld JPS always gives the optimal solution value\n",i,range);
 else  fprintf(fichier,"LB+  %d  %ld %3.2lf  %3.2lf  %3.2lf %3ld %3ld\n", i,range,permin*100.0,permoy*100.0/(double)nbval,permax*100.0,nbval,uiNbValImp); 
 fclose(fichier);

 if (permin<agpermin) agpermin=permin;
 if (permax>agpermax) agpermax=permax;
 if (nbval!=0) 
 {
	 agpermoy+=permoy/(double)nbval;
	 agnbval++;
	 uiAgNbValImp+=uiNbValImp;
	 uiAgNbVal+=nbval;
 }
 if (perminS<agperminS) agperminS=perminS;
 if (permaxS>agpermaxS) agpermaxS=permaxS;
 if (uiNbDiff!=0) 
 {
	 agpermoyS+=permoyS/(double)uiNbDiff;
	 agnbvalS++;
 }

 }

 fichier=fopen("perimpaggreg.txt","at");
 fprintf(fichier,"LB+  %d  %3.2lf  %3.2lf  %3.2lf %3ld %3ld\n", i,agpermin*100.0,agpermoy*100.0/(double)agnbval,agpermax*100.0,uiAgNbVal,uiAgNbValImp);  
 fclose(fichier);

 fichier=fopen("perimpaggregSLB.txt","at");
 fprintf(fichier,"LB+  %d  %3.2lf  %3.2lf  %3.2lf %3ld %3ld %3ld\n", i,agperminS*100.0,agpermoyS*100.0/(double)agnbvalS,agpermaxS*100,nbS_best,nbU_best,nbEqual);  
 fclose(fichier);

 //printf("\nPourcentage de fois o?min(L1,L2,L3) améliore JPS : %lf\n\n",100.0*(double)uiSumRes/(double)agnbval);
 //getch();
}
}
