#include <stdio.h>
#include <list>
#include <map>
#include <vector>
#include <deque>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <hash_map.h>
#include <sys/time.h>

#include <pthread.h>


#define FALSE 0
#define TRUE 1

#include "StopWordProcessor.cpp"
#include "StemmingProcessor.cpp"
#include "Parser.cpp"
#include "DocumentVector.cpp"
#include "VectorFactory.cpp"
#include "RedBlackTree.cpp"

using namespace std;

#ifndef HASH_MAP_TOKENS
#define HASH_MAP_TOKENS hash_map<char*, int, hash<char*>, eqstr>
#endif

#ifndef HASH_MAP_VECTOR
#define HASH_MAP_VECTOR hash_map<char*, DocumentVector*, hash<char*>, eqstr>
#endif

#ifndef HASH_MAP_PARSED_DOCS
#define HASH_MAP_PARSED_DOCS hash_map<char*, HASH_MAP_TOKENS, hash<char*>, eqstr>
#endif

#ifndef HASH_MAP_OCCURENCE_TABLE
#define HASH_MAP_OCCURENCE_TABLE hash_map<char*, int, hash<char*>, eqstr>
#endif

#define TEST_DOC_PATH "resources/test-docs"
char cwd[1024];
char base_path[1024];
list<char*> docs;   

int n; //number of threads
typedef list<char*> dlist;
list<char*>::const_iterator itr;
pthread_mutex_t doc_lock;
HASH_MAP_PARSED_DOCS all_parsedDocs;
pthread_mutex_t allParsedDocs_lock;
pthread_cond_t docsParsed_cv, corpus_cv;
pthread_mutex_t docParsed_mutex;
int docsParsed_flag = 0, corpus_flag = 0;
pthread_mutex_t corpus_flag_mutex;
HASH_MAP_OCCURENCE_TABLE corpusOccurences;
typedef HASH_MAP_PARSED_DOCS::iterator pdocs_itr;
pdocs_itr pd_itr;
pthread_barrier_t corpus_barrier, tfidf_barrier;
pthread_mutex_t vector_lock;
HASH_MAP_VECTOR vectors;
float all_parsedDocs_size;

void loadTestDocs(char *oldDirPrev) {
    DIR* dir;
    struct dirent* entry;
    struct stat dir_stat;
    char *fileName = NULL;
    char *oldDir;
    int len;
    struct dirent** namelist;
    int i, max;
    
    strcat(cwd, "/");
    dir = opendir(cwd);
    if (!dir) {
        cout << "Cannot read directory "<< cwd <<endl;
        return;
    }

    max = scandir(cwd, &namelist, NULL, alphasort);
    for (i = 0; i < max; i++) {
        /* skip the "." and ".." entries, to avoid loops. */
        if (strcmp(namelist[i]->d_name, ".") == 0)
            continue;
        if (strcmp(namelist[i]->d_name, "..") == 0)
            continue;

               /* check if the given namelist[i] is a directory. */
               if (stat(namelist[i]->d_name, &dir_stat) == -1) {
                    perror("stat:");
                    continue;
                }

        if (S_ISDIR(dir_stat.st_mode)) {
            /* Change into the new directory */
            oldDir = (char*)malloc(strlen(cwd)+1);
            strcpy(oldDir, cwd);
            strcat(cwd, namelist[i]->d_name);
            if (chdir(namelist[i]->d_name) == -1) {
                cout<< "Cannot chdir into "<<namelist[i]->d_name<<endl;
                continue;
            }
            /* check this directory */
            loadTestDocs(oldDir);

            memset(cwd, '\0', 1024);
            strcpy(cwd, oldDir);
            if (chdir("..") == -1) {
                cout << "Cannot chdir back to "<<cwd<<endl;
                exit(1);
            }           
        }
        else
        {
            /*Not a directory. check if the file ends with .txt*/
            len = strlen(namelist[i]->d_name);
            if((namelist[i]->d_name[len-4] == '.')
               && (namelist[i]->d_name[len-3] == 't')
               && (namelist[i]->d_name[len-2] == 'x')
               && (namelist[i]->d_name[len-1] == 't'))
            {
	      fileName = (char*)malloc(strlen(cwd) - strlen(oldDirPrev) + strlen(namelist[i]->d_name)+1);
	      strcpy(fileName, &cwd[strlen(oldDirPrev)]);
	      strcat(fileName, namelist[i]->d_name);
               docs.push_back(fileName);
            }
        }
    }
    free(namelist);
}

