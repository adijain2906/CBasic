//#define INDIVISUAL_MODE
//yeah, the compiler can run as a standalone project
//just define the above to do that

//#define DEBUG_MODE
//define(uncomment) this^ to view debug messages on the output screen itself

#ifdef INDIVISUAL_MODE
	#include <iostream.h>
	#include <conio.h>
	#include <process.h>
	#include <string.h>
	#include <fstream.h>
	#include <ctype.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <math.h>
	#include <graphics.h>
	#include <dos.h>
#endif

//some constants:
#define TRUE 1
#define FALSE 0
//ofcourse there are more constants but they are #define-d near the
//classes they are used for easy refrence


int is_in_compile_mode = FALSE;
int current_exec_line = 0;
//global flag to permit maximum reuse of pre existing code

//evaluator defintions
#define size 30
char stack[size];
int TOP = -1;

fstream error_handle;
class program_class
{
 private:
	int n_rows;
	char*** program_table; //used in "including" other .qb files
	int included_programs;
	char** program_con; //The final program (after "including") is stored here
	fstream source;
	char filename[30];
	int is_open;
 public:
	/*~program_class()
	{
		delete[] program_con;
		//delete program_table;
	}*/

	void count_num_programs();

	void set_name(char* input_name)
	{
		strcpy(filename, input_name);
	}

	void set_included_programs(int a)
	{
		included_programs = a;
		program_table = new char**[included_programs];
	}

	void set_num_rows(int a)
	{
	   n_rows = a;
	   program_con = new char*[n_rows];
	   #ifdef DEBUG_MODE
		 cout << "Number of lines: " << n_rows;
	   #endif
	}

	void add_num_rows(int a)
	{
	   n_rows += a;
	}

	int get_num_rows(){return n_rows;}

	int display();

	int add_row(char* content, int length, int in);

	void read_source();

	int load_source_toram(char* filename);

	int open()
	{
		char filename[30];
		clrscr();
		cout << "\nEnter the name of the file(with extension): ";
		cin >> filename;
		source.open(filename, ios::in);
		#ifdef DEBUG_MODE
		cout << "\nOpened from open";
		#endif
		if(source.fail())
		{
			cout << "\nNa hoga tumse";
			getch();
			return FALSE;
		}
		set_name(filename);
		read_source();
		int funct;
		funct = load_source_toram(filename);
		//source.close();
		//cout << "\nClosed by open";
		#ifdef DEBUG_MODE
			getch();
		#endif
		if(funct == TRUE + 2) return TRUE + 3;
		else return TRUE;
	}

	void open_from_editor(char file_n[])
	{
		source.open(file_n, ios::in);
		#ifdef DEBUG_MODE
			cout << "\nOpened by open_from_editor";
		#endif
		if(source.fail())
		{
			source.close();
			source.open(file_n, ios::in);
			if(source.fail())
			{
				cout << "\nCompiler speaking, file not found";
				cout << "\nPress any key to exit";
				getch();
				exit(0);
			}
		}
		set_name(file_n);
		read_source();
		int funct;
		funct = load_source_toram(filename);
		if(funct == TRUE+2)
		{
			//error checking skip
		}
		//source.close();
		//cout << "\nClosed by open_from_editor";
		return;
	}

	char* return_row_ofnumber(int r)
	{
		return program_con[r];
	}

	char** get_program()
	{
		return program_con;
	}

	void refresh();

	void fill_program_table();

	program_class()
	{
		is_open = FALSE;
	}
};
//Global
program_class* program;

void initialize_compiler()
{
   program = new program_class;
}
void destroy_compiler()
{
   //delete program;
}

void program_class::count_num_programs()
{
	int count = 0;
	for(int a = 0; a < n_rows; a++)
	{
		if(strncmp("include",program_con[a],7) == 0)
		{
		      count++;
		}
	}
	#ifdef DEBUG_MODE
		cout << "\nFound " << count << " include statements";
		getch();
	#endif
	if(count == 0)
	{
		//no include statments there, you are free as a bird.
		//otherwise new char**[0] would have given an error.
	}
	else
	{
		//create a 3D table with count pages
		set_included_programs(count);
		fill_program_table();
	}
}

void program_class::fill_program_table()
{
	int total_lines = n_rows;
	int p_index = 0;
	int file_lines[10]; //isse zyada include statments par memory cap hai
	int i,j,k; //indicides
	for(i = 0; i<n_rows; i++)
	//this copies into the first page of program_table
	{
		program_table[p_index][i] = new char[strlen(program_con[i])+2];
		strcpy(program_table[p_index][i], program_con[i]);
	}
	file_lines[p_index] = n_rows;
	p_index++;
	char filename1[30], buffer[100];
	fstream temp;
	for(int a = 0; a < n_rows; a++)
	{
		if(strncmp("include",program_con[a],7) == 0)
		{
			//format: include "qbasic.qb"
			//asummed "qbasic.qb" exists
			//this error checking will be implimented in future^
			for(int len = 9; program_con[a][len] != '\"';len++)
			{
				filename1[len-9] = program_con[a][len];
			}
			filename1[len-9] = '\0';
			#ifdef DEBUG_MODE
				cout << "\nIncluding: " << filename1;
				getch();
			#endif
			temp.open(filename1, ios::in);
			//first read num of lines
			int number_lines = 0;
			temp.getline(buffer,100, '\n');
			++number_lines;
			while(!temp.eof())
			{
				temp.getline(buffer,100,'\n');
				++number_lines;
			}
			program_table[p_index] = new char*[number_lines];
			temp.close();
			temp.open(filename1, ios::in);
			temp.getline(buffer,100, '\n');
			program_table[p_index][0] = new char[strlen(buffer)+2];
			strcpy(program_table[p_index][0], buffer);
			file_lines[p_index] = number_lines;
			int p_y_index = 1;
			total_lines++;
			//filled 0th element above^
			//copy the remaining lines dierctly
			while(!temp.eof())
			{
				temp.getline(buffer,100,'\n');
				program_table[p_index][p_y_index] = new char[strlen(buffer)+2];
				strcpy(program_table[p_index][p_y_index], buffer);
				p_y_index++;
				total_lines++;
			}
			temp.close();
			p_index++;
		}
	}
	//created 3D array
	//free the already allotted program content and read through
	//this table sequentially
	//refresh(); idk why uncommenting this messes everything up :|
	program_con = new char*[total_lines]; //calulated above
	n_rows = total_lines;
	#ifdef DEBUG_MODE
		cout << "\nTotal lines: " << n_rows;
	#endif
	//storing it back

	int a_index = 0; //external index

	for(i = 0; i < p_index; i++)//select page
	{
	       for(j = 0; j  < file_lines[i]; a_index++,j++)
	       {
			program_con[a_index] = new char[strlen(program_table[i][j]) + 2];
			strcpy(program_con[a_index], program_table[i][j]);
	       }
	}
	//now, free the memory you have been hogging for this function

	for(i = 0; i < included_programs; i++)//select page
	{
	       for(j = 0; j  < file_lines[i]; j++)
	       {
			delete[] program_table[i][j];
	       }
	}
}

void program_class::refresh()
{
	//source.close();
	//for(int a = 0;a < n_rows; a++)delete[] program_con[a];
	return;
}

int program_class::display()
{
	  clrscr();

	  cout << "\t\t\t" << filename;

	  for(int a = 0; a < n_rows; ++a)
	  {
		cout << "\n" << program_con[a];
	  }
	  cout << "\n\n\nIs this the program you intended?";
	  try_again:
	  cout << "\n1. Yes";
	  cout << "\n2. No";
	  cout << "\n\nEnter your choice: ";
	  int choice;
	  cin >> choice;
	  if(choice == 1)
	  {
		//make the 3D table here
		//each 3D_Table[index] contains an indipendent .qb file
		return TRUE;
	  }
	  else if (choice == 2)
	  {
		refresh();
		return FALSE;
	  }
	  else if(choice != 1 && choice != 2)
	  {
		cout << "\nInvalid choice. Try Again";
		goto try_again;
	  }
	  return  FALSE;
}

int program_class::add_row(char* content, int length, int inp_index)
{
	   //static int index = 0;
	   //WASTING this memory. Do something.
	   if(inp_index < n_rows)
	   {
		program_con[inp_index] = new char[length+1];
		strcpy(program_con[inp_index], content);
		content[length] = '\0';
	   }
	   else
	   {
		cout << "Error: memory overflow";
	   }

	   if(inp_index == n_rows - 1)//output to message window
	   {

		#ifdef DEBUG_MODE
			cout << "\n\nProgram succesfully loaded.";
			cout << "\nPress any key to display";
			getch();
		#endif

		count_num_programs();

		/*#ifdef INDIVISUAL_MODE
			int funct_lunch = TRUE; //display();
			if(funct_lunch == TRUE)
			{
				return TRUE+1;
			}
			else
			{
				return FALSE;
			}
		#endif*/
		return TRUE+1;
	   }
	   return TRUE;
}

void program_class::read_source()
{
	source.close();
	#ifdef DEBUG_MODE
		cout << "\nClosed by read_source";
	#endif
	source.open(filename,ios::in);
	#ifdef DEBUG_MODE
		cout << "\nOpened by read source";
	#endif
	char buffer[100];
	int number = 0;
	source.getline(buffer,100, '\n');
	++number;
	while(!source.eof())
	{
		source.getline(buffer,100,'\n');
		++number;
	}
	set_num_rows(number);
}

int program_class::load_source_toram(char* filename)
{
		char buffer[100];

		source.close();
		#ifdef DEBUG_MODE
			cout << "\nClosed by load_source_toram";
		#endif
		source.open(filename, ios::in);
		#ifdef DEBUG_MODE
			cout << "\nOpened by load source to ram";
		#endif
		source.getline(buffer,100, '\n');
		int temp_index = 0;
		add_row(buffer, strlen(buffer),temp_index);
		temp_index++;
		int funct=0;
		while(!source.eof())
		{
			source.getline(buffer,100,'\n');
			funct = add_row(buffer, strlen(buffer),temp_index);
			temp_index++;
			//if(temp_index > n_rows)break;
		}

		source.close();
		#ifdef DEBUG_MODE
			cout << "\nClosed by load source to ram";
		#endif
		if(funct == TRUE + 1) return TRUE + 2;
		else return TRUE;
}

//Now comes the class where magic takes place
//C++ is a statically typed language. You can't create variables at runtime.
//or, can you?
template <class datatype> class variable
{
  private:
	char name[20];
	void* location;
	datatype value;
  public:
	variable(char *n, datatype v)
	{
		strcpy(name, n);
		value = v;
	}

	char* get_name(){return name;}

	void set_location()
	{
		location = (void*)this;
	}

	void set_value(datatype new_value);

	datatype get_value() {return value;}

	void refresh();

	~variable()
	{
		refresh();
	}

};

/*
why are you doing this?
Just don't do this.
There is an easier and clever way to impliment the arrays
void variable<int*>::allocate_array(int size)
{
	datatype a;
	value = (datatype)malloc((sizeof(&a))*size);
	sz = size;
}
*/

//template specialization implimented so that I can free the
//memory pointed to by char* value
void variable<char*>::refresh()
{
	delete[] value;
}
void variable<int>::refresh()
{
	//reqd to meet all compiler errors
}
void variable<float>::refresh()
{
	//reqd to meet all compiler errors
}
void variable<int>::set_value(int new_value)
{
	value = new_value;
}
void variable<float>::set_value(float new_value)
{
	value = new_value;
}
void variable<char*>::set_value(char* new_value)
{
	value = new char[strlen(new_value)+1];
	strcpy(value, new_value);
	value[strlen(new_value)] = '\0';
}

#define OKAY_V 1
#define NOTOKAY_V 2
//Q) What do you do when you want to create an array of diffrent datatypes?
//A) remove the notion of "datatype" itself using void pointers
//P.S: Proud member of The Void Club. Now you know why we are named what we are named.
class entery
{       private:
		void* var;
		char type[10];
		char name[15];
		void* location;
		int state;

	protected: //Just in case a better design comes to my mind and
		   //I need some functions in the derived class

		void intialize(char* ty)
		{
			if(strcmpi(ty,"int") == 0)
			{
				//Magic1: cbasic's int variables are intitalized with value 0
				var = new variable<int>(name, 0);
				location = var;
				((variable<int>*)var)->set_location();
				state = OKAY_V;
				//Default arguments may be implimented in a future realse
			}
			else if(strcmpi(ty,"string") == 0)
			{
				char new_str[10];//fuck
				var = new variable<char*>(name, new_str);
				location = var;
				((variable<char*>*)var)->set_location();
				state = OKAY_V;
			}
			else if(strcmpi(ty,"float") == 0)
			{
				//No points for guessing.
				//CBasic's Float defaults to 0.0
				var = new variable<float>(name,0.0);
				location = var;
				((variable<float>*)var)->set_location();
				state = OKAY_V;
			}
			else
			{
				if(is_in_compile_mode)
				{
					error_handle << "\nline 0: undefined data type";
				}
			}
		}

	public:

		void refresh()
		{
			if(strcmp(type, "int") == 0)
			{
				((variable<int>*)var)->refresh();
				delete ((variable<int>*)var);
			}
			else if(strcmp(type, "float") == 0)
			{
				((variable<float>*)var)->refresh();
				delete ((variable<float>*)var);
			}
			else if(strcmp(type, "string") == 0)
			{
				((variable<char*>*)var)->refresh();
				delete ((variable<char*>*)var);
			}
		}

	//public:
		entery(){state = NOTOKAY_V;}


		void set_name(char s[])
		{
			strcpy(name, s);
		}

		char* get_name() { return name;}

		char* get_type()
		{
			return type;
		}

		void set_type(char s[])
		{
			strcpy(type, s);
			#ifdef DEBUG_MODE
				cout << "\nNew variable recorded: " << name;
				getch();
			#endif
			intialize(type);
		}
		void* get_loc()
		{
			return location;
		}
};



#define FUNCTION 1
#define GLOBAL 2
#define OK 1
#define NOTOK 2
#define YES 1
#define NO 0
//Turbo C++ impliments my function stack
//the code below impliments my symbol stack
class sym_stack_node
{
  private:
	entery* node;
	sym_stack_node* next;
	int cur_num_variables;
  public:
	friend class stack_class;
	sym_stack_node(entery* n)
	{
		node = n;
		#ifdef DEBUG_MODE
			cout << "\nSymbol table created at: " << node;
			getch();
		#endif
		next = NULL;
		cur_num_variables = 0;
	}
	~sym_stack_node()
	{
		#ifdef DEBUG_MODE
			cout << "\nSymbol table deleted from: " << node;
		#endif
		delete node;
	}
};

