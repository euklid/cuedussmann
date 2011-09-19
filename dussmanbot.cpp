#include<stdio.h>
#include<curl/curl.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
#include<malloc.h>
#include<memory.h>
#include<ctype.h>
#define _FILE_OFFSET_BITS 64 /* for pre libcurl 7.19.0 curl_off_t magic */
/*
 * TODO:    grep -A (lines after found) :DONE Done
 *
 *          cut -c foo   the first foo characs --> extra function, 'cause many
 *          NULL pointers in cut needed :DONE Done
 *
 *          tail selbiges wie unten??? nö :DONE    F I X M E 
 *
 *          head geht das nicht mit verschieben des Lesepointers???? kein Plan,
 *          jetzt eine extra Funktion:DONE Done    F I X M E
 *
 *          curl :DONE
 *
 *          cat -b and cat -n (number first: non-blank lines, second: all lines)
 * 			 -- das kann man über eine einfache Funktion machen wie bei find und
 * 			so
 *
 *          is cut -f3- <-- the minus   already implement??? test it! nope F I X M E :Done 
 *
 *          learn how to add lines to a file and how to create a file :DONE
 *
 *          add rating of food if no rating was there including making new
 *          groups and adding food to this groups --> SEE CHANGED CONCEPT
 *
 *          extracting the parts of food and find the value for it and getting
 *          the best food for the day by comparing the sums
 * 
 * TODO: Werte für die Arraygrößen bei hidden und bergruen und bergruend usw. übergeben
 * 
 * TODO: Desserts nicht vergessen!!
 *
 */
 
int loginandcookie(char* userid, char* passwd);
int kalwochen(char* userid, char* passwd);
void tagesauswahl();
void getsel_datums();
void createmenufiles(char* userid, char* passwd);
void gethiddenandbestellt();
void getdatensatz();
void getratingandbestelldaten();
void sendbestellung();
char* frstln(char* dest, int size, char inputfile[]);
char* lstln(const char inputfile[]);
char* frstnchr(char* input, int n);
//char* cut(char* input, const char* delim, int fieldstart, int fieldstop);
char* cut2(char* inputstring, char* delim, int fieldstart, int fieldstop);
int find(const char inputfile[], const char searchstring[], int linesafter = 0);
char* removeformattingsigns(char* input);
char *strlwr(char *s);
FILE* findfile;
int startwoche=0, anzwoche=0;
int** setdates; //Array, das speichert, für welche Tage bestellt werden soll.
char** slynmbwochen; //Array, das Werte für sel_datum speichert, sodass bestellt werden kann.
char**** wocheplustagplusdaten; //[woche][tag][datensatz], datensatz=1: Menüname, 2: Variablenname, 3:wert, das gleich für menü 2 und vegetarisch weiter indizieren 
char*** hidden; //[woche][datensatz] datensätze: variablenname=wert
char*** bergruen; //speichert nur die grünen menüs, nicht die grünen dessert!!!
char*** bergruend; //der speichert die Desserts :-)
int ** wirkbestellen; //Für welche Tage wirklich bestellt werden soll
char*** bestelldaten; //Daten, die nicht hidden sind, also die Bestelldaten

char uid[6], pwd[6];
//int main()
//{
//	char uid[5], pwd[5];
//	printf("Herzlich willkommen. Ich bin ihr persönlicher Essensberater!\n \n");
//	printf("Was ist ihre Benutzernummer: ");
//	scanf("%s",uid);
//	printf("Was ist ihr Passwort: ");
//	scanf("%s",pwd);
//	loginandcookie(uid,pwd);
//	//////find("kalendera","<select name=\"sel_datum\" class=\"no_print\" onchange=\"document.form_sel_datum.submit()\">",12);
//	kalwochen(uid, pwd);
//	tagesauswahl();
//	getsel_datums();
//	createmenufiles(uid,pwd);
//	gethiddenandbestellt();
//	getdatensatz();
//	getratingandbestelldaten();
//	sendbestellung();
//    //char text[] = "Guten Tagderefd\npens\0i";
//    //char *cutten = cut(text, "t", 2, 2);
//    //printf("%s",cutten);
//    //find("testfile", "ent");
//    ////printf("%s\n",frstnchr(text,7));
//    //printf("%s", frstln("testfile"));
//}

/*
char* frstln(const char inputfile[])
{
    FILE* input;
    char c;
    int charcount=2000;
    input = fopen(inputfile, "r");
    if (input == NULL) return NULL;
//    while((c=fgetc(input)) != '\n')
//    {
//        charcount++;
//    }
    rewind(input);
    char* output = (char*) malloc(charcount * sizeof(char));
    fgets(output, charcount, input);
    fclose(input);
    //printf("%s",output);
	output[strlen(output)]='\0';
	//printf("%i",strlen(output));
	if(output[strlen(output)-1]=='\n') output[strlen(output)-1]='\0';
	return output;
    
} */


char *strlwr(char *s)
{
  char *p = s;

  while (*p)
  {
    *p = (char) tolower(*p);
    p++;
  }

  return s;
}
/*
 * // Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.  
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.  
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
// SUCH DAMAGE.
 */ //--> für die strlwr function

char* cut2(char* inputstring, char* delim, int fieldstart, int fieldstop)
{
	int fieldcount=1;
	int fldstart=fieldstart, fldstop=fieldstop;
	char* pch;
	char** fields;
	//char* output;
	int outputsize;
	char inputcpy[strlen(inputstring)+1];
	strcpy(inputcpy,inputstring);
	//puts("hello");
	pch=strtok(inputcpy, delim);
	while (pch != NULL)
	{
		//printf("%s\n",pch);
		pch = strtok (NULL,delim);
		fieldcount++;
		//printf("%i\n",fieldcount);
	}
	//puts("hillo");
	fieldcount--;
	//printf("%i",fieldcount);
	if(fieldstop>fieldcount) fldstop=fieldcount;
	if(fieldstart>fieldcount) return '\0';
	if(fieldstart>fieldcount) return'\0';
	if(fieldstart<=0) fldstart=1;
	fields=(char**)calloc(fieldcount,sizeof(char*));
	fieldcount=0;
	//puts("hallo");
	strcpy(inputcpy,inputstring);
	//puts("hi");
	pch=strtok(inputcpy,delim);
	fields[0]=(char*)malloc(sizeof(char)*(strlen(pch)+1));
	//puts("penis");
	strcpy(fields[0],pch);
	//printf("%s",fields[0]);
	//puts("haha");
	//printf("%s",fields[0]);
	do
	{
		pch=strtok(NULL,delim);
		fieldcount++;
		if(pch!=NULL) 
		{
			fields[fieldcount]=(char*)malloc(sizeof(char)*(strlen(pch)+1));
			strcpy(fields[fieldcount],pch);
		}
	}while(pch!= NULL);
	outputsize=fldstop-fldstart+1;
	for(int i=fldstart-1; i<fldstop;i++)
	{
		//printf("%s\n",fields[i]);
		outputsize+=strlen(fields[i]);
	}
	//printf("%i",outputsize);
	//puts("foo");
	//puts("bar");
	//output=(char*)malloc(outputsize); 
	strcpy(inputstring,fields[fldstart-1]);if(fldstart<fldstop) strcat(inputstring,delim);
	for(int i=fldstart; i<fldstop-1;i++)
	{
		strcat(inputstring,fields[i]);strcat(inputstring,delim);	
	}
	if(fldstart<fldstop) strcat(inputstring,fields[fldstop-1]);
	inputstring[outputsize-1]='\0';
	//output=(char*)malloc(outputsize);
	//for(int i=fldstart-1; i<fldstop-1;i++)
	//{
	//	strcat(output,fields[i]);strcat(output," ");	
	//}
	//strcat(output,fields[fldstop-1]);
	//output[outputsize-1]='\0';
	free(fields);
	//inputstring=strcpy(inputstring, output);
	//free(output);
	//printf("%s",inputstring);
	return inputstring;
}