HASH_MAP_VECTOR createVectors(list<char*> docs) {
    StopwordProcessor stopwordProcessor;
    StemmingProcessor stemmingProcessor;
    VectorFactory v1;
    
    Parser parser(stopwordProcessor, stemmingProcessor);
    
    HASH_MAP_VECTOR vectors = v1.createVectors(parser, docs);
    return(vectors);
}

void dumpVectors(HASH_MAP_VECTOR vectors) {
    HASH_MAP_VECTOR::const_iterator it1;
    
    for ( it1=vectors.begin() ; it1 != vectors.end(); it1++)
    {
        char *file1 = (*it1).first;
        DocumentVector *dv1 = (*it1).second;
        
        cout << file1 <<endl;
	//        cout << dv1->toString()<<endl;
    }
}

void dumpTop10Similarities(HASH_MAP_VECTOR vectors) {
    const float NEG_INF = -9999.0;
    float ITEM_NOT_FOUND = NEG_INF;
    HASH_MAP_VECTOR::const_iterator it1;    
    list <char*> *filelst;
    list <char*> *filelist;
    
    for ( it1=vectors.begin() ; it1 != vectors.end(); it1++)
    {
        char *file1 = (*it1).first;
        DocumentVector *dv1 = (*it1).second;
        
//        cout<<"*** " <<&file1[strlen(cwd)]<<endl;
        cout<<"*** " <<file1<<endl;
        
        RedBlackTree *tm = new RedBlackTree( ITEM_NOT_FOUND );
        
        HASH_MAP_VECTOR::const_iterator it2;
        for ( it2=vectors.begin() ; it2 != vectors.end(); it2++)
        {
            char *file2 = (*it2).first;
            DocumentVector *dv2 = (*it2).second;
            
            float similarity = 1.0f - dv1->getSimilarity(dv2);
            if ((filelst = tm->get(similarity)) == NULL)
            {
                filelist = new list<char*>;
                filelist->push_back(file2);
                tm->insert( similarity, filelist);
            }
            else
            {
                filelst->push_back(file2);
            }
        }

        int count = 0;        
        tm->createList();
        while((tm->hasNext())&& count < 10)
        {
            RedBlackNode *node = tm->next();
            float similarity = node->getSimilarity();
            list<char*> *files=node->getFiles();
            list<char*>::iterator it = files->begin();
            
            while( (it != files->end()) && count < 10)
            {
//	      cout<<"      - " <<(1.0f - similarity) <<" = " <<&(*it)[strlen(cwd)]<<endl;
  	        cout<<"      - " <<(1.0f - similarity) <<" = " <<(*it)<<endl;
                it++;
                count++; 
            }           
        }
    }
}



struct timeval load_start;
struct timeval load_end;
struct timeval createvector_start;
struct timeval createvector_end;
struct timeval dump_start;
struct timeval dump_end;

//author: Shalki
void *thread_func(void* doc_list)
{ 
    list<char*>* thread_docp;
    thread_docp = (list<char*>*) doc_list;
    list<char*> thread_doc = *thread_docp;
    HASH_MAP_PARSED_DOCS thread_list_pdocs;


    while(docsParsed_flag!=1)       
    {
        if(thread_doc.empty())
        {
            pthread_mutex_lock(&doc_lock);

            if(itr==docs.end())
            {   
                if(docsParsed_flag != 1)
                {
                    pthread_mutex_lock(&docParsed_mutex);
                    docsParsed_flag = 1;
                    pthread_cond_broadcast(&docsParsed_cv);
                    pthread_mutex_unlock(&docParsed_mutex);
                }
                // while(corpus_flag!=1)
                //     pthread_cond_wait(&corpus_cv,&corpus_flag_mutex);
                break;
            }
            else
            {
                thread_doc.push_back(*itr); 
                itr++;
            }
            pthread_mutex_unlock(&doc_lock);
        }
        
        VectorFactory v1;
        StopwordProcessor stopwordProcessor;
        StemmingProcessor stemmingProcessor;
        Parser parser(stopwordProcessor, stemmingProcessor);            
    
        HASH_MAP_PARSED_DOCS parsed_thread_doc = v1.createParsedDoc(parser, thread_doc);   //parsing individual doc by the thread 
        
        pthread_mutex_lock(&allParsedDocs_lock);
        all_parsedDocs.insert(parsed_thread_doc.begin(), parsed_thread_doc.end());
        thread_list_pdocs.insert(parsed_thread_doc.begin(), parsed_thread_doc.end());
        pthread_mutex_unlock(&allParsedDocs_lock);
        
        thread_doc.clear(); //empty the list   
    } 
    // while(corpus_flag!=1)
    //     pthread_cond_wait(&corpus_cv,&corpus_flag_mutex);
    pthread_barrier_wait(&corpus_barrier);
    printf("\nstarting tfidf calculation\n");
    fflush(stdout);
    // pthread_barrier_wait(&tfidf_barrier);

    VectorFactory v3;
    HASH_MAP_VECTOR thread_vector;

    thread_vector = v3.cal_tfidf(thread_list_pdocs,corpusOccurences,all_parsedDocs_size);
    pthread_mutex_lock(&vector_lock);
    vectors.insert(thread_vector.begin(), thread_vector.end());
    pthread_mutex_unlock(&vector_lock);

    pthread_exit(NULL);
}

	