//stack of symbol tables (each function has its private symbol table)
//Yes, that's how i impliment scope^

class stack_class
{
  private:
	sym_stack_node* head;
  public:
	stack_class()
	{
		#ifdef INDIVISUAL_MODE
		cout << "\nInitating a symbol table";
		#endif
		head = NULL;
	}
	void push(entery* set_to);
	void pop();
	entery* get_current(int&);
	int get_cur_num_vars();
	void set_cur_num_vars(int);
	~stack_class();
};
stack_class::~stack_class()
{
	while(head!=NULL)pop();
}
void stack_class::push(entery* set_to)
{
	if(head == NULL)
	{
		head = new sym_stack_node(set_to);
		head->cur_num_variables = 0;
		head->next = NULL;
	}
	else
	{
		sym_stack_node* create = new sym_stack_node(set_to);
		create->next = head;
		head = create;
	}
}
void stack_class::pop()
{
	if(head == NULL)return;
	else
	{
		sym_stack_node* save = head;
		head = head->next;
		delete save;
	}
}
entery* stack_class::get_current(int &use)
{
	if(head!=NULL)
	{
		use = TRUE;
		return head->node;
	}
	else
	{
		use = FALSE;
		entery* random = new entery;
		return random;
	}
}

int stack_class::get_cur_num_vars()
{
	if(head == NULL)
	{
		//in case in some special condition I seriosly mess up,
		error_handle << "\nline " << current_exec_line << ": linker error, please relaunch to continue.";
		return 1;
	}
	return head->cur_num_variables;
}

void stack_class::set_cur_num_vars(int n)
{
	if(head!=NULL)head->cur_num_variables = n;
}

//As i said, each function has a copy of this:
class symbol_table
{
	private:

		entery* variables_list;
		int state;
		int index;
		stack_class stack_1;

		//Expression evaluator functions
		void calc(float ans[],int &pos,char c);

		int presidence_check()
		{
			if (stack[TOP] == '(' || stack[TOP] == '+' || stack[TOP] == '-')
			return 1;
			else
			return 0;
		}

		void push(char ch)
		{
			if(TOP == size-1)
			{
				return;
			}
			stack[++TOP] = ch;
		}

		char pop()
		{
			if (TOP==-1)
			{
				return -1;
			}
			return stack[TOP--];
		}
		//end of evalutor functions

		char* add_bracket(char expression[]);

		float get_float_from_variable(char var[]);

		float evaluate(char expression[]);

		void check(char expression[]);
		void remove_space(char str[]);

	public:
		friend function_class;

		~symbol_table();

		symbol_table(){index = 0; state = NOTOK;}

		int get_state() {return state;}

		entery get_variable(char* in_var, int &valid);

		void display_vars();

		void add_var(int line);

		void is_array(char* in_var, int &valid);

		void* get_arr_loc_at_index(char* in_var, int arr_index);

		void add_var_args(int line, int num_args, char** args_list, void** args_locs);

		void init(int start, int end, int taste,int num_args, char** args_list, void** args_locs);

		void* get_var_loc(char* in_var, int &valid);

		int get_index(char* in_var);

		char* get_type_at_index(int i)
		{
			#ifdef DEBUG_MODE
				cout << "\nThe index: " << i << " at which resised: " << variables_list[i].get_type()
				<< " Asked as " << variables_list[i].get_name();
			#endif
			return variables_list[i].get_type();
		}
		float evaluator(char str[]);

		entery* get_current(int &x)
		{
			return stack_1.get_current(x);
		}
		void set_current(entery* s)
		{
			variables_list  = s;
		}
		void pop_table()
		{
			stack_1.pop();
		}
};

void symbol_table::calc(float ans[],int &pos,char c)
{
	switch(c)
	{
		case '+':
			ans[pos-1] = ans[pos-1] + ans[pos];break;
		case '-':
			ans[pos-1] = ans[pos-1] - ans[pos];break;
		case '*':
			ans[pos-1] = ans[pos-1] * ans[pos];break;
		case '/':
			ans[pos-1] = ans[pos-1] / ans[pos];break;
		case '^':
			ans[pos-1] = pow(ans[pos-1],ans[pos]);break;
	}
	pos--;
}

//evalutor functions
char* symbol_table::add_bracket(char expression[])
{
	char *exp = new char[strlen(expression)+2];
	exp[0] = '(';
	int i;
	for(i=0;expression[i]!='\0';i++)
	{
		exp[i+1] = expression[i];
	}
	exp[i+1] = ')';
	exp[i+2] = '\0';
	return exp;
}

float symbol_table::get_float_from_variable(char var[])
{
  int validate = 0;
  float ans;
  entery variable_holder = get_variable(var, validate);

  if(validate == 0)
  {
	error_handle << "\nline " << current_exec_line << ": undefined variable: " << var;
	#ifdef DEBUG_MODE
		display_vars();
	#endif
  }
  else
  {
	void* variable_location = variable_holder.get_loc();
	//char* var_type = new char[strlen(variable_holder.get_type())+1];
	char var_type[10];
	strcpy(var_type, variable_holder.get_type());
	void* vari;
	if(strcmpi(var_type,"int") == 0)
	{
		vari = (variable<int>*)variable_location;
		ans = (float)((variable<int>*)vari)->get_value();
	}
	else if(strcmpi(var_type,"string") == 0)
	{
		//generate error
	}
	else if(strcmpi(var_type,"float") == 0)
	{
		vari = (variable<float>*)variable_location;
		ans = ((variable<float>*)vari)->get_value();
	}
	return ans;
 }
 return ans;
}

float symbol_table::evaluate(char expression[])
{
	float ans[30];
	check(expression);
	char *exp = add_bracket(expression);
	if (exp==NULL)
	{
		exp = new char[strlen(expression)];
		strcpy(exp,expression);
	}
	char number[20],p=0;
	int i,pos=-1;
	char c;
	for (i=0;exp[i]!='\0';i++)
	{
		if (exp[i]=='(')
		{
			push(exp[i]);
		}
		else if(exp[i] == '+' || exp[i] == '-' || exp[i] == '/' || exp[i] == '*' || exp[i] == '^')
		{
			if (presidence_check()==0)
			{
				calc(ans,pos,pop());
			}
			push(exp[i]);
		}
		else if (isdigit(exp[i]))
		{
			for(p=0;isdigit(exp[i])||exp[i]=='.';p++,i++)
			{
				number[p] = exp[i];
			}
			i--;
			number[p] = '\0';
			ans[++pos] = atof(number);
		}
		else if (isalnum(exp[i]))
		{
			 for(p=0; isalnum(exp[i]); p++, i++)
			 {
				number[p] = exp[i];
			 }
			 i--;
			 number[p] = '\0';
			 ans[++pos] = get_float_from_variable(number);
		}
		else if (exp[i]==')')
		{
			while (1)
			{
				c = pop();
				if (c=='(') break;
				if (c!=')')
				calc(ans,pos,c);
			}
		}
		else
		{
			cout << "Invalid expression";
			getch();
			exit(0);
		}
	}
	return ans[0];
}

void symbol_table::check(char expression[])
{
	int bracket_check = 0;
	int i;
	for (i=0;expression[i] != '\0';i++)
	{
		if (bracket_check < 0) break;
		if (expression[i] == '(')
		{
			bracket_check++;
		}
		if (expression[i] == ')')
		{
			bracket_check--;
		}
	}
	if (bracket_check != 0)
	{
		cout << "Invalid Expression";
		getch();
		exit(0);
	}
}
void symbol_table::remove_space(char string[])
{
	int i,j;
	for (i=0;string[i]!='\0';i++)
	{
		if (string[i]==' ')
		{
			for (j=i+1;string[j]!='\0';j++)
			{
				string[j-1] = string[j];
			}
			j--;
			i--;
			string[j] = '\0';
		}
	}
}
float symbol_table::evaluator(char string[])
{
	float final_ans;
	//remove_space(string);
	final_ans = evaluate(string);
	return final_ans;
}

//end of evalutor functions


symbol_table::~symbol_table()
{
       for(int t = 0; t < stack_1.get_cur_num_vars(); t++)variables_list[t].refresh();
       delete  variables_list;
}
void symbol_table::display_vars()
{
     for(int i = 0; i < stack_1.get_cur_num_vars(); i++)
     {
	cout << "\nIndex: " <<i << " at loc " << variables_list[i].get_loc() << " with name " << variables_list[i].get_name();
     }
     cout << "\nError inocurred. Press any key to continue";
     getch();
}
int symbol_table::get_index(char* in_var)
{
	for(int i = 0; i < stack_1.get_cur_num_vars(); i++)
	{
		if(strcmp(variables_list[i].get_name(),in_var) == 0)
		{
			return i;
		}
	}
	return -1; //just returning to satisfy the syntax conditons
}

void* symbol_table::get_var_loc(char* in_var, int &valid)
{
	for(int i = 0; i < stack_1.get_cur_num_vars(); i++)
	{
		if(strcmp(variables_list[i].get_name(),in_var) == 0)
		{
			valid = TRUE;
			return variables_list[i].get_loc();
		}
	}
	valid = FALSE;
	return (void*)variables_list; //just returning to satisfy the syntax conditons
}

void symbol_table::is_array(char* in_var2, int &valid)
{
	char in_var[20];
	strcpy(in_var, in_var2);
	strcat(in_var, "[1]");
	for(int i = 0; i < stack_1.get_cur_num_vars(); i++)
	{
		if(strcmp(variables_list[i].get_name(),in_var) == 0)
		{
			valid = TRUE;
			return;
		}
	}
	valid = FALSE;
	return;
}

void* symbol_table::get_arr_loc_at_index(char* in_var2, int arr_index)
{
	char in_var[20];
	strcpy(in_var, in_var2);
	strcat(in_var, "[");
	char num_holder[10];

	itoa(arr_index, num_holder, 10);
	strcat(in_var, num_holder);
	strcat(in_var, "]");

	for(int i = 0; i < stack_1.get_cur_num_vars(); i++)
	{
		if(strcmp(variables_list[i].get_name(),in_var) == 0)
		{
			return variables_list[i].get_loc();
		}
	}
	return (void*)variables_list; //just returning to satisfy the syntax conditons
}

entery symbol_table::get_variable(char* in_var2, int &valid)
{
	//x[q]
	//take the string variable argument and resolve it here it self

	char in_var[20];
	strcpy(in_var, in_var2);
	int len, i;
	len = strlen(in_var);
	char var1[20], var_type[20];
	for(i = 0; in_var[i-1] != '[' && in_var[i] != '\0'; i++);
	if(in_var[i-1] == '[')
	{
		for(len = 0; in_var[i] != ']' && in_var[i] != '\0'; len++, i++)
		var1[len] = in_var[i];
		var1[len] = '\0';
		//check if above is a variable or a index
		int is_valid = 0, data = 0;
		entery  variable_holder = get_variable(var1, is_valid);
		if(is_valid == 1)
		{
			void* variable_location = variable_holder.get_loc();
			char var_type[10];// = new char[strlen(variable_holder.get_type())+1];
			strcpy(var_type, variable_holder.get_type());
			void* vari;
			if(strcmpi(var_type,"int") == 0)
			{
				vari = (variable<int>*)variable_location;
				data = ((variable<int>*)vari)->get_value();
			}
			else if(strcmpi(var_type,"string") == 0)
			{
				//error
			}
			else if(strcmpi(var_type,"float") == 0)
			{
				//error
			}

			//extract numerical value
			char end_bracket[] = "]";
			char num_holder[10];
			int t_index;
			for(t_index = 2; in_var[t_index-1] != '['; t_index++);
			in_var[t_index] = '\0';

			//num_holder[0] = '\0';
			//ready to concatinate^
			itoa(data, num_holder, 10);
			strcat(in_var, num_holder);
			strcat(in_var, end_bracket);

			//construct the string and move forward
		}
		else //mathmatical expression
		{
			int i,h;
			char data_val[10];      //array[x+2]
			for(int t_index = 0; in_var[t_index-1] != '['; t_index++);
			in_var[t_index] = '\0';
			int final_index = evaluator(var1);
			itoa(final_index, data_val, 10);
			strcat(in_var, data_val);
			strcat(in_var, "]");
		}

	}

	for(i = 0; i < stack_1.get_cur_num_vars(); i++)
	{
		if(strcmp(variables_list[i].get_name(),in_var) == 0)
		{
			valid = TRUE;
			return variables_list[i];
		}
	}
	valid = FALSE;
	return variables_list[0]; //just returning to satisfy the syntax conditons
}

