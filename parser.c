/*对于带优先级的表达式，可以考虑整理之后采用递归下降分析的方法来编写语法分析器
 * 假设最大优先级的本身就是数字，其次是'*'和'/'，再然后是'+'和'-'，那么可以如下
 * 使用，优先级由低到高。
 *expression /*表达式的规则*/
 *          ：term /*表达式*/
 *          |expression ADD term /*或表达式+表达式*/
 *          |expression SUB term /*或表达式-表达式*/
 *
 *
 *term
 * 					:primary_expression
 * 					|term MUL primary_expression
 * 					|term DIV primary_expression
 *
 *
 * primary_expression
 * 					: DOUBLE_LITERAL
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

#define LINE_BUF_SIZE (1024)

static Token st_look_ahead_token;
static int st_look_ahead_token_exists;

static void my_get_token(Token *token)
{
	if(st_look_ahead_token_exists)
	{
		*token = st_look_ahead_token;
		st_look_ahead_token_exists = 0;
	}else
		get_token(token);
}

static void unget_token(Token* token)
{
	st_look_ahead_token = *token;
	st_look_ahead_token_exists = 1;
}

double parse_expression(void);

static double parse_primary_expression()
{
	Token token;

	my_get_token(&token);
	if(token.kind == NUMBER_TOKEN)
		return token.value;
	fprintf(stderr,"syntax error.\n");
	exit(1);
	return 0.0;/*make compiler happy*/
}
static double parse_term()
{
	double v1;
	double v2;
	Token token;

	v1 = parse_primary_expression();
	for(;;)
	{
		my_get_token(&token);
		if(token.kind != MUL_OPERATOR_TOKEN
				&& token.kind != DIV_OPERATOR_TOKEN)
		{
			unget_token(&token);
			break;
		}
		v2 = parse_primary_expression();
		if(token.kind == MUL_OPERATOR_TOKEN)
			v1 *= v2;
		else if(token.kind == DIV_OPERATOR_TOKEN)
			v1 /= v2;
	}
	return v1;
}

double parse_expression()
{
	double v1;
	double v2;
	Token token;
	v1 = parse_term();
	for(;;)
	{
		my_get_token(&token);
		if(token.kind != ADD_OPERATOR_TOKEN)
		{
			unget_token(&token);
			break;
		}
		v2 = parse_term();
		if(token.kind == ADD_OPERATOR_TOKEN)
			v1 += v2;
		else if(token.kind == SUB_OPERATOR_TOKEN)
			v1 -= v2;
		else
			unget_token(&token);
	}
	return v1;
}

double parse_line()
{
	double value;
	st_look_ahead_token_exists = 0;
	value = parse_expression();

	return value;
}

int main(int argc,char** argv)
{
	char line[LINE_BUF_SIZE];
	double value;

	while(fgets(line,LINE_BUF_SIZE,stdin) !=  NULL)
	{
		set_line(line);
		value = parse_line();
		printf(">>%f\n",value);
	}
	return 0;
}