char* frstln(char* dest,int size ,char inputfile[])
{
	FILE* input;
	input=fopen(inputfile,"r");
	fgets(dest, size, input);
	if(dest[strlen(dest)-1]=='\n') dest[strlen(dest)-1]='\0';
	else { if(strlen(dest)<size) dest[strlen(dest)]='\0';
			else dest[strlen(dest)-1]='\0';};
	fclose(input);
	return dest;
}

char* lstln(const char inputfile[])
{
    FILE* input;
    input = fopen(inputfile, "r");
    if (input == NULL) return NULL;
    int numlines=0;
    char c;
    char* lastline= (char*)malloc(2000);
    while ((c = fgetc(input)) != EOF)
    {
        if (c == '\n')
        {
            numlines++;
        }
    }
    rewind(input);
    for(int i=1; i<=numlines; i++)
    {
        fgets(lastline, 2000, input);
    }
    fclose(input);
    //printf("%s",lastline);
    char* lstline=lastline;
    free(lastline);
    return lstline;


}

char* frstnchr(char* input, int n)
{
    char* outputstr;
    if (n >= strlen(input))
    {
        outputstr = (char*) malloc(strlen(input)+1);
        outputstr = input;
    }
    else
    {
        outputstr = (char*) malloc(n + 1);
        for (int i = 0; i < n; i++) outputstr[i] = input[i];
        outputstr[n]='\0';
    }
    return outputstr;
    free(outputstr);
}

/******************************************************************************
 * The following routine is for cutting strings, does not include the start or*
 * end delimiter, but all characters that are between them                    *
 ******************************************************************************/

/*char* cut2(char inputstring[], char* delim, int fieldstart, int fieldstop)
{
	int fieldcount=1;
	int fldstart=fieldstart, fldstop=fieldstop;
	char* pch;
	char** fields;
	char* output;
	int outputsize;
	char inputcpy[strlen(inputstring)+1];
	strcpy(inputcpy,inputstring);
	//puts("hello");
	pch=strtok(inputcpy, delim);
	while (pch != NULL)
	{
		//printf("%s\n",pch);
		pch = strtok (NULL,delim);
		fieldcount++;
		//printf("%i\n",fieldcount);
	}
	//puts("hillo");
	fieldcount--;
	//printf("%i",fieldcount);
	if(fieldstop>fieldcount) fldstop=fieldcount;
	if(fieldstart>fieldcount) return '\0';
	if(fieldstart>fieldcount) return'\0';
	if(fieldstart<=0) fldstart=1;
	fields=(char**)calloc(fieldcount,sizeof(char*));
	fieldcount=0;
	//puts("hallo");
	strcpy(inputcpy,inputstring);
	//puts("hi");
	pch=strtok(inputcpy,delim);
	fields[0]=(char*)malloc(sizeof(char)*(strlen(pch)+1));
	//puts("penis");
	strcpy(fields[0],pch);
	//printf("%s",fields[0]);
	//puts("haha");
	//printf("%s",fields[0]);
	do
	{
		pch=strtok(NULL,delim);
		fieldcount++;
		if(pch!=NULL) 
		{
			fields[fieldcount]=(char*)malloc(sizeof(char)*(strlen(pch)+1));
			strcpy(fields[fieldcount],pch);
		}
	}while(pch!= NULL);
	outputsize=fieldstop-fieldstart+1;
	for(int i=fldstart-1; i<fldstop;i++)
	{
		//printf("%s\n",fields[i]);
		outputsize+=strlen(fields[i]);
	}
	//printf("%i",outputsize);
	//puts("foo");
	//puts("bar");
	output=(char*)malloc(outputsize); 
	strcpy(output,fields[fldstart-1]);if(fldstart<fldstop) strcat(output,delim);
	for(int i=fldstart; i<fldstop-1;i++)
	{
		strcat(output,fields[i]);strcat(output,delim);	
	}
	strcat(output,fields[fldstop-1]);
	output[outputsize-1]='\0';
	free(fields);
	return output;	
} */

/*char* cut(char input[], const char delim[], int fieldstart, int fieldstop) // ICH bin im Arsch --> nun mit strtok schreiben!
{
	
    int counter = 1;
    bool began = false;
    char* output;//free(output);
    int maxlength;
    int startpos=0;
    int charcount=0;
    int fldcount=1;
    //printf("stringlenge %i\n",strlen(input));
    //output = (char*) malloc(strlen(input));
    int outputstart = -2;
    for(int i=0;i<strlen(input);i++)
    {
		if(input[i]==delim[0]) fldcount++;

		if((fldcount>=fieldstart) && (fldcount<=fieldstop) && (input[i]!=delim[0])) charcount++;
		if(input[i]<32) break;
	}

	//printf("fotze %i\n",charcount);
    output = (char*)malloc(sizeof(char*)*charcount);free(output);    output = (char*)malloc(sizeof(char*)*charcount);//printf("fotze %i\n",charcount);
    //printf("Das ist outputs groeße: %i",sizeof(output));
    for (int i = 0; i < strlen(input); i++)
    {
		//printf("%i \n",strlen(output));
        if (outputstart != -2) outputstart++;
        if ((input[i] == delim[0]))
        {
            counter++;
        }
        if (counter > fieldstop) 
        {
			output[outputstart]='\0';
			break;
		}
		if(i-charcount>startpos) { output[outputstart]='\0';break;}
		if((input[i]=='\n') || (input[i]=='\0')){ output[outputstart]=='\0';break;}

        if ((counter == fieldstart) && (began == false))
        {
            began = true;
            startpos=i;
            //printf("startpos %i\n",startpos);
            if (fieldstart != 1) outputstart = -1;
            if (fieldstart == 1) outputstart = 0;
        }
        if ((counter >= fieldstart) && (outputstart >= 0))
        {
            output[outputstart] = input[i];
            
        }
        //putchar(output[i-startpos-1]);
    }
    //printf("Das ist outputslenge: %i",strlen(output));
    return output;
    
} */

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int find(const char inputfile[], const char searchstring[], int linesafter)
{
    FILE *INPUT; //File-Pointer
    INPUT = fopen(inputfile, "rt"); //Open File
    if (INPUT == NULL) printf("Error!");

    int numlines = 0;
    int everfound = 0;
    char c;
    while ((c = fgetc(INPUT)) != EOF)
    {
        if (c == '\n') numlines++;
    }
    //printf("%d\n", numlines);
    fclose(INPUT);
    //printf("%d", numlines ); now it works --> go to toilet

    INPUT = fopen(inputfile, "r");
    char** lines;
    lines = (char **) calloc(numlines, sizeof (char *));
    for (int i = 0; i < numlines; i++)
    {
        lines[i] = (char*) malloc(2000 * sizeof (char*));
        //printf("test %i\n",i);
        fgets(lines[i], 2000, INPUT);
    } //HOLY Shit: It WORKS! --> Implement finding a substring in those lines
    //puts("testblablub");
    //putchar(lines[4][2]);
    
    int foundlines[numlines];
    for (int i = 0; i < numlines; i++) 
    {
		foundlines[i] = 0;
		//printf("%s",lines[i]);
	}
    //printf("penis\n");
    for (int i = 0; i < numlines; i++)
    {
        int position;
        position = 0;
        int found = 0;
		//printf("penis %i   \n",i);
		//int platz = strlen(lines[i]) - strlen(searchstring);
		if( (strlen(lines[i]) - strlen(searchstring)) >=0)
		{
			//puts("ficken");
			if(strstr(lines[i],searchstring) != NULL)
			{
				//printf("pimmerle %i   \n",i);
				everfound=1;
				foundlines[i]=1;
			}
		}
/*
        //while (position <= platz)
        //{
			//if(position > platz) break;
			
            //if (searchstring[0] == lines[i][position])
            //{
                //int stillok = 1;
                //for (int j = 0; j < strlen(searchstring); j++)
					
                    //if (stillok)
                    //{
                        //if (searchstring[j] == lines[i][position + j])
                        //{
							//printf("%i  ",strlen(lines[i]));
							//printf("%i\n",i);
                            //stillok = 1;
                        //}
                        //else
                        //{
                            //stillok = 0;
                            //position++; //if not we'd have an infinite loop
                            //break;
                        //}
                    //}
                    

                    //if ((stillok) && j == strlen(searchstring) - 1)
                    //{
                        //everfound = 1;
                        //found = 1;
                        //foundlines[i] = 1; //ab zum nächsten
                        ////printf("%s",lines[i]);
                        //position++;
                    //}
                    //if(position > platz) break;
                //}

            //}
            //else position++;

            //if (found) break;

        //} */
    }
    //puts("haben wirs geschafft?");
    //nun auf den linesafter Parameter eingehen
    for (int i = 0; i < numlines; i++)
    {
        //printf("%i\n",foundlines[i]);
        if (foundlines[i] == 1)
        {
            if (i + linesafter < numlines)
            {
                for (int j = 1; j <= linesafter; j++)
                {
                    if (foundlines[i + j] != 1) foundlines[i + j] = 2;
                }
            }
            else
            {
                for (int j = 1; i + j < numlines; j++)
                {
                    if (foundlines[i + j] != 1) foundlines[i + j] = 2;
                }
            }
        }
    }

    findfile = fopen("findoutput", "w");
    for (int i = 0; i < numlines; i++)
    {
        //printf("%i\n",foundlines[i]);
        if ((foundlines[i] == 1) || (foundlines[i] == 2))
        {
            //puts("in file schreiben");
            fputs(lines[i], findfile);
        }
    }
    fclose(findfile);

    //irgendwo,irgendwie die gefundenen zeilen speichern, sodass andere fkt.
    //darauf zugreifen können. :DONE
    free(lines);
    //free(*lines);
    
    
    //printf("Das ist linesafter: %i\n",linesafter);
    
    //    INPUT=fopen(inputfile, "r");
    //    char stringtest[100];
    //    for (int i=1; i<=numlines;i++)
    //    {
    //        fgets(stringtest, 99, INPUT); !!!!THIS works... use it as example
    //        printf("%s",stringtest);
    //    }
    //     fclose(INPUT);
    //     return 0;//alles crap--> nochma nachschauen wegen array of chars
    if (everfound)
        return 1;
    else
        return 0;


}

