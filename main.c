//gestionnaire de mémoire swappé
#include <stdio.h>
#include <stdlib.h>

#define Total 64
#define Block 16
#define numberOfBlocks 4


int counter=0 ;

struct memoireLocal
{
    char etat ;
    int taille ;
    int rang ;
    int pid_parent ;
    struct memoireLocal *next ;
};
typedef struct memoireLocal memoireLocal ;



struct memoirePhysique
{
    int nbrDuBlock ;
    char etat ;
    int taille ;
    struct memoirePhysique *next;
}*head;
typedef struct memoirePhysique memoirePhysique ;




struct memoireDisque
{
    int nbrDuBlock ;
    char etat ;
    int taille ;
    struct memoireDisque *suivant;
}*disque_head;
typedef struct memoireDisque memoireDisque ;



struct process
{
    int pid ;
    int tailleCode ;
    int tailleDonnee ;
    int premierBlock ;
    int taille ;
    int emplacement ; //1:processus en memoire 0: processus en Disque
    struct memoireLocal * segmentDonnee ;
};
typedef struct process process ;
process T[100] ;





void add( int num, char e, int s,int memOrDisque)
{

   if(memOrDisque == 1){
         struct memoirePhysique *temp ;
         temp=(struct memoirePhysique *)malloc(sizeof(struct memoirePhysique)) ;
         temp->nbrDuBlock=num ;
         temp->etat=e ;
         temp->taille=s ;
         if (head== NULL){
            head=temp ;
            head->next=NULL ;
        }
        else{
          temp->next=head ;
           head=temp ;
        }

    }
   else{
        struct memoireDisque *temp ;
        temp=(struct memoireDisque *)malloc(sizeof(struct memoireDisque)) ;
        temp->nbrDuBlock=num ;
        temp->etat=e ;
        temp->taille=s ;
        if (disque_head== NULL){
            disque_head=temp ;
            disque_head->suivant=NULL ;
        }
        else{
            temp->suivant=disque_head ;
            disque_head=temp ;
        }
    }
}




void initialiserMemoire(int nbr){
    int i ;
    for(i=0 ; i<nbr ; i++){
       add(nbr-i,'L',0,1) ;
    }
   return ;
}


void initialiserMemoireDisque(int nbr){
    int i ;
    for(i=0 ; i<nbr ; i++){
        add(nbr-i,'L',0,0) ;
    }
    return ;
}



void kill(int index, int nbr){
    memoirePhysique *n = head ;
    while(n->nbrDuBlock!=index)
        n=n->next ;
    int i=0 ;
    for(i=0 ; i<nbr ; i++){
        n->etat='A' ;
        n=n->next ;
   }
}


int alloue(int s,int methodeAllocation){
    int nb =0 , continuer= 1,p=0;
    memoirePhysique *n=head;
    while(continuer && p< numberOfBlocks){
        if(n->etat=='L'){
             nb=0;
             while(nb < s && n->etat =='L'){
                  n=n->next ;
                  nb++ ;
                  p++ ;
             }
             if(nb == s){
                  continuer = 0 ;
                  break ;
             }

        }
        else
            n=n->next ;
        p++ ;
    }

    if(continuer == 0){
         kill(n->nbrDuBlock-s, s) ;
         counter++ ;
         T[counter].pid=counter ;
         T[counter].taille=s;
         T[counter].emplacement=1 ;
         T[counter].premierBlock= n->nbrDuBlock-s ;
         return 1;
    }
    return 0 ;
}


void kill_inDisk(int index, int nbr){
    memoireDisque *n = disque_head ;
    while(n->nbrDuBlock!=index)
         n=n->suivant ;
    int i=0 ;
    for(i=0 ; i<nbr ; i++){
         n->etat='A' ;
         n=n->suivant ;

    }
}



int alloue_inDisk(int id){
     int nb =0 , continuer= 1,s=T[id].taille,p=0 ;
     memoireDisque *n=disque_head;
     while(continuer && p < numberOfBlocks){
          if(n->etat=='L'){
               nb=0;
                while(nb < s && n->etat =='L'){
                        n=n->suivant ;
                        p++ ;
                        nb++ ;
                   }
                if(nb == s){
                        continuer = 0 ;
                        break ;
                }
           }
           else
                n=n->suivant ;
                p++ ;
     }
     if(nb==s){
            kill_inDisk(n->nbrDuBlock-s, s) ;
            T[id].premierBlock= n->nbrDuBlock-s ;
            T[id].emplacement = 0 ;
            return 1;
     }
     return 0 ;
}


void liberer(int idProc){
    memoirePhysique* n=head ;
    while(n->nbrDuBlock!=T[idProc].premierBlock)
        n=n->next ;
    int i ;
    for(i=0 ; i<T[idProc].taille ; i++){
        n->etat='L' ;
        n=n->next ;
    }
}


