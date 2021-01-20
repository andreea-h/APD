#include<stdio.h> 
int main() 
{ 
    FILE* ptr = fopen("abc.txt","r"); 
    if (ptr==NULL) 
    { 
        printf("no such file."); 
        return 0; 
    } 
  
    /* Assuming that abc.txt has content in below 
       format 
       NAME    AGE   CITY 
       abc     12    hyderbad 
       bef     25    delhi 
       cce     65    bangalore */
    char buf1[100]; 
    char buf2[100]; 
    char buf3[100]; 
    while (fscanf(ptr, "horror\n%*[^\n]\n %s", buf1)==1) 
        printf("%s\n", buf1); 
  
    return 0; 
} 