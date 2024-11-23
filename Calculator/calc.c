#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#define PI 3.14
double calculate_expression(const char* expression, int* error);
double arthmetic_calculation(const char* expression, int* error);

double evaluate_parenthesis(const char* expression, int* error);

double check_valid_input(const char* expression, int* error);

double check_parenthesis_balanced(const char* expression);

double evaluate_trignometric(const char* expression, int* error);

int main() {
    char input[256];
    int error;

    while (1) {
        if (fgets(input, 256, stdin) == NULL) {
            printf("Error reading");
            break;
        }

        //remove newline
        input[strcspn(input, "\n")] = 0;

        //check if exit
        if (strcmp(input, "exit") == 0) {
            break;
        }

        //evaluate expression
        error = 0;
        double result = calculate_expression(input, &error);
        if (error) {
            printf("invalid expression\n");
        }
        else {
            printf("%lf\n", result);
        }
    }

    return 0;
}

double calculate_expression(const char* expression, int* error) {
    // if no parenthesis -> do something else do something
    //check if invalid input
    if (!check_parenthesis_balanced(expression)) {
        *error = 1;
        return 0;
    }

    if (!check_valid_input(expression, error)) {
        *error = 1;
        return 0;
    }

    if (strncmp(expression, "sin(", 4) == 0 || strncmp(expression, "cos(", 4) == 0 || strncmp(expression, "tan(", 4) == 0) {
        return evaluate_trignometric(expression, error);
    }

    if (strchr(expression, '(') != NULL || strchr(expression, ')') != NULL) {
        return evaluate_parenthesis(expression, error);
    }
    return arthmetic_calculation(expression, error);
}


double check_valid_input(const char* expression, int* error) {

    if (strncmp(expression, "sin(", 4) == 0 ||
        strncmp(expression, "cos(", 4) == 0 ||
        strncmp(expression, "tan(", 4) == 0) {
        return 1;
    }


    int dot_found = 0;
    int last_operator = 1;  // Expecting an oper
    int last_open_parenthesis = 0;

    for (int i = 0; i < strlen(expression); i++) {
        char current = expression[i];

        if (isdigit(current)) {
            last_operator = 0; // Not expecting an operator
        }
        else if (isspace(current)) {
            continue; // Ignore spaces
        }
        else if (current == '.') {
            if (dot_found || last_operator) {
                *error = 1; // Invalid if multiple dots or dot after an operator
                return 0;
            }
            dot_found = 1; // Found decimal point
        }
        else if (current == '(') {
            if (!last_operator && i != 0) {  // If not expecting an operator before '('
                *error = 1;
                return 0;
            }
            last_operator = 1; // Expecting an operand after '('
        }
        else if (current == ')') {
            if (last_operator) { // If there's an operator or parenthesis right before ')'
                *error = 1;
                return 0;
            }
            last_operator = 0; // Expecting an operator after ')'
        }
        else if (strchr("+-*/%", current)) {
            if (last_operator) {  // Invalid if operator is right after another operator or '('
                *error = 1;
                return 0;
            }
            last_operator = 1; // Expecting an operand after an operator
            dot_found = 0; // Reset dot for the next number
        }
        else {
            *error = 1; // Invalid character
            return 0;
        }
    }

    if (last_operator || last_open_parenthesis) {
        *error = 1; // If the expression ends with an operator or unclosed parenthesis
        return 0;
    }

    return 1;
}

double check_parenthesis_balanced(const char* expression) {
    int balance = 0;
    while (*expression) {
        if (*expression == '(') {
            balance++;
        }
        else if (*expression == ')') {
            balance--;
            if (balance < 0) {
                return 0;
            }
        }
        expression++;
    }
    return balance == 0;
}

double arthmetic_calculation(const char* expression, int* error) {
    double result = 0;
    double temp = 0;
    char Operator = '+';

    const char* current = expression;

    while (*current) {
        if (isdigit(*current) || *current == '.') {
            double num = strtod(current, (char**)&current);  // Convert string to number


            if (Operator == '+') {
                result += temp;
                temp = num;
            }
            else if (Operator == '-') {
                result += temp;
                temp = -num;
            }
            else if (Operator == '*') {
                temp *= num;
            }
            else if (Operator == '/') {
                if (num == 0) {
                    *error = 1;
                    return 0;
                }
                temp /= num;
            }
            else if (Operator == '%') {
                if (num == 0) {
                    *error = 1;
                    return 0;
                }
                temp = (int)temp % (int)num;
            }
        }
        else if (*current == '+' || *current == '-' || *current == '*' || *current == '/' || *current == '%') {
            Operator = *current;
            current++;
        }
        else {
            current++;  // Ignore any invalid characters
        }
    }

    result += temp;  // Add the final value of temp to the result
    return result;
}


double evaluate_parenthesis(const char* expression, int* error) {
    char expr[256];
    char new_expr[256];
    strcpy(new_expr, expression); // Work with a copy of the expression

    while (strchr(new_expr, '(')) {  // While there are parentheses in the expression
        char* open = strrchr(new_expr, '('); // Find the last '('
        char* close = strchr(open, ')');    // Find the matching ')'

        if (!close) {  // Mismatched parentheses
            *error = 1;
            return 0;
        }

        // Extract the sub-expression inside the parentheses
        strncpy(expr, open + 1, close - open - 1);
        expr[close - open - 1] = '\0';

        // Evaluate the sub-expression
        double result = calculate_expression(expr, error);
        if (*error) {
            return 0;
        }

        // Replace the parentheses with the result
        char temp[256];
        snprintf(temp, sizeof(temp), "%.*s%lf%s",
            (int)(open - new_expr), new_expr,  // Part before '('
            result,                            // Result of the sub-expression
            close + 1);                        // Part after ')'

        strcpy(new_expr, temp);  // Update the expression
    }

    // Once all parentheses are resolved, evaluate the final expression
    return calculate_expression(new_expr, error);
}

double evaluate_trignometric(const char* expression, int* error) {
    if (strncmp(expression, "sin(", 4) == 0 ||
        strncmp(expression, "cos(", 4) == 0 ||
        strncmp(expression, "tan(", 4) == 0) {

        char function[4];
        strncpy(function, expression, 3); // Extract "sin", "cos", or "tan"
        function[3] = '\0';

        const char* open = expression + 3; // Move past "sin", "cos", or "tan"
        if (*open != '(') {
            *error = 1;
            return 0;
        }

        const char* close = strchr(open, ')'); // Find the closing parenthesis
        if (!close) {
            *error = 1;
            return 0;
        }

        // Extract the expression inside the parentheses
        char inner_expr[256];
        strncpy(inner_expr, open + 1, close - open - 1);
        inner_expr[close - open - 1] = '\0';

        // Evaluate the inner expression
        double value = calculate_expression(inner_expr, error);
        if (*error) return 0;

        // Convert the value from degrees to radians
        value = value * PI / 180.0;

        // Apply the trigonometric function
        if (strcmp(function, "sin") == 0) {
            return sin(value);
        }
        else if (strcmp(function, "cos") == 0) {
            return cos(value);
        }
        else if (strcmp(function, "tan") == 0) {
            return tan(value);
        }
    }

    *error = 1; // If the expression is not a trigonometric function
    return 0;
}