int main(int argc, char *argv[]) {

    if (argc != 3)
    {
        cout << "usage ./Main base_path_with_trailing_slash num_threads"<<endl;
        exit(1);
    }

	
	n = atoi(argv[2]);
    pthread_t threads[n];

    pthread_barrier_init(&corpus_barrier,NULL,n+1);

	
	pthread_mutex_init(&doc_lock,NULL);


    //loading the test docs
    memset(cwd, '\0', 1024);
    memset(base_path, '\0', 1024);
    strcpy(cwd, argv[1]);
    strcpy(base_path, argv[1]);
    strcat(cwd, TEST_DOC_PATH);
    
    if (chdir(cwd) == -1) {
        cout<< "Cannot chdir to "<<cwd<<endl;
        exit(1);
    }           

    gettimeofday(&load_start, NULL); 
    loadTestDocs(NULL); 
    gettimeofday(&load_end, NULL);
    long loadtime = calcDiffTime(&load_start, &load_end);
    printf("loadtime = %ld\n", loadtime);


    //some declarations and initializations
    itr = docs.begin();
    list<dlist> thread_work[n];
 
    itr = docs.begin();
    
    pthread_mutex_init(&allParsedDocs_lock,NULL); //mutex for updating the global variable all_parsedDocs

    pthread_cond_init(&docsParsed_cv, NULL);
    pthread_mutex_init(&docParsed_mutex, NULL);

    int i;
 
    pthread_mutex_init(&corpus_flag_mutex,NULL);
    pthread_cond_init(&corpus_cv, NULL);    //corpus created yet?

    pthread_mutex_init(&vector_lock,NULL);  //lock the hash map vector while inserting the vector created by each thread


   
    //creating threads
    for(i=0;i<n;i++)
    {   
        pthread_create(&threads[i],NULL,&thread_func,&thread_work[i]);
    }

    // pthread_barrier_wait(&corpus_barrier);

    //main creates the corpus occurence table; //added by Shalki from VectorFactory    
    while(docsParsed_flag!=1)
        pthread_cond_wait(&docsParsed_cv,&docParsed_mutex);

    pd_itr = all_parsedDocs.begin(); 
    
    struct timeval corpus_start;
    struct timeval corpus_end;    
    
    gettimeofday(&corpus_start, NULL); 
    VectorFactory v2;
    corpusOccurences = v2.createCorpusOccurenceTable(all_parsedDocs);
    gettimeofday(&corpus_end, NULL); 
    long corpustime = calcDiffTime(&corpus_start, &corpus_end);
    fflush(stdout);
    printf("corpustime = %ld\n", corpustime);

    fflush(stdout);


    //passing the control back to threads after corpusOccurenceTable creation so that threads can calculate tf-idf
/*
    pthread_mutex_lock(&corpus_flag_mutex);
    corpus_flag = 1;
    pthread_cond_broadcast(&corpus_cv);
    printf("broadcasted");
    fflush(stdout);
    pthread_mutex_unlock(&corpus_flag_mutex);
    pthread_cond_destroy(&corpus_cv);
    fflush(stdout); 
*/
    all_parsedDocs_size = all_parsedDocs.size();

    pthread_barrier_wait(&corpus_barrier);
    pthread_barrier_destroy(&corpus_barrier);


    //waiting for threads to join after they have finished the calculation of tf-idf
    for(i=0;i<n;i++)
    {
        pthread_join(threads[i], NULL);    
    }


//dump the top 10 similar-docs for each document    
#ifndef AFFIXES_ONLY
    gettimeofday(&dump_start, NULL); 
    dumpTop10Similarities(vectors);
    gettimeofday(&dump_end, NULL); 
    long dumptime = calcDiffTime(&dump_start, &dump_end);
    printf("dumptime = %ld\n", dumptime);
#endif

    memset(cwd, '\0', 1024);
    memset(base_path, '\0', 1024);
    strcpy(cwd, argv[1]);
    if (chdir(cwd) == -1) {
        cout<< "Cannot chdir to "<<cwd<<endl;
        exit(1);
    }           

    return 0;    
}