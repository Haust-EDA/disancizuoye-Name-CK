#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_STACK_SIZE 30

// ջ�����ݽṹ
typedef struct {
    double stack[MAX_STACK_SIZE];  // �洢ջ�е�Ԫ��
    int top;  // ջ������
} Stack;

// ջ��ز�������
void initStack(Stack* s) {
    s->top = -1;  // ��ʼ��ջΪ��
}

int isEmpty(Stack* s) {
    return s->top == -1;  // ջ�Ƿ�Ϊ��
}

int isFull(Stack* s) {
    return s->top == MAX_STACK_SIZE - 1;  // ջ�Ƿ���
}

void push(Stack* s, double value) {
    if (!isFull(s)) {
        s->stack[++(s->top)] = value;  // ��Ԫ��ѹ��ջ
    }
}

double pop(Stack* s) {
    if (!isEmpty(s)) {
        return s->stack[(s->top)--];  // ��ջ�е���Ԫ��
    }
    return -1;  // ����ֵ
}

double peek(Stack* s) {
    if (!isEmpty(s)) {
        return s->stack[s->top];  // ����ջ��Ԫ��
    }
    return -1;  // ����ֵ
}

// ��������ȼ�����
int precedence(char op) {
    if (op == '^') return 3;  // ָ���������ȼ����
    if (op == '*' || op == '/') return 2;  // �˷��ͳ�����֮
    if (op == '+' || op == '-') return 1;  // �ӷ��ͼ������
    return 0;
}

// ִ������ĺ���
double applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return pow(a, b);  // ִ��ָ������
    }
    return 0;
}

// �����ʽ�Ƿ�Ϸ����������ֺ������ѹ��ջ
int isValidAndPushToStack(const char* expression, Stack* values, Stack* ops) {
    int len = strlen(expression);
    int i = 0;
    char token[50];  // ���ڴ洢����
    int tokenIndex = 0;
    int prevCharIsOperatorOrOpenParen = 1;  // ǰһ���ַ��Ƿ����������������

    while (i < len) {
        char current = expression[i];

        // �����ո�
        if (current == ' ') {
            i++;
            continue;
        }

        // ��������ֻ򸺺ţ���������
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
            
            // ������ѹ��ջ
            push(values, atof(token));
            prevCharIsOperatorOrOpenParen = 0;
        }
        // ����˷���������ָ�������
        else if (strchr("*/^", current)) {
            if (prevCharIsOperatorOrOpenParen) return 0;  // ���ǰһ����������������ţ����ʽ���Ϸ�
            while (!isEmpty(ops) && precedence(peek(ops)) >= precedence(current)) {
                double val2 = pop(values);
                double val1 = pop(values);
                char op = (char)pop(ops);
                push(values, applyOp(val1, val2, op));  // ִ������
            }
            push(ops, current);  // �������ѹ��ջ
            prevCharIsOperatorOrOpenParen = 1;
            i++;
        }
        // ����ӷ��ͼ���
        else if (current == '+' || current == '-') {
            if (prevCharIsOperatorOrOpenParen) {
                // ���ǰһ����������������ţ��� + �� - ��Ϊ���Ŵ���
                push(values, (current == '+' ? 1 : -1));
                i++;
            } else {
                while (!isEmpty(ops) && precedence(peek(ops)) >= precedence(current)) {
                    double val2 = pop(values);
                    double val1 = pop(values);
                    char op = (char)pop(ops);
                    push(values, applyOp(val1, val2, op));  // ִ������
                }
                push(ops, current);  // �������ѹ��ջ
                i++;
            }
            prevCharIsOperatorOrOpenParen = 1;
        }
        // �����Ŵ���
        else if (current == '(') {
            push(ops, current);  // �� '(' ѹ�������ջ
            prevCharIsOperatorOrOpenParen = 1;
            i++;
        }
        // �����Ŵ���
        else if (current == ')') {
            while (!isEmpty(ops) && peek(ops) != '(') {
                double val2 = pop(values);
                double val1 = pop(values);
                char op = (char)pop(ops);
                push(values, applyOp(val1, val2, op));  // ִ������
            }
            pop(ops);  // ���� '('
            prevCharIsOperatorOrOpenParen = 0;
            i++;
        }
        // �Ƿ��ַ�
        else if (current != '=') {
            return 0;
        }
        else {
            i++;  // ���� '='
        }
    }

    // ����� '=' ��β�����غϷ�
    return !prevCharIsOperatorOrOpenParen;
}

// ������ʽ�Ľ��
double evaluateExpression(Stack* values, Stack* ops) {
    while (!isEmpty(ops)) {
        double val2 = pop(values);
        double val1 = pop(values);
        char op = (char)pop(ops);
        push(values, applyOp(val1, val2, op));  // ִ������
    }

    return pop(values);  // �������ս��
}

int main() {
    char expression[200] = "2*(3-1)^2+(-1)=";  // �������ѧ���ʽ
    
    Stack values, ops;  // �洢���ֺ��������ջ
    initStack(&values);  // ��ʼ��ջ
    initStack(&ops);

    if (isValidAndPushToStack(expression, &values, &ops)) {
        double result = evaluateExpression(&values, &ops);  // ������ʽ
        printf("Result: %.2f\n", result);  // ������
    } else {
        printf("Expression is invalid.\n");  // ������ʽ���Ϸ������������Ϣ
    }
	
	
    return 0;
}
