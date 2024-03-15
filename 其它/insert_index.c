#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

typedef struct Record {
	char file[200];
	char title[200];
	struct tm date;
} Record;

int rank(Record *records, int count) {
	Record temp;
	
	if(count <= 1) return 0;

	for(int i = 1; i < count; ++ i) {
		memcpy(&temp, records + i, sizeof(Record));
		int j = i - 1;
		while(j >=0 && records[j].date.tm_mday < temp.date.tm_mday) {
			memmove(records + j + 1, records + j, sizeof(Record));
			-- j;
		}
		memmove(records + j + 1, &temp, sizeof(Record));
	}

	for(int i = 1; i < count; ++ i) {
		memcpy(&temp, records + i, sizeof(Record));
		int j = i - 1;
		while(j >=0 && records[j].date.tm_mon < temp.date.tm_mon) {
			memmove(records + j + 1, records + j, sizeof(Record));
			-- j;
		}
		memmove(records + j + 1, &temp, sizeof(Record));
	}

	for(int i = 1; i < count; ++ i) {
		memcpy(&temp, records + i, sizeof(Record));
		int j = i - 1;
		while(j >=0 && records[j].date.tm_year < temp.date.tm_year) {
			memmove(records + j + 1, records + j, sizeof(Record));
			-- j;
		}
		memmove(records + j + 1, &temp, sizeof(Record));
	}
	return 0;
}

int main(int argc, char **argv) {
	int i;
	FILE *file;
	Record records[argc - 2];
	char line[200];
	char *temp;

	for(i = 2; i < argc; ++ i) {
		printf("-----%d/%d-----\n", i-1, argc-2);

		int title_get = 0, date_get = 0;
		char date[200];

		memset(&records[i-2], 0, sizeof(Record));
		strcpy(records[i-2].file, argv[i]);
		puts(records[i-2].file);

		file = fopen(argv[i], "r"); assert(file);

		while(!feof(file) && !(title_get && date_get) && fgets(line, 200, file) != NULL) {
			temp = strstr(line, "title");
			if(!title_get && temp != NULL) {
				title_get = 1;
				*strchr(temp, '<') = '\0';
				strcpy(records[i-2].title, temp + 6);
				puts(records[i-2].title);
			}

			temp = strstr(line, "date");
			if(!date_get && temp != NULL) {
				date_get = 1;
				*strchr(temp, '<') = '\0';
				strcpy(date, temp + 6);
				strptime(date, "%Y/%m/%e", &records[i-2].date);
				printf("%d %d %d\n", records[i-2].date.tm_year + 1900, 
				records[i-2].date.tm_mon + 1, records[i-2].date.tm_mday);
			}
		}
		fclose(file);
	}

	rank(records, argc - 2);

	size_t size = 200;
	char *p = NULL;
	FILE *index = fopen(argv[1], "r"); assert(index);
	FILE *nindex = fopen("index.html", "w"); assert(nindex);
	while(!feof(index) && getline(&p, &size, index) != -1) {
		fputs(p, nindex);
		if(strncmp(p, "<ul>", 4) == 0) {
			for(int i = 0; i < argc - 2; ++ i) {
				Record re = records[i];		
				fprintf(nindex, "\t<li>%d年%2d月%2d日&emsp;&emsp;<a href=\"%s\">%s</a></li>\n", 
				re.date.tm_year + 1900, re.date.tm_mon + 1, re.date.tm_mday, re.file, re.title);
			}
		}
	}
	fclose(index);	
	fclose(nindex);
	return 0;
}
