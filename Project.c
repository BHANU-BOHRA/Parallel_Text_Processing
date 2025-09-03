#include<stdio.h>
#include<string.h>
#include<omp.h>

#define MAX_LINES 1000
#define MAX_LENGTH 256

char fileData[MAX_LINES][MAX_LENGTH];
int lineCount = 0;

char toUpperCase(char c)
{
	return (c>=97 && c<=122) ? c-32 : c; 
}

char toLowerCase(char c)
{
	return (c>=65 && c<=90) ? c+32 : c; 
}

void readFile(char *filename)
{
    FILE *fp = fopen(filename, "r");
    
    if(!fp)
	{
        printf("Error opening File: %s\n", filename);
        return;
    }

    while(lineCount < MAX_LINES && fgets(fileData[lineCount], MAX_LENGTH, fp))
	{
        #pragma omp critical
        {
            fileData[lineCount][strcspn(fileData[lineCount], "\n")] = 0;
            lineCount++;
        }
    }

    fclose(fp);
}


void toUpper()
{
	int i;
    #pragma omp parallel for
    for (i = 0; i < lineCount; i++)
	{
		int j;
        for (j = 0; fileData[i][j] != '\0'; j++)
            fileData[i][j] = toUpperCase(fileData[i][j]);
        
    }
}

void toLower()
{
	int i;
    #pragma omp parallel for
    for (i = 0; i < lineCount; i++)
	{
		int j;
        for (j = 0; fileData[i][j] != '\0'; j++)
            fileData[i][j] = toLowerCase(fileData[i][j]);
    }
}

void reverse()
{
	int i;
    #pragma omp parallel for
    for (i = 0; i < lineCount; i++)
	{
        int j = strlen(fileData[i]) - 1, k = 0;
        while (k < j) 
		{
            char tmp = fileData[i][k];
            fileData[i][k] = fileData[i][j];
            fileData[i][j] = tmp;
            k++;
            j--;
        }
    }
}

void orderReverse()
{
	int i;
	
    #pragma omp parallel for
    for (i = 0; i < lineCount / 2; i++)
	{
        char tmp[MAX_LENGTH];
        strcpy(tmp, fileData[i]);

        #pragma omp critical  
        {
            strcpy(fileData[i], fileData[lineCount - i - 1]);
            strcpy(fileData[lineCount - i - 1], tmp);
        }
    }
}

void writeFile(char *filename)
{
    FILE *fp = fopen(filename, "w");
    int i;
    
    if(!fp)
	{
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    for(i = 0; i < lineCount; i++)
	{
        #pragma omp critical
        fprintf(fp, "%s\n", fileData[i]);
    }

    fclose(fp);
}

void main(int argc, char **argv)
{
    if (argc < 4 || argc > 6)
	{
        printf("Usage: %s <input_file> <output_file> <-u/-l> [-rl] [-rc]\n", argv[0]);
        return;
    }

    char *inputFile = argv[1];
    char *outputFile = argv[2];
    
    readFile(inputFile);

    #pragma omp parallel sections
    {
        
        #pragma omp section
        {
        	if(argv[3][1]=='u' || argv[3][1]=='u')
				toUpper();
			
            else
            	toLower();
            
        }
            
        #pragma omp section
        {
        	if(argc == 6)
        	{
        		reverse();
        		orderReverse();
			}
			
			else if(argc == 5)
			{
				if(argv[4][2]=='c' || argv[4][2]=='c')
					reverse();
				else
					orderReverse();
			}	
		}
    }
    
    writeFile(outputFile);

    printf("File processing complete.\n");
}
