%{
#include <stdio.h>
%}

%%

[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,} {
    printf("Valid email: %s\n", yytext);
}

.|\n    { /* ignore other characters */ }

%%

int main() {
    yylex();
    return 0;
}
