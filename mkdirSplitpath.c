#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;


void mkdir(char pathName[]){

    if (pathName == NULL || strlen(pathName) == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64], dirName[128];
    struct NODE* parent = splitPath(pathName, baseName, dirName);

    if (!parent) {
        
        return;
    }

    if (strlen(baseName) == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    
    struct NODE* check = parent->childPtr;
    while (check) {
        if (strcmp(check->name, baseName) == 0 && check->fileType == 'D') {
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return;
        }
        check = check->siblingPtr;
    }

    
    struct NODE* newNode = (struct NODE*)malloc(sizeof(struct NODE));
    strcpy(newNode->name, baseName);
    newNode->fileType = 'D';
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;
    newNode->parentPtr = parent;

    
    if (parent->childPtr == NULL) {
        parent->childPtr = newNode;
    } else {
        struct NODE* temp = parent->childPtr;
        while (temp->siblingPtr)
            temp = temp->siblingPtr;
        temp->siblingPtr = newNode;
    }

    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

struct NODE* findChild(struct NODE* parent, char* name) {
    struct NODE* curr = parent->childPtr;
    while (curr) {
        if (strcmp(curr->name, name) == 0 && curr->fileType == 'D') {
            return curr;
        }
        curr = curr->siblingPtr;
    }
    return NULL;
}


struct NODE* splitPath(char* pathName, char* baseName, char* dirName){

    char temp[128];
    char* token;
    struct NODE* start;

    
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        strcpy(baseName, "");
        return root;
    }

    
    strcpy(temp, pathName);

    
    if (pathName[0] == '/') {
        start = root;
        token = strtok(temp + 1, "/"); 
    } else {
        start = cwd;
        token = strtok(temp, "/");
    }

    
    if (!strchr(pathName, '/')) {
        strcpy(dirName, "");
        strcpy(baseName, pathName);
        return cwd; 
    }

    struct NODE* current = start;
    struct NODE* nextDir = NULL;

    
    char dirPath[128] = "";
    while (token) {
        
        char* nextToken = strtok(NULL, "/");

        if (nextToken == NULL) {
            
            strcpy(baseName, token);

            
            if (pathName[0] == '/')
                strcpy(dirName, "/");
            else
                strcpy(dirName, "");

            
            char tmp2[128];
            strcpy(tmp2, pathName);
            tmp2[strlen(pathName) - strlen(baseName)] = '\0';

            
            if (tmp2[strlen(tmp2) - 1] == '/')
                tmp2[strlen(tmp2) - 1] = '\0';

            strcpy(dirName, tmp2);
            return current;
        }

        
        nextDir = findChild(current, token);
        if (!nextDir) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }

        current = nextDir;
        token = nextToken;
    }

    return current;
}
