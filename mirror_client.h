typedef struct FileId *FileIdPtr; //Pointer t .id element

struct HashEl{       //hash element
	FileIdPtr firstel;
};

struct FileId //.id element
{
    FileIdPtr nextel;
    char id[9];
};

int hashf(char* symb,int max)//hash function for hash table with lists
{
    int sum=0,ln,mod,i;
    int vl=0;
    ln=strlen(symb);
    for(i=0;i<ln;i++)
        sum=sum+symb[i];
    mod=sum%max;
    return mod;
}


FileIdPtr addid(char* symb,struct HashEl * har,int max) //adds a .d in the hash table
{
    int pos,cont=1;
    pos = hashf(symb,max);
    FileIdPtr AddId,FindPos;
    AddId=malloc(sizeof(struct FileId));
    if (har[pos].firstel==NULL)
    {
        har[pos].firstel=AddId;
        strcpy(AddId->id,symb);
        AddId->nextel = NULL;
    }
    else
    {
        FindPos=har[pos].firstel;
        do
        {
            if (strcmp(FindPos->id,symb)==0) {
                free(AddId);
			    AddId=NULL;
                break;
            }
            else
            {
                if (FindPos->nextel!=NULL) {
                    FindPos=FindPos->nextel;
                }
                else
                {
                    cont=0;
                    FindPos->nextel=AddId;
                    strcpy(AddId->id,symb);
                    AddId->nextel = NULL;

                }
            }
            
        } while (cont==1);
    }
    return AddId;
}

FileIdPtr searchid(char* addrs,struct HashEl *  har,int max)//search for an address in the hash table
{
		int pos,ret,cont=1;
		ret=0;
		FileIdPtr ScPn;
		pos=hashf(addrs,max);
		if(har[pos].firstel==NULL)
			ret=0;
		else
		{
			ScPn=har[pos].firstel;
			do
			{
				if(strcmp(ScPn->id,addrs)==0)
				{
					ret=1;
					break;
				}
				else
				{
					if(ScPn->nextel!=NULL)
						ScPn=ScPn->nextel;
					else
						cont=0;
				}
			}while(cont==1);
		}
		if (ret==0)
			ScPn=NULL;
		return ScPn;
}

void RemoveFilesRecursively(char *basePath) //removes a directory by deleting the files and subdirectories inside of it
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            
            if (dp->d_type & DT_REG) 
            {
                if (remove(path) == 0) 
                    printf("Deleted successfully \n"); 
                else
                    printf("Unable to delete the file \n");
            }
            

            RemoveFilesRecursively(path);//enters subdirectory
        }
    }

    int status;
    status=rmdir(basePath);
    closedir(dir);
    
}

void SendFilesRecursively(char *basePath , int ln , int fd , char *log ) //send the data of a directory, also the data in its subdirectories
{
    FILE *flog;
    int nwrite;
    int size;
    char bsl[]="/";
    int buffersize=100;
    char token[100];
    short leng;
    char path[100];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {  
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);


            if (dp->d_type & DT_REG) //if file
            {
                
                            strncpy(token, path+ln+1,strlen(path));
                            for(int i = 0; i < 3; i++)
                            {
                                if (i==1) ///               <-        Note: this is the second!!!
                                {
                                    if(( nwrite=write(fd, token , buffersize +1)) ==  -1)//send name of file
                                    { 
                                        perror("Error  in  Writing");
                                        //kill(getppid(), SIGUSR1);
                                        exit(2); 
                                    }
                                    char fillen[20];
                                    snprintf(fillen, 10, "%d", leng);
                                    flog=fopen(log,"a");
                                    fputs( "send bytes " , flog );    
                                    fputs( fillen , flog );
                                    fputs( " \n" , flog );
                                    fclose(flog);
                                    flog=fopen(log,"a");
                                    fputs( "send file " , flog );    
                                    fputs( token , flog );
                                    fputs( " \n" , flog);
                                    fclose(flog);
                                }
                                if (i==0) ///          <-        Note: this is the first!!!
                                {
                                    leng=strlen(token);
                                    if(( nwrite=write(fd, &leng , sizeof(leng))) ==  -1)//send length of name of file
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
                                }
                                if (i==2) 
                                {
                                    int left=0;

                                    FILE * pFile;
                                    pFile = fopen (path,"rb");
                                    if (pFile==NULL) perror ("Error opening file");
                                    else
                                    {
                                            fseek (pFile, 0, SEEK_END);  
                                            size=ftell (pFile);
                                            //fclose (pFile);
                                            fseek(pFile, 0, SEEK_SET);	
                                    }
                                    if(( nwrite=write(fd, &size , sizeof(size))) ==  -1)//sends size of file
                                    { 
                                        perror("Error  in  Writing");
                                        //kill(getppid(), SIGUSR1);
                                        exit(2); 
                                    }
                                    flog=fopen(log,"a");
                                    fputs( "send bytes " , flog );    
                                    fputs( "4" , flog );
                                    fputs( " \n" , flog );
                                    fclose(flog);
                                    fflush(stdout);
                                    
                                    char *wholf;
                                    wholf=(char*)malloc(sizeof(char)*size);
                                    fread(wholf, sizeof(char), size, pFile);
                                    fclose (pFile);

                                    int last=buffersize;
                                    //int neven=0;
                                    left=size/(buffersize);
                                    if (size%(buffersize)!=0) 
                                    {
                                        last=size-(left*(buffersize));
                                        left++;
                                        //neven=1;
                                    }
                                    if (last<0) {
                                        last=buffersize;
                                        left=left-1;
                                    }
                                    
                                    int base=0;
                                    char part[buffersize];
                                    for(int j = 0; j < left; j++)
                                    {

                                            strncpy ( part, wholf+base,buffersize);
                                            part[buffersize]='\0';
                                            if (j==(left-1)) 
                                            {
                                                part[last]='\0';
                                            }
                                            base=base+(buffersize);
                                            if(( nwrite=write(fd, part , buffersize +1)) ==  -1)//sends file by sending parts equal to the buffersize
                                            { 
                                                perror("Error  in  Writing");
                                                //kill(getppid(), SIGUSR1);
                                                exit(2); 
                                            }        
                                    }
                                    char fillsz[20];
                                    snprintf(fillsz, 10, "%d", size);
                                    flog=fopen(log,"a");
                                    fputs( "send bytes " , flog );    
                                    fputs( fillsz , flog );
                                    fputs( " \n" , flog );
                                    fclose(flog);
                                
                                    free(wholf);
                                }
            
                            }
                        
            }
            else if (dp->d_type & DT_DIR)//if directory 
            {
                char *sdir=malloc(strlen(path) + strlen("^") +1);
                strcpy(sdir,path);
                strcat(sdir, "^");
                strncpy(token, sdir+ln+1,strlen(sdir) );
                for(int i = 0; i < 2; i++)
                            {
                                if (i==1) ///               <-        Note: this is the second!!!
                                {
                                    if(( nwrite=write(fd, token , buffersize +1)) ==  -1)//sends directory name
                                    { 
                                        perror("Error  in  Writing");
                                        //kill(getppid(), SIGUSR1);
                                        exit(2); 
                                    }
                                    char fillen[20];
                                    snprintf(fillen, 10, "%d", leng);
                                    flog=fopen(log,"a");
                                    fputs( "send bytes " , flog );    
                                    fputs( fillen , flog );
                                    fputs( " \n" , flog );
                                    fclose(flog);
                                    free(sdir);
                                }
                                if (i==0) ///               <-        Note: this is the first!!!
                                {
                                    leng=strlen(token);
                                    if(( nwrite=write(fd, &leng , sizeof(leng))) ==  -1) //send  length of directory's name 
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
                                }
                            }
            }

            SendFilesRecursively(path,ln,fd,log); //enters a sudirectory
        }
    }

    closedir(dir);
}

