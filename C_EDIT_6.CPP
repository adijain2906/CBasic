#include <iostream.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <fstream.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <graphics.h>
#include "c_comp_9.cpp"

/*
ASCII(s)
F1  = 0,59       RIGHT ARROW = 0,77
F2  = 0,60       LEFT ARROW  = 0,75
F3  = 0,61       UP ARROW    = 0,72
F4  = 0,62       DOWN ARROW  = 0,80
F5  = 0,63       DELETE    = 0,86
F6  = 0,64	 ENTER     = 8
F7  = 0,65       BACKSPACE = 13
F8  = 0,66       ESCAPE    = 27
F9  = 0,67
F10 = 0,68
F11 = 0,69
F12 = 0,70
*/

int cursur_x, cursur_y;

void scrollbar(int display_scroll = 0) //To display the scroll bar char at display_scroll location
{
	gotoxy(80,2);
	cout << (char)280;//Up arrow on top of the scroll bar
	for(int y = 3; y < 18; y++)//The complete scroll bar
	{
		gotoxy(80,y);
		cout << (char)177;
	}
	gotoxy(80,y);
	cout << (char)281; //Down arrow belay the scroll bar
	if (display_scroll != 0)// Only if any file is open
	{
		gotoxy(80, display_scroll);
		cout << (char)219; //Scrollbar char
	}
}

void toolbar()
{
	gotoxy(1,25);
	//Some functions related to text editor
	cout << "<F1 - Help>  <F2 - Save>  <F3 - Open>  <F5 - Run>  <F9 - Compile>  <F10 - Exit>";
}

void drawwindow(char* title,int scrollbar_set = 0)//Title of the window
{                                                 //Display scroll bar char at scrollbar_set location
	clrscr();
	int len = strlen(title);
	gotoxy(1,1);
	//Title bar
	cout << (char)201; //Upper left char
	for(int x = 1; x < 79; x++)
	{
		if(x == (38-(len/2))) //Center align
		{
			cout << " " << title << " ";
			x+=len+2;
		}
		if(x == 75)
		{
			cout << "[X]";
			x+=3;
		}
		cout << (char)205;
	}
	cout << (char)187; //Upper right char
	//Left line
	for(int y = 1; y < 18; y++)
	{
		cout << (char)(186) << "\n";
	}
	cout << (char)199;//Lower left
	for(int t = 1; t < 5; t++)cout << (char)196;//Aligning the cursur position
	cout << " " << cursur_y << ":" << cursur_x << " "; //Line(cursur_y) and Character in that line(cursur_x) number
	//Aligning
	t += 5;
	if(cursur_x > 9)t++;
	if(cursur_y > 9)t++;
	char msg[] = "Message";
	int len2 = strlen(msg);
	for(; t < 79; t++)
	{
		if(t == (38-(len2/2))) //Center align
		{
			cout << " " << msg << " ";
			t+=len2+2;
		}
		cout << (char)196;
	}
	cout << (char)182;//Lower right
	scrollbar(scrollbar_set);
	//Message window
	gotoxy(1,24);
	cout << (char)200; //Lower lower left
	for(int z = 2; z < 80; z++)cout << (char)205;//Lower line
	cout << (char)188; //Lower lower right
	//The vertical lines of the immediate window
	for(y = 20; y < 24; y++)
	{       gotoxy(1,y);
		cout << (char)(186) << "\n";
		gotoxy(80,y);
		cout << (char)(186) << "\n";
	}
	toolbar();
}

fstream file1;//File opened

class line_char_queue;//To be able to make it friend with line_char

class line_char//To store character
{
	private:
		char content;
		line_char* next;
		line_char()
		{
			content = '\0';
			next = NULL;
		}
		friend line_char_queue;
};

class line_char_queue//Queue of characters to form a line
{
	private:
		line_char* start; //Starting character of the line
		int length; //Length of the line
	public:

		int len()
		{
			return length;
		}
		void set_beg(line_char*);
		line_char* get_line_char(int);
		line_char* get_start()
		{
			return start;
		}
		line_char_queue()
		{
			length  = 0;
			start = NULL;
		}
		void push(char);
		void join(line_char*, int );
		void add_at(int, char);
		void pop(int);
		void display();
		void refresh();
		void save_file();
};

