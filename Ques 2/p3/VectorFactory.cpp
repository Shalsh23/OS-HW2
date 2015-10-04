#include <list>
#include <hash_map.h>

#ifndef HASH_MAP_TOKENS
#define HASH_MAP_TOKENS hash_map<char*, int, hash<char*>, eqstr>
#endif

#define HASH_MAP_VECTOR hash_map<char*, DocumentVector*, hash<char*>, eqstr>
#define HASH_MAP_OCCURENCE_TABLE hash_map<char*, int, hash<char*>, eqstr>
#define HASH_MAP_PARSED_DOCS hash_map<char*, HASH_MAP_TOKENS, hash<char*>, eqstr>

extern char cwd[1024];

// extern int num_pdocs;
// extern pthread_mutex_t num_pdocs_lock;

class VectorFactory {

    public:
    HASH_MAP_VECTOR createVectors(Parser parser, list<char*> docs);

    HASH_MAP_PARSED_DOCS createParsedDoc(Parser parser, list<char*> docs); //added by Shalki

    HASH_MAP_OCCURENCE_TABLE createCorpusOccurenceTable(HASH_MAP_PARSED_DOCS parsedDocs);
    HASH_MAP_PARSED_DOCS parseDocs(Parser parser, list<char*> docs);

    HASH_MAP_VECTOR cal_tfidf(HASH_MAP_PARSED_DOCS pDocs, HASH_MAP_OCCURENCE_TABLE coTable, float all_parsedDocs_size); //added by Shalki

    void dumpDocs(Parser parser, HASH_MAP_PARSED_DOCS parsedDocs);
};

struct timeval parser_start;
struct timeval parser_end;
struct timeval corpus_start;
struct timeval corpus_end;
struct timeval normalize_start;
struct timeval normalize_end;

long calcDiffTime(struct timeval* strtTime, struct timeval* endTime)
{
    return(
        endTime->tv_sec*1000000 + endTime->tv_usec
        - strtTime->tv_sec*1000000 - strtTime->tv_usec
        );
  
}

// author: Shalki
HASH_MAP_PARSED_DOCS VectorFactory::createParsedDoc(Parser parser, list<char*> docs)
{
    gettimeofday(&parser_start, NULL); 
    HASH_MAP_PARSED_DOCS parsedDocs = parseDocs(parser, docs);
    gettimeofday(&parser_end, NULL); 
    long parsetime = calcDiffTime(&parser_start, &parser_end);
    printf("parsetime = %ld\n", parsetime);

    // pthread_mutex_lock(&num_pdocs_lock);
    // num_pdocs++;
    // printf("num_pdocs=%d",num_pdocs);
    // pthread_mutex_unlock(&num_pdocs_lock);

    return parsedDocs;
}

//author: Shalki
// Calculate tf-idf from parsed docs and corpus occurence table
HASH_MAP_VECTOR VectorFactory::cal_tfidf(HASH_MAP_PARSED_DOCS pDocs, HASH_MAP_OCCURENCE_TABLE coTable, float all_parsedDocs_size){

    HASH_MAP_VECTOR vectors;
/*
    gettimeofday(&parser_start, NULL); 
    HASH_MAP_PARSED_DOCS parsedDocs = parseDocs(parser, docs);
    gettimeofday(&parser_end, NULL); 
    long parsetime = calcDiffTime(&parser_start, &parser_end);
    printf("parsetime = %ld\n", parsetime);

#ifdef AFFIXES_ONLY
    VectorFactory v1;
    v1.dumpDocs(parser, parsedDocs);
    return(vectors);
#endif

    gettimeofday(&corpus_start, NULL); 
    HASH_MAP_OCCURENCE_TABLE corpusOccurences = createCorpusOccurenceTable(parsedDocs);
    gettimeofday(&corpus_end, NULL); 
    long corpustime = calcDiffTime(&corpus_start, &corpus_end);
    printf("corpustime = %ld\n", corpustime);
*/

    HASH_MAP_PARSED_DOCS parsedDocs = pDocs;
    HASH_MAP_OCCURENCE_TABLE corpusOccurences = coTable;

    HASH_MAP_PARSED_DOCS::const_iterator it1;
    long normalizetime = 0;
    
    for ( it1=parsedDocs.begin() ; it1 != parsedDocs.end(); it1++ )
    {
        char *file = (*it1).first;
        HASH_MAP_TOKENS freqTable = (*it1).second;
        
        DocumentVector *dv = new DocumentVector();
        
        HASH_MAP_TOKENS::const_iterator it2;
        for ( it2=freqTable.begin() ; it2 != freqTable.end(); it2++)
        {
            char *token = (*it2).first;
            int freq;
            freq= (int)((*it2).second);
            
            float tf = (float)freq;
            //float docQuan = (float) parsedDocs.size();
            float docQuan = all_parsedDocs_size;
            float corpOcc = (float)(corpusOccurences[token]);
            float idf = (float) (log(docQuan / corpOcc));
            
            float tfidf = tf * idf;
            
            dv->addElement(token, tfidf);
        }
        
        gettimeofday(&normalize_start, NULL); 
        vectors[file] = dv->normalize();
        gettimeofday(&normalize_end, NULL); 
        normalizetime += calcDiffTime(&normalize_start, &normalize_end);
    }

    printf("normalizetime = %ld\n", normalizetime);
    return(vectors);
}






