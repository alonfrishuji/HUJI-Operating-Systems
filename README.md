yonibor,alonfrishberg

personal details:
yonathan bornfeld, 207049461
alon frishberg, 315878314

Exercise 1:

Assignment 1:
1. the program creates a directory called './welcome_dir_1', in the current directory, 
	with premissions of 'rwxrwxrwx'
	then it creates a directory in './welcome_dir_1/welcome_dir_2' with the same premissions
2. afterwards it checks where the heap memory allocated to the program ends, and changes it's ending position,
	for future usage of memory.
3. than the program opens two files- 
	3.1. a file at the relative path '"./welcome_dir_1/welcome_dir_2/welcome_file.txt' 
		 (if the file doesn't exist it will create it, if it exists it will clear it's previous content),
		 it is opened in write only mode
	3.2. a file at "/etc/localtime" in readonly mode
4. than it gets the stats of the localtime file (twice, for some reason), afterwards it reads the file to some buffer.
	and than rereads the last 1506 bytes and saves them to the same buffer (unless there was some part of the 
	address that strace truncated), than it closes the file.
5. than it gets the stats of the welcome_file, then it writes "welcome to OS-2023" to the file and closes it.
6. then the program deletes "./welcome_dir_1/welcome_dir_2/welcome_file.txt", tries to delete the folder
   ./welcome_dir_1/welcome_dir_2 with the wrong command (deleting file, while it's a folder), and than uses
   the correct command for removing a dir. than the same happens for ./welcome_dir_1
7. finally the program terminates all processes related to it, and returns a success exit code

Assignment 2:
files:
osm.cpp- contains the functions that measures the 3 required operations
graph- compares the time of the 3 operations on CSE computer, VM, and container