char* removeformattingsigns(char* input) //<-- remove html escape sequences
{ 	//Attention: string is not \0 terminated. A \0 has to be added after \n!!!
	char* output;
	int charnum = 0 ;
	while(input[charnum]!='\n') 
	{
		charnum++;
	}
	charnum++;
	output=(char*)malloc(1+charnum*sizeof(char));
	output=strcpy(output,input);
	output[charnum]='\0'; //<-- now output is '\0' terminated
	if((strstr(input,"<br />")==NULL) && (strstr(input,"&amp;")==NULL) && (strstr(input,"&quot;")==NULL))
	{
		free(output);
		return input;
	}
	while(strstr(output,"<br />")!=NULL)
	{
		int position=0;
		char* pointchar;
		pointchar=strstr(output,"<br />");
		while(&output[position]!=pointchar)
		{
			position++;
		}//<-- get absolute position in string where first occurence found
		output[position]=' '; position++;//< wird zum Leerzeichen, von nun an werden alle Zellen um 5 vorgerückt, bis man das Ende des Strings erreicht hat
		while(position+4!=charnum)
		{
			output[position]=output[position+5];
			position++;
		}
		while(position<charnum)
		{
			output[position]='\0';
			position++;
		}		
	}
	
	while(strstr(output,"&amp;")!=NULL)
	{
		int position=0;
		char* pointchar;
		pointchar=strstr(output,"&amp;");
		while(&output[position]!=pointchar)
		{
			position++;
		}//<-- get absolute position in string where first occurence found
		output[position]=' '; position++;//< wird zum Leerzeichen, von nun an werden alle Zellen um 5 vorgerückt, bis man das Ende des Strings erreicht hat
		while(position+3 != charnum)
		{
			output[position]=output[position+4];
			position++;
		}
		while(position<charnum)
		{
			output[position]='\0';
			position++;
		}		
	}
	
	while(strstr(output,"&quot;")!=NULL)
	{
		int position=0;
		char* pointchar;
		pointchar=strstr(output,"&quot;");
		while(&output[position]!=pointchar)
		{
			position++;
		}//<-- get absolute position in string where first occurence found
		output[position]=' '; position++;//< wird zum Leerzeichen, von nun an werden alle Zellen um 5 vorgerückt, bis man das Ende des Strings erreicht hat
		while(position+4!=charnum)
		{
			output[position]=output[position+5];
			position++;
		}
		while(position<charnum)
		{
			output[position]='\0';
			position++;
		}		
	}
	strcpy(input, output);
	free(output);
	return input;
}

int loginandcookie(char* userid, char* passwd)
{
	FILE* essen;
	essen=fopen("kalendera","w");
	char postfield[100];
	strcpy(postfield,"Login_Name=");
	strcat(postfield,userid);
	strcat(postfield,"&Login_Passwort=");
	strcat(postfield,passwd);
	CURLcode ret;
	CURL *hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, essen);
	curl_easy_setopt(hnd, CURLOPT_INFILESIZE_LARGE, -1);
	curl_easy_setopt(hnd, CURLOPT_URL, "http://dussmann-lpf.rcs.de/index.php?m=1;3&a=akt_login");
	curl_easy_setopt(hnd, CURLOPT_PROXY, NULL);
	curl_easy_setopt(hnd, CURLOPT_PROXYUSERPWD, NULL); 
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postfield);
 	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux i686; de; rv:1.9.2.12) Gecko/20101027 Firefox/3.6.12");
	curl_easy_setopt(hnd, CURLOPT_COOKIEFILE, NULL);
	curl_easy_setopt(hnd, CURLOPT_COOKIEJAR, "Cookiedatei");
	ret = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	fclose(essen);
	return (int)ret;
	
	
}

int kalwochen(char* userid, char* passwd)
{
	//int numwochen=0;
	//int startwoche;
	//char* startwochenbuffer;
	//startwochenbuffer=(char*)malloc(sizeof(char*)*4);
	FILE* wochenliste;
	//if(kalender==NULL) return 0;
	find("kalendera","<select name=\"sel_datum\" class=\"no_print\" onchange=\"document.form_sel_datum.submit()\">",12);
	find("findoutput","KW");
	find("findoutput","selected=\"selected\"",12);
	wochenliste=fopen("findoutput","r");
	char c;
	while((c=fgetc(wochenliste)) != EOF)
	{
		if(c == '\n') anzwoche++;
	}
	fclose(wochenliste);
	//wochenliste=fopen("findoutput","r");
	//puts("test");
	//fgets(buffer,130,wochenliste);
	//buffer[strlen(buffer)-1]='\0';
	char* puffer=(char*)malloc(150);
	frstln(puffer,150,"findoutput");
	//printf("%s\n",puffer);
	//char*tmp=strdup(puffer);
	cut2(puffer,":",2,2);	
	//fclose(wochenliste);
	//printf("%s \n",puffer);
	cut2(puffer," ",1,1);
	//free(puffer);
	//printf("buffer is %s\n",puffer);
	//startwochenbuffer[1]+=4;
	//printf("pimmel %c\n",startwochenbuffer[1]);
	for(int i=48; i<=53; i++)
	{
		if(puffer[0]==i) 
		{
			startwoche=10*(i-48);
			break;
		}
	}
	for(int i=48;i<=57; i++)
	{
		if(puffer[1]==i)
		{
			startwoche+=(i-48);
			break;
		}
	}
	printf("startwoche: %i und anzahlwochen: %i\n",startwoche,anzwoche);
	//free(buffer);
	free(puffer)	;
	//free(puffer);
	return 1;		
}