void symbol_table::add_var_args(int line, int num_args, char** arg_type_list, void** arg_locs)
{
	index = 0;
	char** temp_prog = program->get_program();
	char t_name[20];
	char t_type[20];

	#ifdef DEBUG_MODE
		for(int test = 0; test < num_args; test++)
		{
			cout << "\nArg"<<test<< " is the type of " << arg_type_list[test];
		}
	#endif

	int z = 0;
	int limit;

	for(limit = 0; temp_prog[line][limit] != ')'; limit++);
	for(z = 0; temp_prog[line][z] != '('; z++);
	for(; temp_prog[line][z] != ')' && z <= limit; z++)
	{
		//if(index + 1 == num_args)break;
		//sub void print(dim z[2] int, dim l int, dim sfarr[3] string)
		if(temp_prog[line][z] == 'd' && temp_prog[line][z+1] == 'i' && temp_prog[line][z+2] == 'm')
		{
			z += 4;
			for(int c = 0; temp_prog[line][z] != ' ' ; c++,z++)
			{
				t_name[c] = temp_prog[line][z];
			}
			t_name[c] = '\0';

			for(z++, c = 0; temp_prog[line][z] != ' ' && temp_prog[line][z] != ',' &&  temp_prog[line][z] != ')' &&  isalnum(temp_prog[line][z]); c++,z++)
			t_type[c] = temp_prog[line][z] ;
			t_type[c] = '\0';
			//making an array now:
			if(t_name[strlen(t_name)-1] == ']')
			{
				int t_index, size1, len, name_index;
				char size_c[10];

				for(t_index = 0; t_name[t_index] != '['; t_index++);
				for(len = 0, t_index++; t_name[t_index] != ']'; len++, t_index++)

				size_c[len] = t_name[t_index];
				size_c[len] = '\0';
				size1 = atoi(size_c);

				char num_holder[10];

				for(name_index = 1; name_index <= size1; name_index++, index++)
				{
					for(t_index = 0; t_name[t_index-1] != '['; t_index++);
					t_name[t_index] = '\0';
					num_holder[0] = '\0';
					//ready to concatinate^
					itoa(name_index, num_holder, 10);
					strcat(t_name, num_holder);
					strcat(t_name, "]");

					variables_list[index].set_name(t_name);
					variables_list[index].set_type(t_type);
					//write from args here
					void* temp_var;
					void* vari = variables_list[index].get_loc();
					if(index < num_args)
					{
						if(strcmp(arg_type_list[index], "int") == 0)
						{
							temp_var = arg_locs[index];
							int data;
							data = ((variable<int>*)temp_var)->get_value();
							((variable<int>*)vari)->set_value(data);
						}
						if(strcmp(arg_type_list[index], "float") == 0)
						{
							temp_var = arg_locs[index];
							float data;
							data = ((variable<float>*)temp_var)->get_value();
							((variable<float>*)vari)->set_value(data);
						}
						if(strcmp(arg_type_list[index], "string") == 0)
						{
							temp_var = arg_locs[index];
							char data[80]; //width of screen is 80, feesible assumption
							strcpy(data,((variable<char*>*)temp_var)->get_value());
							((variable<char*>*)vari)->set_value(data);
						}
					}
				}
			}
			else
			{
				variables_list[index].set_name(t_name);
				variables_list[index].set_type(t_type);
				//read variable and write it here and now
				void* temp_var;
					void* vari = variables_list[index].get_loc();
					if(index < num_args)
					{
						if(strcmp(arg_type_list[index], "int") == 0)
						{
							temp_var = arg_locs[index];
							int data;
							data = ((variable<int>*)temp_var)->get_value();
							((variable<int>*)vari)->set_value(data);
						}
						if(strcmp(arg_type_list[index], "float") == 0)
						{
							temp_var = arg_locs[index];
							float data;
							data = ((variable<float>*)temp_var)->get_value();
							((variable<float>*)vari)->set_value(data);
						}
						if(strcmp(arg_type_list[index], "string") == 0)
						{
							temp_var = arg_locs[index];
							char data[80]; //width of screen is 80, feesible assumption
							strcpy(data,((variable<char*>*)temp_var)->get_value());
							((variable<char*>*)vari)->set_value(data);
						}
					}
					index++;
			}
		}
	}
}


void symbol_table::add_var(int line)
{
	char** temp_prog = program->get_program();
	char prog_dim[] = "dim";
	char t_name[20];
	char t_type[20];
	//dim x int
	if(strncmp(prog_dim, temp_prog[line], 3) == 0)
	{

		for(int c = 0; temp_prog[line][c+4] != ' ' ; c++)
		{
			t_name[c] = temp_prog[line][c+4];
		}
		t_name[c] = '\0';

		int point_to = strlen(t_name) + 5;
		for(int g = point_to; temp_prog[line][g] != '\0' &&  temp_prog[line][g] != '\n' &&  temp_prog[line][g] != ' '; ++g)
		t_type[g-point_to] = temp_prog[line][g];
		t_type[g - point_to] = '\0';

		if(t_name[c-1] == ']')
		{
			int t_index, size1, len, name_index;
			char size_c[10];
			for(t_index = 0; t_name[t_index] != '['; t_index++);
			for(len = 0, t_index++; t_name[t_index] != ']'; len++, t_index++)
			size_c[len] = t_name[t_index];
			size_c[len] = '\0';
			size1 = atoi(size_c);
			char end_bracket[] = "]";
			char num_holder[10];
			for(name_index = 1; name_index <= size1; name_index++)
			{
				for(t_index = 0; t_name[t_index-1] != '['; t_index++);
				t_name[t_index] = '\0';
				num_holder[0] = '\0';
				//ready to concatinate^
				itoa(name_index, num_holder, 10);
				strcat(t_name, num_holder);
				strcat(t_name, end_bracket);

				variables_list[index].set_name(t_name);
				variables_list[index].set_type(t_type);
				index++;
			}
		}
		else
		{
			variables_list[index].set_name(t_name);
			variables_list[index].set_type(t_type);
			++index;
			return;
		}
	}
	return;
}

void symbol_table::init(int start, int end, int taste, int num_args, char** arg_type_list, void** arg_locs)
{
   int temp_num_vars = 0;

   char** temp_prog = program->get_program();

   if(taste == FUNCTION)
   {
	char dim_prog[] = "dim";
	int z = 0;
	int len, q;
	for(z = 0; temp_prog[start][z] != '('; z++);
	for(; temp_prog[start][z] != ')'; z++)
	{
		if(temp_prog[start][z] == 'd' && temp_prog[start][z+1] == 'i' && temp_prog[start][z+2] == 'm')
		{
			//read name and check if an array then expand
			char name[10];
			for(len = 0, q = z+4; temp_prog[start][q] != ' '; len++, q++)
			{
				name[len] = temp_prog[start][q];
			}
			name[len] = '\0';
			if(name[len-1] == ']')
			{
				int t_index, size1;
				char size_c[10];
				for(t_index = len; name[t_index] != '['; t_index--);
				for(len = 0, t_index++; name[t_index] != ']'; len++, t_index++)
				size_c[len] = name[t_index];
				size_c[len] = '\0';
				size1 = atoi(size_c);
				//stack_1.set_cur_num_vars(stack_1.get_cur_num_vars() + size1);
				temp_num_vars+=size1;
			}
			else temp_num_vars++; //stack_1.set_cur_num_vars(stack_1.get_cur_num_vars() + 1);
		}
	}
	//create a seprate parser for function args
	for(int a = start+1; a <= end; ++a)
	{
		if(strncmp(dim_prog, temp_prog[a], 3) == 0)
		{
			char name[10];
			for(len = 0, q = 4; temp_prog[a][q] != ' '; len++, q++)
			{
				name[len] = temp_prog[a][q];
			}
			name[len] = '\0';
			if(name[len-1] == ']')
			{
				int t_index, size1;
				char size_c[10];
				for(t_index = len; name[t_index] != '['; t_index--);
				for(len = 0, t_index++; name[t_index] != ']'; len++, t_index++)
				size_c[len] = name[t_index];
				size_c[len] = '\0';
				size1 = atoi(size_c);
				//stack_1.set_cur_num_vars(stack_1.get_cur_num_vars() + size1);
				temp_num_vars += size1;
			}
			else temp_num_vars++;// stack_1.set_cur_num_vars(stack_1.get_cur_num_vars() + 1);
		}
	}
	#ifdef DEBUG_MODE
		cout << "\nNo. of varibales recorded(incl. function args): " << temp_num_vars;
		getch();
	#endif
	variables_list = new entery[temp_num_vars];
	stack_1.push(variables_list);//pop_up_at_ret
	stack_1.set_cur_num_vars(temp_num_vars);


	add_var_args(start, num_args, arg_type_list, arg_locs);

	for(int b = start+1; b <= end; ++b)
	{
		if(strncmp(dim_prog, temp_prog[b], 3) == 0)
		{
			add_var(b);
		}
	 }
	 state = OK;
    }
 }

class function_class
{
  private:
	char return_type[10];
	char name[20];
	char arg_list[40];
	int start;
	int end;

	symbol_table priv_vars;
	friend class function_table;

  public:

	char* get_name() {return name;}


	char* get_return_type()
	{
		//cout << "\nRet_type here: " << return_type;
		return return_type;
	}

	void function(char* rt, char* n, char* arg, int s, int e)
	{
		//return_type = new char[strlen(rt)+2];
		strcpy(return_type, rt);
		cout << "\nThe type of function " << n << " is " << rt;
		return_type[strlen(rt)] = '\0';
		//name = new char[strlen(n)+2];
		name[strlen(n)] = '\0';
		strcpy(name, n);
		//arg_list = new char[strlen(arg)+2];
		strcpy(arg_list, arg);
		arg_list[strlen(arg)] = '\0';
		start = s;
		end = e;
		#ifdef DEBUG_MODE
			cout << "\nIndivisual function speaking, I am " << n;
			getch();
		#endif

	}
};

#define OKAY 1
#define NOTOKAY 0
class function_table
{
	private:
		function_class* function_arr;
		int no_of_functions;
		int state;
		char** temp_prog;

		int return_int;
		float return_float;
		int is_in_if_then_else;

		int global_x, global_y;

		int check_comma(char a, char b)
		{
			if(a == '\"' && b == ',')return TRUE;
			return FALSE;
		}

		void cls()
		{
			clrscr();
		}
		void sleep()
		{
			getch();
		}
		void graphics()
		{
			int gd = DETECT, gm;
			initgraph(&gd, &gm, "c://tc/bgi");
			setbkcolor(BLACK);
			rectangle(0,0,getmaxx(),getmaxy());
			setfillstyle(1,BLACK);
			floodfill(10,10,WHITE);
			cleardevice();
		}

		void draw(char,int);
		//aditya evaluator functions
		void calc(float ans[],int &pos,char c);

		int presidence_check()
		{
			if (stack[TOP] == '(' || stack[TOP] == '+' || stack[TOP] == '-')
			return 1;
			else
			return 0;
		}

		void push(char ch)
		{
			if(TOP == size-1)
			{
				return;
			}
			stack[++TOP] = ch;
		}

		char pop()
		{
			if (TOP==-1)
			{
				return -1;
			}
			return stack[TOP--];
		}
		//end of evalutor functions

		int check_if_define(int exec_line, int index);

		void define(int exec_line, int index);


		void input(char str[], int index)
		{
			int validate = 0;
			entery variable_holder = function_arr[index].priv_vars.get_variable(str, validate);
			if(validate == 0)
			{
				error_handle << "\nline "<< current_exec_line << ": undefined variable: " <<str;
				//cout << "\nline: "<< current_exec_line << " undefined parameter: " <<str;
			}
			void* variable_location = variable_holder.get_loc();
			char* var_type = new char[strlen(variable_holder.get_type())+1];
			strcpy(var_type, variable_holder.get_type());
			void* vari;
		    if(is_in_compile_mode == FALSE)//no input in compile mode
		    {
			if(strcmpi(var_type,"int") == 0)
			{
				vari = (variable<int>*)variable_location;
				int data;
				cin >> data;
				((variable<int>*)vari)->set_value(data);
			}
			else if(strcmpi(var_type,"string") == 0)
			{
				vari = (variable<char*>*)variable_location;
				char data[80];
				gets(data);
				((variable<char*>*)vari)->set_value(data);
			}
			else if(strcmpi(var_type,"float") == 0)
			{
				vari = (variable<float>*)variable_location;
				float data;
				cin >> data;
				((variable<float>*)vari)->set_value(data);
			}
		    }

		}

		void print(char str[], int index);

		void* return_value(char str[], int index);

		void if_evaluate(char str[], int index, int exec_line, int &start_line, int &endif_line, int &endelse_line);

		int evaluate_bool_expression(const char str[], int index, int exec_line);

		int actual_bool(char var1[], char oper[], char var3[], int index);

		void for_intialize(char str[], int index);

		void next_step(char str[], int index, int &exec_line);

		//evaluator functions
		char* add_bracket(char expression[]);

		float get_float_from_variable(char var[], int index_of_function);

		float evaluate(char expression[],int index_of_function);

		void check(char expression[]);

		void remove_space(char str[]);

		void return_void(int exec_line, int index);
	public:
		char* call(char* f_name_arg, int num_args, int calling_function, int line, void* &ref);

		void* excute(int index, int num_args, char** args_add_list, void** arg_locs);
		//free all these pointers before exiting^

		function_table();

		void function_table_test();

		float evaluator(char string[], int index_of_function);

		~function_table();

		void set_color(char string[], int index);

		void set_bkcolor(char string[], int index);

		void pset(char string[], int index);

		void draw_circle(char string[], int index);
};

void function_table::draw(char command, int num)
{
 switch (command)
  {
	case 'u':
	{
		line(global_x,global_y, global_x, global_y-num);
		global_y -= num;
		break;
	}
	case 'd':
	{
		line(global_x,global_y, global_x, global_y+num);
		global_y += num;
		break;
	}
	case 'l':
	{
		line(global_x,global_y, global_x-num, global_y);
		global_x -= num;
		break;
	}
	case 'r':
	{
		line(global_x,global_y, global_x+num, global_y);
		global_x += num;
		break;
	}
	case 'e':
	{
		line(global_x,global_y, global_x + num, global_y -num);
		global_x += num;
		global_y -= num;
		break;
	}
	case 'f':
	{
		line(global_x,global_y, global_x + num, global_y +num);
		global_x += num;
		global_y += num;
		break;
	}
	case 'g':
	{
		line(global_x,global_y, global_x - num, global_y +num);
		global_x -= num;
		global_y += num;
		break;
	}
	case 'h':
	{
		line(global_x,global_y, global_x - num, global_y -num);
		global_x -= num;
		global_y -= num;
		break;
	}
 };
}

function_table::~function_table()
{
	//...
}
void function_table::calc(float ans[],int &pos,char c)
{
	switch(c)
	{
		case '+':
			ans[pos-1] = ans[pos-1] + ans[pos];break;
		case '-':
			ans[pos-1] = ans[pos-1] - ans[pos];break;
		case '*':
			ans[pos-1] = ans[pos-1] * ans[pos];break;
		case '/':
			ans[pos-1] = ans[pos-1] / ans[pos];break;
		case '^':
			ans[pos-1] = pow(ans[pos-1],ans[pos]);break;
	}
	pos--;
}

//evalutor functions
char* function_table::add_bracket(char expression[])
{
	char *exp = new char[strlen(expression)+2];
	exp[0] = '(';
	int i;
	for (i=0;expression[i]!='\0';i++)
	{
		exp[i+1] = expression[i];
	}
	exp[i+1] = ')';
	exp[i+2] = '\0';
	return exp;
}