void liberer_fromDisk(int idProc){
    memoireDisque* n=disque_head ;
    while(n->nbrDuBlock!=T[idProc].premierBlock)
        n=n->suivant ;
    int i ;
    for(i=0 ; i<T[idProc].taille ; i++){
        n->etat='L' ;
        n=n->suivant ;
    }
}



int swap_out(int tailleDemande){
    int i=0,cpt=1;
     do{
        i++ ;
        if(T[i].taille >= tailleDemande && T[i].emplacement == 1)
                cpt = 0 ;
     }while(cpt);
    int r=alloue_inDisk(T[i].pid);
    if( r != 0) {
        liberer(T[i].pid);
        return T[i].pid ;
    }
    else
        return 0 ;
}


int swap_in(int id){
    int nb =0 , continuer= 1,s=T[id].taille,p=0 ;
    memoirePhysique *n=head;
    while(continuer && p < numberOfBlocks){
           if(n->etat=='L'){
                nb=0;
                while(nb < s && n->etat =='L'){
                        n=n->next ;
                        p++;
                        nb++ ;
                }
                if(nb == s){
                        continuer = 0 ;
                        break ;
                   }

            }
            else
                n=n->next ;
            p++;

    }
    if(nb==s){
        kill(n->nbrDuBlock-s, T[id].taille) ;
        liberer_fromDisk(id);
        T[id].premierBlock= n->nbrDuBlock-s ;
        T[id].emplacement= 1 ;
        return 1;
    }
   return 0 ;
 }



int  main(){
    int i,taille, id,s,k;
    initialiserMemoire(numberOfBlocks+1);
    initialiserMemoireDisque(numberOfBlocks+1);

    while(1){
        printf("====================\n");
        printf("1.Allouer\n");
        printf("2.Liberer\n");
        printf("3.afficher l'emplacement d'un processus\n");
        printf("4.afficher l'emplacement de tous les processus\n");
        printf("5.récuppérer un processus du disque\n");
        printf("6.exit\n") ;
        printf("====================\n");
        if(scanf("%d",&i)<=0){
            printf("Enter only an Integer\n");
            exit(0);
        }
        else{
            switch(i){
                case 1: {
                    printf("entrer la taille de processus à allouer : ") ;
                    scanf("%d",&taille) ;
                    s = taille/Block ;
                    if(taille % Block != 0){
                        s++ ;
                    }
                    if(alloue(s,1)){
                        int donnee,code ;
                        printf("entrer la taille du segment de code : ") ;
                        scanf("%d",&code) ;
                        printf("entrer la taille du segment de donnee : ") ;
                        scanf("%d",&donnee) ;
                        if(donnee+code<=T[counter].taille*Block){
                            T[counter].tailleCode=code ;
                            T[counter].tailleDonnee=donnee ;
                        }
                       else{
                            printf("invalide process\n") ;
                       }
                     }
                    else{
                        printf("******Mémore pleine********\n");
                        if (k==swap_out(s)){
                            printf("*******Pid=%d   swapping_out*******\n ",k);
                            alloue(s,1) ;
                            int donnee,code ;
                            printf("entrer la taille du segment de code : ") ;
                            scanf("%d",&code) ;
                            printf("entrer la taille du segment de donnee : ") ;
                            scanf("%d",&donnee) ;
                            if(donnee+code<=T[counter].taille*Block){
                                T[counter].tailleCode=code ;
                                T[counter].tailleDonnee=donnee ;
                            }
                            else{
                                printf("invalide process\n") ;
                            }
                        }
                        else
                            printf("impossible d'éxecuter temporairement ce processus !");
                     }
                     break ;
                }


        case 2:     printf("entrer l'identifiant de processus à libèrer : ");
                    scanf("%d",&id);
                    liberer(id);
                    printf("processus libérer avec succés ") ;
                    break;

        case 3:     printf("entrer l'id de processus pour connaitre son emplacement\n");
                    scanf("%d",&id);
                    if(T[id].emplacement == 1)
                    printf("le processus est en MC\n");
                    else printf("prosessus swapped out vers Disque\n") ;
                    break ;


        case 4:     printf("liste de processus !\n");
                    int i ;
                    char *ch;
                    for(i=1;i<=counter;i++){
                        if(T[i].emplacement)
                            ch="MC";
                        else
                            ch="Disk" ;
                        printf("pid= %d    emplacement= %s      nbBlocks= %d\n",T[i].pid,ch,T[i].taille) ;
                    }
                    break ;

        case 5:     printf("entrer l'identifiant de processus à récupperer\n") ;
                    scanf("%d",&id);
                    if(swap_out(T[id].taille)){
                        swap_in(id);
                        printf("processus swapped_in MC !\n") ;
                    }
                    else
                        printf("impossible de swapped_in ce processus !") ;
                    break ;

        case 6:     return 0 ;

        default:
                    printf("Invalid option\n");

            }
        }
    }
    return 0;
}