void tagesauswahl()
{
	//puts("hallo");
	setdates=(int**)calloc(anzwoche,sizeof(int*));
	//int setdates[anzwoche][7];
	for(int i=0; i<anzwoche;i++) setdates[i]=(int*)calloc(7,sizeof(int));
	char janein1=32, janein3[7]; for(int i=0;i<7;i++) janein3[i]=32;
	char janein2=32;
	//int lauf1=0, lauf2=0, lauf3[]={0,0,0,0,0,0,0};
	char wochentage[][11]={"Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Samstag","Sonntag"};
	for(int i=0; i<anzwoche; i++)
	{
		janein1=32; for(int f=0;f<7;f++) janein3[f]=32; janein2=32;
		//puts("hallihallo");
		printf("Willst du in der %i. Woche bestellen? Bitte nur j oder n eingeben:", startwoche+i);
		//puts("keinbock");
		do
		{ 	//lauf1++;
			//puts("keinelust");
			//scanf("%c",&janein1);
			getchar();
			janein1=getchar(); //wait(5000);
			
			//puts("nanunana");
			//if(lauf1==1)continue;
			if(janein1=='n' || janein1=='j')
			{
				if(janein1=='n')
				{
					for(int j=0; j<7;j++)
					{
						setdates[i][j]=0;
					}
				} 
				else
				{
					printf("Alles bestellen? Bitte nur j oder n eingeben:");
					do
					{
						//lauf2++;
						getchar();
						scanf("%c",&janein2);//if(lauf2==1)continue;
						//usw.
						if((janein2=='n') || (janein2=='j'))
						{
							if(janein2=='j')
							{
								for(int j=0;j<7;j++)
								{
									setdates[i][j]=1; printf("%i",setdates[i][j]);
								}
							}
							else
							{
								for(int k=0; k<7; k++)
								{
									do
									{
										//lauf3[k]++;
										printf("Für %s bestellen? Bitte nur j oder n eingeben.:",wochentage[k]);
										getchar();
										scanf("%c",&janein3[k]);
										//if(lauf3[k]==1)continue;
										if((janein3[k]=='n') || (janein3[k]=='j'))
										{
											//puts("hi");
											if(janein3[k]=='j') { setdates[i][k]=1;} //printf("%i",setdates[i][k]);}
											else setdates[i][k]=0; 
										} else printf("Falsche Eingabe! Nochmal bitte:");										
									
									}while((janein3[k]!='j') && (janein3[k]!='n'));
								}
							}
							
						} else puts("Falsche Eingabe! Nochmal bitte.");
					}while((janein2!='n') && (janein2!='j'));
				}
				
			} else puts("Falsche Eingabe! Nochmal bitte.");
		}while((janein1!='n') && (janein1!='j')) ;
	}
	//printf("%i",setdates[0][0]);
	
}

void getsel_datums()
{
	FILE* wochenliste;
	//puts("hihi");
	//wochenliste=fopen("findoutput","r");
	//puts("ho");
	char* buffer; //puts("duda"); 
	buffer=(char*)malloc(150*sizeof(char)); 
	//puts("hihohehohehi");
	//find("findoutput","selected=\"selected\"",anzwoche); //--> schon nach selected bla in kalwochen suchen :DONE Done
	slynmbwochen=(char**)calloc(anzwoche,sizeof(char*));
	for(int i=0;i<anzwoche;i++) slynmbwochen[i]=(char*)malloc(11);
	//puts("muhahaha");
	//printf("%i",setdates[0][0]);
	wochenliste=fopen("findoutput","r");
	//puts("dreimalschwarzerkater");
	for(int i=0;i<anzwoche;i++)
	{
		int bestellen=0;
		fgets(buffer, 150, wochenliste);
		for(int j=0;j<7;j++)
		{
			if(setdates[i][j]==1)
			{
				//printf("%i %i %i",i,j,setdates[i][j]);
				bestellen=1;break;
			}	
		}
		if(bestellen==1) 
		{
			strcpy(slynmbwochen[i],cut2(buffer,"\"",2,2)); //get silly numbers
			//printf("%s\n",slynmbwochen[i]);
		}
		else strcpy(slynmbwochen[i],"0");
	}	
	fclose(wochenliste);	
}

void createmenufiles(char* userid, char* passwd)
{
	FILE* menus[anzwoche];
	char postfield[25];
	char menufilename[8];
	char menunumber[2];
	for(int i=0;i<anzwoche;i++)
	{
		if(strlen(slynmbwochen[i])>2)
		{
		strcpy(menufilename,"menu");
		menunumber[0]=48+i; menunumber[1]='\0';
		strcat(menufilename,menunumber); strcat(menufilename,"\0");
		//printf("%s\n",menufilename) ;
		menus[i]=fopen(menufilename, "w");
		strcpy(postfield,"sel_datum=");
		strcat(postfield,slynmbwochen[i]);
		//printf("%s\n",postfield);
		CURLcode ret;
		CURL *hnd = curl_easy_init();
		curl_easy_setopt(hnd, CURLOPT_WRITEDATA, menus[i]);
		curl_easy_setopt(hnd, CURLOPT_INFILESIZE_LARGE, -1);
		curl_easy_setopt(hnd, CURLOPT_URL, "http://dussmann-lpf.rcs.de/index.php?m=1;3");
		curl_easy_setopt(hnd, CURLOPT_PROXY, NULL);
		curl_easy_setopt(hnd, CURLOPT_PROXYUSERPWD, NULL); 
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postfield);
	 	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux i686; de; rv:1.9.2.12) Gecko/20101027 Firefox/3.6.12");
		curl_easy_setopt(hnd, CURLOPT_COOKIEFILE, "Cookiedatei");
		curl_easy_setopt(hnd, CURLOPT_COOKIEJAR, NULL);
		ret = curl_easy_perform(hnd);
		curl_easy_cleanup(hnd);
		fclose(menus[i]);
		} else continue;
	}
}

