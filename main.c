#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
bool isDelimiter(char ch)
{ // sıradaki karakterin işaret olup olmadığının kontrolü
    if (ch == ' ' || ch == '+' || ch == '-' || ch == '*' ||
        ch == '/' || ch == ',' || ch == ';' || ch == '>' ||
        ch == '<' || ch == '=' || ch == '(' || ch == ')' ||
        ch == '[' || ch == ']' || ch == '{' || ch == '}' ||
        ch == ':' || ch == '"' || ch == '\n')
        return (true);
    return (false);
}
bool identifierCheck(char* str)
{ // substring değerinin identifier olup olmadığının kontrolü
    if (isalpha(str[0]) && strlen(str) < 21)
        return (true);
    return (false);
}
char* toLower(char* str)
{ // substring karakterlerini keyword kontrolü için küçültme fonksiyonu.
    char* lowerStr = calloc(strlen(str) + 1, sizeof(char));
    for (int i = 0; i < (strlen(str) + 1); ++i) {
        lowerStr[i] = tolower((unsigned char)str[i]);
    }
    return lowerStr;
}
bool isKeyword(char* str)
{ // substring değerinin keyword olup olmadığının kontrolü
    char keywords[18][10] = { "break","case","char","const","continue","do","else","enum","float","for","goto","if","int",
                              "long","record","return","static","while" };
    str = toLower(str);
    int counter = 0;
    bool keyword = false;
    while (counter < 19) {
        if (strcmp(str, keywords[counter]) == 0) {
            keyword = true;
        }
        counter++;
    }
    return keyword;
}
bool isBracket(char ch)
{ // parantez kontrolü
    if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}')
        return (true);
    return(false);
}
char* subString(char* str, int left, int right)
{ // okuduğumuz filedan parça parça input alarak parçalar üzerinde işlem yapmamızı sağlıyor
    int i;
    char* subStr = (char*)malloc(
            sizeof(char) * (right - left + 2)); // Memoryde yer ayırma

    for (i = left; i <= right; i++) {
        subStr[i - left] = str[i];
    }
    subStr[right - left + 1] = '\0'; // string sonunda gerekli olan null
    return (subStr);
}
bool integerCheck(char* str){
    if(strlen(str)>10){
        return false;
    }
    for(int i = 0;i< strlen(str);i++){
        if(!isdigit(str[i])){
            return false;
        }
    }
    return true;
}
void parse(char* str, FILE *file_out,int maxLen)
{
    int commentopener = 0;
    int stringopener = 0;
    int left = 0;
    int right = 0;
    int len = maxLen;
    while (right <= len && left <= right) {
        if (isDelimiter(str[right]) == false)
            right++;

        if (isDelimiter(str[right]) == true && left == right) {

            if ((str[right] == '+' && str[right + 1] != '+') || (str[right] == '-' && str[right + 1] != '-') || str[right] == '*' ||
                str[right] == '/') { // 1 char operators
                fprintf(file_out,"Operator('%c')\n", str[right]);
            }
            else if ((str[right] == '+' && str[right + 1] == '+') || (str[right] == '-' && str[right + 1] == '-') || (str[right] == ':' && str[right + 1] == '=')) // ++ / --
            {
                char* oprStr = (char*)malloc(sizeof(char) * (3));
                oprStr[0] = str[right];  // Operator 2 character ise bir pointer ile birleştirip yazdırma
                oprStr[1] = str[right + 1];
                oprStr[2] = '\0';
                fprintf(file_out,"Operator('%s')\n", oprStr);
                right++;
            }
            else if ((str[right] == ';')) // EOL
                fprintf(file_out,"EndOfLine\n", str[right]);
            else if ((str[right] == '\n')){

            }
            else if (str[right] == '"') {       // STRING AYRISTIRMA
                stringopener += 1;
                while (str[right + 1] != '"' ) {
                    right++;
                    if (right >= maxLen)
                        break;
                }
                if (str[right + 1] == '"'){
                    stringopener = 0;
                    char* String_content = subString(str, left + 1, right);
                    fprintf(file_out,"StrConst('%s')\n", String_content);
                    right++;
                }
            }
            else if ((str[right] == '(' & str[right + 1] == '*')) { // COMMENT
                commentopener += 1;
                right++;
                while ((str[right + 1] != '*' && str[right + 2] != ')') | (str[right+1] == '*' && str[right+2] != ')') | (str[right+1] != '*' && str[right+2] == ')')) {
                    right++;
                    if (right > maxLen)
                        break;
                }
                if (str[right + 1] == '*' & str[right + 2] == ')'){ // açılan commenti kapama
                    commentopener -= 1;
                    char* Comment_content = subString(str, left + 2, right);
                    right++;
                    right++;
                }
            }
            else if (isBracket(str[right])) { // Bracket
                if (str[right] == '(')
                    fprintf(file_out,"%s \n", "LeftPar");
                else if (str[right] == ')')
                    fprintf(file_out,"%s \n", "RightPar");
                else if (str[right] == '{')
                    fprintf(file_out,"%s \n", "LeftCurlyBracket");
                else if (str[right] == '}')
                    fprintf(file_out,"%s \n", "RightCurlyBracket");
                else if (str[right] == '[')
                    fprintf(file_out,"%s \n", "LeftSquareBracket");
                else if (str[right] == ']')
                    fprintf(file_out,"%s \n", "RightSquareBracket");
            }

            right++;
            left = right;
        }
        else if (isDelimiter(str[right]) == true && left != right
                 || (right == len && left != right)) {
            char* subStr = subString(str, left, right - 1);
            if (isKeyword(subStr))
                fprintf(file_out,"Keyword('%s')\n", subStr);
            else if (identifierCheck(subStr) == true
                && isDelimiter(str[right - 1]) == false)
                fprintf(file_out,"Identifier('%s')\n", subStr);
            else if (integerCheck(subStr) == true){
                fprintf(file_out,"IntConst('%s')\n", subStr);
            }

            left = right;
        }
    }
    if (commentopener > 0){
        fprintf(file_out,"Error, last comment is not closed.\n");
    }
    if (stringopener != 0){
        fprintf(file_out,"Error, last string is not closed.\n");
    }
    return;
}


int main()
{
    FILE *f_ptr, *f_out_ptr;
    f_ptr = fopen("code.psi", "r");
    f_out_ptr = fopen("code.lex", "w");
    char str[1000];

    // check if file is actually opened
    if (f_ptr == NULL) {
        printf("Error while opening the read file\n");
        exit(1);
    }
    if (f_out_ptr == NULL) {
        printf("Error while opening the write file\n");
        exit(1);
    }
    int i = 0;
    char ch;
    while ((ch = fgetc(f_ptr)) != EOF) {
        str[i] = ch;
        i++;
    }

    parse(str,f_out_ptr,i);

    fclose(f_ptr);
    fclose(f_out_ptr);
    return(0);
}
