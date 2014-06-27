#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "vector.c"
#include <sys/stat.h>
#include <fcntl.h>

int nprocess = 12, groups = 4, position_queue = -1, child_status;


Vector_pointers_char* queue;

pid_t sons[4];

int fds[4];


void create_multiple_child_process(int* sons, int num_childdren, void(*action)(char*));

void create_child_process(int* sons, int index, void(*action)(char*));

void consume_file(char* path);

void search_files(char* dir);

int get_index_from_value(int* list, int size, int value);

int count_slash_from_string(char* str, int size);

Vector_chars* get_name_file_without_path(char* str, int size);

void handler(int sig) {
	int child_status;

	pid_t wpid = wait(&child_status);

	int index_free = get_index_from_value(sons, 4, wpid);
	printf("Creamos otro hijo en el hueco %d\n", index_free);
	create_child_process(sons, index_free, consume_file);
	position_queue--;

}

int main(int argc, char** argv)
{
  	int child_status, i;

  	

  	//create_multiple_child_process(sons, 4, consume_queue);

  	search_files("ficheros/");

  // 	position_queue = queue->data.size-1;
  // 	//printf("%d\n", position_queue);

  // 	i = 0;
  // 	while(i < 4) {
  // 		printf("Se va a consumir la ruta: %s\n", queue->list[position_queue].list);
  // 		create_child_process(sons, i, consume_file);
  // 		i++;
  // 		position_queue--;	
  // 	}

  // 	while(position_queue >= 0) {
  // 		pid_t wpid = wait(&child_status);

		// int index_free = get_index_from_value(sons, 4, wpid);
		// printf("Se va a consumir la ruta: %s\n", queue->list[position_queue].list);
		// create_child_process(sons, index_free, consume_file);
		// position_queue--;
  // 	}



	exit(0);
}



void create_child_process(int* sons, int index, void(*action)(char*)) {
	
	if((sons[index] = fork())  == 0) {
		Vector_chars* name_file = get_name_file_without_path(queue->list[position_queue].list, queue->list[position_queue].data.size);

		char* pre_name = "soluciones/validator-";
		char* filename = (char*)malloc(sizeof(pre_name)*strlen(name_file->list));
		strcpy(filename, pre_name);
		strcat(filename, name_file->list);

		//printf("Comienza el hijo %d\n", sons[index]);
		char* args[] = {"valida_uno",queue->list[position_queue].list,filename};
		execve("valida_uno", args, NULL);
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

void consume_file(char* path) {
	//consume
}

void search_files(char* dir) {
	pid_t child;
	int child_status, i = 1;

	int filedes[2], nbytes;
	char buffer[1];

	pipe(filedes);

	if((child = fork()) == 0) {
		//execl("exec_search_paths", "exec_search_paths", dir, (char *) 0);

		//pipe

		dup2(filedes[1], 1);
		close(filedes[0]);

		execl("busca_program", "busca_program", dir, "-n", ".txt", "-print", (char *) 0);

		//close(filedes[1]);

		exit(1);
	} else {
		close(filedes[1]);

		int init = 1;
		Vector_chars* str;

		queue = create_vector_of_pointers_chars(5);

		while((nbytes = read(filedes[0], buffer, sizeof(buffer))) > 0) {
			if(init == 1) {
				str = create_vector_of_chars(10);
				init = 0;
			}
			//printf("asdsdasda\n");
			if(buffer[0] != '\n') {
				//printf("%c\n", buffer[0]);
				push_back_char(str, buffer[0]);
			//printf("dosdos\n");
			} else {
				//printf("trestress\n");
				//printf("%d\n", queue);
				push_back_char_pointer(queue, str);
				//printf("cuatrocuatro\n");
				init = 1;
			}
			
		}

        pid_t wpid = wait(&child_status);

        int i = queue->data.size-1;

        printf("Elementos: \n");

        while(i >= 0) {
        	printf("%s\n", queue->list[i].list);
        	i--;
        }
        printf("\n\n");
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

	//printf("Cadena after: %s | before: %s\n", str, result->list);

	return result;
}

/*void create_multiple_child_process(int* sons, int num_childdren, void(*action)(char*)) {
	int i = 0;
	while(i < num_childdren) {
		if((sons[i] = fork()) == 0) {
			//printf("Soy el hijo %d (%d) y mi padre es: %d\n", i, getpid(), getppid());
			kill(getppid(), SIGUSR1);
			exit(1);
		}

		i++;
	}
}*/