void gethiddenandbestellt()
{
	char menufilename[8];
	char menunumber[2];
	hidden=(char***)calloc(anzwoche,sizeof(char**));
	for(int i=0;i<anzwoche; i++)
	{
		int numlines=0;
		char c;
		char* tmp=(char*)malloc(sizeof(char)*300);
		char* tmp2=(char*)malloc(sizeof(char)*300);
		char* puffer=(char*)malloc(sizeof(char)*50);
		FILE* listhidden;
		hidden[i]=(char**)calloc(35,sizeof(char*));
		//printf("dudel\n");
		for(int k=0;k<35;k++) {hidden[i][k]=(char*)malloc(50);strcpy(hidden[i][k],"\0");}
		if(strlen(slynmbwochen[i])>2)
		{
			strcpy(menufilename,"menu");
			menunumber[0]=48+i; menunumber[1]='\0';
			strcat(menufilename,menunumber); strcat(menufilename,"\0");
			find(menufilename,"\"hidden\" name");
			listhidden=fopen("findoutput","r");
			while((c=fgetc(listhidden))!= EOF)
			{
				if(c=='\n') numlines++;
			}
			rewind(listhidden);
			for(int j=0;j<numlines;j++)
			{
				fgets(tmp,300,listhidden);
				tmp2=strcpy(tmp2,tmp);
				strcpy(puffer,cut2(tmp2,"\"",4,4));
				tmp2=strcpy(tmp2,tmp);
				strcat(puffer,"=");
				strcat(puffer,cut2(tmp2,"\"",6,6));
				//printf("%s\n",puffer);
				hidden[i][j]=strcpy(hidden[i][j],puffer);
				//printf("diedeldu %s\n",hidden[i][j]);
			}
			fclose(listhidden);
		}
		free(tmp);
		free(tmp2);
		free(puffer);
	}
	bergruen=(char***)calloc(anzwoche,sizeof(char**));
	for(int i=0;i<anzwoche; i++)
	{
		int numlines=0;
		char c;
		char* tmp=(char*)malloc(sizeof(char)*300);
		char* tmp2=(char*)malloc(sizeof(char)*300);
		char* puffer=(char*)malloc(sizeof(char)*50);
		FILE* listgruen;
		bergruen[i]=(char**)calloc(35,sizeof(char*));
		//printf("dudel\n");
		for(int k=0;k<35;k++) {bergruen[i][k]=(char*)malloc(50);strcpy(bergruen[i][k],"\0");}
		if(strlen(slynmbwochen[i])>2)
		{
			strcpy(menufilename,"menu");
			menunumber[0]=48+i; menunumber[1]='\0';
			strcat(menufilename,menunumber); strcat(menufilename,"\0");
			find(menufilename,"gruen pointer\"",3);
			find("findoutput","type=\"radio\"");
			listgruen=fopen("findoutput","r");
			while((c=fgetc(listgruen))!= EOF)
			{
				if(c=='\n') numlines++;
			}
			rewind(listgruen);
			for(int j=0;j<numlines;j++)
			{
				fgets(tmp,300,listgruen);
				tmp2=strcpy(tmp2,tmp);
				strcpy(puffer,cut2(tmp2,"\"",4,4));
				tmp2=strcpy(tmp2,tmp);
				strcat(puffer,"=");
				strcat(puffer,cut2(tmp2,"\"",6,6));
				//printf("%s\n",puffer);
				bergruen[i][j]=strcpy(bergruen[i][j],puffer);
				//printf("diedeldu %s\n",bergruen[i][j]);
			}
			fclose(listgruen);
		}
		free(tmp);
		free(tmp2);
		free(puffer);
	}
	bergruend=(char***)calloc(anzwoche,sizeof(char**));
	for(int i=0;i<anzwoche; i++)
	{
		int numlines=0;
		char c;
		char* tmp=(char*)malloc(sizeof(char)*300);
		char* tmp2=(char*)malloc(sizeof(char)*300);
		char* puffer=(char*)malloc(sizeof(char)*50);
		FILE* listgruend;
		bergruend[i]=(char**)calloc(35,sizeof(char*));
		//printf("dudel\n");
		for(int k=0;k<35;k++) {bergruend[i][k]=(char*)malloc(50);strcpy(bergruend[i][k],"\0");}
		if(strlen(slynmbwochen[i])>2)
		{
			strcpy(menufilename,"menu");
			menunumber[0]=48+i; menunumber[1]='\0';
			strcat(menufilename,menunumber); strcat(menufilename,"\0");
			find(menufilename,"gruen pointer\"",3);
			find("findoutput","type=\"hidden\" name=\"fld_");
			listgruend=fopen("findoutput","r");
			while((c=fgetc(listgruend))!= EOF)
			{
				if(c=='\n') numlines++;
			}
			rewind(listgruend);
			for(int j=0;j<numlines;j++)
			{
				fgets(tmp,300,listgruend);
				tmp2=strcpy(tmp2,tmp);
				strcpy(puffer,cut2(tmp2,"\"",12,12));
				tmp2=strcpy(tmp2,tmp);
				strcat(puffer,"=");
				strcat(puffer,cut2(tmp2,"\"",14,14));
				//printf("%s\n",puffer);
				bergruend[i][j]=strcpy(bergruend[i][j],puffer);
				//printf("diedeldu %s\n",bergruend[i][j]);
			}
			fclose(listgruend);
		}
		free(tmp);
		free(tmp2);
		free(puffer);
	}
}

void getdatensatz()
{
	wirkbestellen=(int**)calloc(anzwoche,sizeof(int*));
	for(int i=0;i<anzwoche;i++)
	{
		wirkbestellen[i]=(int*)malloc(7*sizeof(int));
		for(int j=0;j<7;j++)
		{
			wirkbestellen[i][j]=0;
			if(setdates[i][j]==1) wirkbestellen[i][j]=1; //ACHTUNG: Das kann noch zu viel sein!!
		}
	}
	wocheplustagplusdaten=(char****)calloc(anzwoche,sizeof(char***));
	for(int i=0;i<anzwoche;i++) 
	{
		wocheplustagplusdaten[i]=(char***)calloc(7,sizeof(char**));
		for(int j=0;j<7;j++) 
		{
			wocheplustagplusdaten[i][j]=(char**)calloc(9,sizeof(char*));
			for(int k=0;k<9;k++) 
			{
				wocheplustagplusdaten[i][j][k]=(char*)malloc(150*sizeof(char));
				strcpy(wocheplustagplusdaten[i][j][k],"\0");
			}
		}
	}
	char menufilename[8];
	char menunumber[2];
	for(int i=0;i<anzwoche;i++)
	{
		if(strlen(slynmbwochen[i])>3)
		{
			strcpy(menufilename,"menu");
			menunumber[0]=48+i; menunumber[1]='\0';
			strcat(menufilename,menunumber); strcat(menufilename,"\0");
			find(menufilename,"class=\" auflistung");
			FILE* auflistungen;
			auflistungen=fopen("findoutput","r");
			char* tmp=(char*)malloc(300);
			for(int j=0;j<21;j++) //21, weil bis dahin es zum sonntag menü 3 geht
			{
				fgets(tmp, 300,auflistungen);
				if(wirkbestellen[i][j%7]==1)
				{
					if(strstr(tmp,"pointer")==NULL) 
					{
						
						wirkbestellen[i][j%7]=0;
						continue; //--> Ausschließen des Tages, da dieser schon bestellt ==> alle folgenden unbestellt oder gruen
					}
					if(strstr(tmp,"gruen pointer")!=NULL)
					{
						wirkbestellen[i][j%7]=0;
						continue;
					}
					wirkbestellen[i][j%7]=1;
				}
			}
			rewind(auflistungen);
			for(int j=0; j<21;j++)
			{
				fgets(tmp,300,auflistungen);
				if(wirkbestellen[i][j%7]==1)
				{
					cut2(tmp,">",2,7); //hier muss noch fallunterscheidung wegen \n am ende rein!!!
					if((tmp[0]=='\n') &&(j<7)) {wirkbestellen[i][j%7]=0; continue;}
					if((tmp[0]=='\n') && (j/7==1)){continue;} //--> die leeren Menü2s bleiben '\0'
					strcpy(wocheplustagplusdaten[i][j%7][3*(j/7)],tmp); //Menünamen abspeichern, damit man nach ihm in den folgenden Zeilen suchen kann, damit man die restlichen Daten ermitteln kann
					//printf("%i %i %s",i,j,wocheplustagplusdaten[i][j%7][3*(j/7)]); //TODO: '\n' am Ende der Zeile löschen ('\r' auch(?) )
				}
			}
			//free(tmp);//Nun weiß man die Tage, für welche MENÜS bestellt müssen, man weiß nicht, wo Desserts bestellt werden müssen 
			fclose(auflistungen);
			FILE* essendata;
			for(int j=0; j<21;j++)
			{
				if(wirkbestellen[i][j%7]==1)
				{
					if(strlen(wocheplustagplusdaten[i][j%7][3*(j/7)])>2) //--> Ausschließen, dass für ein leeres Menü 2 Daten gesammelt werden sollen
					{
						find(menufilename,wocheplustagplusdaten[i][j%7][3*(j/7)],2); //Suche nach Zeile über den Essensnamen
						find("findoutput","<input type=\"radio\" name=\"rad_");
						essendata=fopen("findoutput","r");
						fgets(tmp, 300, essendata);
						cut2(tmp,"\"",4,4);
						strcpy(wocheplustagplusdaten[i][j%7][3*(j/7)+1],tmp);
						//printf("%s",wocheplustagplusdaten[i][j%7][3*(j/7)+1]);
						rewind(essendata);
						fgets(tmp,300, essendata);
						cut2(tmp,"\"",6,6);
						strcpy(wocheplustagplusdaten[i][j%7][3*(j/7)+2], tmp);
						//wocheplustagplusdaten[i][j%7][3*(j/7)+2][strlen(wocheplustagplusdaten[i][j%7][3*(j/7)+2])-1]='\0';
						//printf("%s",wocheplustagplusdaten[i][j%7][3*(j/7)+2]);
						fclose(essendata);	
					}
				}
			} 
			free(tmp);
				//--> Die Daten für die Menüs für die wirklich zu bestellenden Tage sind abgespeichert.
				// -->TODO: Die Nummer des entsprechendenden Desserts lässt sich aus der rad_ nummer herleiten ==>
				// wenn die Suche nach der Nummer ergibt, dass die schon bereits grün ist, wird die nicht bestellt,
				// ansonsten wird diese bestellt
		}		
	}	
}