float function_table::get_float_from_variable(char var[], int index_of_function)
{
  int validate = 0;
  float ans;
  entery variable_holder = function_arr[index_of_function].priv_vars.get_variable(var, validate);
  if(validate == 0)
  {
	cout << "\nExpression variable value ABORT " << var;
	function_arr[index_of_function].priv_vars.display_vars();
  }
  else
  {
	void* variable_location = variable_holder.get_loc();
	char var_type[10];// = new char[strlen(variable_holder.get_type())+1];
	strcpy(var_type, variable_holder.get_type());
	void* vari;
	if(strcmpi(var_type,"int") == 0)
	{
		vari = (variable<int>*)variable_location;
		ans = (float)((variable<int>*)vari)->get_value();
	}
	else if(strcmpi(var_type,"string") == 0)
	{
		//generate error
	}
	else if(strcmpi(var_type,"float") == 0)
	{
		vari = (variable<float>*)variable_location;
		ans = ((variable<float>*)vari)->get_value();
	}
	return ans;
 }
 return ans;
}

float function_table::evaluate(char expression[], int index_of_function)
{
	float ans[30];
	check(expression);
	char *exp = add_bracket(expression);
	if (exp==NULL)
	{
		exp = new char[strlen(expression)];
		strcpy(exp,expression);
	}
	char number[20],p=0;
	int i,pos=-1;
	char c;
	for (i=0;exp[i]!='\0';i++)
	{
		if (exp[i]=='(')
		{
			push(exp[i]);
		}
		else if(exp[i] == '+' || exp[i] == '-' || exp[i] == '/' || exp[i] == '*' || exp[i] == '^')
		{
			if (presidence_check()==0)
			{
				calc(ans,pos,pop());
			}
			push(exp[i]);
		}
		else if (isdigit(exp[i]))
		{
			for(p=0;isdigit(exp[i])||exp[i]=='.';p++,i++)
			{
				number[p] = exp[i];
			}
			i--;
			number[p] = '\0';
			ans[++pos] = atof(number);
		}
		else if (isalnum(exp[i]))
		{
			 for(p=0; isalnum(exp[i]); p++, i++)
			 {
				number[p] = exp[i];
			 }
			 i--;
			 number[p] = '\0';
			 ans[++pos] = get_float_from_variable(number, index_of_function);
		}
		else if (exp[i]==')')
		{
			while (1)
			{
				c = pop();
				if (c=='(') break;
				if (c!=')')
				calc(ans,pos,c);
			}
		}
		else
		{
			cout << "Invalid expression";
			getch();
			exit(0);
		}
	}
	return ans[0];
}
void function_table::check(char expression[])
{
	int bracket_check = 0;
	int i;
	for (i=0;expression[i] != '\0';i++)
	{
		if (bracket_check < 0) break;
		if (expression[i] == '(')
		{
			bracket_check++;
		}
		if (expression[i] == ')')
		{
			bracket_check--;
		}
	}
	if (bracket_check != 0)
	{
		cout << "Invalid Expression";
		getch();
		exit(0);
	}
}
void function_table::remove_space(char string[])
{
	int i,j;
	for (i=0;string[i]!='\0';i++)
	{
		if (string[i]==' ')
		{
			for (j=i+1;string[j]!='\0';j++)
			{
				string[j-1] = string[j];
			}
			j--;
			i--;// to check for space on the current location
			string[j] = '\0';
		}
	}
}
float function_table::evaluator(char string[], int index_of_function)
{
	float final_ans;
	//remove_space(string);
	final_ans = evaluate(string, index_of_function);
	return final_ans;
}

//end of evalutor functions

void function_table::function_table_test()
{

	char prog_dim[] = "sub";
	int index = 0;
	int end = 0;
	for(int a = 0; a <= program->get_num_rows(); ++a)
	{
		end = 0;
		if(strncmp(prog_dim, temp_prog[a], 3) == 0)
		{
			char t_name[15];
			char t_type[10];
			char t_arg[40];

			for(int b = 4;temp_prog[a][b] != ' '; ++b);
			b = b - 4;

			for(int c = 0; temp_prog[a][c+4] != ' ' ; ++c)
			{
				t_type[c] = temp_prog[a][c+4];

			}

			t_type[b] = '\0';
			int point_to = strlen(t_type) + 5;
			for(int g = point_to; temp_prog[a][g] != '('; ++g);
			//t_name = new char[g - point_to + 1];
			for(g = point_to; temp_prog[a][g] != '('; ++g)t_name[g-point_to] = temp_prog[a][g] ;
			t_name[g - point_to] = '\0';
			int point_to2 = strlen(t_type) + strlen(t_name) + 6;
			for(int g2 = point_to2; temp_prog[a][g2] != ')'; ++g2);
			//t_arg = new char[g2 - point_to2 + 1];
			for(g2 = point_to2; temp_prog[a][g2] != ')'; ++g2)t_arg[g2-point_to2] = temp_prog[a][g2];
			t_arg[g2 - point_to2] = '\0';
			for(int z = a; z < program->get_num_rows(); ++z)
			{
				if(strncmp("endsub", temp_prog[z], 6) == 0)
				{
					end = z;
					break;
				}
			}
			function_arr[index].function(t_type, t_name, t_arg, a, end);
			index++;
			//error checking^ index > no_of_functiuons
			#ifdef DEBUG_MODE
				cout << "\nNew function recorded: " << t_name << " at index " << index - 1;
				getch();
			#endif
		}
	}
}

function_table::function_table() //error checking: undefined end points for functions
{
  int num = 0;
  state = NOTOKAY;
  temp_prog = program->get_program();
  for(int z = 0; z < program->get_num_rows(); ++z)
  {
	if(strncmp("endsub", temp_prog[z], 6) == 0)num++;
  }
  no_of_functions = num;
  #ifdef DEBUG_MODE
	cout << "\nNo. Of Functions: "<< no_of_functions;
	getch();
  #endif
  if(no_of_functions>1)function_arr = new function_class[no_of_functions];
  state = OKAY;
  return_int = 0;
  return_float = 0.0;
  is_in_if_then_else = FALSE;
}

void* function_table::excute(int index, int num_args, char** args_list, void** arg_locs)
{

	#ifdef DEBUG_MODE
		{
			cout << "\nExcuting function " << function_arr[index].name;
			getch();
		}
	#endif


	function_arr[index].priv_vars.init(function_arr[index].start, function_arr[index].end, FUNCTION,num_args, args_list, arg_locs);


	char string2[150];
	int start_line = 0, endif_line = 0, endelse_line = 0;
	for(int exec_line = function_arr[index].start + 1; exec_line < function_arr[index].end; exec_line++)
	{
		//cout << "\nCurrently testing: " << exec_line << " " << temp_prog[exec_line];
		/*if(is_in_if_then_else == TRUE)
		{
		   if(exec_line == endelse_line || exec_line == endif_line)
		   {
			exec_line = endif_line;
			start_line = 0; endif_line = 0; endelse_line = 0;
			is_in_if_then_else = FALSE;
		   }
		}*/
		current_exec_line = exec_line + 1;//offsetting zero index
		if(strncmp("print", temp_prog[exec_line], 5) == 0) //error checked
		{
			int i = 6;
			int temp_i = 0;
			int len = 0;
			label:
			for(len = 0, temp_i = i; temp_prog[exec_line][temp_i] != ';' && temp_prog[exec_line][temp_i] != '\0'; len++, temp_i++);
			for(len = 0; temp_prog[exec_line][i] != ';' && temp_prog[exec_line][i] != '\0'; i++, len++)
			{
				string2[len] =  temp_prog[exec_line][i];
			}
			string2[len] = '\0';
			print(string2, index);
			string2[0] = '\0';
			if(temp_prog[exec_line][i] == ';'){i++; goto label;}
			//that's how print command chaining works^
		}
		else if(strncmp("input", temp_prog[exec_line], 5) == 0)
		{
			char string[40];
			//string = new char[strlen(temp_prog[exec_line])-5];
			for(int i = 6;temp_prog[exec_line][i] != '\0'; i++)
			{
				string[i-6] =  temp_prog[exec_line][i];
			}
			string[strlen(temp_prog[exec_line])-6] = '\0';
			input(string, index);
		}
		else if(strcmp("cls", temp_prog[exec_line]) == 0)
		{
			if(is_in_compile_mode == FALSE)cls();
		}
		else if(strcmp("sleep", temp_prog[exec_line]) == 0)
		{
			if(is_in_compile_mode == FALSE)sleep();
		}
		else if(strncmp("dim", temp_prog[exec_line], 3) == 0)
		{
			continue;
		}
		else if(strncmp("call", temp_prog[exec_line], 4) == 0)//see waht's happening
		{
			return_void(exec_line,index);
		}
		else if(strncmp("rem", temp_prog[exec_line], 3) == 0)
		{
			continue;
		}
		else if(strncmp("return", temp_prog[exec_line], 6) == 0)
		{
			char string[40];
			//string = new char[strlen(temp_prog[exec_line])-6];
			void* random = new void*;
			if(temp_prog[exec_line][6] == '\0')return random;
			for(int i = 7;temp_prog[exec_line][i] != '\0'; i++)
			{
				string[i-7] =  temp_prog[exec_line][i];
			}
			string[strlen(temp_prog[exec_line])-7] = '\0';
			void* return_variable_at;
			return_variable_at = return_value(string, index);
			return return_variable_at;
		}
		else if(strncmp("if", temp_prog[exec_line], 2) == 0)
		{
			char string[40];
			//string = new char[strlen(temp_prog[exec_line])];
			int len = 0;
			/*for(int i = 3;; i++, len++)
			{
				if(temp_prog[exec_line][i] == ' ' && temp_prog[exec_line][i+1] == 't'
				&& temp_prog[exec_line][i+2] == 'h' && temp_prog[exec_line][i+3] == 'e'
				&& temp_prog[exec_line][i+4] == 'n')break;
				string[len] =  temp_prog[exec_line][i];
			}*/
			for(int i = 3; temp_prog[exec_line][i] != '\0'; i++, len++)
			string[len] = temp_prog[exec_line][i];
			string[len-4] = '\0';
			//cout << "\nCompare string: " << string;
			//int start_line = 0, endif_line = 0, endelse_line = 0;
			start_line = exec_line;

			if_evaluate(string, index, exec_line, start_line, endif_line, endelse_line);

			if(is_in_if_then_else == TRUE)
			{
				//cout << "current exec: " << exec_line;
				exec_line = start_line;
				#ifdef DEBUG_MODE
					cout << "\nC++, if asked to start at exec: " << exec_line;
					cout << "\nand end at: " << endelse_line << " and get out at " << endif_line;
				#endif
			}
		}
		else if(strncmp("endif", temp_prog[exec_line], 5) == 0)
		{
			if(is_in_if_then_else == TRUE)
			{
			   if(exec_line == endelse_line || exec_line == endif_line)
			   {
				exec_line = endif_line;
				start_line = 0; endif_line = 0; endelse_line = 0;
				is_in_if_then_else = FALSE;
			   }
			}
		}
		else if(strncmp("else", temp_prog[exec_line], 4) == 0)
		{
			 if(is_in_if_then_else == TRUE)
			{
			   if(exec_line == endelse_line || exec_line == endif_line)
			   {
				exec_line = endif_line;
				start_line = 0; endif_line = 0; endelse_line = 0;
				is_in_if_then_else = FALSE;
			   }
			}
		}
		else if(strncmp("for", temp_prog[exec_line], 3) == 0)
		{
			//record the intial and final values and step size
			//for intialize
			for_intialize(temp_prog[exec_line], index);
		}
		else if(strncmp("next", temp_prog[exec_line], 4) == 0)
		{
			int temp_e_line = exec_line;
			next_step(temp_prog[exec_line], index, exec_line);
			if(is_in_compile_mode)
			{
				exec_line = temp_e_line;
			}
			//incriment the variable that follows by step size
			//recorded previosly
			//go up by-- and then incriment and check to see if excedded
		}
		else if(strncmp("open canvas", temp_prog[exec_line], 11) == 0)
		{
			if(is_in_compile_mode == FALSE)graphics();
			global_x = getmaxx()/2;
			global_y = getmaxy()/2;
		}
		else if(strncmp("close canvas", temp_prog[exec_line], 12) == 0)
		{
			if(is_in_compile_mode == FALSE)closegraph();
		}
		else if(strncmp("clear canvas", temp_prog[exec_line], 12) == 0)
		{
			if(is_in_compile_mode == FALSE)cleardevice();
		}
		else if(strncmp("draw", temp_prog[exec_line], 4) == 0)
		{
			//draw "u40d40"
			char command, digit[4];
			int num;
			for(int i = 6; temp_prog[exec_line][i] != '\"';)
			{
				command = temp_prog[exec_line][i];
				++i;
				for(int len = 0;isdigit(temp_prog[exec_line][i]);++i, len++)
				{
					digit[len] = temp_prog[exec_line][i];
				}
				digit[len] = '\0';
				num = atoi(digit);
				#ifdef DEBUG_MODE
					cout << "\nDrawing: " << command << " " << num;
				#endif
				if(is_in_compile_mode == FALSE)	draw(command,num);
			 }
		}
		else if(strncmp("delay", temp_prog[exec_line], 5) == 0)
		{
			//delay 10
			char digit[4];
			int num;
			for(int i = 6, len = 0; temp_prog[exec_line][i] != '\0' && temp_prog[exec_line][i] != '\n'; ++i, len++)
			 {
				digit[len] = temp_prog[exec_line][i];
			 }
			 digit[len] = '\0';
			 num = atoi(digit);
			#ifdef DEBUG_MODE
				cout << "\nDelaying: " << num;
			#endif
			delay(num*50);
		}
		else if(strncmp("color", temp_prog[exec_line], 5) == 0)
		{
			//color 4
			set_color(temp_prog[exec_line], index);
			//only 15 color avail in C++
		}
		else if(strncmp("bkcolor", temp_prog[exec_line], 7) == 0)
		{
			//bkcolor 4
			set_bkcolor(temp_prog[exec_line], index);
		}
		else if(strncmp("circle", temp_prog[exec_line], 6) == 0)
		{
			//color 4
			draw_circle(temp_prog[exec_line], index);
			//only 15 color avail in C++
		}
		else if(strncmp("pset", temp_prog[exec_line], 4) == 0)
		{
			//pset(x,y)
			pset(temp_prog[exec_line], index);
		}
		else if(check_if_define(exec_line, index) == TRUE)
		{

		     define(exec_line, index);
		}
		else
		{
			if(temp_prog[exec_line][0] != '\0' && temp_prog[exec_line][0] != ' ') error_handle << "\nline " << current_exec_line << ": undefined symbol " << temp_prog[exec_line];
			continue;
		}
	}
	void* random = new void*;
	return random;
}