HASH_MAP_VECTOR VectorFactory::createVectors(Parser parser, list<char*> docs){

    HASH_MAP_VECTOR vectors;

    gettimeofday(&parser_start, NULL); 
    HASH_MAP_PARSED_DOCS parsedDocs = parseDocs(parser, docs);
    gettimeofday(&parser_end, NULL); 
    long parsetime = calcDiffTime(&parser_start, &parser_end);
    printf("parsetime = %ld\n", parsetime);

#ifdef AFFIXES_ONLY
    VectorFactory v1;
    v1.dumpDocs(parser, parsedDocs);
    return(vectors);
#endif

    gettimeofday(&corpus_start, NULL); 
    HASH_MAP_OCCURENCE_TABLE corpusOccurences = createCorpusOccurenceTable(parsedDocs);
    gettimeofday(&corpus_end, NULL); 
    long corpustime = calcDiffTime(&corpus_start, &corpus_end);
    printf("corpustime = %ld\n", corpustime);

    HASH_MAP_PARSED_DOCS::const_iterator it1;
    long normalizetime = 0;
    
    for ( it1=parsedDocs.begin() ; it1 != parsedDocs.end(); it1++ )
    {
        char *file = (*it1).first;
        HASH_MAP_TOKENS freqTable = (*it1).second;
        
        DocumentVector *dv = new DocumentVector();
        
        HASH_MAP_TOKENS::const_iterator it2;
        for ( it2=freqTable.begin() ; it2 != freqTable.end(); it2++)
        {
            char *token = (*it2).first;
            int freq;
            freq= (int)((*it2).second);
            
            float tf = (float)freq;
            float docQuan = (float) parsedDocs.size();
            float corpOcc = (float)(corpusOccurences[token]);
            float idf = (float) (log(docQuan / corpOcc));
            
            float tfidf = tf * idf;
            
            dv->addElement(token, tfidf);
        }
        
        gettimeofday(&normalize_start, NULL); 
        vectors[file] = dv->normalize();
        gettimeofday(&normalize_end, NULL); 
        normalizetime += calcDiffTime(&normalize_start, &normalize_end);
    }

    printf("normalizetime = %ld\n", normalizetime);
    return(vectors);
}
    
HASH_MAP_PARSED_DOCS VectorFactory::parseDocs(Parser parser, list<char*> docs) {
    HASH_MAP_PARSED_DOCS parsedDocs;
        
    list<char*>::iterator it1;
    for ( it1=docs.begin() ; it1 != docs.end(); it1++ )
    {
        char *file = (char *) malloc(strlen(cwd) + strlen(*it1) + 1);
	strcpy(file, cwd);
	strcat(file, *it1);
	
        HASH_MAP_TOKENS freqTable;
        FILE *fp = fopen(file, "r");
        freqTable = parser.parseFile(fp);
        fclose(fp);
        parsedDocs[*it1] = freqTable;
    }
    return(parsedDocs);
}
    
void VectorFactory::dumpDocs(Parser parser, HASH_MAP_PARSED_DOCS parsedDocs){
    HASH_MAP_PARSED_DOCS::const_iterator it1;

    for ( it1=parsedDocs.begin() ; it1 != parsedDocs.end(); it1++ )
    {
        char *file = (*it1).first;
        HASH_MAP_TOKENS freqTable = (*it1).second;

//	cout << &file[strlen(cwd)] << ":" << endl;
        cout << file << ":" << endl;
	parser.mapIterate(freqTable);
    }
}
    
HASH_MAP_OCCURENCE_TABLE VectorFactory::createCorpusOccurenceTable
(
    HASH_MAP_PARSED_DOCS parsedDocs
) 
{
    int result;
    HASH_MAP_OCCURENCE_TABLE occurenceTable;
    
    HASH_MAP_PARSED_DOCS::const_iterator it1;
    for ( it1=parsedDocs.begin() ; it1 != parsedDocs.end(); it1++ )
    {
        char *file = (*it1).first;
        HASH_MAP_TOKENS freqTable = (*it1).second;

        HASH_MAP_TOKENS::const_iterator it2;
        for ( it2=freqTable.begin() ; it2 != freqTable.end(); it2++)
        {
            char *token = (*it2).first;
            HASH_MAP_TOKENS::const_iterator it3;
            it3 = freqTable.find(token);
            result = (it3 != freqTable.end()?1:0);
            
            if (result == 1) {
                occurenceTable[token] += 1;
            }
            else {
                occurenceTable[token] = 1;
            }
        }
    }
    
    return(occurenceTable);
}


