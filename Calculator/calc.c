#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#define e 2.141592653589793
#define PI 3.14
double calculate_expression(const char *expression, int *error);
double arthmetic_calculation(const char *expression, int *error);

double evaluate_parenthesis(const char *expression, int *error);

double check_valid_input(const char *expression, int *error);

double check_parenthesis_balanced(const char *expression);

double evaluate_trignometric(const char *expression, int *error);

double evaluate_logarithmic(const char *expression, int *error);
double evaluate_logarithmic(const char *expression, int *error);

double check_valid_logarithmic(const char *expression, int *error);

double check_valid_trignometric(const char *expression, int *error);




int main(){
  char input[256];
  int error;

  while(1){
    if(fgets(input, 256, stdin)==NULL){
      printf("Error reading");
      break;
    }

  
    input[strcspn(input, "\n")] = 0;

    
    if(strcmp(input, "exit") == 0){
      break;
    }

    
    error = 0;
    double result = calculate_expression(input, &error);
    if(error){
      printf("invalid expression\n");
    }else{
      printf("%lf\n", result);
    }
  }

  return 0;
}

double calculate_expression(const char *expression, int *error){
  // if no parenthesis -> do something else do something
  //check if invalid input
  if(!check_parenthesis_balanced(expression)){
    *error = 1;
    return 0;
  }

  if(!check_valid_input(expression, error)){
    *error = 1;
    return 0;
  }

  if(strncmp(expression, "sin(",4)== 0 || strncmp(expression, "cos(",4) == 0 ||  strncmp(expression, "tan(",4) == 0){
    if(!check_valid_trignometric(expression,error)){
        *error = 1;
        return 0;
    }
    return evaluate_trignometric(expression,error);
  } else if(strncmp(expression, "log(",4) == 0){
    if(!check_valid_logarithmic(expression, error)){
        *error = 1;
        return 0;
    }
    return evaluate_logarithmic(expression,error);
  }

  if(strchr(expression, '(') != NULL || strchr(expression, ')') != NULL){
    return evaluate_parenthesis(expression, error);
   }
    return arthmetic_calculation(expression, error);
}


double check_valid_input(const char *expression, int *error) {
    int dot_found = 0;
    int last_operator = 1;  
    int last_open_parenthesis = 0;
    int last_was_digit_or_paren = 0; 

    for (int i = 0; i < strlen(expression); i++) {
        char current = expression[i];

        if (isdigit(current)) {
            last_operator = 0;  
            last_was_digit_or_paren = 1; 
        } else if (isspace(current)) {
            continue; 
        } else if (current == '.') {
            if (dot_found || last_operator) {
                *error = 1; 
                return 0;
            }
            dot_found = 1;
        } else if (current == '(') {
            if (!last_operator && i != 0) {  
                *error = 1;
                return 0;
            }
            last_operator = 1;
            last_was_digit_or_paren = 0;  
        } else if (current == ')') {
            if (last_operator) {  
                *error = 1;
                return 0;
            }
            last_operator = 0;
            last_was_digit_or_paren = 1;
        } else if (strchr("+-*/%^", current)) {
            if (last_operator && current != '-' && current != '+') { 
                *error = 1;
                return 0;
            }
            last_operator = 1;
            dot_found = 0; 
        } else if (strncmp(&expression[i], "sin(", 4) == 0 || strncmp(&expression[i], "cos(", 4) == 0 || 
                   strncmp(&expression[i], "tan(", 4) == 0 || strncmp(&expression[i], "log(", 4) == 0) {
            i += 3; 
            last_operator = 1;  
            continue;
        } else {
            *error = 1;  
            return 0;
        }
    }

    if (last_operator || last_open_parenthesis) {  
        *error = 1;
        return 0;
    }

    return 1;
}

double check_parenthesis_balanced(const char *expression){
  int balance = 0;
  while(*expression){
    if(*expression == '('){
      balance++;
    }else if(*expression == ')'){
      balance--;
      if(balance < 0){
        return 0;
      }
    }
    expression++;
  }
  return balance == 0;
}

