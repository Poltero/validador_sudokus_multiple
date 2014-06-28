#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "vector.c"
#include <sys/stat.h>
#include <fcntl.h>

int nproceses = 0, group_proceses = 0, position_queue = 0;


Vector_pointers_char* queue;

pid_t* sons;

char *dirin, *dirout;

void create_child_process(int* sons, int index);

void consume_file(char* path);

void search_files(char* dir);

int get_index_from_value(int* list, int size, int value);

int count_slash_from_string(char* str, int size);

Vector_chars* get_name_file_without_path(char* str, int size);

int main(int argc, char** argv)
{
  	int child_status, i;

  	if(argc < 4) {
  		printf("Error: Malformed\n");
  		exit(0);
  	}
	
	dirin = argv[1];
	dirout = argv[2];
	group_proceses = atoi(argv[3]);

	if(group_proceses <= 0) {
		printf("Hay %d validadores para crear, por lo que el programa termina aqui\n", group_proceses);
		exit(0);
	}

  	search_files(dirin);

  	if(group_proceses > queue->data.size) {
  		group_proceses = queue->data.size;
  		printf("Hay mas validadores que el numero de tareas (%d). Se crearan solo %d validadores\n", group_proceses,group_proceses);
  	}

  	sons = (pid_t*) realloc(NULL, sizeof(pid_t)*group_proceses);

  	position_queue = queue->data.size-1;

  	i = 0;
  	while(i < group_proceses) {
  		printf("Se va a validar el fichero: %s\n", queue->list[position_queue].list);
  		create_child_process(sons, i);
  		i++;
  		position_queue--;
  		nproceses++;
  	}

  	while(nproceses > 0) {
  		pid_t wpid = wait(&child_status);
  		nproceses--;

  		if(position_queue >= 0) {
			int index_free = get_index_from_value(sons, group_proceses, wpid);
			printf("Se va a validar el fichero: %s\n", queue->list[position_queue].list);
			create_child_process(sons, index_free);
			position_queue--;
			nproceses++;
		}
  	}



	exit(0);
}



void create_child_process(int* sons, int index) {
	
	if((sons[index] = fork())  == 0) {
		Vector_chars* name_file = get_name_file_without_path(queue->list[position_queue].list, queue->list[position_queue].data.size);

		char* pre_name = "validator-";
		char* filename = (char*)malloc(sizeof(dirout)*sizeof(pre_name)*sizeof(name_file->list));
		strcpy(filename, dirout);
		strcat(filename, pre_name);
		strcat(filename, name_file->list);


		execl("valida_uno", "valida_uno", queue->list[position_queue].list, filename, (char *) 0);
		perror("Error in valida_uno process");
		exit(0);
	}
	
}

int get_index_from_value(int* list, int size, int value) {
	int i = 0;

	while(i < size) {
		if(list[i] == value) {
			return i;
		}
		i++;
	}

	return -2;

}

void search_files(char* dir) {
	pid_t child;
	int child_status, i = 1;

	int filedes[2], nbytes;
	char buffer[1];

	pipe(filedes);

	if((child = fork()) == 0) {

		//pipe

		dup2(filedes[1], 1);
		close(filedes[0]);

		execl("busca_program", "busca_program", dir, "-n", ".txt", "-print", (char *) 0);
		perror("Error in busca_program");
		exit(0);

	} else {
		close(filedes[1]);

		int init = 1;
		Vector_chars* str;

		queue = create_vector_of_pointers_chars(10);

		while((nbytes = read(filedes[0], buffer, sizeof(buffer))) > 0) {
			if(init == 1) {
				str = create_vector_of_chars(20);
				init = 0;
			}

			if(buffer[0] != '\n') {
				push_back_char(str, buffer[0]);
			} else {
				push_back_char(str, '\0');
				push_back_char_pointer(queue, str);
				init = 1;
			}
			
		}

        pid_t wpid = wait(&child_status);

        int i = queue->data.size-1;

        /*printf("Elementos: \n");

        while(i >= 0) {
        	printf("%s\n", queue->list[i].list);
        	i--;
        }
        printf("\n\n");*/
	}
}

int count_slash_from_string(char* str, int size) {
	int slashs  = 0;

	while(size >= 0) {
		if(str[size] == '/')
			slashs++;
		size--;
	}

	return slashs;
}

Vector_chars* get_name_file_without_path(char* str, int size) {
	Vector_chars* result;

	result = create_vector_of_chars(15);

	int slashs = count_slash_from_string(str, size);
	int i = 0;

	while(i < size) {
		if(slashs == 0) {
			push_back_char(result, str[i]);
		}

		if(str[i] == '/') {
			slashs--;
		}
		i++;
	}

	push_back_char(result, '\0');

	return result;
}

