#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdlib.h>
#include "log_util.h"

static char log_file_prefix[64];
static char log_folder[1024];
static FILE* log_file;
static int log_level;
static char file_path[1024];
static char lv[10];

/* 설정파일에서 경로를 불러오고 로그파일 이름을 설정 */
	void OpenConfig(const char* prefix) {
		
		FILE* fp;
		char buf[1024];
		char* path;
		char* level;
	
		fp = fopen("config.cfg","r"); // 설정파일을 읽기 전용으로 열기
	
		if (fp == NULL) {
			
			fprintf(stderr,"File Open Error\n");
			exit(1);
		}

		if ((fread(buf,sizeof(buf),1,fp)) == -1) { 
			
			fprintf(stderr, "File Read Error\n");
			exit(1);
		}

		path = strtok(buf, "="); // 문자열 자르는 함수
		path = strtok(NULL, "\n");

		strcpy(file_path, path);

		strncpy(log_file_prefix, prefix, 64);
		strncpy(log_folder, file_path, 1024);

		fclose(fp);

	}	

/* 로그 레벨 받아오는 함수 */
	int LOGgetLevel(void) {

		FILE* fp;
		char buf[1024];
		char* path;
		char* level;
		
		fp = fopen("config.cfg","r"); /* 설정파일을 읽기 전용으로 열기 */
	
		if (fp == NULL) {
		
			fprintf(stderr,"File Open Error\n");
			exit(1);
		}

		if ((fread(buf,sizeof(buf),1,fp)) == -1) { 
		
			fprintf(stderr, "File Read Error\n");
			exit(1);
		}

		level = strtok(buf, "=");  /* 문자열 자르는 함수 */
		level = strtok(NULL, "\n");

		level = strtok(NULL, "=");
		level = strtok(NULL, "\n");

		strcpy(lv, level);
		
		if (strcmp(lv, "A") == 0) {
		
			log_level = LOG_LV_TRACE_A;
		}

		else if (strcmp(lv, "B") == 0) {
				
			log_level = LOG_LV_DEBUG_B;
		}

		else if (strcmp(lv, "C") == 0) {
				
			log_level = LOG_LV_INFO_C;
		}

		else if (strcmp(lv, "D") == 0) {
          
			log_level = LOG_LV_WARNING_D;
    	}
 			
		else if (strcmp(lv, "E") == 0) {
        
			log_level = LOG_LV_ERROR_E;
    	}

		else if (strcmp(lv, "F") == 0) {
        
			log_level = LOG_LV_FATAL_F;
    	}

		else {
				
			log_level = LOG_LV_INFO_C;
		}
		

		return log_level;

	}

/* LOGcreateFile 날짜가 변경되는 경우 자동으로 새로운 파일이 생성 */

	static int LOGcreateFile (struct tm* time, const char* src_file) {
		
		char filename[1024];

		snprintf(filename, 1126, "%s/%s-%04d%02d%02d.log",
		log_folder,log_file_prefix, 1900 + time->tm_year, time->tm_mon +1, time->tm_mday);

		/* snprintf함수 - 여러 개의 데이터를 형식에 맞추어 문자열에 저장하는 동적 파라미터 함수
				(char* str, size_t size, const char* format, ...(나열된 순서에 맞는 데이터)) */

		if (log_file != NULL) {
			
			fclose(log_file);
			log_file = NULL;
		}

		if ((log_file = fopen(filename, "a")) == NULL) {
			
			return -1;
		}

		setvbuf(log_file, NULL, _IOLBF, 0); 
		/* setvbuf함수 - 파일 스트림 버퍼링 방식을 변경
				(FILE* stream, char* buffer, int mode, size_t size)
				
				_IOFBF - 완전한 버퍼링 - fully buffered 스트림

				_IOLBF - 행 버퍼링 - 출력시 버퍼가 채워지거나 스트림에 개행문자가 입력되었다면
					     데이터가 버퍼에서 출력된다. 입력 시에는 버퍼가 개행 문자를
					     만날 때까지 버퍼를 채움.

				_IONBF - 버퍼링 사용 안함 - 버퍼를 사용하지 않는다. 각각의 입출력 작업은 버퍼를
						 지나지 않고 요청 즉시 진행되며 이 경우 buffer 인자와
                         size 인자는 모두 무시. */

		return 0;

	}

/* LOGlogging() 로그파일을 생성함 */

	int LOGlogging (char log_type, const char* src_file, const char* func, int line_no, const char* format, ...) {
		
		va_list ap; // 가변 인수
		int sz = 0;
		struct timeval tv;
		struct tm* time;
		static int day = -1;
		static pid_t pid = -1;
		char src_info[128];

		gettimeofday(&tv, NULL);
		time = localtime(&tv.tv_sec);
		va_start(ap, format);

		if (pid == -1) {
			
			pid = getpid(); // get process id
		}

/* 날짜가 변경되었거나 최초 실행시 */

		if (day != time->tm_mday) {
		
			if (LOGcreateFile(time, src_file) != 0) {
			
				return -1;
			}
		
			day = time->tm_mday;

		}

		sz += fprintf(log_file, "(%c) ", log_type);
		sz += fprintf(log_file, "%s=%s","PATH",log_folder);
		sz += fprintf(log_file, "%04d%02d%02d %02d%02d%02d:%05d", 
				1900 + time->tm_year, time->tm_mon + 1, time->tm_mday,
				time->tm_hour, time->tm_min, time->tm_sec, pid);
	
		snprintf(src_info, 128, "%s:%s(%d)",src_file, func, line_no);
		sz += fprintf(log_file, ":%s: ", src_info);
		sz += vfprintf(log_file, format, ap);
		/* vfprintf 함수 - 버퍼에 포멧을 지정하여 가변 인자를 출력하는 함수
			   	(FILE* stream, const char* format, va_list arg_ptr) */
		sz += fprintf(log_file, "\n");
		va_end(ap);

		return 0;

	}


	
		
	