double arthmetic_calculation(const char *expression, int *error) {
    double result = 0;
    double temp = 0;
    char operator = '+'; 

    const char *current = expression;

    while (*current) {
        if (isdigit(*current) || *current == '.') {
            double num = strtod(current, (char **)&current);  
            
          
            if (operator == '+') {
                result += temp;
                temp = num;
            } else if (operator == '-') {
                result += temp;
                temp = -num;
            } else if (operator == '*') {
                temp *= num;
            } else if (operator == '/') {
                if (num == 0) {
                    *error = 1;  
                    return 0;
                }
                temp /= num;
            } else if (operator == '%') {
                if (num == 0) {
                    *error = 1;
                    return 0;
                }
                temp = (int)temp % (int)num;  
            } else if (operator == '^') {
                temp = pow(temp, num);  
            }
        } else if (*current == '+' || *current == '-' || *current == '*' || *current == '/' || *current == '%' || *current == '^') {
            operator = *current;  
            current++;
        } else if (*current == '(') {
            current++;
        } else {
            current++;  
        }
    }

    result += temp;  
    return result;
}



double evaluate_parenthesis(const char *expression, int *error) {
    char expr[256];
    char new_expr[256];
    strcpy(new_expr, expression); 

    while (strchr(new_expr, '(')) {  
        char *open = strrchr(new_expr, '(');
        char *close = strchr(open, ')');   

        if (!close) { 
            *error = 1;
            return 0;
        }

        strncpy(expr, open + 1, close - open - 1);
        expr[close - open - 1] = '\0';

    
        double result = calculate_expression(expr, error);
        if (*error) {
            return 0;
        }

        char temp[256];
        snprintf(temp, sizeof(temp), "%.*s%lf%s",
                 (int)(open - new_expr), new_expr,  
                 result,                           
                 close + 1);                        

        strcpy(new_expr, temp); 
    }

    
    return calculate_expression(new_expr, error);
}

double evaluate_trignometric(const char *expression, int *error) {
    if (strncmp(expression, "sin(", 4) == 0 ||
        strncmp(expression, "cos(", 4) == 0 ||
        strncmp(expression, "tan(", 4) == 0) {

        char function[4];
        strncpy(function, expression, 3); 
        function[3] = '\0';

        const char *open = expression + 3;
        if (*open != '(') {
            *error = 1;
            return 0;
        }

        const char *close = strchr(open, ')'); 
        if (!close) {
            *error = 1;
            return 0;
        }

        
        char inner_expr[256];
        strncpy(inner_expr, open + 1, close - open - 1);
        inner_expr[close - open - 1] = '\0';

      
        double value = calculate_expression(inner_expr, error);
        if (*error) return 0;

       
        value = value * PI / 180.0;

        
        if (strcmp(function, "sin") == 0) {
            return sin(value);
        } else if (strcmp(function, "cos") == 0) {
            return cos(value);
        } else if (strcmp(function, "tan") == 0) {
            return tan(value);
        } 
    }

    *error = 1;
    return 0;
}



double evaluate_logarithmic(const char *expression, int *error){
    if(strncmp(expression, "log(", 4) == 0){
        const char *open = expression + 3; 
        if (*open!= '(') {
            *error = 1;
            return 0;
        }

        const char *close = strchr(open, ')');
        if (!close) {
            *error = 1;
            return 0;
        }

        char inner_expr[256];
        strncpy(inner_expr,open + 1, close - open - 1);
        inner_expr[close - open - 1] = '\0';

        double value = calculate_expression(inner_expr, error);
        if(*error ) return 0;

        if(value <= 0){
            *error = 1;
            return 0;
        }

        return log(value);
    }

    *error = 1;
    return 0;
}

double check_valid_trignometric(const char *expression, int *error) {
    if (strncmp(expression, "sin(", 4) == 0 || strncmp(expression, "cos(", 4) == 0 ||
        strncmp(expression, "tan(", 4) == 0) {

        const char *open = expression + 3; 
        if (*open != '(') {
            *error = 1;
            return 0;
        }

        const char *close = strchr(open, ')');
        if (!close) {
            *error = 1;
            return 0;
        }

        
        char inner_expr[256];
        strncpy(inner_expr, open + 1, close - open - 1);
        inner_expr[close - open - 1] = '\0';

        
        if (!check_valid_input(inner_expr, error)) {
            return 0;
        }

        return 1;  
    }

    *error = 1;
    return 0;
}
double check_valid_logarithmic(const char *expression, int *error) {
    if (strncmp(expression, "log(", 4) == 0) {
        const char *open = expression + 3; 
        if (*open != '(') {
            *error = 1;
            return 0;
        }

        const char *close = strchr(open, ')'); 
        if (!close) {
            *error = 1;
            return 0;
        }

        
        char inner_expr[256];
        strncpy(inner_expr, open + 1, close - open - 1);
        inner_expr[close - open - 1] = '\0';

       
        if (!check_valid_input(inner_expr, error)) {
            return 0;
        }

        return 1;  
    }

    *error = 1;
    return 0;
}

