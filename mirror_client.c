#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "mirror_client.h"


static int keepRunning = 1; //varible for the infinite loop
//int att=0;

/*void handler1(int sig) 
{ 
	att=0;   
}*/

void intHandler(int sig) //signal handler
{
    keepRunning = 0;
}

int main(int argc, char **argv)
{
    char *id=NULL;
    char *common=NULL;
    char *input=NULL;
    char *mirror=NULL;
    char *buffsize=NULL;
    char *log=NULL;
    int option_index = 0;
    while (( option_index = getopt(argc, argv, "n:c:i:m:b:l:")) != -1){ //getting the arguments, use of getopt
        switch (option_index) {
            case 'n':
                id = optarg;
                break;
            case 'c':
                common = optarg;
                break;
            case 'i':
                input = optarg;
                break;
            case 'm':
                mirror = optarg;
                break;
            case 'b':
                buffsize = optarg;
                break;        
            case 'l':
                log = optarg;
                break;    
            default:
                printf("Option incorrect\n");
            return 1;
        } //end block for switch
   }  //end block for while


    int buffersize=atoi(buffsize);
    char buffer[buffersize];



    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);

    int size;
    int max=50;
    struct HashEl har[max];
	for(int i=0;i<max;i++)//initialisation of arrays ellements
	{
		har[i].firstel=NULL;
	}
    FileIdPtr AddFile,ScFile,PrPn;

    FILE *flog; //makes log file
    flog = fopen(log, "a");
    fputs( "id " , flog );    
    fputs( id , flog );
    fputs( " in \n" , flog );
    fclose(flog);

    DIR* dir = opendir(common); //checks for the directories makes mirror directory
    if (dir)
    {
        /* Directory exists. */
        closedir(dir);
    }
    else if (ENOENT == errno)
    {
        mkdir(common, S_IRWXU | S_IRWXG | S_IRWXO);
    }
    else
    {
        /* opendir() failed for some other reason. */
        printf("common fail\n");
    }

    dir = opendir(input);
    if (dir)
    {
        closedir(dir);
    }
    else if (ENOENT == errno)
    {
        printf("input doesnt exist\n");
        return 1;
    }
    else
    {
        /* opendir() failed for some other reason. */
        printf("input fail\n");
    }

    dir = opendir(mirror);
    if (dir)
    {
        /* Directory exists. */
        printf("mirror exist\n");
        closedir(dir);
        return 1;
    }
    else if (ENOENT == errno)
    {
        mkdir(mirror, S_IRWXU | S_IRWXG | S_IRWXO);
    }
    else
    {
        /* opendir() failed for some other reason. */
        printf("mirror fail\n");
    }
    char bsl[]="/"; 
    char *wfile=malloc(strlen(common) + strlen(id) + strlen(bsl) + strlen(".id") +1); //makes .id file in common 
    strcpy(wfile, common);
    strcat(wfile, bsl);
    strcat(wfile, id);
    strcat(wfile, ".id");

    
    //
    snprintf(buffer, sizeof(buffer), wfile , getpid());
    
    FILE *f;
    if ((f = fopen(buffer, "r")))
    {
        fclose(f);
        return 1;
    }
    
    f = fopen(buffer, "w");
    if (f < 0) {
        perror("CLIENT:\n");
        exit(1);
    }
    char prid[20];
    snprintf(prid, 10, "%d", getpid());
    fputs( prid , f );                  //writes process id
    //
    fclose(f);
    //free(wfile);
    char *commonf=malloc(strlen(id) + strlen(".id") +1); //adds own .id in hash table
    strcpy(commonf, id);
    strcat(commonf, ".id");
    AddFile = addid(commonf,har,max);
    free(commonf);

    struct dirent *de;
    while(keepRunning) //start of infinite loop
    {
    dir = opendir(common);
    if (dir)
    {
        while ((de = readdir(dir)) != NULL) 
        {  
            if ((strcmp("..",de->d_name)!=0) && (strcmp(".",de->d_name)!=0)) 
            {
                //AddFile = addid(de->d_name,har,max);
                if (de->d_type & DT_REG) 
                { 
                    ScFile = searchid(de->d_name,har,max);  //if a new .id exists in common
                    if ( ScFile==NULL ) 
                    {
                        AddFile = addid(de->d_name,har,max);          //add it in hash table
                        char *mirrd=malloc(strlen(de->d_name) +1);
                        strcpy(mirrd,de->d_name);
                        mirrd[strlen(de->d_name)-3]='\0';
                        char *mkid=malloc(strlen(mirror) + strlen(mirrd) + strlen(bsl) +1); //make mirror/(new)id
                        strcpy(mkid, mirror);
                        strcat(mkid, bsl);
                        strcat(mkid, mirrd);
                        mkdir( mkid , S_IRWXU | S_IRWXG | S_IRWXO);

                        char *fifoot=malloc( strlen(".") + strlen(bsl) + strlen(common) + strlen(bsl) + strlen(mirrd) + strlen("_to_") + strlen(id) + strlen(".fifo") + 1);//make the fifos
                        strcpy(fifoot, ".");
                        strcat(fifoot, bsl);
                        strcat(fifoot, common);
                        strcat(fifoot, bsl);
                        strcat(fifoot, id);
                        strcat(fifoot, "_to_");
                        strcat(fifoot, mirrd);
                        strcat(fifoot, ".fifo");

                        char *fifoto=malloc( strlen(".") + strlen(bsl) + strlen(common) + strlen(bsl) + strlen(mirrd) + strlen("_to_") + strlen(id) + strlen(".fifo") + 1);
                        strcpy(fifoto, ".");
                        strcat(fifoto, bsl);
                        strcat(fifoto, common);
                        strcat(fifoto, bsl); 
                        strcat(fifoto, mirrd);
                        strcat(fifoto, "_to_");
                        strcat(fifoto, id);
                        strcat(fifoto, ".fifo");

                        mkfifo(fifoot , 0600);
                        mkfifo(fifoto , 0600);

                        int times=1;
                        //signal(SIGUSR1, handler1);
                        int fd,nwrite;

                        //while (att==0 && times<=3)
                        //{
                        //att=1;
                        pid_t   pidw,pidr;
                        short leng;
                        int  status  ;
                        if( (pidw = fork()) < 0 ) { perror("fork  failed");exit(1); } //fork for the child that sends bytes & folders
                            if( pidw == 0 )
                            { 
                                mkfifo(fifoot , 0600);    
                                if ((fd = open(fifoot, O_WRONLY)) < 0)
                                {   
                                    perror("Failed to open fifo for writing: ");
                                    //kill(getppid(), SIGUSR1);
                                    exit(2);
                                }
                                else
                                {
                                    int ln=strlen(input);
                                    SendFilesRecursively(input,ln,fd,log); //recursivelly sends subdirectories and files of the input directory
                                    leng=0;
                                    if(( nwrite=write(fd, &leng , sizeof(leng))) ==  -1) //at the sends two bytes: 00
                                    { 
                                        perror("Error  in  Writing");
                                        //kill(getppid(), SIGUSR1);  
                                        exit(2); 
                                    }   
                                    flog=fopen(log,"a");
                                    fputs( "send bytes " , flog );    
                                    fputs( "2" , flog );
                                    fputs( " \n" , flog);
                                    fclose(flog);
                                    close(fd);
                                }   
                                getpid ();
                                exit(57);
                            }


                            if( (pidr = fork()) < 0 ) { perror("fork  failed");exit(1); } //fork for the child that receives bytes & folders
                                if( pidr == 0 )
                                { 
                                    sleep (1);
                                    mkfifo(fifoto , 0600); 
                                    char  msgbuf[buffersize +1];
                                    if( (fd=open(fifoto , O_RDWR)) < 0)
                                    {
                                        perror("fifo  open  problem");
                                        //kill(getppid(), SIGUSR1);
                                        exit(3);
                                    }
                                    int i=0;
                                    for(;;)
                                    {
                                        int left=0;
                                        const char ch = '^';
                                        char *ret;

                                        if ( i==0 ) 
                                        {
                                            if(read(fd, &leng , sizeof(leng)) < 0) //receives name of file length
                                            {
                                                perror("problem  in  reading");
                                                //kill(getppid(), SIGUSR1);
                                                exit(5);
                                            }
                                            flog=fopen(log,"a");
                                            fputs( "received bytes " , flog );    
                                            fputs( "2" , flog );
                                            fputs( " \n" , flog);
                                            fclose(flog);
                                            if (leng==0) 
                                            {
                                                break;
                                            }
                                            fflush(stdout);
                                            i++;
                                        }
                                        if ( i==1 ) 
                                        {
                                            if(read(fd, msgbuf , buffersize +1) < 0)  //receives name of file
                                            {
                                                perror("problem  in  reading");
                                                //kill(getppid(), SIGUSR1);
                                                exit(5);
                                            }           
                                            char fillen[20];
                                            snprintf(fillen, 10, "%d", leng);
                                            flog=fopen(log,"a");
                                            fputs( "received bytes " , flog );    
                                            fputs( fillen , flog );
                                            fputs( " \n" , flog );
                                            fclose(flog);
                                            ret=strchr(msgbuf,ch);  //checks if file or directory
                                            if (ret==NULL)          //in mirror if file
                                            {
                                                flog=fopen(log,"a");
                                                fputs( "received file " , flog );    
                                                fputs( msgbuf , flog );
                                                fputs( " \n" , flog);
                                                fclose(flog);
                                                char *kfile=malloc(strlen(mkid) + strlen(bsl) + leng +1);
                                                strcpy(kfile, mkid);
                                                strcat(kfile, bsl);
                                                strcat(kfile, msgbuf);
                                                snprintf(buffer, sizeof(buffer), kfile , getpid());
                                                f = fopen(buffer, "a");
                                                if (f < 0) 
                                                {
                                                    perror("CLIENT:\n");
                                                    //kill(getppid(), SIGUSR1);
                                                    exit(1);
                                                }
                                                fflush(stdout);
                                                i++;
                                                free(kfile);
                                            }
                                            if (ret!=NULL) //makes subdir if directory
                                            {
                                                msgbuf[leng-1]='\0';
                                                char *wdir=malloc(strlen(mkid) +strlen(bsl)+strlen(msgbuf)+1);
                                                //strcpy(wdir, bsl);
                                                strcpy(wdir, mkid);
                                                strcat(wdir, bsl);
                                                strcat(wdir, msgbuf);
                                                mkdir( wdir , S_IRWXU | S_IRWXG | S_IRWXO);
                                                fflush(stdout);
                                                i=0;
                                                free(wdir);
                                            }
                
                
                                        }
                                        if ( i==2 ) //if file gets size of the file
                                        {
                                            if(read(fd, &size , sizeof(size) ) < 0) 
                                            {
                                                perror("problem  in  reading");
                                                //kill(getppid(), SIGUSR1);
                                                exit(5);
                                            }
                                            flog=fopen(log,"a");
                                            fputs( "received bytes " , flog );    
                                            fputs( "4" , flog );
                                            fputs( " \n" , flog );
                                            fclose(flog);
                                            fflush(stdout);
                                            i++;
                                        }
                                        if (i==3)
                                        {

                                            left=size/(buffersize);
                                            if (size%(buffersize)!=0) 
                                            {
                                                left++;
                                            }
                                            for(int j = 0; j < left; j++) //reads the file 
                                            {
                                                if(read(fd, msgbuf , buffersize +1) < 0) // reading parts equal to the size of buffer
                                                {
                                                    perror("problem  in  reading");
                                                    //kill(getppid(), SIGUSR1);
                                                    exit(5);
                                                }
                                                fputs( msgbuf , f );
                                                fflush(stdout);
                                            }
                                            char fillsz[20];
                                            snprintf(fillsz, 10, "%d", size);
                                            flog=fopen(log,"a");
                                            fputs( "received bytes " , flog );    
                                            fputs( fillsz , flog );
                                            fputs( " \n" , flog );
                                            fclose(flog);
                                            fclose(f);
                                            free(wfile);
                                            i=0;                
                                        }
                                        //break;    
                                    }
                                    close(fd);
                                    getpid ();
                                    exit(57);
                                }

                                while( (waitpid(pidw , &status , WNOHANG)) == 0 || (waitpid(pidr , &status , WNOHANG)) == 0 ){ //waits for children to exit
                                    sleep (1);
                                }
                                /*if (att==0) 
                                {
                                    RemoveFilesRecursively(mkid);
                                }
                                else
                                {
                                    printf("mirroring completed \n");
                                }*/
                                printf("mirroring completed \n"); //prints message 
                                free(mirrd);
                                free(mkid);

                            //}//while
                            }
                            else
                            {
                            }

                        }
                    }
                }  
                closedir(dir);
                PrPn=RunHash(max,har,common); //if a .id no longer exists in common
                if (PrPn!=NULL) 
                {
                    pid_t   pidd;
                    int  status  ;
                    if( (pidd = fork()) < 0 ) { perror("fork  failed");exit(1); }//fork for child to
                        if( pidd == 0 )
                        {
 
                            char *mirrd=malloc(strlen(PrPn->id) +1);
                            strcpy(mirrd,PrPn->id);
                            mirrd[strlen(PrPn->id)-3]='\0';
                            char *mkid=malloc(strlen(mirror) + strlen(mirrd) + strlen(bsl) +1);
                            strcpy(mkid, mirror);
                            strcat(mkid, bsl);
                            strcat(mkid, mirrd);   
                            RemoveFilesRecursively(mkid); //remove mirror/id
                            free(mkid);
                            free(mirrd);
                            getpid ();
                            exit(57);
                        }

                        while( (waitpid(pidd , &status , WNOHANG)) == 0  )
                        {
                                    sleep (1);
                        }
                        RemId(max,har,PrPn);//and the .id from the hash table
                }
            }
            sleep(10);//period of loop
    }

    if (remove(wfile) == 0)  //deletes own .id from common
      printf("Deleted successfully \n"); 
    else
      printf("Unable to delete the file \n"); 
    free(wfile); 

    RemoveFilesRecursively(mirror); //removes mirror

    flog=fopen(log,"a");
    fputs( "end of " , flog );
    fputs( id , flog );
    fputs( " \n" , flog );
    fclose(flog);

    for(int i = 0; i < max; i++) //frees hash elements
	{
		if (har[i].firstel!=NULL) 
		{
			freetable(har[i].firstel);
		}
		
	}


    return 0;
}