void line_char_queue:: set_beg(line_char* beg)//To set the beginning character(s)
{
	start = beg;
	line_char* temp = start;
	while(temp != NULL)
	{
		temp = temp->next;
		length++;
	}
}

line_char* line_char_queue:: get_line_char(int x)//To return the character(s) from the place x from start
{
	int i = 1;
	line_char* temp = start;
	for(;i < x-1 ;temp = temp->next,i++);
	line_char *save;
	if (x==1)
	{
		save = start;
		start = NULL;
	}
	else
	{
		save = temp->next;
		temp->next = NULL;
	}
	return save;
}

void line_char_queue:: push(char ch)//To push a character
{
	line_char* temp = new line_char;
	temp->content = ch;
	if(start == NULL)
	{
		start = temp;
	}
	else
	{
		for(line_char* i = start; i->next != NULL; i = i -> next);
		i->next = temp;
	}
	length++;
}

void line_char_queue::join(line_char* next_line, int len)//To push a bunch of characters
{
	for(line_char* i = start; i->next != NULL; i = i -> next);
	i->next = next_line;
	length  += len;
}

void line_char_queue::add_at(int x, char ch)//To push a character at x place from start
{
	int i = 1;
	line_char* inti = new line_char;
	inti->content = ch;
	line_char* temp = start;
	if( x != 1)
	{
		for(;i < x-1; temp = temp->next, i++);
		line_char* save = temp->next;
		temp->next = inti;
		inti->next = save;
	}
	else
	{
		line_char* save = start;
		start = inti;
		start->next = save;
	}
	length++;
}

void line_char_queue::pop(int index)//To pop a character at index place from start
{
	int i = 1;
	if (index == 0)//Used when length is 0
	{
		start = NULL;
		return;
	}
	if(index > length)//Pop will not be possible otherwise
	return;
	line_char* j = start;
	while(i < index-1)
	{
		j = j->next;
		i++;
	}
	line_char* del;
	if (index==1)//To pop the first character
	{
		del = start;
		start = start->next;
	}
	else // To pop any middle or last character
	{
		del = j->next;
		j->next = j->next->next;
	}
	delete del;
	length--;
}

void line_char_queue::display()//Display content of line
{
	for(line_char* i = start; i != NULL; i = i->next)
	{
		cout << i->content;
	}
}

void line_char_queue::refresh()//To free the memory of queue
{
	while(start!=NULL)
	{
		pop(length);
	}
}

void line_char_queue:: save_file()//To save
{
	for(line_char* i = start; i != NULL; i = i->next)
	{
		file1 << i->content;
	}
}

struct line_str //To store queue of characters which forms a line
{
	line_char_queue line;
	int local_index;
	line_str* next;
	line_str()
	{
		next = NULL;
		local_index = 0;
	}
	~line_str()
	{
		line.refresh();
	}
};

void message_box(int start_from_top = 1)//Error message box
{
	char message[50][70];
	int line_no[50];
	fstream fp;
	int number_of_lines = 0;
	fp.open("message.txt",ios::in);
	char ch;
	fp >> ch;
	if (fp.eof()) return;
	fp.seekg(0,ios::beg);
	char digit[4];
	int len = 0;
	//line 12
	while(!fp.eof())
	{
		fp.getline(message[number_of_lines],60,'\n');
		if(number_of_lines != 0)
		{
			for(len = 5; message[number_of_lines][len] != ':'; len++)
			{
			 //cout << message[number_of_lines][len];
			 digit[len-5] = message[number_of_lines][len];
			}
			digit[len-5] = '\0';
			//cout << "\ndigit: " << digit;
		}
		line_no[number_of_lines] = atoi(digit);
		number_of_lines++;
	}

	char temp_line[70];
	int temp_no = 0;
	for(int ii = 2; ii < number_of_lines; ++ii)//compiler reports
	//messages in a very intertesting way thus we have to sort
	//before displaying to seem logical
	{
		for(int jj = 0; jj < (number_of_lines-ii); ++jj)
		{
		    if(line_no[jj] > line_no[jj+1])
		    {
			strcpy(temp_line, message[jj]);
			temp_no = line_no[jj];
			line_no[jj] = line_no[jj+1];
			strcpy(message[jj], message[jj+1]);
			line_no[jj+1] = temp_no;
			strcpy(message[jj+1], temp_line);
		    }
		}
	}
	int j = 20;
	int i;

	for (i = start_from_top;j<24 && i<number_of_lines;j++,i++)
	{
		gotoxy(2,j);
		if (strncmp("line",message[i],4));
		cout << message[i];
	}
	fp.close();
}

