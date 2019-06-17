id				(_)*[a-zA-z]+[a-zA-z0-9]*
integer				-?([0-9]+|[0-9]*)
fraction			-?([0-9]+|[0-9]*(\.)[0-9]+([eE][-+]?[0-9]+)?)
relational_operator 		".lt."|".gt."
keyword 			"if"|"else"|"int"|"float"|"void"|"read"|"print"|"main"
arithmetic_operator 		"-"|"+"|"*"|"\\"|"="
separator 			[;,{}()]

%%

\n ;
[\t ' '] ;
\/\/.* ;
\/\*(.*\n)*.*\*\/ ;
{keyword}			{return 1;}
{arithmetic_operator} 		{return 1;}
{relational_operator} 		{return 1;}
{separator} 			{return 1;}
{id} 				{return 2;}
{integer} 			{return 3;}
{fraction}			{return 4;}
.				return 0;

%%

int yywrap(){
	return 1;
}