void function_table::set_color(char string[], int index)
{
	//color x
	char digit[15];
	int num;
	for(int i = 6, len = 0; string[i] != '\0' && string[i] != '\n'; ++i, len++)
	{
		digit[len] = string[i];
	}
	 digit[len] = '\0';
	int validate = FALSE;
	entery variable_holder_to = function_arr[index].priv_vars.get_variable(digit, validate);

	if(validate == 0 && atoi(digit) == 0 && is_in_compile_mode == TRUE)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << digit;
	}
	else if(validate == 0 && atoi(digit) !=0)
	{
		num = atoi(digit);
	}
	else
	{
		void* variable_location_to = variable_holder_to.get_loc();
		char var_type_to[10];// = new char[strlen(variable_holder_to.get_type())+1];
		strcpy(var_type_to, variable_holder_to.get_type());
		void* vari = new void*;
		if(strcmpi(var_type_to,"int") == 0)
		{
			vari = (variable<int>*)variable_location_to;
			num = ((variable<int>*)vari)->get_value();
		}
		else if(strcmpi(var_type_to,"string") == 0)
		{
			//generate error
		}
		else if(strcmpi(var_type_to,"float") == 0)
		{
			vari = (variable<float>*)variable_location_to;
			num = ((variable<float>*)vari)->get_value();
		}

	}
	#ifdef DEBUG_MODE
		cout << "\nSEtting color to: " << num;
	#endif
	if(is_in_compile_mode == FALSE)setcolor(num%16);
}

void function_table::set_bkcolor(char string[], int index)
{
	//bkcolor x
	char digit[15];
	int num;
	for(int i = 8, len = 0; string[i] != '\0' && string[i] != '\n'; ++i, len++)
	{
		digit[len] = string[i];
	}
	 digit[len] = '\0';
	int validate = FALSE;
	entery variable_holder_to = function_arr[index].priv_vars.get_variable(digit, validate);

	if(validate == 0 && atoi(digit) == 0 && is_in_compile_mode == TRUE)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << digit;
	}
	else if(validate == 0 && atoi(digit) !=0)
	{
		num = atoi(digit);
	}
	else
	{
		void* variable_location_to = variable_holder_to.get_loc();
		char var_type_to[10];// = new char[strlen(variable_holder_to.get_type())+1];
		strcpy(var_type_to, variable_holder_to.get_type());
		void* vari = new void*;
		if(strcmpi(var_type_to,"int") == 0)
		{
			vari = (variable<int>*)variable_location_to;
			num = ((variable<int>*)vari)->get_value();
		}
		else if(strcmpi(var_type_to,"string") == 0)
		{
			//generate error
		}
		else if(strcmpi(var_type_to,"float") == 0)
		{
			vari = (variable<float>*)variable_location_to;
			num = ((variable<float>*)vari)->get_value();
		}

	}
	#ifdef DEBUG_MODE
		cout << "\nSEtting bkcolor to: " << num;
	#endif
	if(is_in_compile_mode == FALSE)setbkcolor(num%15);
}

void function_table::pset(char string[], int index)
{
	//pset(x,y)
	char digit[15];
	int num_x;
	for(int i = 5, len = 0; string[i] != ','; ++i, len++)
	{
		digit[len] = string[i];
	}
	 digit[len] = '\0';
	int validate = FALSE;
	entery variable_holder_to = function_arr[index].priv_vars.get_variable(digit, validate);

	if(validate == 0 && atoi(digit) == 0 && is_in_compile_mode == TRUE)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << digit;
	}
	else if(validate == 0 && atoi(digit) !=0)
	{
		num_x = atoi(digit);
	}
	else
	{
		void* variable_location_to = variable_holder_to.get_loc();
		char var_type_to[10];// = new char[strlen(variable_holder_to.get_type())+1];
		strcpy(var_type_to, variable_holder_to.get_type());
		void* vari = new void*;
		if(strcmpi(var_type_to,"int") == 0)
		{
			vari = (variable<int>*)variable_location_to;
			num_x = ((variable<int>*)vari)->get_value();
		}
		else if(strcmpi(var_type_to,"string") == 0)
		{
			//generate error
		}
		else if(strcmpi(var_type_to,"float") == 0)
		{
			vari = (variable<float>*)variable_location_to;
			num_x = ((variable<float>*)vari)->get_value();
		}

	}
	#ifdef DEBUG_MODE
		cout << "\nSEtting graphical x to: " << num_x;
	#endif

	if(is_in_compile_mode == FALSE) global_x = num_x;

	int num_y;
	for(len = 0, i++; string[i] != ')'; ++i, len++)
	{
		digit[len] = string[i];
	}
	 digit[len] = '\0';

	variable_holder_to = function_arr[index].priv_vars.get_variable(digit, validate);

	if(validate == 0 && atoi(digit) == 0 && is_in_compile_mode == TRUE)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << digit;
	}
	else if(validate == 0 && atoi(digit) !=0)
	{
		num_y = atoi(digit);
	}
	else
	{
		void* variable_location_to = variable_holder_to.get_loc();
		char var_type_to[10];// = new char[strlen(variable_holder_to.get_type())+1];
		strcpy(var_type_to, variable_holder_to.get_type());
		void* vari = new void*;
		if(strcmpi(var_type_to,"int") == 0)
		{
			vari = (variable<int>*)variable_location_to;
			num_y = ((variable<int>*)vari)->get_value();
		}
		else if(strcmpi(var_type_to,"string") == 0)
		{
			//generate error
		}
		else if(strcmpi(var_type_to,"float") == 0)
		{
			vari = (variable<float>*)variable_location_to;
			num_y = ((variable<float>*)vari)->get_value();
		}

	}
	#ifdef DEBUG_MODE
		cout << "\nSEtting graphical y to: " << num_y;
	#endif

	if(is_in_compile_mode == FALSE) global_y = num_y;

}

void function_table::draw_circle(char string[], int index)
{
	//circle(x,y,r)
	char digit[15];
	int num_x;
	for(int i = 7, len = 0; string[i] != ','; ++i, len++)
	{
		digit[len] = string[i];
	}
	 digit[len] = '\0';
	int validate = FALSE;
	entery variable_holder_to = function_arr[index].priv_vars.get_variable(digit, validate);

	if(validate == 0 && atoi(digit) == 0 && is_in_compile_mode == TRUE)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << digit;
	}
	else if(validate == 0 && atoi(digit) !=0)
	{
		num_x = atoi(digit);
	}
	else
	{
		void* variable_location_to = variable_holder_to.get_loc();
		char var_type_to[10];// = new char[strlen(variable_holder_to.get_type())+1];
		strcpy(var_type_to, variable_holder_to.get_type());
		void* vari = new void*;
		if(strcmpi(var_type_to,"int") == 0)
		{
			vari = (variable<int>*)variable_location_to;
			num_x = ((variable<int>*)vari)->get_value();
		}
		else if(strcmpi(var_type_to,"string") == 0)
		{
			//generate error
		}
		else if(strcmpi(var_type_to,"float") == 0)
		{
			vari = (variable<float>*)variable_location_to;
			num_x = ((variable<float>*)vari)->get_value();
		}

	}
	#ifdef DEBUG_MODE
		cout << "\nSEtting circle x to: " << num_x;
	#endif

	int num_y;
	for(len = 0, i++; string[i] != ','; ++i, len++)
	{
		digit[len] = string[i];
	}
	 digit[len] = '\0';

	variable_holder_to = function_arr[index].priv_vars.get_variable(digit, validate);

	if(validate == 0 && atoi(digit) == 0 && is_in_compile_mode == TRUE)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << digit;
	}
	else if(validate == 0 && atoi(digit) !=0)
	{
		num_y = atoi(digit);
	}
	else
	{
		void* variable_location_to = variable_holder_to.get_loc();
		char var_type_to[10];// = new char[strlen(variable_holder_to.get_type())+1];
		strcpy(var_type_to, variable_holder_to.get_type());
		void* vari = new void*;
		if(strcmpi(var_type_to,"int") == 0)
		{
			vari = (variable<int>*)variable_location_to;
			num_y = ((variable<int>*)vari)->get_value();
		}
		else if(strcmpi(var_type_to,"string") == 0)
		{
			//generate error
		}
		else if(strcmpi(var_type_to,"float") == 0)
		{
			vari = (variable<float>*)variable_location_to;
			num_y = ((variable<float>*)vari)->get_value();
		}

	}

	int num_r;
	for(len = 0, i++; string[i] != ')'; ++i, len++)
	{
		digit[len] = string[i];
	}
	 digit[len] = '\0';

	variable_holder_to = function_arr[index].priv_vars.get_variable(digit, validate);

	if(validate == 0 && atoi(digit) == 0 && is_in_compile_mode == TRUE)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << digit;
	}
	else if(validate == 0 && atoi(digit) !=0)
	{
		num_r = atoi(digit);
	}
	else
	{
		void* variable_location_to = variable_holder_to.get_loc();
		char var_type_to[10];// = new char[strlen(variable_holder_to.get_type())+1];
		strcpy(var_type_to, variable_holder_to.get_type());
		void* vari = new void*;
		if(strcmpi(var_type_to,"int") == 0)
		{
			vari = (variable<int>*)variable_location_to;
			num_r = ((variable<int>*)vari)->get_value();
		}
		else if(strcmpi(var_type_to,"string") == 0)
		{
			//generate error
		}
		else if(strcmpi(var_type_to,"float") == 0)
		{
			vari = (variable<float>*)variable_location_to;
			num_r = ((variable<float>*)vari)->get_value();
		}

	}
	#ifdef DEBUG_MODE
		cout << "\nSEtting circle r to: " << num_r;
	#endif

	if(is_in_compile_mode == FALSE)circle(num_x, num_y, num_r);
}

char* function_table::call(char* f_name_arg, int num_args, int calling_index, int line, void* &return_here)
{


	void** arg_locs;
	char** args_type_list;

	int a_index = 0;
	//construct arg list here and take the calling fucntion as argument
	//assuming we have correct num_args (check if define se)
	if(calling_index == -1)
	{
		arg_locs = new void*[1];
		args_type_list = new char*[1];
	}

	if(calling_index != -1)
	{
		if(num_args != 0)
		{
			args_type_list = new char*[num_args];
			for(int def = 0; def < num_args; def++)
			{
				args_type_list[def] = new char[15];
				strcpy(args_type_list[def], "Not changed");
			}
			arg_locs = new void*[num_args];
		}
		else
		{

		}

		char** temp_prog = program->get_program();
		char t_name[20];
		int a, z = 0;
		for(z = 0; temp_prog[line][z] != '('; z++);
		z++;
		int temp = z; //baar baar reset karne ke liye loop ke anadr
		int len = 0;
		int sub_index = 0;
		int first_arg = TRUE;

		for(int q = 0; a_index <= num_args; q++)
		{


			for(len = 0;temp_prog[line][temp] != ')' && temp_prog[line][temp] != ','; temp++, len++)
			t_name[len] = temp_prog[line][temp];
			t_name[len] = '\0';
			if(temp_prog[line][temp] != ')')temp++;
			//offsetting for comma

			int arr_valid = FALSE;
			function_arr[calling_index].priv_vars.is_array(t_name, arr_valid);
			if(arr_valid == FALSE)
			{
				int valid = FALSE;
				int temp_index;
				void* t_point = function_arr[calling_index].priv_vars.get_var_loc(t_name, valid);
			       if (valid == TRUE)
			       {
					temp_index = function_arr[calling_index].priv_vars.get_index(t_name);
					char temp_type[20];
					strcpy(temp_type,function_arr[calling_index].priv_vars.get_type_at_index(temp_index));
					arg_locs[a_index] =  t_point;
					strcpy(args_type_list[a_index],temp_type);
					a_index++;
					if(a_index >= num_args) break;
				}
			}
			else if (arr_valid == TRUE)
			{
				int l, len, int_sz_arr = 0;
				int temp_line = line;
				char temp_name[20];
				char size_array[10];
				char comp_string[30];
				comp_string[0] = '\0';
				//new:
				for(; ;temp_line--)
				 {

					if(int_sz_arr != 0) break;

					if (strncmp("sub",temp_prog[temp_line],3) == 0)
					{
						//sub void function(dim l int, dim x[5] int)
						if(first_arg == TRUE)
						{
							for(sub_index = 0; temp_prog[temp_line][sub_index] != '('; sub_index++);
							first_arg = FALSE;
						}
						int copy_index;
						//do the below in a loop

						for(copy_index = 0, sub_index++; temp_prog[temp_line][sub_index] != ',' && temp_prog[temp_line][sub_index] != ')'; copy_index++, sub_index++)
						{
							comp_string[copy_index] = temp_prog[temp_line][sub_index];
						}
						comp_string[copy_index] = '\0';
						//now check if comp_string is valid or not
					}
					else if(strncmp("sub",temp_prog[temp_line+1],3) == 0)
					{
						error_handle << "\nline " << current_exec_line << ": array defintion not foud";
						//generate error (var defineing not found)
					}
					else if (strncmp("dim",temp_prog[temp_line],3) == 0)
					{
						strcpy(comp_string, temp_prog[temp_line]);
					}
					//we construct the string to be compared above and check it with the code below
					if(strncmp("dim",comp_string,3) == 0)
					{
						for(l = 4; comp_string[l] != '[' && comp_string[l] != ' '; l++)
						{
							temp_name[l-4] = comp_string[l];
						}
						temp_name[l-4] = '\0';

						if(strcmp(t_name, temp_name) == 0)
						{
																		//cout << "\nTemp name: " << temp_name << "comparing with" << t_name;
							for(len = 0, l++; comp_string[l] != ']'; l++, len++)
							{
								size_array[len] = comp_string[l];
							}
							size_array[len] = '\0';
							int_sz_arr  = atoi(size_array);
						}
					}
				}

				//got the size of the array now expand the string
				void* cur_point;
				int is_it_valid = FALSE;
				for(int aa = 1; aa <= int_sz_arr;aa++)
				{
					char new_name[25], char_dig[10];
					strcpy(new_name, t_name);
					strcat(new_name, "[");
					itoa(aa, char_dig, 10);
					strcat(new_name, char_dig);
					strcat(new_name, "]");
					strcpy(args_type_list[a_index],
					function_arr[calling_index].priv_vars.get_type_at_index(function_arr[calling_index].priv_vars.get_index(new_name)));
					cur_point = function_arr[calling_index].priv_vars.get_var_loc(new_name, is_it_valid);
					arg_locs[a_index] = cur_point;
					a_index++;
					if(is_it_valid == FALSE)
					{
						//in case I really mess up somewhere during runtime
						error_handle << "line " << current_exec_line << ": linker error, relaunch to continue";
					}
					if(a_index >= num_args)break;
				}
				if(a_index >= num_args)break;
			}

			if(a_index >= num_args)break;
			//z = z + strlen(t_name) +1;
		}
	}

	char ret_type[10];

	int found = FALSE;
	for(int a = 0; a < no_of_functions; ++a)
	{
	     #ifdef DEBUG_MODE
		cout << "\nmaybe it is: " <<  function_arr[a].get_name() << " at index " << a;
		getch();

	     #endif
		if(strcmp(f_name_arg, function_arr[a].get_name()) == 0)
		  {
			#ifdef DEBUG_MODE
			    cout << "\nYes it is: " <<  function_arr[a].get_name();
			#endif
			found = TRUE;
			//ret_type = new char[strlen(function_arr[a].get_return_type())+2];
			strcpy(ret_type, function_arr[a].get_return_type());

			return_here = excute(a, num_args, args_type_list, arg_locs);

			//function_arr[a].priv_vars.refresh();
			//if(calling_index != -1)
			{
				function_arr[a].priv_vars.pop_table();
				#ifdef DEBUG_MODE
					cout << "\nPopped from " << function_arr[a].get_name();
				#endif
				int use = FALSE;
				entery* temp = function_arr[a].priv_vars.get_current(use);
				if(use != FALSE)function_arr[a].priv_vars.set_current(temp);

			}

			break;
			//catch returns here rather than in the main
		  }
	}

	if(found != TRUE)//function not found
	{
		error_handle << "\nline " << current_exec_line << ": function " << f_name_arg << " should have a prototype";
	}
	void* random = new void*;
	if(calling_index == -1)return_here = random;
	if(calling_index != -1)
	{
		delete[] args_type_list;
		delete[] arg_locs;
	}

	return ret_type; //will return void in case of main
}

