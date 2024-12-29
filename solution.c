#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024
#define MAX_WORD_SIZE 1024
#define ALPHABET_SIZE 26


typedef struct Trie{
    struct Trie *children[ALPHABET_SIZE];
    int end_of_word; 
} Trie;


struct MsgBuffer{
    long mtype;
    int key;
};


Trie* createNewNode(){
    Trie* node = (Trie*)malloc(sizeof(Trie));
    if (!node) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < ALPHABET_SIZE; i++){
        node->children[i] = NULL;
    }
    node->end_of_word = 0;
    return node;
}


void insert(Trie* trie_root, const char* word){
    Trie* curr = trie_root;
    int index;

    for(int i = 0; i < strlen(word); i++){
        index = word[i] - 'a';
        if (index > ALPHABET_SIZE){
            return;
        }
        if (curr->children[index] == NULL){
            curr->children[index] = createNewNode();
        }
        curr = curr->children[index];
    }
    curr->end_of_word += 1;
}


int searchTrie(Trie* trie_root, const char* word){
    Trie* curr = trie_root;
    int index;

    for (int i = 0; i < strlen(word); i++){
        index = word[i] - 'a';
        if (index > ALPHABET_SIZE){
            return 0;
        }
        if (curr->children[index] == NULL){
            return 0;
        }
        curr = curr->children[index];
    }
    return curr->end_of_word;
}


char* cipher(char* word, int key){
    key = key % 26;
    while (key < 0){
        key += 26;
    }
    
    char* ans = (char*)malloc(strlen(word) + 1);
    for(int i = 0; i < strlen(word); i++){
        // printf("%c\n", word[i]);
        // printf("%d\n", (int)word[i]);
        // printf("%d\n", (int)word[i] - 'a' + key);
        // printf("%c\n", ((int)word[i] - 'a' + key) % 26 + 'a');
        if (word[i] >= 'a' && word[i] <= 'z'){
            ans[i] = (char) ( ((int)word[i] - 'a' + key) % 26 + 'a');
        }else{
            ans[i] = word[i];
        }
    }
    ans[strlen(word)] = '\0';

    return ans;
}

int main(int argc, char* argv[]){
    // write to open the input file
    char filename[100] = "input";
    strcat(filename, argv[1]);
    strcat(filename, ".txt");

    FILE* file = fopen(filename, "r");
    char line[MAX_LINE_LENGTH];

    if (file == NULL){
        perror("error in reading input file\n");
        exit(EXIT_FAILURE);
    }

    int size_of_matrix, max_len_word, shared_key, helper_key;

    fscanf(file, "%d", &size_of_matrix);
    fscanf(file, "%d", &max_len_word);
    fscanf(file, "%d", &shared_key);
    fscanf(file, "%d", &helper_key);
    fclose(file);
    // printf("%d %d %d %d", size_of_matrix, max_len_word, shared_key, helper_key);
    // printf("success on input file \n");


    // to open the words file
    char filename1[100] = "words";
    strcat(filename1, argv[1]);
    strcat(filename1, ".txt");

    FILE* file2 = fopen(filename1, "r");

    if (file2 == NULL){
        perror("error in reading the words file");
        exit(EXIT_FAILURE);
    }

    char word[max_len_word];
    Trie* trie_root = createNewNode();

    while (fscanf(file2, "%s", word) == 1){
        insert(trie_root, word);
    }

    fclose(file2);
    // printf("success on words file \n");



    int shmid;
    char (*shmPtr) [size_of_matrix][max_len_word];

    shmid = shmget(shared_key, size_of_matrix * size_of_matrix * max_len_word * sizeof(char), 0644);
    if (shmid == -1){
        perror("error in getting shmid");
        exit(EXIT_FAILURE);
    }
    shmPtr = shmat(shmid, NULL, 0);
    if ((shmPtr) == (void*) -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    int msqid = msgget(helper_key, 0);
    if (msqid == -1){
        perror("msqid");
        exit(EXIT_FAILURE);
    }
    int key = 0;



    // traversing through diagnols
    for(int possible_diagnol_Sums = 0; possible_diagnol_Sums < 2*size_of_matrix - 1; possible_diagnol_Sums++ ){
        int sum1 = 0;
        for(int current_row = 0; current_row <= possible_diagnol_Sums; current_row++){
            int current_col = possible_diagnol_Sums - current_row;
            if (current_row < size_of_matrix && current_col < size_of_matrix){
                char* word = shmPtr[current_row][current_col];
                char* word2 = cipher(word, key);
                sum1 += searchTrie(trie_root, word2);
            }
        }
        
        // can use forking here for child and parent message queue passing
        struct MsgBuffer my_buf;
        my_buf.mtype = 1;
        my_buf.key = sum1;     
        msgsnd(msqid, &my_buf, sizeof(my_buf) - sizeof(my_buf.mtype) , 0);
       
        struct MsgBuffer rec_buf;
        msgrcv(msqid, &rec_buf, sizeof(rec_buf) - sizeof(rec_buf.mtype), 0, 0);
        key = rec_buf.key;
    }

    return 0;
}