class queue //Queue of line(queue of characters)
{
	private:
		line_str* top; //Top most line
		int num_lines; //Number of lines
	public:
		queue()
		{
			num_lines  = 0;
			top = NULL;
		}

		void edit(int, int, char);

		int get_length(int );

		void internal_pop(int, int);

		void internal_push(int, char);
		void push();
		void push_at_index(int );

		void push_at_xy(int , int);

		void pop(int);
		void pop2(int);

		void join_ext(int);
		void refresh();
		void read(char [],int &);

		void display(char[],int,int);
		void save(char []);
};

void queue:: edit(int x, int y, char ch) //To push a character at y(th) line and x place
{
	int i = 1;
	for(line_str* temp = top; i != y; i++, temp = temp->next);
	temp->line.add_at(x,ch);
}

int queue::get_length(int y)//To return the length of the y(th) line
{
	int i = 1;
	for(line_str* temp = top; i != y; i++, temp = temp->next);
	return temp->line.len();
}

void queue::internal_pop(int x, int y) //To pop a character from y(th) line and x place from start
{
	int i = 1;
	for(line_str* temp = top; i != y; i++, temp = temp->next);
	temp->line.pop(x);
}

void queue::internal_push(int y, char ch)//To push a character in the y(th) line
{
	int i = 1;
	if(top == NULL)
	{
		line_str* header = new line_str;
		top = header;
		num_lines++;
	}
	if(y>num_lines)return;
	for(line_str* temp = top; i != y; i++, temp = temp->next);
	temp->line.push(ch);
}

void queue:: push() //To push a new line
{
	line_str* temp = new line_str;
	if(top == NULL)
	{
		top = temp;
	}
	else
	{
		for(line_str* i = top; i->next != NULL; i = i -> next);
		i->next = temp;
	}
	num_lines++;
}

void queue::push_at_index(int y)//To push a line after y(th) line(in middle of two lines)
{
	line_str* temp = new line_str;
	if(top == NULL)
	{
		top = temp;
	}
	else
	{
		int j = 1;
		line_str* i = top;
		for(; j != y; i = i -> next, j++);
		line_str* save = i->next;
		i->next = temp;
		temp->next = save;
	}
	num_lines++;
}

void queue:: push_at_xy(int x, int y)//To push a line after the x index character in the y(th) line
{
	line_str* temp = new line_str;
	if(top == NULL)
	{
		top = temp;
	}
	else
	{
		int j = 1;
		for (line_str* i= top;j < y;j++,i = i->next);
		line_str* save = i->next;
		i->next = temp;
		temp->next = save;
		temp->line.set_beg(i->line.get_line_char(x)/*To know the characters after x index*/);//To set the beginning of the line with those characters
	}
	num_lines++;
}

void queue::pop(int index)//To pop the line at y index
{
	int i = 1;
	if (index==0) //When num_lines is 0
	{
		top = NULL;
		return;
	}
	if(index > num_lines)//Pop would not happen otherwise
	return;
	line_str* j = top;
	for(; i < index-1; i++, j = j->next);
	line_str* del;
	if (index == 1 )
	{
		del = top;
		top = top->next;
	}
	else
	{
		del = j->next;
		j->next = j->next->next;
	}
	delete del;
	num_lines--;
}

void queue::pop2(int index)
{
	int i = 1;
	if (index == 0)
	{
		top = NULL;
		return;
	}
	if(index > num_lines)return;
	for(line_str* j = top; i < index-1; i++, j = j->next);
	line_str* del = j->next;
	j->next = j->next->next;
	if(del == NULL)
	{
		//nothing
	}
	num_lines--;
}

void queue:: join_ext(int y)//To join two lines(used when backspace is pressed from below of the y(th) line)
{
	int i = 1;
	for(line_str* temp = top; i != y-1; i++, temp = temp->next);
	temp->line.join(temp->next->line.get_start(), temp->next->line.len());
	pop2(y);
}

