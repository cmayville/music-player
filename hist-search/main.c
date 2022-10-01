#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sqlite3.h>
#include <sys/wait.h>

#define DB_FILE "/home/charli/.mozilla/firefox/943ch4wa.default-esr/places.sqlite"
#define DB_COPY "/home/charli/.cache/copy.sqlite"

int printrows(void *, int, char **, char **);

int main(int argc, char *argv[])
{
    char *filename = DB_FILE;
    char *query    = "SELECT type, name FROM sqlite_schema";

	int opt;
	while ((opt = getopt(argc, argv, "f:q:")) != -1) {
		switch (opt) {
		case 'f':
			filename = strdup(optarg); // dont forget to freeee
			break;
        case 'q':
            query = strdup(optarg);
            break;
		}
	}
    
    int rc;

	sqlite3 *database;
	rc = sqlite3_open(filename, &database);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "%s\n", sqlite3_errmsg(database));
		sqlite3_close(database);
		return 1;
	}

    char *error;
    rc = sqlite3_exec(database, query, printrows, 0, &error);
	if (rc == SQLITE_BUSY) {

        int status;
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "problem with copy\n");
            return 1;
        } else if (pid > 0) {
            waitpid(pid, &status, 0);
        } else {
            rc = execl("/bin/cp", "/bin/cp", filename, DB_COPY);
            if (rc < 0) {
                return 1;
            }
        }

        if (status < 0) {
            fprintf(stderr, "problem with copy\n");
            return 1;
        }
        
	    rc = sqlite3_open(DB_COPY, &database);
        if (rc != SQLITE_OK) {
		    fprintf(stderr, "%s\n", sqlite3_errmsg(database));
		    sqlite3_close(database);
		    return 1;
	    }

        rc = sqlite3_exec(database, query, printrows, 0, &error);
	}

    if (rc != SQLITE_OK) {
		fprintf(stderr, "%s\n", error);
		sqlite3_close(database);
		return 1;
	}

    sqlite3_close(database);
    return 0;
}    
    
int printrows(void *not_used, int argc, char **argv, char **az_col_name)
{
    for (int i = 0; i < argc; i++) {
        printf("\"%s\" ", argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

	
	
	
	
