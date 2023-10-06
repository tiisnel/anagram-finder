#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "dictionary.h"

HashMap *cases;
HashMap *dict;
char comm = '#'; // how comments are marked in text file
char com = '#';  // check if comment variable hasn't changed yet

int is_next_byte(char byte)
{ // in multybite utf8 each extra nite must start with '10'
    return ((byte & 0xc0) == 0x80);
}
int utf_bytes(char *utf_string)
{ // determines, how mamy bytes were used to encode next char

    if ((utf_string[0] & 0x80) == 0)
        return 1; // if leftmost bit is low, then one bit encoding is used

    else if ((utf_string[0] & 0xE0) == 0xc0)
    { // if byte starts with 110, then it is 2 byte encoding
        if (!(is_next_byte(utf_string[1])))
            return -1;
        else
            return 2;
    }
    else if ((utf_string[0] & 0xF0) == 0xE0)
    { // if byte starts with 1110, then it is 3 byte encoding
        if (!(is_next_byte(utf_string[1]) || is_next_byte(utf_string[2])))
            return -1;
        else
            return 3;
    }
    else if ((utf_string[0] & 0xF8) == 0xF0)
    { // if byte starts with 11110, then it is 4 byte encoding
        if (!(is_next_byte(utf_string[1]) || is_next_byte(utf_string[2]) || is_next_byte(utf_string[2])))
            return -1;
        else
            return 4;
    }
    return -1;
}

int sub(char *old[100], int depth, int len, char out[])
{ // find partial permutations
    int i;
    int indexes[depth];
    int maxsize = pow(2, len);
    char found[maxsize];
    for (i = 0; i < depth; i++)
        indexes[i] = i; // init

    while (indexes[0] <= len - depth)
    {
        out[0] = '\0';
        for (i = 0; i < depth; i++)
        { // found one permutation!
            strcat(out, old[indexes[i]]);
        }
        if (has(dict, out))
        {

            char *new = get(dict, out);
            char *temp =malloc(strlen(new+1));
            strcpy(temp, new);

            char *one = strtok(temp, "\n"); //multiple values under same key
            while (one != NULL)
            {
                if (strstr(found, one) == NULL) // avoid duplicate outcomes
                {
                    strcat(found, one);
                    puts(one);
                }
                one = strtok(NULL,"\n");
            }
            free(temp);
        }

        int right = depth - 1;
        while (right >= 0 && indexes[right] == len - depth + right)
            right--;
        indexes[right]++;
        for (i = right + 1; i < depth; i++)
            indexes[i] = indexes[i - 1] + 1;
    }
}

int flatten(char *old[100], int size, char new[]) // 2d array to 1d
{
    new[0] = '\0'; // emptying
    for (int k = 0; k <= size; k++)
    {
        strcat(new, old[k]);
    }

    if (com != comm) // user input only
    {
        new[0] = '\0';
        for (int i = size + 1; i > 1; i--)
        {
            printf("%d letter anagrams:\n", i);
            sub(old, i, size + 1, new);
        }
    }
}
int sort_word(char *word[100], int size) //using 'slow' sorting algorithm, because words themselves are small and time goes mainly to reading files
{

    int i, j, minimum;
    char temp[5];

    for (i = 0; i < size - 1; i++)
    {

        minimum = i;
        for (j = i; j < size; j++)
        {

            if (strcmp(word[j], word[minimum]) < 0)
            {
                minimum = j; 
            }
        }

        strcpy(temp, word[minimum]);
        strcpy(word[minimum], word[i]);
        strcpy(word[i], temp);
    }
}