void queue:: refresh()//To free up memory
{
	while(top!=NULL)
	{
		pop(num_lines);
	}
}

void queue:: read(char file_name[],int &y_max)//Open a file to edit with file_name
{
	file1.open(file_name,ios::in);
	char string[100];
	char ch;
	y_max=1;
	file1 >> ch;
	if (file1.eof()) //is file empty?
	{
		y_max = 1;
		push();
		return;
	}
	file1.seekg(0,ios::beg);
	if(top == NULL)
	{
		push();
	}
	line_str* j = top;
	while(1)
	{
		file1.getline(string,100,'\n');
		if(file1.eof())
		{
			break;
		}
		for (int i = 0;string[i]!='\0';i++)
		{
			j->line.push(string[i]);
		}

		push();
		j = j->next;
		y_max++;
	}
	file1.close();
}

void queue:: display(char file_name[],int top_line = 1,int scroll_set = 1)//Display on screen
{
	clrscr();
	drawwindow(file_name,scroll_set + 2);
	if (strcmp(file_name,"---")!=0 && strcmp(file_name,"help.txt")!=0 && strcmp(file_name,"message.txt")!=0)
	message_box();
	int line_ctr = 2;
	line_str* i = top;
	int j = 1;
	for ( ;j < top_line;j++,i = i->next);
	for( ; i != NULL && line_ctr < 19; i = i->next, line_ctr++)
	{
		gotoxy(2,line_ctr);
		i->line.display();
	}
}

void queue:: save(char file_name[])//To save the file
{
	file1.open(file_name,ios::out);
	for(line_str* i = top; i != NULL; i = i->next)
	{
		i->line.save_file();
		if (i->next!=NULL)file1 << "\n";
	}
	file1.close();
}

void processing()//Just for fun
{
	gotoxy(2,20);
	cout << "              	        			";
	//clreol() would have cleared the frame of the message box, thus^
	char string[] = "\\|/";
	gotoxy(2,20);
	cout << "Processing ";
	for(int j=0;j<10;j++)
	{
		for(int i=0;string[i]!='\0';i++)
		{
			gotoxy(14,20);
			cout << string[i];
			delay(70);
		}
	}
}

void saved_box()//Box appearing to confirm save
{
	int i;
	gotoxy(30,7);//uperleft
	cout << (char)218;
	for (i=31;i<50;i++)
	{
		gotoxy(i,7);
		cout << (char)196;
		gotoxy(i,9);
		cout << (char)196;
	}
	gotoxy(i,7);
	cout << (char)191;//upper right
	gotoxy(i,9);
	cout << (char)217;//bottom right
	for (i=8;i<9;i++)
	{
		gotoxy(30,i);
		cout << (char)179;
		gotoxy(50,i);
		cout << (char)179;
	}
	gotoxy(30,9);
	cout << (char)192;//bottom left
	gotoxy(31,8);
	cout << "     File saved";
	i = 45;
	while(i!=50)
	{
		gotoxy(i++,8);
		cout << " ";
	}
}

void save_box()//Box to ask if unsaved changes are there
{
	int i;
	gotoxy(15,7);//uperleft
	cout << (char)218;
	for (i=16;i<65;i++)
	{
		gotoxy(i,7);
		cout << (char)196;
		gotoxy(i,10);
		cout << (char)196;
	}
	gotoxy(i,7);
	cout << (char)191;//upper right
	gotoxy(i,10);
	cout << (char)217;//bottom right
	for (i=8;i<10;i++)
	{
		gotoxy(15,i);
		cout << (char)179;
		gotoxy(65,i);
		cout << (char)179;
	}
	gotoxy(15,10);
	cout << (char)192;//bottom left
	gotoxy(16,8);
	cout << "Do you want to save : -";
	i = 39;
	while(i!=65)
	{
		gotoxy(i++,8);
		cout << " ";
	}
	gotoxy(16,9);
	cout << "Yes or No (Y/N)";
	i = 31;
	while (i!=64)
	{
		gotoxy(i++,9);
		cout << " ";
	}
}

