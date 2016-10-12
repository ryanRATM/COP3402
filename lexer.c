#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef enum token {
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym,
  // these are the errors
  identNameToLongsym, identStartNumbersym, numberToLargesym, invalidTokensym, commentsym
} token_type;

union lval {
  char *id;
  int num;
} lval;

token_type lex();
void printCode(int isSource, int isClean);

int main(int argc, char** argv) {
    int i;
    int isSource = 0, isClean = 0;
    for(i = 0; i < argc; i++) {
        if(strcmp(argv[i], "--source") == 0) {
            isSource = 1;
            continue;
        }
        if(strcmp(argv[i], "--clean") == 0) {
            isClean = 1;
            continue;
        }
    }

    token_type tok;
    // if we should print the code
    printCode(isSource, isClean);

    // set to beginning of input
    rewind(stdin);
    printf("tokens:\n-------\n");
    while ((tok=lex()) != nulsym) {
        if(tok == numbersym) {
            printf("%-12d %d\n", lval.num, tok);
        }
        // catch if error
        else if(elsesym < tok) {
            printf("\n");
        }
        else if(tok != commentsym) {
            printf("%-12s %d\n", lval.id, tok);
        }
    }

    return 0;
}

token_type lex() {
  char c;
  // ignore space, tab, newline
  while ((c=getchar()) == ' ' || c== '\t' || c == '\n');
  if (c == EOF) {
    return nulsym;
  }

  // identifier
  if (isalpha(c)) {
    char sbuf[100], *p = sbuf;
    do {
        *p++ = c;
    } while ((c = getchar()) != EOF && isalnum(c));
    ungetc(c, stdin);
    *p = '\0';
	// check to see if is a reserved word
    lval.id = sbuf;
    if(strcmp(sbuf, "odd") == 0) {
        return oddsym;
    }
    else if(strcmp(sbuf, "begin") == 0) {
        return beginsym;
    }
    else if(strcmp(sbuf, "end") == 0) {
        return endsym;
    }
    else if(strcmp(sbuf, "if") == 0) {
        return ifsym;
    }
    else if(strcmp(sbuf, "else") == 0) {
        return elsesym;
    }
    else if(strcmp(sbuf, "then") == 0) {
        return thensym;
    }
    else if(strcmp(sbuf, "while") == 0) {
        return whilesym;
    }
    else if(strcmp(sbuf, "do") == 0) {
        return dosym;
    }
    else if(strcmp(sbuf, "call") == 0) {
        return callsym;
    }
    else if(strcmp(sbuf, "const") == 0) {
        return constsym;
    }
    else if(strcmp(sbuf, "var") == 0) {
        return varsym;
    }
    else if(strcmp(sbuf, "procedure") == 0) {
        return procsym;
    }
    else if(strcmp(sbuf, "write") == 0) {
        return writesym;
    }
    else if(strcmp(sbuf, "read") == 0) {
        return readsym;
    }
    else {
		if(12 < strlen(sbuf)) {
            printf("--!-- Error: Identifier is too large --!--");
            return identNameToLongsym;
		}
		else {
			// valid identifier name
            return identsym;
		}
    }
  }
  else if(isdigit(c)) {
    ungetc(c, stdin);
    scanf("%d", &lval.num);

    // checking if invalid identifier name, case where tried to start with variable name
    c = getchar();
    if(isalpha(c)) {
        printf("--!-- Error: Identifier started with number --!--");
        while ((c = getchar()) != EOF && isalpha(c));
        ungetc(c, stdin);
        return identStartNumbersym;
    }
    ungetc(c, stdin); // give back character borrowed
    // checking if have error for
    if(65535 < lval.num || lval.num < 0) {
        printf("--!-- Error: Number is too large --!--");
        return numberToLargesym;
    }
    return numbersym;
  }
  else {

	char op[3];
	op[0] = c;
	op[1] = getchar();
	op[2] = '\0';
    lval.id = op;
    // check all possible 2 character tokens
	if(strcmp(op,"<=") == 0) {
		return leqsym;
	}
	if(strcmp(op,">=") == 0) {
		return geqsym;
	}
	if(strcmp(op,"!=") == 0) {
		return neqsym;
	}
	if(strcmp(op,":=") == 0) {
		return becomessym;
	}
	if(strcmp(op,"/*") == 0) {
		// continue while until don't get '*/'
		while(strcmp(op, "*/") != 0) {
            op[0] = op[1];
            op[1] = getchar();
		}
		return commentsym;
	}

    // return the last borrowed character
    ungetc(op[1], stdin);

    op[1] = '\0';
    lval.id = op;
	// test all single character operators
	switch (op[0]) {
		case '+':
            return plussym;
        case '-':
            return minussym;
        case '*':
            return multsym;
        case '/':
            return slashsym;
        case '=':
            return eqsym;
        case '<':
			return lessym;
        case '>':
            return gtrsym;
        case ',':
            return commasym;
        case ';':
            return semicolonsym;
        case '.':
            return periodsym;
        case '(':
            return lparentsym;
        case ')':
            return rparentsym;
		default:
            printf("--!-- Error: invalid token --!--");
            return invalidTokensym;
	}
  }
  // if does not match anything else then error of invalid variable name
}

void printCode(int isSource, int isClean) {
    char c;
    if(isSource == 1) {
        printf("source code:\n------------\n");
        while((c = getchar()) != EOF) {
            printf("%c", c);
        }
        printf("\n\n");
    }
    if(isClean == 1) {
        rewind(stdin);
        printf("source code without comments:\n------------\n");
        while((c = getchar()) != EOF) {
            if(c == '/') {
                c = getchar();
                if(c == '*') {
                    char op[3];
                    op[1] = c;
                    op[2] = '\0';
                    while(strcmp(op, "*/") != 0) {
                        op[0] = op[1];
                        op[1] = getchar();
                        printf(" ");
                    }
                    c = getchar();
                    printf("  ");
                } else {
                    ungetc(c, stdin);
                }
            }
            printf("%c", c);
        }
        printf("\n\n");
    }
}