void getratingandbestelldaten()
{
	FILE* ratinglist;
	//ratinglist=fopen("ratings","a+"); //nochmal wegen anhängen modus nachschauen!!
	float ratings[anzwoche][7][3]; //--> stores ratings for foods
	char** hackstring;
	for(int i=0; i<anzwoche;i++)
	{
		for(int j=0;j<7;j++)
		{
			for(int b=0;b<3;b++) ratings[i][j][b]=0;
			if(wirkbestellen[i][j]==1)
			{
				//Nun kommt Menü1, ich stelle fest, dass ich eine for-schleife machen kann:
				for(int p=0;p<3;p++)
				{
					if(strlen(wocheplustagplusdaten[i][j][3*p])==0) continue; //DAS IST SCHWACHSINN!!! //Dadurch bleiben die Rating für ein leeres Menü 2 0
					char* tmp=(char*)malloc(150);
					char* pch;
					int numwords=1;
					tmp=strcpy(tmp,wocheplustagplusdaten[i][j][0+3*p]);
					tmp=removeformattingsigns(tmp); //must be done to get the correct number of words
					pch=strtok(tmp," ,.1234567890");
					while(pch!=NULL)
					{
						pch=strtok(NULL," ,.1234567890");
						numwords++;
					}
					numwords--;
					hackstring=(char**)calloc(numwords,sizeof(char*));
					for(int k=0;k<numwords;k++) {hackstring[k]=(char*)malloc(70*sizeof(char)); strcpy(hackstring[k],"\0");}
					tmp=strcpy(tmp, wocheplustagplusdaten[i][j][0+3*p]); //<-- hier muss jetzt das mit den &amp; s und so weiter rein, damit das erst später zerhackt wird. Auch sollte immer noch ein Leerzeichen für ein Entferntes Ding reingebracht werden. Das wird ja eh wieder DANACH zur Trennung entfernt.
					tmp=removeformattingsigns(tmp);
					pch=strtok(tmp," ,.1234567890");
					strcpy(hackstring[0],pch);
					//hackstring[k]=strlwr(hackstring[0]); hier noch nicht, wegen der pseudosubstantiven adjektive (Szegediner Gulasch)
					for(int k=1;k<numwords;k++)
					{
						pch=strtok(NULL," ,.1234567890");
						strcpy(hackstring[k],pch);
					} 
					for(int k=numwords-1;k>0;k--)//--> Bindestrichwörter werden geconcatenated ==> mehr leere strings.
					{
						if( hackstring[k][strlen(hackstring[k])-1] == '\n') hackstring[k][strlen(hackstring[k])-1]='\0'; //TODO: das ist dann nicht mehr, wenn das Todo gemacht wurde!!!
						if(hackstring[k-1][strlen(hackstring[k-1])-1]=='-') 
						{ 
							hackstring[k-1]=strcat(hackstring[k-1],hackstring[k]); 
							for(int m=k;m<numwords-1;m++)
							{
								strcpy(hackstring[m],hackstring[m+1]);
							}
							strcpy(hackstring[numwords-1],"\0");
							//strcpy(hackstring[k],"\0"); <-- Das war doof, nun sind alle hackstrings "hintereinander", endene "\0"-en !!
						} //Bindestriche zusammenführen
					} //DONE: " im " , " nach " nicht löschen, sowie die substantivierten Adjektive. diese Bindewörter für concatenation benützen
					for(int k=1;k<numwords-1;k++) //jetzt kommt die Entfernung von "mit", "dazu", "und", "an"
					{
						if((strlen(hackstring[k])<=4) && (hackstring[k][0]!='\0'))
						{
							if((strstr(hackstring[k],"mit")!=NULL) || (strstr(hackstring[k],"dazu")) ||(strstr(hackstring[k],"und")!=NULL) ||(((strlen(hackstring[k])==2)) && (strstr(hackstring[k],"an")!=NULL)))
							{
								strcpy(hackstring[k],"\0");
							} //--> die hackstrings, die noch zusammengehören wegen adjektive oder im , nach sind noch 
							  //"hintereinander", auch wenn es zwischen diesen "Komplexen" '\0'-er Lücken gibt. --> bei 
							  //Bindestrichen muss aber nachgerückt werden!!!! Habe ich gemacht!
						}
					}
					for(int k=0;k<numwords-1;k++) //nun die "<br />" s entfernen, falls vorhanden
					{
						if((strstr(hackstring[k],"<br")!= NULL) && (strstr(hackstring[k+1],"/>")!=NULL))
						{
							for(int m=k;m<numwords-2;m++)
							{
								strcpy(hackstring[m],hackstring[m+2]);
							}
							strcpy(hackstring[numwords-1],"\0");
							strcpy(hackstring[numwords-2],"\0");
						}
					} //TODO: 	new tactic is needed! BEFORE hacking the string into pieces, delete all <br /> and &amp; 's since they then appear as coherent string.
					  // 		therefore we seek if there are any of those and then remove them --> new function in function. with strstr find first appearance and then shift all cha
					  //		characters 5 fields left, in both cases, hell yeah!: DONE
					for(int k=0;k<numwords;k++) //nun die "&amp;" s entfernen
					{
						if(strstr(hackstring[k],"&amp;")!=NULL)
						{
							for(int m=k;m<numwords-1;m++)
							{
								strcpy(hackstring[m],hackstring[m+1]);
							}
							strcpy(hackstring[numwords-1],"\0");
						}
					}
					//nun die neuen &quot; entfernen, analog zu &amp; ???
					//for(int k=0;k<numwords;k++) //nun die "&quot;" s entfernen
					//{
						//if(strstr(hackstring[k],"&quot;")!=NULL)
						//{
							//printf("Das ist der zu kürzende String: %s",hackstring[k]);
							////for(int m=k;m<numwords-1;m++)
							////{
								////strcpy(hackstring[m],hackstring[m+1]);
							////} //Todo: keine stringverschiebung in den Arrays, sondern Buchstabenverschiebung!
							//for(int m=0;m<strlen(hackstring[k]-1-6);m++)
							//{
								//for(int l=0; l<6;l++
							//}
							//strcpy(hackstring[numwords-1],"\0");
						//} //TODO hier noch Baustelle
					//} // TODO Also: es stehen ja meistens zwei quots, d.h. auch dass ein string, der zutatquotzutat ohne leerzeichen ist, aufgesplittet werden muss 
					  // --> daher muss überprüft werden, ob ein String bereits leer ist, sodass in diesem die neu entstandene Zutat hineingeschrieben werden kann
					  // --> sollte ich daher vielleicht die Anzahl der Hackstrings schon vorher etwas vergrößern? Nein! Der String wird in ein '""' übersetzt!
					  // --> nur noch überlegen wie:
					  // --> Achtung: es werden dadurch eigennamen zerstört, da der ganze quot string überschrieben wird.
					//für adjektive und den ganzen Rest folgende Idee: alles was jetzt "direkt" hintereinander ist, wird geconcatenated, der Rest
					//mit "\0" überschrieben.
					int start=0;
					int count=0;
					while(count<numwords)
					{
						if(strlen(hackstring[count])>0)
						{
							start=count;
							count++;//der nachfolgende String soll erst angehängt werden, nicht der gleiche zweimal
							while((count<numwords) && (strlen(hackstring[count])>0))//nur die nichtleeren Strings sollen zusammengefügt werden
							{
								hackstring[start]=strcat(hackstring[start],hackstring[count]);//die nachfolgend Strings anhängen, bis ein leerer String erscheint
								strcpy(hackstring[count],"\0");//Wenn dieser String kopiert wurde, ihn löschen
								count++; //bereit für den nächsten String
							}
						} else count++; //wenn dieser String gerade leer ist weiter machen bis man einen nichtleeren String findet
					} //wenn jetzt nichts schiefgegangen ist, dann sind die String fertig verbunden ;-)
					//jetzt kommt die bewertung:
					
					float bew=0;
					int summand=0;
					char* bewertung=(char*)malloc(5); strcpy(bewertung,"\0");
					int foodcount=0;
					char* alllow;//=(char*)malloc(70);strcpy(alllow,"\0");
					char* storing=(char*)malloc(100); strcpy(storing,"\0");
					for(int k=0; k<numwords;k++)
					{
						strcpy(storing,"\0"); strcpy(bewertung,"\0");
						summand=0;
						alllow=strdup(hackstring[k]);
						strlwr(alllow);
						if((strlen(hackstring[k])>0) && (find("ratings",alllow)))
						{
							foodcount++;
							tmp=frstln(tmp,100,"findoutput");
							tmp=cut2(tmp," ",1,1);
							bewertung=strcpy(bewertung,tmp);
							for(int n=0;n<strlen(bewertung);n++)
							{
								summand=10*summand+(bewertung[n]-48)*1;
							}						
							bew+=summand;						
						}
						if((strlen(hackstring[k])>0) && (find("ratings",alllow)==0))
						{
							foodcount++;
							printf("Wie bewertest du %s ? Bitte Ganzzahl von 1 bis 10: ",hackstring[k]);
							fflush(stdout);
							fflush(stdin);
							scanf("%i",&summand);
							ratinglist=fopen("ratings","a+");
							if(summand<10)
							{
								strcpy(storing,"  \0"); 
								storing[0]=48+summand;
							}
							if(summand==10) strcpy(storing,"10");
							storing=strcat(storing," ");
							storing=strcat(storing,alllow);
							storing=strcat(storing,"\n");
							fputs(storing,ratinglist);
							fclose(ratinglist);
							bew+=summand;
						}
						free(alllow);
					}
					ratings[i][j][p]=bew/foodcount;
					free(tmp);
					free(bewertung);
					free(hackstring);
					free(storing);
					
				}
				
			}
			
		}
	}
	bestelldaten=(char***)calloc(anzwoche,sizeof(char**));
	for(int i=0; i<anzwoche;i++)
	{
		bestelldaten[i]=(char**)calloc(7,sizeof(char*));
		for(int j=0;j<7;j++)
		{
			bestelldaten[i][j]=(char*)malloc(25);
			strcpy(bestelldaten[i][j],"\0");
			if(wirkbestellen[i][j]==1)
			{
				if((ratings[i][j][0]>=ratings[i][j][1]) && (ratings[i][j][0]>=ratings[i][j][2])) 
				{
					strcpy(bestelldaten[i][j],wocheplustagplusdaten[i][j][1]);
					strcat(bestelldaten[i][j],"=");
					strcat(bestelldaten[i][j],wocheplustagplusdaten[i][j][2]);
					//strcat(bestelldaten[i][j],"\0");
				}
				if((ratings[i][j][1]>=ratings[i][j][0]) && (ratings[i][j][1]>=ratings[i][j][2]))
				{
					strcpy(bestelldaten[i][j],wocheplustagplusdaten[i][j][4]);
					strcat(bestelldaten[i][j],"=");
					strcat(bestelldaten[i][j],wocheplustagplusdaten[i][j][5]);
					//strcat(bestelldaten[i][j],"\0");
				}
				if((ratings[i][j][2]>=ratings[i][j][1]) && (ratings[i][j][2]>=ratings[i][j][0])) 
				{
					strcpy(bestelldaten[i][j],wocheplustagplusdaten[i][j][7]);
					strcat(bestelldaten[i][j],"=");
					strcat(bestelldaten[i][j],wocheplustagplusdaten[i][j][8]);
					//strcat(bestelldaten[i][j],"\0");
				}
			}
		}
	}
	for(int i=0;i<anzwoche;i++)
	{
		for(int j=0;j<7;j++)
		{
			if(wirkbestellen[i][j]==1)	printf("%s \n", bestelldaten[i][j]);
		}
	}
}