FileIdPtr RunHash(int max,struct HashEl * har,char * dirpath)//checks if an .id is removed from common without beeing removed from the hash table
{
	int i,cont,retn;
    struct dirent *dp;
	FileIdPtr PrPn;
	for(i=0;i<max;i++)
	{
		cont=1;
		if(har[i].firstel!=NULL)
        {
            PrPn=har[i].firstel;
            do
            {
                retn=0;
                DIR *dir = opendir(dirpath);
                while ((dp = readdir (dir)) != NULL) 
                {
                    if (dp->d_type & DT_REG)
                    {
                        if (strcmp(dp->d_name,PrPn->id)==0) 
                        {
                            retn=1;
                        }
                    }
                }
                closedir (dir);
                if (retn==0) 
                {
                    return PrPn;
                    break;
                }
            	if(PrPn->nextel!=NULL)
            		PrPn=PrPn->nextel;
                else
                    cont=0;
            }while(cont==1);
		}
	}
    if (retn==1) 
    {
        return NULL;
    }
}

void RemId(int max,struct HashEl * har,FileIdPtr RmPtr)//removes a hash element
{
    int pos,cont=1;
    pos = hashf(RmPtr->id,max);
    FileIdPtr FindPos;
    if (strcmp(RmPtr->id,har[pos].firstel->id)==0) 
    {
        if (RmPtr->nextel==NULL) 
        {
            har[pos].firstel=NULL;
            free(RmPtr);
        }
        else
        {
            har[pos].firstel=RmPtr->nextel;
            RmPtr->nextel=NULL;
            free(RmPtr);
        }
    }
    else
    {
        FindPos=har[pos].firstel;
        while(strcmp(FindPos->nextel->id,RmPtr->id)!=0)
        {
            FindPos=FindPos->nextel;
        }
        if (RmPtr->nextel!=NULL)
        {
            FindPos->nextel=RmPtr->nextel;
            RmPtr->nextel=NULL;
            free(RmPtr);   
        }
        else
        {
            FindPos->nextel=NULL;
            free(RmPtr);
        }
    }
}

void freetable(FileIdPtr RmPtr)//frees hash table
{
    int cont=1,count;
    FileIdPtr FnPtr;
    FnPtr=RmPtr;
    while(cont==1)
    {
        FnPtr=RmPtr;
        count=0;
        
        if (FnPtr->nextel!=NULL) 
        {
            while(FnPtr->nextel->nextel!=NULL)
            {
                FnPtr=FnPtr->nextel;
                count++;
            }
            free(FnPtr->nextel);
            FnPtr->nextel=NULL;
        }
        if (count==0) {
            cont=0;
        }
    }
    free(RmPtr);
}