void open_box(char file_name[])//Box used to open a file
{
	int i;
	gotoxy(15,7);//uperleft
	cout << (char)218;
	for (i=16;i<65;i++)
	{
		gotoxy(i,7);
		cout << (char)196;
		gotoxy(i,10);
		cout << (char)196;
	}
	gotoxy(i,7);
	cout << (char)191;//upper right
	gotoxy(i,10);
	cout << (char)217;//bottom right
	for (i=8;i<10;i++)
	{
		gotoxy(15,i);
		cout << (char)179;
		gotoxy(65,i);
		cout << (char)179;
	}
	gotoxy(15,10);
	cout << (char)192;//bottom left
	gotoxy(16,8);
	cout << "Enter File Name : -";
	i = 34;
	while (i!=65)
	{
		gotoxy(i++,8);
		cout << " ";
	}
	i = 16;
	while(i!=65)
	{
		gotoxy(i++,9);
		cout << " ";
	}
	gotoxy(16,9);
	cin >> file_name;
}

void welcome()//Welcome screen
{
	//80x25
	char *arr[] = { "Welcome to MS-DOS CBasic compiler,",
			"Copyright (c) The Void Club",
			"All rights reserved.",
			"Press any key to continue..."};
	int i,j;
	gotoxy(15,8);//uperleft
	cout << (char)201;
	for (i=16;i<65;i++)
	{
		gotoxy(i,8);
		cout << (char)205;
		gotoxy(i,16);
		cout << (char)205;
	}
	gotoxy(i,8);
	cout << (char)187;//upper right
	gotoxy(i,16);
	cout << (char)188;//bottom right
	for (i=9;i<16;i++)
	{
		gotoxy(15,i);
		cout << (char)186;
		gotoxy(65,i);
		cout << (char)186;
	}
	gotoxy(15,16);
	cout << (char)200;//bottom left
	for (i=0;i<4;i++)
	{
		for (j = 0;j < 48;j++)
		{
			if (j == (48-strlen(arr[i]))/2)
			{
				gotoxy(j+16,i*2+9);
				cout << arr[i];
				j+= strlen(arr[i]);
			}
		}
	}
	getch();
}

