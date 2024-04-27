#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

// incase any process IDs have leading zero/es
char *remove_leading_zeroes(char *str)
{
	int x;
	if ((x = strspn(str, "0")) != 0 && str[x] != '\0')
	{
		return &str[x];
	}
	return str;
}

// function to check if pid belongs to tree rooted at given root process
int belongs_to_process_tree(int argc, char *argv[])
{
	char *pid = argv[1];
	pid = remove_leading_zeroes(pid);
	char *r_pid = argv[2];
	r_pid = remove_leading_zeroes(r_pid);
	// running a system command to check all processes rooted under given root
	char search[200] = {'\0'};
	strcat(search, "pstree -p ");
	strcat(search, r_pid);
	strcat(search, " | grep -Eo \"[0-9]+\" > pid_list.txt");
	// printf("%s\n", search);
	system(search);
	FILE *f = fopen("pid_list.txt", "r");
	char l[100];
	// in case there's no records
	if (f == NULL)
	{
		fclose(f);
		return 0;
	}
	else
	{
		int exists = 0;
		while (fgets(l, sizeof(l), f))
		{
			l[strcspn(l, "\r\n")] = 0;
			char *l1 = remove_leading_zeroes(l);
			// if the process is found rooted under root process tree which is saved under pid_list.txt, then we can print that process id and it's parent pid
			if (strcmp(l1, pid) == 0 && strcmp(l1, r_pid) != 0)
			{
				exists = 1;
				// using ps to find parent for process
				char find_parent[200] = {'\0'};
				strcat(find_parent, "ps -o ppid= -p ");
				strcat(find_parent, pid);
				strcat(find_parent, " > parent.txt");
				system(find_parent);
				break;
			}
		}
		// if the process is not found
		if (!exists)
		{
			fclose(f);
			return 0;
		}
		else
		{
			fclose(f);
			return 1;
		}
	}
	fclose(f);
}