void sendbestellung()//hier muss sowohl das Senden der daten für die Woche, als auch das senden der bestätigung!!!,
						//praktisch die letzte Funktion auf dem Schlachtfeld
{
	int bestellsumme=0; //summiert einfach die wirkbestellen, sodass man weiß, ob diese woche überhaupt bestellt werden soll
	char* postfield;
	postfield=(char*)malloc(1000); strcpy(postfield,"\0");
	char* menufilename=(char*)malloc(8);
	char* menunumber=(char*)malloc(3);
	FILE* bestbest1[anzwoche]; //Bestellbestätigung 1;
	FILE* bestbest2[anzwoche]; //Der User soll nichts davon sehen...
	for(int i=0;i<anzwoche;i++) //Das ist doof, wir sollten bereits hier schon unterscheiden, ob für die Woche überhaupt bestellt werden soll :DONE Done
	{
		bestellsumme=0;
		for(int j=0;j<7;j++) bestellsumme+=wirkbestellen[i][j]; //hier war ein ganz dummer fehler!
		
		if(bestellsumme>0)
		{
			strcpy(menufilename,"menu");
			menunumber[0]=48+i; menunumber[1]='\0';
			strcat(menufilename,menunumber); strcat(menufilename,"\0");
			strcpy(postfield,"\0");
			strcpy(postfield,hidden[i][0]);
			
			for(int j=1;j<35;j++) 
			{
				if(strlen(hidden[i][j])>0)
				{
					strcat(postfield,"&");
					strcat(postfield,hidden[i][j]);
				}
			}
			strcat(postfield,"&");
			for(int j=0;j<35;j++)
			{
				if(strlen(bergruen[i][j])>0)
				{
					strcat(postfield,"&");
					strcat(postfield,bergruen[i][j]);
				}
			}
			for(int j=0;j<35;j++)
			{
				if(strlen(bergruend[i][j])>0)
				{
					strcat(postfield,"&");
					strcat(postfield,bergruend[i][j]);
				}
			}
			for(int j=0;j<7;j++)
			{
				
				if(wirkbestellen[i][j]==1)
				{
					strcat(postfield,"&");
					strcat(postfield,bestelldaten[i][j]);
				}
			}
			for(int j=0;j<35;j++) //nun kommen die fehlenden Desserts, hier menü bereits grün, aber dessert noch nicht
			{
				if((strlen(bergruen[i][j])>0) && (strstr(bergruen[i][j],"rad_")!=NULL)/* bereits grün, sodass es rad enthält, die nummer, davon das entsprechende fld, gucken, ob das schon grün ist*/)//(wirkbestellen([i][j]==1) //das ist schwachsinn!//nun die Desserts:-->menünamen benötigt.
				{
					char* dessertstring=(char*)malloc(25);strcpy(dessertstring,"\0");
					char* tmp=(char*)malloc(150); tmp=strcpy(tmp,bergruen[i][j]);
					tmp=cut2(tmp,"_",2,3);
					tmp=cut2(tmp,"=",1,1);
					strcpy(dessertstring,"fld_");
					strcat(dessertstring,tmp);
					strcat(dessertstring,"_4");
					find(menufilename,dessertstring);
					if((find("findoutput","gruen pointer")==0)) //&& (strlen(cut2(frstln(tmp,150,"findoutput"),">",2,2))>0))
					{
						strcat(postfield,"&");
						strcat(postfield,dessertstring);
						strcat(postfield,"=1");
					}
					free(dessertstring);
					free(tmp);
				}
			}
			for(int j=0;j<7;j++)//hier zu bestellendes Menü und gucken, ob dessert schon bestellt ist oder noch nicht, wenn nicht, dann halt an postfield anhängen	/*if(das gleiche wie oben, bloß bei dem bestelldata menü die nummer nehmen, dann fld gucken, ob das schon grün ist) */
			{
				if(wirkbestellen[i][j]==1)
				{
					char* dessertstring=(char*)malloc(25);strcpy(dessertstring,"\0");
					char* tmp=(char*)malloc(150); tmp=strcpy(tmp,bestelldaten[i][j]);
					tmp=cut2(tmp,"_",2,3);
					tmp=cut2(tmp,"=",1,1);
					strcpy(dessertstring,"fld_");
					strcat(dessertstring,tmp);
					strcat(dessertstring,"_4");
					find(menufilename,dessertstring);
					if((find("findoutput","gruen pointer")==0)) //&& (strlen(cut2(frstln(tmp,150,"findoutput"),">",2,2))>0))
					{
						strcat(postfield,"&");
						strcat(postfield,dessertstring);
						strcat(postfield,"=1");
					}
					free(dessertstring);
					free(tmp);
				}
			}//nun ist das POST-Field fertig, jetzt kommt das Senden und das empfangen der neuen Datei.
			//Da wegen der ersten if-Bedingung eine Bestellung erfolgen muss( scheiße, was ist, wenn nur menü gruen, aber keine desserts???, hmm, die desserts werden dann nicht mehr bestellt)	stimmt. da hat man halt pech, kann weiter entwickelt werden, es geht um das KONZEPT
			
			//--------------------------------------------------------------------------------------------
			//Die erste Bestätigung herunterladen
			//--------------------------------------------------------------------------------------------
			printf("Der erste postcode: %s",postfield);
			fflush(stdout);
			strcpy(menufilename,"bstst"); //Das soll die erste Bestellbestätigung werden;
			menunumber[0]=48+i; menunumber[1]='\0';
			strcat(menufilename,menunumber); strcat(menufilename,"\0");
			bestbest1[i]=fopen(menufilename,"w");
			CURLcode ret; //achtung, diese mehrfachen rets führen zu fehlern--> umbenennen
			CURL *hnd = curl_easy_init();
			curl_easy_setopt(hnd, CURLOPT_WRITEDATA, bestbest1[i]);
			curl_easy_setopt(hnd, CURLOPT_INFILESIZE_LARGE, -1);
			curl_easy_setopt(hnd, CURLOPT_URL, "http://dussmann-lpf.rcs.de/index.php?m=150;0;1;3");
			curl_easy_setopt(hnd, CURLOPT_PROXY, NULL);
			curl_easy_setopt(hnd, CURLOPT_PROXYUSERPWD, NULL); 
			curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postfield);
			curl_easy_setopt(hnd, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux i686; de; rv:1.9.2.12) Gecko/20101027 Firefox/3.6.12");
			curl_easy_setopt(hnd, CURLOPT_COOKIEFILE, "Cookiedatei");
			curl_easy_setopt(hnd, CURLOPT_COOKIEJAR, NULL);
			ret = curl_easy_perform(hnd);
			curl_easy_cleanup(hnd);
			fclose(bestbest1[i]);
			
			//------------------------------------------------------------------------------------------
			//Die hidden-values wegen bestellen und so aus der ersten Bestätigung extrahieren
			//------------------------------------------------------------------------------------------
			/* 
			 * action string extrahieren, damit man sich die Suche nach starttag sparen kann 
			 * Achtung, die bereits gespeicherten Daten wegen hidden und so nicht nochmal schicken, da dort auch falsche Daten drin sind
			 *  ==> nochmal die neuen hidden werte ermitteln, auf js_hidden aufpassen (nach "hidden suchen) --> bis inklusive zeile bestellpreis einlesen, den rest 
			 *      nicht mehr als hidden-zeugs abspeichern
			 * um die stelle für schneiden wegen bestellenfoo zu ermitteln einfach ein strstr(zeile,"best_") machen
			 */
			char* actionurl=(char*)malloc(120); strcpy(actionurl,"http://dussmann-lpf.rcs.de/index.php?m=150;0;1;3&a=akt_bestellen&");
			char** newhidden;
			int lnstillpreis=0;
			FILE* hiddenposts;
			char* tmp=(char*)malloc(150); strcpy(tmp,"\0");
			char* tmp2=(char*)malloc(150);strcpy(tmp2,"\0");
			find(menufilename,"<form action="); //still the same name 
			hiddenposts=fopen("findoutput","r");
			fgets(tmp, 150,hiddenposts);
			fclose(hiddenposts);
			tmp=cut2(tmp,"\"",2,2);
			tmp=cut2(tmp,";",6,6);
			strcat(actionurl,tmp);
			strcat(actionurl,"\0");
			find(menufilename,"\"hidden");
			hiddenposts=fopen("findoutput","r");
			do
			{
				lnstillpreis++;
			}while(strstr(fgets(tmp,150,hiddenposts),"bestellpreis")==NULL);
			newhidden=(char**)calloc(lnstillpreis,sizeof(char*));
			rewind(hiddenposts);
			for(int k=0; k<lnstillpreis;k++) newhidden[k]=(char*)malloc(30);
			for(int k=0;k<lnstillpreis;k++)
			{
				fgets(tmp,150,hiddenposts);
				tmp2=strcpy(tmp2,tmp);
				if(strstr(tmp,"best_")!=NULL)
				{
					tmp=cut2(tmp,"\"",6,6);
					strcpy(newhidden[k],tmp);
					strcat(newhidden[k],"=");
					strcat(newhidden[k],"\0");
				}
				else if(strstr(tmp,"guthaben")==NULL)
				{
					tmp=cut2(tmp,"\"",4,4);
					strcpy(newhidden[k],tmp);
					strcat(newhidden[k],"=");
					tmp2=cut2(tmp2,"\"",6,6);
					strcat(newhidden[k],tmp2);
					strcat(newhidden[k],"\0");
				} else if(strstr(tmp,"guthaben")!=NULL)
				{
					strcpy(newhidden[k],"guthaben=");
					strcat(newhidden[k],"\0");
				}
			}
			
			//--------------------------------------------------------------------------------------
			// Nun kommt das erstellen von postfield
			//--------------------------------------------------------------------------------------
			strcpy(postfield,newhidden[0]);
			for(int k=1; k<lnstillpreis;k++)
			{
				strcat(postfield,"&");
				strcat(postfield,newhidden[k]);
			}
			strcat(postfield,"\0");
			
			//-------------------------------------------------------------------------------------
			// Nun kommt das Senden dieses postfields an die zuvor ermittelte action-url
			// gespeichert wird in einer extra-Datei
			//-------------------------------------------------------------------------------------
			strcpy(menufilename,"bstbt"); //Das soll die zweite Bestellbestätigung werden;
			menunumber[0]=48+i; menunumber[1]='\0';
			strcat(menufilename,menunumber); strcat(menufilename,"\0");
			bestbest2[i]=fopen(menufilename,"w");
			CURLcode ret2;
			CURL *hnd2 = curl_easy_init();
			curl_easy_setopt(hnd2, CURLOPT_WRITEDATA, bestbest2[i]);
			curl_easy_setopt(hnd2, CURLOPT_INFILESIZE_LARGE, -1);
			curl_easy_setopt(hnd2, CURLOPT_URL, actionurl);
			curl_easy_setopt(hnd2, CURLOPT_PROXY, NULL);
			curl_easy_setopt(hnd2, CURLOPT_PROXYUSERPWD, NULL); 
			curl_easy_setopt(hnd2, CURLOPT_POSTFIELDS, postfield);
			curl_easy_setopt(hnd2, CURLOPT_REFERER, "http://dussmann-lpf.rcs.de/index.php?m=150;0;1;3");
			curl_easy_setopt(hnd2, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux i686; de; rv:1.9.2.12) Gecko/20101027 Firefox/3.6.12");
			curl_easy_setopt(hnd2, CURLOPT_COOKIEFILE, "Cookiedatei");
			curl_easy_setopt(hnd2, CURLOPT_COOKIEJAR, NULL);
			ret2 = curl_easy_perform(hnd2);
			curl_easy_cleanup(hnd2);
			fclose(bestbest2[i]);
			fclose(hiddenposts);
			free(actionurl);
			free(newhidden);
			free(tmp);
			free(tmp2);
			//puts("liesmich");
			printf("Das ist der zweite Postcode: %s",postfield);
		}	
		
	} //STOPP, diese for-schleife darf ich nicht verlassen, da das postfield kein array ist!!
	free(postfield);
	free(menunumber);
	free(menufilename);
}