void function_table::next_step(char str[], int index, int &exec_line)
{
	char next_var_name[20];
	int len, i;
	int temp_exec_line = exec_line;
	for(len = 0, i = 5; str[i] != '\0' && str[i] != ' '; i++, len++)
	{
		next_var_name[len] = str[i];
	}
	next_var_name[len] = '\0';
	for(;;temp_exec_line--)
	{
		if(strncmp("for", temp_prog[temp_exec_line], 3) == 0)
		{
			char var_name[20];
			for(i = 4, len = 0; temp_prog[temp_exec_line][i] != ' ' && temp_prog[temp_exec_line][i+1] != '='; len++,i++)
			{
				var_name[len] = temp_prog[temp_exec_line][i];
			}
			var_name[len] = '\0';
			if(strcmp(var_name, next_var_name) == 0)
			{
				break;
			}
		}
	}

	float from = 0, to = 0, step = 0;
	char from_c[10], to_c[10], step_c[10];
	for(len = 0, i+= 3;temp_prog[temp_exec_line][i] != ' ' && temp_prog[temp_exec_line][i+1] != 't' && temp_prog[temp_exec_line][i+2] != 'o'; len++, i++)
	{
		from_c[len] = temp_prog[temp_exec_line][i];
	}
	from_c[len] = '\0';
	for(len = 0, i += 4; temp_prog[temp_exec_line][i] != ' ' && temp_prog[temp_exec_line][i+1] != 's' && temp_prog[temp_exec_line][i+2] != 't' && temp_prog[temp_exec_line][i+3] != 'e' && temp_prog[temp_exec_line][i+4] != 'p'; i++,len++)
	{
		to_c[len] = temp_prog[temp_exec_line][i];
	}
	to_c[len] = '\0';
	for(len= 0, i+= 6; temp_prog[temp_exec_line][i] != '\0' && temp_prog[temp_exec_line][i] != ' '; len++, i++)
	{
		step_c[len] = temp_prog[temp_exec_line][i];
	}
	step_c[len] = '\0';
	from = atof(from_c);
	to = atof(to_c);
	step = atof(step_c);
	void* vari;

	int validate = 0;
	//from check karna useless hai, sirf step and to check karo
	if(atof(to_c) == 0)
	{
	entery variable_holder_to = function_arr[index].priv_vars.get_variable(to_c, validate);

	if(validate == 0)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << to_c;
	}
	else
	{
		void* variable_location_to = variable_holder_to.get_loc();
		char var_type_to[10];// = new char[strlen(variable_holder_to.get_type())+1];
		strcpy(var_type_to, variable_holder_to.get_type());
		if(strcmpi(var_type_to,"int") == 0)
		{
			vari = (variable<int>*)variable_location_to;
			to = ((variable<int>*)vari)->get_value();
		}
		if(strcmpi(var_type_to,"string") == 0)
		{
			//generate error
		}
		if(strcmpi(var_type_to,"float") == 0)
		{
			vari = (variable<float>*)variable_location_to;
			to = ((variable<float>*)vari)->get_value();
		}

	}
	}
	else
	{
		to = atof(to_c);
	}

	if(atof(step_c) == 0)
	{
	entery variable_holder_step = function_arr[index].priv_vars.get_variable(step_c, validate);

	if(validate == 0)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << step_c;
	}
	else
	{
		void* variable_location_step = variable_holder_step.get_loc();
		char var_type_step[10];// = new char[strlen(variable_holder_step.get_type())+1];
		strcpy(var_type_step, variable_holder_step.get_type());
		if(strcmpi(var_type_step,"int") == 0)
		{
			vari = (variable<int>*)variable_location_step;
			step = ((variable<int>*)vari)->get_value();
		}
		if(strcmpi(var_type_step,"string") == 0)
		{
			//generate error
		}
		if(strcmpi(var_type_step,"float") == 0)
		{
			vari = (variable<float>*)variable_location_step;
			step = ((variable<float>*)vari)->get_value();
		}

	}
	}
	else
	{
		step = atof(step_c);
	}

	if(from > 0){}//removing a warning as I may need the variables in future versions
	if(to > 0){}//same as above
	if(step > 0){}//same as above

	entery variable_holder = function_arr[index].priv_vars.get_variable(next_var_name, validate);

	if(validate == 0)
	{
		#ifdef DEBUG_MODE
			cout << "\nFor incriment ABORT";
			cout << "\nFrom: " << from << " to: " << to << " step: " << step;
		#endif
		//getch();
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << next_var_name;
		//generate error
	}

	void* variable_location = variable_holder.get_loc();
	char* var_type = new char[strlen(variable_holder.get_type())+1];
	strcpy(var_type, variable_holder.get_type());
	float data;
	if(strcmpi(var_type,"int") == 0)
	{
		vari = (variable<int>*)variable_location;
		data = ((variable<int>*)vari)->get_value();
		data += step;
		((variable<int>*)vari)->set_value((int)data);
	}
	if(strcmpi(var_type,"string") == 0)
	{
		//generate error
	}
	if(strcmpi(var_type,"float") == 0)
	{
		vari = (variable<float>*)variable_location;
		data = ((variable<float>*)vari)->get_value();
		data += step;
		((variable<float>*)vari)->set_value(data);
	}

	if(data > to) return;
	else exec_line = temp_exec_line;
}

void function_table::for_intialize(char str[], int index)
{
	char var_name[20];
	int len = 0,i;
	for(i = 4, len = 0; str[i] != ' ' && str[i+1] != '='; len++,i++)
	{
		var_name[len] = str[i];
	}
	var_name[len] = '\0';
	float from = 0, to = 0, step = 0;
	char from_c[10], to_c[10], step_c[10];
	for(len = 0, i+= 3;str[i] != ' ' && str[i+1] != 't' && str[i+2] != 'o'; len++, i++)
	{
		from_c[len] = str[i];
	}
	from_c[len] = '\0';
	for(len = 0, i += 4; str[i] != ' ' && str[i+1] != 's' && str[i+2] != 't' && str[i+3] != 'e' && str[i+4] != 'p'; i++,len++)
	{
		to_c[len] = str[i];
	}
	to_c[len] = '\0';
	for(len= 0, i+= 6; str[i] != '\0' && str[i] != ' '; len++, i++)
	{
		step_c[len] = str[i];
	}
	step_c[len] = '\0';

	//make these return variables :3
	//atof returns 0 if the string is not convertible to a float
	//so, check if it returned zero if yes then go on to check with a varibale
	//if still gives a problem, user ki galti.
	from = atof(from_c);
	to = atof(to_c);
	step = atof(step_c);
	void* vari;


	int for_valid = FALSE;
	if(atof(from_c) == 0)
	{
	entery variable_holder_from = function_arr[index].priv_vars.get_variable(from_c, for_valid);

	if(for_valid == 0)
	{
		//cout << "\nFor intialization ABORTED because variables are not playing nice";
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << from_c;
	}
	else
	{
		void* variable_location_from = variable_holder_from.get_loc();
		char var_type_from[10];// = new char[strlen(variable_holder_from.get_type())+1];
		strcpy(var_type_from, variable_holder_from.get_type());
		if(strcmpi(var_type_from,"int") == 0)
		{
			vari = (variable<int>*)variable_location_from;
			from = ((variable<int>*)vari)->get_value();
		}
		if(strcmpi(var_type_from,"string") == 0)
		{
			error_handle << "\nline " << current_exec_line << ": variables of string type invalid here ";
		}
		if(strcmpi(var_type_from,"float") == 0)
		{
			vari = (variable<float>*)variable_location_from;
			from = ((variable<float>*)vari)->get_value();
		}
		//delete[] var_type_from;
	}
	}
	else
	{
		from = atof(from_c);
	}

	if(from > 0){}//removing a warning as I may need the variables in future versions
	if(to > 0){}//same as above
	if(step > 0){}//same as above


	//useless to make functions for getting numerical value on the current implimentation
	//of to and step variables. I'll make them in the next_step function
	int validate = FALSE;
	entery variable_holder = function_arr[index].priv_vars.get_variable(var_name, validate);

	#ifdef DEBUG_MODE
		cout << "\nFor being intialized by the follwong: " << var_name << " " << from << " " << to << " " << step;
	#endif

	if(validate == 0)
	{
		//cout << "\nFor intialization ABORT " << var_name;
		//cout << "\nFrom: " << from << " to: " << to << " step: " << step;
		//getch();
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << var_name;
		return;
		//generate error

	}

	void* variable_location = variable_holder.get_loc();
	char* var_type = new char[strlen(variable_holder.get_type())+1];
	strcpy(var_type, variable_holder.get_type());
	//void* vari;  decleared above

	if(strcmpi(var_type,"int") == 0)
	{
		vari = (variable<int>*)variable_location;
		((variable<int>*)vari)->set_value((int)from);
	}
	if(strcmpi(var_type,"string") == 0)
	{
		//generate error
	}
	if(strcmpi(var_type,"float") == 0)
	{
		vari = (variable<float>*)variable_location;
		((variable<float>*)vari)->set_value(from);
	}
}

int function_table::actual_bool(char var1[], char oper[], char var2[], int index)
{
	int validate1 = 0, validate2 = 0;

	int return_value = FALSE;

	entery variable_holder1 = function_arr[index].priv_vars.get_variable(var1, validate1);
	entery variable_holder2 = function_arr[index].priv_vars.get_variable(var2, validate2);

	float data1 = 0, data2 = 0;

	if(validate1 == 0)
	{
		if(atof(var1) != 0)data1 = atof(var1);
		else
		{
			error_handle << "\nline " << current_exec_line << ": undefined variable " << var1;
		}
	}
	else
	{
		void* variable_location1 = variable_holder1.get_loc();
		char var_type1[10];// = new char[strlen(variable_holder1.get_type())+1];
		strcpy(var_type1, variable_holder1.get_type());
		void* vari1;
		if(strcmpi(var_type1,"int") == 0)
		{
			vari1 = (variable<int>*)variable_location1;
			data1 = ((variable<int>*)vari1)->get_value();
		}
		else if(strcmpi(var_type1,"float") == 0)
		{
			vari1 = (variable<float>*)variable_location1;
			data1 = ((variable<float>*)vari1)->get_value();
		}
		else if(strcmpi(var_type1, "string") == 0)
		{
			//generate some kind of error
		}
		//delete[] var_type1;
	}

	if(validate2 == 0)
	{
		if(atof(var2) != 0)data2 = atof(var2);
		else
		{
			error_handle << "\nline " << current_exec_line << ": undefined variable " << var2;
		}
	}
	else
	{
		void* variable_location2 = variable_holder2.get_loc();
		char var_type2[10];// = new char[strlen(variable_holder2.get_type())+1];
		strcpy(var_type2, variable_holder2.get_type());
		void* vari2;
		if(strcmpi(var_type2,"int") == 0)
		{
			vari2 = (variable<int>*)variable_location2;
			data2 = ((variable<int>*)vari2)->get_value();
		}
		else if(strcmpi(var_type2,"float") == 0)
		{
			vari2 = (variable<float>*)variable_location2;
			data2 = ((variable<float>*)vari2)->get_value();
		}
		else if(strcmpi(var_type2, "string") == 0)
		{
			//generate some kind of error
		}

	}
	#ifdef DEBUG_MODE
		cout << "\nnow applying the logical operator on " << data1 << " "  << data2;
		getch();
	#endif
	if(strcmp(oper,"<")==0)
	{
		if(data1 < data2)return_value = TRUE;
	}
	else if(strcmp(oper,"<=")==0)
	{
		if(data1 <= data2)return_value = TRUE;
	}
	else if(strcmp(oper,"=<")==0)
	{
		if(data1 <= data2)return_value = TRUE;
	}
	else if(strcmp(oper,">")==0)
	{
		if(data1 > data2)return_value = TRUE;
	}
	else if(strcmp(oper,">=")==0)
	{
		if(data1 >= data2)return_value = TRUE;
	}
	else if(strcmp(oper,"=>")==0)
	{
		if(data1 >= data2)return_value = TRUE;
	}
	else if(strcmp(oper,"=")==0)
	{
		if(data1 == data2)return_value = TRUE;
	}
	else if(strcmp(oper,"<>")==0)
	{
		if(data1 != data2)return_value = TRUE;
	}
	#ifdef DEBUG_MODE
		cout << "\nExperssion of "<< data1 << " and " << data2 << " evaluated to: " << return_value;
		getch();
	#endif
	return return_value;
}