char *prepare(char line[100])
{

    char newline[100];
    char *letters[100];
    char test[5]; // has null = remove, has value = replace; !has = do nothing
    int i = 0;    // currentbyte index
    int n = 0;    // number of chars printed
    int bytes;

    while (line[i] != '\0')
    {
        bytes = utf_bytes(&line[i]);

        if (bytes > 0)
        {
            strncpy(test, &line[i], bytes);
            test[bytes] = '\0';

            if (has(cases, test))
            {

                if (get(cases, test) == NULL)
                {
                    i += bytes;
                    continue;
                }
                else
                {
                    letters[n] = malloc((bytes + 1) * sizeof(char));
                    strcpy(letters[n], get(cases, test));
                    letters[n][bytes] = '\0';
                    i += bytes;
                    n++;
                }
            }
            else
            {
                letters[n] = malloc((bytes + 1) * sizeof(char));
                strcpy(letters[n], test);
                i += bytes;
                n++;
            }
        }
        else
        {
            puts("error in reading byte"); // should not really happen as we read text, not raw bytes
            return NULL;
        }
    }
    sort_word(letters, n);

    flatten(letters, n - 1, newline);
    char *ret = malloc(sizeof(newline) + 1);
    strcpy(ret, newline); // can't return local address
    return (ret);
}

int read(char *name) // wrapper function, so that preparing works for both file and stdin inputs
{

    FILE *fp;

    char line[100];
    char *new;

    fp = fopen(name, "r");
    if (fp == NULL)
    {
        printf("file %s could not be found!", name);
        return 0;
    }

    while (fgets(line, 100, fp) != NULL)
    {
        new = prepare(line);
        set(dict, new, line);
        free(new);
    }
}

bool endline(char symbol[16])
{
    if (strchr(symbol, '\n'))
        return true;
    else
        return false;
}

int convert(bool caps) // rules for case converting and removing unneccessary chars
{
    FILE *fp = fopen("limits.txt", "r");
    char symbol[11];
    char key[20];
    char value[20];

    bool comment = false;
    while (fgets(symbol, 11, fp) != NULL)
    {

        if (comment)
        {
            if (endline(symbol))
            {
                comment = false;
                continue;
            }
            else
                continue; // longer continious comment here
        }

        int i = 0;
        int n = utf_bytes(&symbol[i]);
        if (symbol[i] == comm) // comment symbol
            if (endline(symbol))
                continue;
            else
            {
                comment = true;
                continue;
            }
        if (symbol[i] == '\n')
        {
            {
                strncpy(key, &symbol[i], n); // set uses char* not char
                key[n] = '\0';
                set(cases, key, NULL);
                continue;
            }
        }
        strncpy(key, &symbol[i], n);
        key[n] = '\0';

        i += n; // separator (space) or new line
        if (symbol[i] == comm || symbol[i] == '\n' || symbol[i + 1] == comm)
        {
            set(cases, key, NULL); // no value used, replacement only
            if (endline(symbol))
                continue;
            else
            {
                comment = true;
                continue;
            }
        }
        if (!caps)
        { // just ignoring uppercase convertions
            comment = true;
            continue;
        }

        i += 1; // already tested that its not comment block
        n = utf_bytes(&symbol[i]);

        strncpy(value, &symbol[i], n);
        value[n] = '\0';

        set(cases, key, value);
        if (endline(symbol))
            continue;
        else
        {
            comment = true;
            continue;
        }
    }
    fclose(fp);
}
int main(int argc, char *argv[])
{
    if(argc ==1){
        puts("missing input dictionary!!");
        return 0;
    }

    char input[100];
    char *output;


    
    cases = create(1000);
    puts("working with input files, please wait");
    bool caps = true; // convert all chars to uppercase, default
    if (argc > 2 && strcmp(argv[2], "-nocaps") == 0)
    {
        caps = false;
    }

    convert(caps);

    char *end = strrchr(argv[1], '.'); //check if using dictionary file
    if(end!=NULL && strcmp(end,".dict")==0){
        puts("loading dictionary");
        dict = load(argv[1], 200000);
    }
    else{



    dict = create(100000); //increase for very large dictionaries
    char *name =argv[1];
    read(name);

    }
    puts("ready");

    comm = '\0'; // assume that user does not enter comments
    while (true)
    {
        input[0] = '\0'; // reset
        // printf("\nenter anagram search keyword:  ");// doesnt work well on windows, use pipes instead
        fgets(input, sizeof(input), stdin);
        if (input[0] == '\n' || input[0] == '\0')
            break;

        printf("\noriginal word: %s\n", input);
        output = prepare(input);
        free(output);
    }
    
    //save(dict, argv[1]); // too slow to be useful, maybe switch to binary?
    destroy(cases);
    destroy(dict);
    
    puts("exited successfully!");
    return 0;
}