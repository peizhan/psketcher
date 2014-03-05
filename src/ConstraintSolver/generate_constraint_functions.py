"""This module parses the constraint equations in the file solver_functions.txt and generates
GiNaC functions in C++ for each constraint equation. The generated C++ files are 
ConstraintFunctions.h and ConstraintFunctions.cpp. The existing C++ files are deleted. """

from sympy import sympify,symbols,ccode
from string import split,strip,rstrip
from mako.template import Template
import re

class ConstraintEquation:
    def __init__(self,line):
        self.parse_equation(line)
    
    def parse_equation(self,line):
        """Takes one line from the solver_functions.txt file and parses it. Defines the
        following parameters (function_name, paramater_list, expression). """
        
        # First, remove and parse any substitutions that were defined (substititutions
        # are enclosed in square braces and immediately follow the parameter list and proceed
        # the equals sign). 
        substitution_list = None
        match = re.search(r'\[(.+)\]',line)
        if match is not None:        
            substitution_string = match.group(1)
            line = re.sub(r'\[.+\]','',line)  # remove the substitution list from the input line
            substitution_list = list()
            for substitution in split(substitution_string,","):
                (lhs,rhs) = split(substitution,"=")
                substitution_list.append((strip(lhs),strip(rhs)))
            substitution_list.reverse() # substitution needs to happen in reverse order since substitutions my depend on those to the left in the input line
        
        (lhs,rhs) = split(line,"=")
        (function_name,parameter_list) = split(lhs,"(")
        self.function_name = strip(function_name)
        parameter_list = rstrip(strip(parameter_list),")")
        self.parameter_list = [strip(parameter) for parameter in split(parameter_list,",")]
        
        self.expression = sympify(strip(rhs))
        
        if substitution_list is not None:
            self.expression = self.expression.subs(substitution_list)
        

if __name__ == "__main__":
    input_file = open('solver_functions.txt')
    
    constraint_equation_list = list()
    
    for line in input_file:
        # remove any comments from the input line (any text after and including # is deleted)
        line = re.sub(r'#.*','',line)
        if re.match(r'\w+\([\w\s,]+\).*=.*',line):  # check to make sure that the line at least looks like an equation definition
            constraint_equation_list.append(ConstraintEquation(line))
        elif re.match(r'\S',line):  # warn the user if a line was skipped 
            print "Warning: The following line was ignored: \n" + line  

    header_template = Template(filename='SolverFunctions.h.template')
    header_file = open('SolverFunctions.h','w')
    header_file.writelines(header_template.render(equations=constraint_equation_list))
    header_file.close()

    source_template = Template(filename='SolverFunctions.cpp.template')
    source_file = open('SolverFunctions.cpp','w')
    source_file.writelines(source_template.render(equations=constraint_equation_list))
    source_file.close()