int function_table::evaluate_bool_expression(const char str2[], int index, int exec_line)
{

	char str[50];// = new char[strlen(str2) + 2];
	strcpy(str, str2);

	if(temp_prog[exec_line][0] == 'i' && temp_prog[exec_line][1] == 'f') str[strlen(str2)-1] = '\0';
	else str[strlen(str2)] = '\0';

	#define AND 1
	#define OR 2

	int num_opers = 0;

	for(int z = 1; str[z] != '\0'; z++)
	{
		if(str[z-1] == '&' && str[z] == '&')num_opers++;
		if(str[z-1] == '|' && str[z] == '|')num_opers++;
	}

	int bool_arr[20];
	int oper_arr[20];

	char var1[20];
	char oper[3];
	char var2[20];
	char logical_oper[3];
	int len, i, index_int = 0;
	int str_len = strlen(str);
	int ret;

	#ifdef DEBUG_MODE
		cout << "\nBoolean string to evaulate: " <<str;
		getch();
	#endif
	for(i = 0;str[i] != '\0'; i++, index_int++)//evaluating by sets
	{
		if(i+1 >= str_len)break;
		for(len = 0;str[i] != ' '; len++,i++)var1[len] = str[i];
		var1[len] = '\0';
		for(i++,len = 0;str[i] != ' '; len++,i++)oper[len] = str[i];
		oper[len] = '\0';
		if(num_opers != 0)
		{
			for(i++,len = 0;str[i] != ' ' && str[i] != '\0'; len++,i++)
			{
				var2[len] = str[i];
			}
		}
		else
		{

			for(i++,len = 0;str[i] != ' ' && str[i] != '\0'; len++,i++)
			{
				var2[len] = str[i];
			}
		}
		var2[len] = '\0';
		if(str[i] != '\0' && (i+4) < strlen(str) && num_opers != 0)
		{
			for(i++,len = 0;str[i] != ' '; len++,i++)logical_oper[len] = str[i];
			logical_oper[len] = '\0';
			if(strcmp(logical_oper, "&&") == 0)
			{
				oper_arr[index_int] = AND;
			}
			if(strcmp(logical_oper, "||") == 0)
			{
				oper_arr[index_int] = OR;
			}
		}
		#ifdef DEBUG_MODE
			cout << "\nBoolean batch recorded: " << var1 << " " << oper << " " << var2;
			getch();
		#endif

		bool_arr[index_int] = actual_bool(var1, oper, var2, index);
		if(num_opers == 0)
		{
			ret = bool_arr[0];
			break;
		}
	}
	//evaluate the oper_arr and bool_arr
	if(num_opers != 0)
	{
		for(int q = 0; q < (index_int-1); q++)
		{
			#ifdef DEBUG_MODE
				cout << "Evaluation stack:\nBool Arr: ";
				int w1;
				for(w1 = 0; w1 < index_int; w1++)cout << bool_arr[w1];
				cout << "\nOperator Arr: ";
				for(w1 = 0; w1 < index_int-1; w1++)cout << oper_arr[w1];
				getch();
			#endif
			if(oper_arr[q] == AND)
			{
				if(bool_arr[q] == TRUE && bool_arr[q+1] == TRUE)
				{
					bool_arr[q+1] = TRUE;
				}
				else
				{
					bool_arr[q+1] = FALSE;
				}
			}
			else if(oper_arr[q] == OR)
			{
				if(bool_arr[q] == TRUE || bool_arr[q+1] == TRUE)
				{
					bool_arr[q+1] = TRUE;
				}
				else
				{
					bool_arr[q+1] = FALSE;
				}
			}
		}
		ret = bool_arr[q];
	}
	else
	{
		ret = bool_arr[0];
	}


	return ret;
}

void function_table::if_evaluate(char str[], int index, int exec_line, int &start_line, int &endif_line, int &endelse_line)
{
	//return the number start and end to execute and the endline

	int temp_exec_line = exec_line;
	endif_line = exec_line; //you'll have to return this so that the compiler can jump
	for(;;endif_line++)  //checiking for end if ERROR reporting baaki
	{
		if(strncmp("endif", temp_prog[endif_line], 5) == 0)
		{
			endif_line++;
			break;
		}
	}

	int truth = evaluate_bool_expression(str, index, exec_line);

	check:

	if(truth == TRUE)
	{
		start_line = temp_exec_line;
		endelse_line = start_line + 1;
		for(;;endelse_line++)  //checiking for end if ERROR reporting baaki
		{
			if(strncmp("elseif", temp_prog[endelse_line], 6) == 0)
			{
				is_in_if_then_else = TRUE;
				break;
			}
			else if(strncmp("else", temp_prog[endelse_line], 4) == 0)
			{
				is_in_if_then_else = TRUE;
				break;
			}
			else if(strncmp("endif", temp_prog[endelse_line], 5) == 0)
			{
				is_in_if_then_else = TRUE;
				break;
			}
		}
	}
	else if(truth == FALSE)
	{
	     temp_exec_line = start_line + 1;
	     #define ELSEIF 1
	     #define ELSE 2
	     int mode = 0;
	     for(;;temp_exec_line++)
	     {
			if(strncmp("elseif ", temp_prog[temp_exec_line], 7) == 0)
			{
				mode = ELSEIF;
				start_line = temp_exec_line;
				break;
			}
			else if(strncmp("else", temp_prog[temp_exec_line], 4) == 0)
			{
				mode = ELSE;
				start_line = temp_exec_line;
				break;
			}
			else if(strncmp("endif", temp_prog[temp_exec_line], 5) == 0)
			{
				start_line = temp_exec_line;
				break;
			}
	     }
	     if(mode == ELSEIF)
	     {       char string[50];
		     //string = new char[strlen(temp_prog[start_line])-12];
		     int len = 0;
		     //elseif num1 <> num4 then
		     for(int i = 7; ; i++, len++)
		     {
			if(temp_prog[start_line][i] == ' ' && temp_prog[start_line][i+1] == 't'
			&& temp_prog[start_line][i+2] == 'h' && temp_prog[start_line][i+3] == 'e'
			&& temp_prog[start_line][i+4] == 'n')break;
			string[len] =  temp_prog[start_line][i];
		     }
		     string[len] = '\0';

		     truth = evaluate_bool_expression(string, index, start_line);

		     goto check;
	     }
	     else if(mode == ELSE)
	     {
		endelse_line = endif_line - 1;
		is_in_if_then_else = TRUE;
		return;
	     }
	     else
	     {
		endelse_line = endif_line;
		is_in_if_then_else = TRUE;
		return;
	     }
	     //make a string and parse it
	}
	is_in_if_then_else = TRUE;
	 //make this when str evaluates to true
}
void function_table::return_void(int exec_line, int index)
{ //make the input string here
	char input_string[30];
	for(int aaa = 5; temp_prog[exec_line][aaa] != '('; aaa++)
	{
		input_string[aaa-5] = temp_prog[exec_line][aaa];
	}
	input_string[aaa-5] = '\0';
	#ifdef DEBUG_MODE
		cout << "\nTrying to void call: " << input_string << "\n at line: "<<exec_line << " with index " << index;
		getch();
	#endif
	int args = 0,len, arr_valid = 2;
	int sub_index = 0, first_arg = TRUE;
	for(int z = 0; temp_prog[exec_line][z] != '('; z++);
	for(z++; temp_prog[exec_line][z-1] != ')'; z++)
	{
		char t_name[20];
		for(len = 0;temp_prog[exec_line][z] != ',' && temp_prog[exec_line][z] != ')'; z++, len++)
		{
			t_name[len] = temp_prog[exec_line][z];
		}
		t_name[len] = '\0';
		function_arr[index].priv_vars.is_array(t_name, arr_valid);
		if(arr_valid == 1)
		{
			int l, len, int_sz_arr = 0;
			int temp_line = exec_line-1;
			char temp_name[20];
			char size_array[10];
			for(; ;temp_line--)
			{
				char comp_string[20];
				if(int_sz_arr != 0) break;
				if (strncmp("sub",temp_prog[temp_line],3) == 0)
				{
					//sub void function(dim l int, dim x[5] int)
					int copy_index;
						if(first_arg == TRUE)
						{
							for(sub_index = 0;temp_prog[temp_line][sub_index] != '('; sub_index++);
							first_arg = FALSE;
						}
						for(copy_index = 0, sub_index++; temp_prog[temp_line][sub_index] != ',' && temp_prog[temp_line][sub_index] != ')'; copy_index++, sub_index++)
						{
							comp_string[copy_index] = temp_prog[temp_line][sub_index];
						}
							comp_string[copy_index] = '\0';

							//now check if comp_string is valid or not
					}
					else if(strncmp("sub",temp_prog[temp_line+1],3) == 0 || temp_line < 0)
					{
						error_handle << "\nline: " << current_exec_line << " undefined parameters being passed\n";
						break;
						//generate error (var defineing not found)
					}
					else if (strncmp("dim",temp_prog[temp_line],3) == 0)
					{
						strcpy(comp_string, temp_prog[temp_line]);
					}
					//we construct the string to be compared above and check it with the code below
					if(strncmp("dim",comp_string,3) == 0)
					{
						for(l = 4; comp_string[l] != '[' && comp_string[l] != ' '; l++)
						{
							temp_name[l-4] = comp_string[l];
						}
						temp_name[l-4] = '\0';
						if(strcmp(t_name, temp_name) == 0)
					{
																		//cout << "\nTemp name: " << temp_name << "comparing with" << t_name;
							for(len = 0, l++; comp_string[l] != ']'; l++, len++)
							{
								size_array[len] = comp_string[l];
							}
							size_array[len] = '\0';
							int_sz_arr  = atoi(size_array);
							args += int_sz_arr;
							break;
						}
					}
				}
			}
			if(arr_valid ==0)args++;
		}

	#ifdef DEBUG_MODE
		cout << "\nNumber of fucntion args recorded: " << args;
		cout << "\nCalling: " << input_string;
	#endif
	char ret_type[20];
	void* ret_var_loc = new void*;
	strcpy(ret_type, call(input_string, args,index, exec_line, ret_var_loc));
	if(strcmp(ret_type, "void"))
	{
		//cout <<"C++, alright, no problems here";
	}
	else
	{
		error_handle << "\nline " << current_exec_line << ": function does not return a value";
	}

}

void* function_table::return_value(char str[], int index)
{
	int validate = 0;
	entery variable_holder = function_arr[index].priv_vars.get_variable(str, validate);
	if(validate == 0)
	{
		error_handle << "\nline " << current_exec_line << ": undefined variable: " << str;
	}
	else
	{
		void* variable_location = variable_holder.get_loc();
		return variable_location;
	}
	void* random = new void*;
	return random;
}

void function_table::print(char str[], int index)
{
	#ifdef DEBUG_MODE
		cout << "\nC++, Prinintg from: " <<  function_arr[index].get_name();
		getch();
	#endif

	if(str[0] == '\"')
	{
		char str2[50];
		//str2 = new char[strlen(str)];
		for(int i = 1; str[i] != '\"'; i++)
		{
			str2[i-1] = str[i];
		}
		str2[i-1] = '\0';
		if(is_in_compile_mode == FALSE)cout << str2;
		return;
	}
	else if(strcmp(str, "newline") == 0)//it looks so cool, right?
	{
		if(is_in_compile_mode == FALSE)
		cout << "\n";
		return;
	}
	else if(strcmp(str, "tab") == 0)
	{
		if(is_in_compile_mode == FALSE)cout << "\t";
		return;
	}
	else
	{       if(str[0] == '\"' && is_in_compile_mode == TRUE)return;
		int validate = 0;
		entery variable_holder = function_arr[index].priv_vars.get_variable(str, validate);
		if(validate == 0)
		{
		    if(is_in_compile_mode ==TRUE)
		    {
			error_handle << "\nline " << current_exec_line << ": undefined variable: " << str;
		    }
		}
		else
		{
			void* variable_location = variable_holder.get_loc();
			char var_type[10];// = new char[strlen(variable_holder.get_type())+1];
			strcpy(var_type, variable_holder.get_type());
			void* vari;
		   if(is_in_compile_mode == FALSE)
		   {
			if(strcmpi(var_type,"int") == 0)
			{
				vari = (variable<int>*)variable_location;
				int data = ((variable<int>*)vari)->get_value();
				cout << data;
			}
			else if(strcmpi(var_type,"string") == 0)
			{
				vari = (variable<char*>*)variable_location;
				char* data = ((variable<char*>*)vari)->get_value();
				cout << data;
			}
			else if(strcmpi(var_type,"float") == 0)
			{
				vari = (variable<float>*)variable_location;
				float data = ((variable<float>*)vari)->get_value();
				cout << data;
			}
		   }
			return;
		}
	}
}