void main()
{
	clrscr();
	queue edit_text;
	char input[40];
	char file_name[30] = "---"; //Initially no file is loaded
	int unsaved_changes = 0; //unsaved changes is false
	int top_line = 1; //line from which the text has to be displayed in the edit window
	int scroll_bar_set = 1;//In order to set scroll bar character
	float lines_per_scroll = 0;//lines distributed to the each scroll unit(in this case 15)
	int ly, x, y, y_max, x_max;//y_max(the number of lines pushed),x_max(the number of characters pushed)
				   //x,y(to be able to scroll through page),ly(local y used to display text under a restricted area)
	ly = x = y = y_max = x_max = 1;
	edit_text.push();//A lines is pushed to begin with
	char inp;
	cursur_x = 1; //Telling the coordinates of the file as "line number":"character number"
	cursur_y = 1;
	int X,Y,B; //For the mouse(X x-coodinate,Y y-coordinate,B button status
	drawwindow(file_name);
	welcome();
	drawwindow(file_name);
	while(1)
	{
		start:
		gotoxy(x+1,ly+1);
		inp = getch();
		if(inp == 13)//enter
		{
			if (strcmp(file_name,"---")==0||strcmp(file_name,"help.txt")==0) continue;
			if(y == y_max)
			{
				edit_text.push();
				x = x_max = 1;
				if (ly==y)
				ly++;
				y++;
			}
			else
			{
				if(x >= edit_text.get_length(y))
				{
					edit_text.push_at_index(y);
					x = x_max = 1;
					if (ly==y)
					ly++;
					y++;
				}
				else
				{
				     edit_text.push_at_xy(x,y);
				}
				y_max++;
			}
		}
		else if(inp == 8)//backspace
		{
			if (strcmp(file_name,"---")==0||strcmp(file_name,"help.txt")==0) continue;
			if(x!=1)
			{
				edit_text.internal_pop(x-1,y);
				x--;
			}
			else if(y>1 && x == 1 && edit_text.get_length(y) == 0)
			{
				edit_text.pop(y);
				if (ly==y)
				ly--;
				y--;
				x = x_max = edit_text.get_length(y)+1;
				y_max--;
			}
			else if(y>1)
			{
				x = edit_text.get_length(y)+1;
				edit_text.join_ext(y);
				if (ly==y)
				ly--;
				y--;
				y_max--;
			}
			unsaved_changes=1;
		}
		else if(strcmp(file_name,"help.txt")==0  && inp == 27)//go back from help
		{
			edit_text.refresh();
			strcpy(file_name, "---");
			x = y =ly = x_max = 1;
		}
		else if(inp == 0)//As the following char have 2 ASCIIs
		{
			inp = getch();
			if (strcmp(file_name,"---")==0 && inp != 59 && inp != 61 && inp!=68) continue;
			if (inp == 72)//up arrow
			{
				if(y == 1)continue;
				else
				{
					if (ly==y)
					ly--;
					y--;
				}
				x = x_max = 1;
			}
			else if(inp == 80)//down arrow
			{
				if(y == y_max)continue;
				else
				{

					if (ly==y)
					ly++;
					y++;
				}
				x = x_max = 1;
			}
			else if(inp == 75)//left arrow
			{
				if(x == 1) continue;
				x--;
			}
			else if(inp == 77)//right arrow
			{
				if(x-1 == edit_text.get_length(y)) continue;
				x++;
			}
			else if (inp == 59)//F1
			{
				edit_text.refresh();
				strcpy(file_name,"help.txt");
				edit_text.read(file_name,y_max);
				x = y = ly = x_max = 1;
			}

			else if (strcmp(file_name,"help.txt")==0 && inp!=68) continue;
			else if (inp == 60)//F2
			{
				edit_text.save(file_name);
				saved_box();
				unsaved_changes = 0;
				delay(700);
				if (file_name[strlen(file_name)-1]=='*')
				{
					file_name[strlen(file_name)-1] = '\0';
				}
				edit_text.refresh();
				edit_text.read(file_name,y_max);
				x = ly = y = x_max = 1;
			}
			else if (inp == 61)//F3
			{
				open_box(file_name);
				edit_text.refresh();
				edit_text.read(file_name,y_max);
				x = y = ly= x_max = 1;
			}
			else if (inp == 63)//F5
			{
				processing();
				run_compiler(file_name);
				edit_text.refresh();
				edit_text.read(file_name,y_max);
				x = y = ly = x_max = 1;
			}
			else if (inp == 67)//F9
			{
				processing();
				compile_compiler(file_name);
				edit_text.refresh();
				edit_text.read(file_name,y_max);
				x = y = ly = x_max = 1;
			}
			else if (inp == 68)//F10
			{
				if (strcmp(file_name,"---")==0||strcmp(file_name,"help.txt")==0)
				exit(0);
				if (unsaved_changes==1)
				save_box();
				else
				exit(0);
				char choice;
				 enter_choice_:
				choice = getch();
				if (choice=='Y'||choice=='y')
				{
					edit_text.save(file_name);
					getch();
					exit(0);
				}
				else if (choice == 'N'||choice=='n')
				{
					exit(0);
				}
				else
				{
					goto enter_choice_;
				}
			}
		}
		else if(inp==27) continue; //esc
		else
		{
			if (strcmp(file_name,"---")==0||strcmp(file_name,"help.txt")==0) continue;
			if(y == y_max && x == edit_text.get_length(y))
			{
				edit_text.internal_push(y, inp);//internal push
				x++;
			}
			else
			{
				edit_text.edit(x,y,inp);
				x++;
			}
			unsaved_changes = 1;
		}
		if (unsaved_changes==1 && strcmp(file_name,"---")!=0)
		{
			if (file_name[strlen(file_name)-1]!='*')
			strcat(file_name,"*");
		}
		if (ly>17) ly = 17;
		else if (ly<1) ly = 1;

		if(y > y_max) y_max = y;//Updating y_max and x_max
		if(x > x_max) x_max = x;

		cursur_x = x;//Updating the cursur coordinates
		cursur_y = y;

		if (y>17)
		top_line = y-16;
		else
		top_line = 1;
		if (y > 17)//For the scroll bar
		{
			int i,j;
			lines_per_scroll = ceil((y_max-17)/15.0);
			for (i=17,j=0;i<y;i+=lines_per_scroll,j++);
			scroll_bar_set = j;
		}
		else
		scroll_bar_set = 1;
		edit_text.display(file_name,top_line,scroll_bar_set);
	}
}
//Congratulations, you made it to the bottom!