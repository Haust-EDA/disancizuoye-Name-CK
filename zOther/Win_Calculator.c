#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_STACK_SIZE 30

// 栈的数据结构
typedef struct {
    double stack[MAX_STACK_SIZE];  // 存储栈中的元素
    int top;  // 栈顶索引
} Stack;

// 栈相关操作函数
void initStack(Stack* s) {
    s->top = -1;  // 初始化栈为空
}

int isEmpty(Stack* s) {
    return s->top == -1;  // 栈是否为空
}

int isFull(Stack* s) {
    return s->top == MAX_STACK_SIZE - 1;  // 栈是否满
}

void push(Stack* s, double value) {
    if (!isFull(s)) {
        s->stack[++(s->top)] = value;  // 将元素压入栈
    }
}

double pop(Stack* s) {
    if (!isEmpty(s)) {
        return s->stack[(s->top)--];  // 从栈中弹出元素
    }
    return -1;  // 错误值
}

double peek(Stack* s) {
    if (!isEmpty(s)) {
        return s->stack[s->top];  // 返回栈顶元素
    }
    return -1;  // 错误值
}

// 运算符优先级函数
int precedence(char op) {
    if (op == '^') return 3;  // 指数运算优先级最高
    if (op == '*' || op == '/') return 2;  // 乘法和除法次之
    if (op == '+' || op == '-') return 1;  // 加法和减法最低
    return 0;
}

// 执行运算的函数
double applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return pow(a, b);  // 执行指数运算
    }
    return 0;
}

// 检查表达式是否合法，并将数字和运算符压入栈
int isValidAndPushToStack(const char* expression, Stack* values, Stack* ops) {
    int len = strlen(expression);
    int i = 0;
    char token[50];  // 用于存储数字
    int tokenIndex = 0;
    int prevCharIsOperatorOrOpenParen = 1;  // 前一个字符是否是运算符或左括号

    while (i < len) {
        char current = expression[i];

        // 跳过空格
        if (current == ' ') {
            i++;
            continue;
        }

        // 如果是数字或负号，处理数字
        if (isdigit(current) || current == '.' || (current == '-' && (i == 0 || strchr("+-*/^(", expression[i - 1])))) {
            tokenIndex = 0;
            if (current == '-') {
                token[tokenIndex++] = current;
                i++;
            }
            while (isdigit(expression[i]) || expression[i] == '.') {
                token[tokenIndex++] = expression[i++];
            }
            token[tokenIndex] = '\0';
            
            // 将数字压入栈
            push(values, atof(token));
            prevCharIsOperatorOrOpenParen = 0;
        }
        // 处理乘法、除法、指数运算符
        else if (strchr("*/^", current)) {
            if (prevCharIsOperatorOrOpenParen) return 0;  // 如果前一个是运算符或左括号，表达式不合法
            while (!isEmpty(ops) && precedence(peek(ops)) >= precedence(current)) {
                double val2 = pop(values);
                double val1 = pop(values);
                char op = (char)pop(ops);
                push(values, applyOp(val1, val2, op));  // 执行运算
            }
            push(ops, current);  // 将运算符压入栈
            prevCharIsOperatorOrOpenParen = 1;
            i++;
        }
        // 处理加法和减法
        else if (current == '+' || current == '-') {
            if (prevCharIsOperatorOrOpenParen) {
                // 如果前一个是运算符或左括号，把 + 和 - 作为符号处理
                push(values, (current == '+' ? 1 : -1));
                i++;
            } else {
                while (!isEmpty(ops) && precedence(peek(ops)) >= precedence(current)) {
                    double val2 = pop(values);
                    double val1 = pop(values);
                    char op = (char)pop(ops);
                    push(values, applyOp(val1, val2, op));  // 执行运算
                }
                push(ops, current);  // 将运算符压入栈
                i++;
            }
            prevCharIsOperatorOrOpenParen = 1;
        }
        // 左括号处理
        else if (current == '(') {
            push(ops, current);  // 将 '(' 压入运算符栈
            prevCharIsOperatorOrOpenParen = 1;
            i++;
        }
        // 右括号处理
        else if (current == ')') {
            while (!isEmpty(ops) && peek(ops) != '(') {
                double val2 = pop(values);
                double val1 = pop(values);
                char op = (char)pop(ops);
                push(values, applyOp(val1, val2, op));  // 执行运算
            }
            pop(ops);  // 弹出 '('
            prevCharIsOperatorOrOpenParen = 0;
            i++;
        }
        // 非法字符
        else if (current != '=') {
            return 0;
        }
        else {
            i++;  // 跳过 '='
        }
    }

    // 如果以 '=' 结尾，返回合法
    return !prevCharIsOperatorOrOpenParen;
}

// 计算表达式的结果
double evaluateExpression(Stack* values, Stack* ops) {
    while (!isEmpty(ops)) {
        double val2 = pop(values);
        double val1 = pop(values);
        char op = (char)pop(ops);
        push(values, applyOp(val1, val2, op));  // 执行运算
    }

    return pop(values);  // 返回最终结果
}

int main() {
    char expression[200] = "2*(3-1)^2+(-1)=";  // 输入的数学表达式
    
    Stack values, ops;  // 存储数字和运算符的栈
    initStack(&values);  // 初始化栈
    initStack(&ops);

    if (isValidAndPushToStack(expression, &values, &ops)) {
        double result = evaluateExpression(&values, &ops);  // 计算表达式
        printf("Result: %.2f\n", result);  // 输出结果
    } else {
        printf("Expression is invalid.\n");  // 如果表达式不合法，输出错误信息
    }
	
	
    return 0;
}