void function_table::define(int exec_line, int index)
{
	char var_name[15];
	int var_i = 0;
	for(var_i = 0; temp_prog[exec_line][var_i] != ' '; var_i++);

	for(var_i = 0; temp_prog[exec_line][var_i] != ' '; var_i++)
	{
		var_name[var_i] =  temp_prog[exec_line][var_i];
	}

	entery catch_obj;
	int valid = FALSE;
	var_name[var_i] = '\0';
	catch_obj = function_arr[index].priv_vars.get_variable(var_name, valid);
	void* variable_location = catch_obj.get_loc();
	char var_type[10];// = new char[strlen(catch_obj.get_type())+1];
	strcpy(var_type, catch_obj.get_type());
	void* vari;
	int is_function = FALSE;
	for(int bracket_scan = 0; temp_prog[exec_line][bracket_scan] != '\0'; bracket_scan++)
	{
		if(temp_prog[exec_line][bracket_scan] == '(')
		{
			is_function = TRUE;
			break;
		}
	}

	if((temp_prog[exec_line][strlen(var_name) + 3] == '\"') || (isdigit(temp_prog[exec_line][strlen(var_name) + 3])))
	{
	       if(strcmpi(var_type,"int") == 0)
		{
			vari = (variable<int>*)variable_location;
			int data;
			char data_val[10];
			int i;
			for(i = 0; isdigit(temp_prog[exec_line][i + strlen(var_name) + 3]) || temp_prog[exec_line][i + strlen(var_name) + 3] == '.'; i++);
			//data_val = new char[i+1];
			for(i = 0; isdigit(temp_prog[exec_line][i + strlen(var_name) + 3]) || temp_prog[exec_line][i + strlen(var_name) + 3] == '.'; i++)
			{
				data_val[i] = temp_prog[exec_line][i + strlen(var_name) + 3];
			}
			data_val[i] = '\0';
			data = atoi(data_val);
			((variable<int>*)vari)->set_value(data);
		}
		else if(strcmpi(var_type,"float") == 0)
		{
			vari = (variable<float>*)variable_location;
			float data;
			char data_val[10];
			int i;
			for(i = 0; isdigit(temp_prog[exec_line][i + strlen(var_name) + 3]) || temp_prog[exec_line][i + strlen(var_name) + 3] == '.'; i++);
			//data_val = new char[i+1];
			for(i = 0; isdigit(temp_prog[exec_line][i + strlen(var_name) + 3]) || temp_prog[exec_line][i + strlen(var_name) + 3] == '.'; i++)
			{
				data_val[i] = temp_prog[exec_line][i + strlen(var_name) + 3];
			}
			data_val[i] = '\0';
			data = atof(data_val);
			((variable<float>*)vari)->set_value(data);
		}
		else if(strcmpi(var_type,"string") == 0)
		{
			vari = (variable<char*>*)variable_location;
			char data[30];
			char data_val[30];
			int i;
			for(i = 0; temp_prog[exec_line][i + strlen(var_name) + 4] != '\"'; i++);
			///data_val = new char[i+1];
			for(i = 0; temp_prog[exec_line][i + strlen(var_name) + 4] != '\"'; i++)
			{
				data_val[i] = temp_prog[exec_line][i + strlen(var_name) + 4];
			}
			data_val[i] = '\0';
			strcpy(data, data_val);
			((variable<char*>*)vari)->set_value(data);

		}
	}       	//a = b

	// a = b()
	else
	{
		//read string before bracket, check if exists var with name string, else funct call


		char input_string[20];
		int i;
		int offset =  strlen(var_name) + 3;
		for(i = 0; temp_prog[exec_line][i+ offset] != '\0'; i++)
		{
			if(temp_prog[exec_line][i+ offset] == '(')break;
			input_string[i] = temp_prog[exec_line][i+ offset];
		}
		input_string[i] = '\0';



		entery catch_obj2;
		int valid2 = FALSE;

		catch_obj2 = function_arr[index].priv_vars.get_variable(input_string, valid2);

		if(valid2 != 0) //0 = false
		{
			void* variable_location2 = catch_obj2.get_loc();
			char var_type2[10];// = new char[strlen(catch_obj2.get_type())+1];
			strcpy(var_type2, catch_obj2.get_type());
			void* vari2;
			if(strcmpi(var_type2,"int") == 0)
			{
				vari = (variable<int>*)variable_location;
				vari2 = (variable<int>*)variable_location2;
				int data;
				data = ((variable<int>*)vari2)->get_value();
				((variable<int>*)vari)->set_value(data);
			}
			if(strcmpi(var_type2,"float") == 0)
			{
				vari = (variable<float>*)variable_location;
				vari2 = (variable<float>*)variable_location2;
				float data;
				data = ((variable<float>*)vari2)->get_value();
				((variable<float>*)vari)->set_value(data);
			}
			if(strcmpi(var_type2,"string") == 0)
			{
				vari = (variable<char*>*)variable_location;
				vari2 = (variable<char*>*)variable_location2;
				char data[30];
				//data = new char[strlen(((variable<char*>*)vari2)->get_value())+1];
				data[strlen(((variable<char*>*)vari2)->get_value())] = '\0';
				///DO some thing
				strcpy(data,((variable<char*>*)vari2)->get_value());
				((variable<char*>*)vari)->set_value(data);

			}


		}
		else if(is_function == FALSE)
		{
			if(strcmpi(var_type,"int") == 0)
			{
				vari = (variable<int>*)variable_location;
				int data;
				char data_val[35];
				int i,h;       //qwerty = 98
				for(h = 0; temp_prog[exec_line][h] != '=';h++);
				for(i = 0, h += 2; temp_prog[exec_line][h] != '\0'; i++,h++)
				data_val[i] = temp_prog[exec_line][h];
				data_val[i] = '\0';
				//keyboard input:
				if(strcmp("inarrow$", data_val) == 0)
				{
					char ch;
					ch = getch();
					ch = getch();
					((variable<int>*)vari)->set_value((int)ch);
				}
				else if(strcmp("midx$", data_val) == 0)
				{
					((variable<int>*)vari)->set_value(getmaxx()/2);
				}
				else if(strcmp("midy$", data_val) == 0)
				{
					((variable<int>*)vari)->set_value(getmaxy()/2);
				}
				else
				{
					data = evaluator(data_val, index);
					((variable<int>*)vari)->set_value(data);
				}
			}
			else if(strcmpi(var_type,"float") == 0)
			{
				vari = (variable<float>*)variable_location;
				float data;
				char data_val[35];
				int i,h;       //qwerty = 98.3+3.2
				for(h = 0; temp_prog[exec_line][h] != '=';h++);
				for(i = 0, h += 2; temp_prog[exec_line][h] != '\0'; i++,h++)
				data_val[i] = temp_prog[exec_line][h];
				data_val[i] = '\0';
				data = evaluator(data_val, index);
				((variable<float>*)vari)->set_value(data);
			}
			else if(strcmpi(var_type,"string") == 0)
			{
			vari = (variable<char*>*)variable_location;
			char data[30];
			char data_val[30];
			int i;
			for(i = 0; temp_prog[exec_line][i + strlen(var_name) + 4] != '\"'; i++);
			//data_val = new char[i+1];
			for(i = 0; temp_prog[exec_line][i + strlen(var_name) + 4] != '\"'; i++)
			{
				data_val[i] = temp_prog[exec_line][i + strlen(var_name) + 4];
			}
			data_val[i] = '\0';
			strcpy(data, data_val);
			((variable<char*>*)vari)->set_value(data);

			}
		}
		else if(is_function == TRUE)
		{
			#ifdef DEBUG_MODE
				cout << "\nTrying to call: " << input_string << "\n at line: "<<exec_line << " with index " << index;
				getch();
			#endif
			int args = 0,len, arr_valid = 2;
			int sub_index = 0, first_arg = TRUE;
			for(int z = 0; temp_prog[exec_line][z] != '('; z++);
			for(z++; temp_prog[exec_line][z-1] != ')'; z++)
			{
				char t_name[20];
				for(len = 0;temp_prog[exec_line][z] != ',' && temp_prog[exec_line][z] != ')'; z++, len++)
				{
					t_name[len] = temp_prog[exec_line][z];
				}
				t_name[len] = '\0';
				function_arr[index].priv_vars.is_array(t_name, arr_valid);
				if(arr_valid == 1)
				{
					int l, len, int_sz_arr = 0;
					int temp_line = exec_line-1;
					char temp_name[20];
					char size_array[10];
					for(; ;temp_line--)
					{
						char comp_string[20];
						if(int_sz_arr != 0) break;
						if (strncmp("sub",temp_prog[temp_line],3) == 0)
						{
							//sub void function(dim l int, dim x[5] int)
							int copy_index;
							if(first_arg == TRUE)
							{
								for(sub_index = 0;temp_prog[temp_line][sub_index] != '('; sub_index++);
								first_arg = FALSE;
							}
							for(copy_index = 0, sub_index++; temp_prog[temp_line][sub_index] != ',' && temp_prog[temp_line][sub_index] != ')'; copy_index++, sub_index++)
							{
								comp_string[copy_index] = temp_prog[temp_line][sub_index];
							}

							comp_string[copy_index] = '\0';

							//now check if comp_string is valid or not
						}
						else if(strncmp("sub",temp_prog[temp_line+1],3) == 0 || temp_line < 0)
						{
							cout << "\nError: array definition not found";
							cout << "\nHere is the line: " << temp_prog[temp_line+1];
							break;
							//generate error (var defineing not found)
						}
						else if (strncmp("dim",temp_prog[temp_line],3) == 0)
						{
							strcpy(comp_string, temp_prog[temp_line]);
						}
						//we construct the string to be compared above and check it with the code below
						if(strncmp("dim",comp_string,3) == 0)
						{
							for(l = 4; comp_string[l] != '[' && comp_string[l] != ' '; l++)
							{
								temp_name[l-4] = comp_string[l];
							}
							temp_name[l-4] = '\0';
							if(strcmp(t_name, temp_name) == 0)
							{
																			//cout << "\nTemp name: " << temp_name << "comparing with" << t_name;
								for(len = 0, l++; comp_string[l] != ']'; l++, len++)
								{
									size_array[len] = comp_string[l];
								}
								size_array[len] = '\0';
								int_sz_arr  = atoi(size_array);
								args += int_sz_arr;
								break;
							}
						}
					}

				}
				if(arr_valid ==0)args++;
			}

			#ifdef DEBUG_MODE
				cout << "\nNumber of fucntion args recorded: " << args;
				cout << "\nCalling: " << input_string;
			#endif

			char ret_type[20];
			void* ret_var_loc = new void*;



			strcpy(ret_type, call(input_string, args,index, exec_line, ret_var_loc));


				if(strcmpi(ret_type,"int") == 0)
				{
					int data;
					variable<int>* temp_var_int;
					temp_var_int = (variable<int>*)ret_var_loc;
					data = temp_var_int->get_value();
					vari = (variable<int>*)variable_location;
					((variable<int>*)vari)->set_value(data);
				}
				else if(strcmpi(ret_type,"float") == 0)
				{
					float data;
					variable<float>* temp_var_int;
					temp_var_int = (variable<float>*)ret_var_loc;
					data = temp_var_int->get_value();
					vari = (variable<float>*)variable_location;
					((variable<float>*)vari)->set_value(data);
				}
				else if(strcmpi(ret_type,"string") == 0)
				{
					char data[30];
					variable<char*>* temp_var_int;
					temp_var_int = (variable<char*>*)ret_var_loc;
					strcpy(data,temp_var_int->get_value());
					vari = (variable<char*>*)variable_location;
					((variable<char*>*)vari)->set_value(data);
				}
				else if(strcmpi(ret_type, "void") == 0)
				{
					cout << "\nC++, assigining a value from a function with a void return value? Great. Keep it up.";
					getch();
					exit(0);
				}

			//function call which returns something parse the string to get num_args
		}
	}
		//mayvbe check type
		//else
}
int function_table::check_if_define(int exec_line, int index)
{
		char var_name[15];
		int var_i = 0;
		for(var_i = 0; temp_prog[exec_line][var_i] != ' '; var_i++);
		//var_name = new char[var_i+1];
		for(var_i = 0; temp_prog[exec_line][var_i] != ' '; var_i++)
		{
			var_name[var_i] =  temp_prog[exec_line][var_i];
		}
		entery catch_obj;
		int valid = FALSE;
		var_name[var_i] = '\0';
		catch_obj = function_arr[index].priv_vars.get_variable(var_name, valid);

		if(valid == TRUE)
		{
			return TRUE;
		}
		else
		{
			//error_handle << "\nline " << current_exec_line << ": undefined symbol: " << var_name;
			return FALSE;
		}
}

#ifdef INDIVISUAL_MODE
void message_box(int start_from_top = 1)//Error message box
{
	char message[5][70];
	fstream fp;
	int i = 0;
	fp.open("message.txt",ios::in);
	while(!fp.eof())
	{
		fp.getline(message[i],50,'\n');
		i++;
	}
	int j = 1;
	for (i = start_from_top-1;j<8;j++,i++)
	{
		gotoxy(1,j);
		cout << j <<" : " << message[i];
	}
	fp.close();
	getch();
}

#endif

void load_global_vars()
{
	current_exec_line=0;
	function_table t;
	t.function_table_test();
	clrscr();
	void* syntax = new void*;
	char launcher_var[5];

	strcpy(launcher_var, t.call("main", 0, -1, -1, syntax));

	if (is_in_compile_mode==FALSE)
	{
		gotoxy(1,25);
		cout << "Press any key to continue...";
		getch();
	}
	//error_handle.close();
}

void main_menu()
{
   int choice = 0, funct_launcher = 0;
   do
   {
	clrscr();
	program->refresh();
	cout << "Welcome to QBasic Compiler";
	cout << "\n1. Open File";
	cout << "\n2. Create File";
	cout << "\n3. Exit";
	cout << "\n\nEnter your choice: ";
	cin >> choice;
	if(choice > 3)continue;
	if(choice == 1)
	{
		initialize_compiler();
		funct_launcher = program->open();
		//program->open_from_editor("prog15.txt");
		if(funct_launcher == FALSE)
		{
		  cout << "\nError: No file exists with that name.";
		  cout << "\nPress any key to continue";
		  destroy_compiler();
		  getch();
		}
		if(funct_launcher == TRUE +3)
		{
			load_global_vars();
			destroy_compiler();
		}
	}
   } while (choice != 3);

   if(choice == 3)
   {
	clrscr();
	cout << "Thanks for using.";
	cout << "\nPress any key to exit";
	getch();
   }
}

#ifdef INDIVISUAL_MODE
void main()
{
	getch();
	is_in_compile_mode = FALSE;
	main_menu();
	cout << "\nBack in C++ main";
	getch();
	getch();
}
#endif

void run_compiler(char file_n[])
{
	is_in_compile_mode = FALSE;
	initialize_compiler();
	program->open_from_editor(file_n);
	load_global_vars();
	destroy_compiler();
}


void compile_compiler(char file_n[])
{
	initialize_compiler();
	is_in_compile_mode = TRUE;
	error_handle.open("Message.txt", ios::out);
	program->open_from_editor(file_n);
	load_global_vars();
	is_in_compile_mode = FALSE;
	destroy_compiler();
	error_handle.close();
}

/*void main()
{
	cout << "\nPress any key to begin";
	compile_compiler("prog16.txt");
}*/