int main(int argc, char *argv[])
{
	int belongs;
	char *pid, *r_pid;
	if (argc == 3 || argc == 4)
	{
		// this function returns a 1 or true is process belongs to process tree rooted under root process, 0 otherwise
		belongs = belongs_to_process_tree(argc, argv);
		pid = argv[1];
		pid = remove_leading_zeroes(pid);
		r_pid = argv[2];
		r_pid = remove_leading_zeroes(r_pid);
	}
	else
	{
		printf("Invalid number of arguments entered!\n");
		exit(0);
	}
	// no additional option entered
	if (argc == 3)
	{
		if (belongs)
		{
			FILE *f = fopen("parent.txt", "r");
			char l[100];
			// parent list empty
			if (f == NULL)
			{
				printf("Does not belong to the process tree\n");
			}
			else
			{
				// printing the parent and process
				fgets(l, sizeof(l), f);
				l[strcspn(l, "\r\n")] = 0;
				printf("%s %s\n", pid, l);
			}
		}
		else
		{
			printf("Does not belong to the process tree\n");
		}
	}
	// option entered
	else if (argc == 4)
	{
		// store the option selected in opt variable
		char *opt = argv[3];
		// kill the process
		if (strcmp(opt, "-rp") == 0)
		{
			if (belongs)
			{
				// to send kill signal
				int k = kill(atoi(pid), 9);
				if (k == 0)
				{
					printf("Killed %s successfully\n", pid);
				}
				else
				{
					printf("Kill unsuccessful\n");
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// kill the root process
		else if (strcmp(opt, "-pr") == 0)
		{
			if (belongs)
			{
				// to kill root process
				int k = kill(atoi(r_pid), 9);
				if (k == 0)
				{
					printf("Killed %s successfully\n", r_pid);
				}
				else
				{
					printf("Kill unsuccessful\n");
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// to find non-direct descendants
		else if (strcmp(opt, "-xn") == 0)
		{
			if (belongs)
			{
				// first, saving all descendant in pid_all.txt
				char search[200] = {'\0'};
				strcat(search, "pstree -p ");
				strcat(search, pid);
				strcat(search, " | grep -Eo \"[0-9]+\" > pid_all.txt");
				system(search);
				// find direct descendants using pgrep and store in exclude list called exclude.txt
				char exclude[200] = {'\0'};
				strcat(exclude, "pgrep -P ");
				strcat(exclude, pid);
				strcat(exclude, " > exclude.txt");
				system(exclude);
				FILE *f1 = fopen("pid_all.txt", "r");
				FILE *f2 = fopen("exclude.txt", "r");
				char output[200] = {'\0'};
				// get only the non-direct descendants and store in common.txt
				strcat(output, "grep -vf exclude.txt pid_all.txt > common.txt");
				system(output);
				FILE *f3 = fopen("common.txt", "r");
				// in case there's no non-direct descendants
				if (f3 == NULL)
				{
					printf("No non-direct descendants\n");
				}
				else
				{
					int exists = 0;
					char l[100];
					while (fgets(l, sizeof(l), f3))
					{
						l[strcspn(l, "\r\n")] = 0;
						char *l1 = remove_leading_zeroes(l);
						// printing non-direct descendants
						if (strcmp(l1, pid) != 0)
						{
							exists = 1;
							printf("%s\n", l1);
						}
					}
					// no non-direct descendants
					if (!exists)
					{
						printf("No non-direct descendants\n");
					}
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// to find direct descendants
		else if (strcmp(opt, "-xd") == 0)
		{
			if (belongs)
			{
				// using pgrep to store direct descendants in direct.txt
				char direct[200] = {'\0'};
				strcat(direct, "pgrep -P ");
				strcat(direct, pid);
				strcat(direct, " > direct.txt");
				system(direct);
				FILE *f = fopen("direct.txt", "r");
				if (f == NULL)
				{
					printf("No direct descendants\n");
				}
				else
				{
					int exists = 0;
					char l[100];
					while (fgets(l, sizeof(l), f))
					{
						l[strcspn(l, "\r\n")] = 0;
						char *l1 = remove_leading_zeroes(l);
						exists = 1;
						printf("%s\n", l1);
					}
					// no direct descendants
					if (!exists)
					{
						printf("No direct descendants\n");
					}
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// to find siblings
		else if (strcmp(opt, "-xs") == 0)
		{
			if (belongs)
			{
				// while checking for process in process tree, we already saved it's parent in parent.txt and we can use that to find it's other children which will be the siblings
				FILE *f = fopen("parent.txt", "r");
				char parent[100];
				if (f == NULL)
				{
					printf("Error\n");
				}
				else
				{
					fgets(parent, sizeof(parent), f);
					parent[strcspn(parent, "\r\n")] = 0;
					char *parent1 = remove_leading_zeroes(parent);
					// using pgrep to find direct descendants of parent which are going to be the siblings of process
					char direct[200] = {'\0'};
					strcat(direct, "pgrep -P ");
					strcat(direct, parent1);
					strcat(direct, " > direct_parent.txt");
					system(direct);
					FILE *f1 = fopen("direct_parent.txt", "r");
					if (f1 == NULL)
					{
						printf("No sibling/s\n");
					}
					else
					{
						int exists = 0;
						char l[100];
						while (fgets(l, sizeof(l), f1))
						{
							l[strcspn(l, "\r\n")] = 0;
							char *l1 = remove_leading_zeroes(l);
							// printing the siblings
							if (strcmp(l1, pid) != 0)
							{
								exists = 1;
								printf("%s\n", l1);
							}
						}
						// no siblings
						if (!exists)
						{
							printf("No sibling/s\n");
						}
					}
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// to pause the process with SIGSTOP
		else if (strcmp(opt, "-xt") == 0)
		{
			if (belongs)
			{
				// sending SIGSTOP to process
				int k = kill(atoi(pid), SIGSTOP);
				if (k == 0)
				{
					printf("Sent SIGSTOP to %s successfully\n", pid);
				}
				else
				{
					printf("SIGSTOP unsuccessful\n");
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// to signal all paused processes to continue
		else if (strcmp(opt, "-xc") == 0)
		{
			if (belongs)
			{
				// sending SIGCONT to all paused processes
				int k = kill(-1, SIGCONT);
				if (k == 0)
				{
					printf("Sent SIGCONT to all paused processes\n");
				}
				else
				{
					printf("SIGCONT unsuccessful\n");
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// to find zombie descendants
		else if (strcmp(opt, "-xz") == 0)
		{
			if (belongs)
			{
				// first, find all descendants and store in descendants_list.txt
				char descendants[200] = {'\0'};
				strcat(descendants, "pstree -p ");
				strcat(descendants, pid);
				strcat(descendants, " | grep -Eo \"[0-9]+\" > descendants_list.txt");
				system(descendants);
				FILE *f = fopen("descendants_list.txt", "r");
				if (f == NULL)
				{
					printf("No descendant zombie process/es\n");
				}
				else
				{
					int exists = 0;
					char l[100];
					// for each descendant
					while (fgets(l, sizeof(l), f))
					{
						l[strcspn(l, "\r\n")] = 0;
						char *l1 = remove_leading_zeroes(l);
						if (strcmp(l1, pid) != 0)
						{
							// check the state using ps and store in is_zombie.txt
							char zombie_check[100] = {'\0'};
							strcat(zombie_check, "ps -o state -p ");
							strcat(zombie_check, l1);
							strcat(zombie_check, " > is_zombie.txt");
							system(zombie_check);
							// printf("%s\n", zombie_check);
							FILE *f1 = fopen("is_zombie.txt", "r");
							if (f1 == NULL)
							{
								printf("ERROR!");
							}
							else
							{
								char zc[100];
								// check if in is_zombie.txt, we have "Z" which means it's a defunct/zombie process and we print it
								while (fgets(zc, sizeof(zc), f1))
								{
									zc[strcspn(zc, "\r\n\" ")] = 0;
									char *zc1 = remove_leading_zeroes(zc);
									// printf("->%s", zc);
									if (strcmp(zc1, "Z") == 0)
									{
										exists = 1;
										printf("%s\n", l1);
									}
								}
							}
						}
					}
					// no zombie descendants
					if (!exists)
					{
						printf("No descendant zombie process/es\n");
					}
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// to find grandchildren
		else if (strcmp(opt, "-xg") == 0)
		{
			if (belongs)
			{
				// we first find direct descendants of process and store in direct.txt
				char direct[200] = {'\0'};
				strcat(direct, "pgrep -P ");
				strcat(direct, pid);
				strcat(direct, " > direct.txt");
				system(direct);
				// printf("->%s\n", direct);
				FILE *f = fopen("direct.txt", "r");
				if (f == NULL)
				{
					printf("No grandchildren\n");
				}
				else
				{
					int exists = 0;
					char l[100];
					// for each direct descendant, we find their direct descendants which are grandchildren of original process and store them in gc.txt
					while (fgets(l, sizeof(l), f))
					{
						l[strcspn(l, "\r\n")] = 0;
						char *l1 = remove_leading_zeroes(l);
						char gc[200] = {'\0'};
						strcat(gc, "pgrep -P ");
						strcat(gc, l1);
						strcat(gc, " > gc.txt");
						system(gc);
						FILE *f1 = fopen("gc.txt", "r");
						if (f1 == NULL)
						{
						}
						else
						{
							char ll[100];
							// printing all grandchildren
							while (fgets(ll, sizeof(ll), f1))
							{
								exists = 1;
								ll[strcspn(ll, "\r\n")] = 0;
								char *ll1 = remove_leading_zeroes(ll);
								printf("%s\n", ll1);
							}
						}
					}
					if (!exists)
					{
						printf("No grandchildren\n");
					}
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		// to check if a process is defunct or not
		else if (strcmp(opt, "-zs") == 0)
		{
			if (belongs)
			{
				// we use ps to find state of process and store result in is_zombie.txt
				char zombie_check[100] = {'\0'};
				strcat(zombie_check, "ps -o state -p ");
				strcat(zombie_check, pid);
				strcat(zombie_check, " > is_zombie.txt");
				system(zombie_check);
				FILE *f1 = fopen("is_zombie.txt", "r");
				if (f1 == NULL)
				{
					printf("Does not belong to the process tree\n");
				}
				else
				{
					int exists = 0;
					char zc[100];
					// if we find "Z" in is_zombie.txt, that means the process is defunct/zombie
					while (fgets(zc, sizeof(zc), f1))
					{
						zc[strcspn(zc, "\r\n\" ")] = 0;
						char *zc1 = remove_leading_zeroes(zc);
						if (strcmp(zc1, "Z") == 0)
						{
							exists = 1;
							printf("Defunct\n");
						}
					}
					// in case it's not a zombie process
					if (!exists)
					{
						printf("Not Defunct\n");
					}
				}
			}
			else
			{
				printf("Does not belong to the process tree\n");
			}
		}
		else
		{
			// wrong option as the fourth agrument
			printf("Invalid option entered!\n");
		}
	}
	// invalid number of agruments
	else
	{
		printf("Invalid number of arguments entered!\n");
	}
	return 0;
}
