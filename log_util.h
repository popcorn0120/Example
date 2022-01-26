#ifndef __LOG_UTIL_H__
#define __LOG_UTIL_H__

#define LOG_LV_TRACE_A 50
#define LOG_LV_DEBUG_B 40
#define LOG_LV_INFO_C 30
#define LOG_LV_WARNING_D 20
#define LOG_LV_ERROR_E 10
#define LOG_LV_FATAL_F 0

// 각각의 Level에서 LOG를 생성가능한 상태인지를 체크하는 Macro

#define LOG_IS_TRACE (LOGgetLevel() >= LOG_LV_TRACE_A)
#define LOG_IS_DEBUG (LOGgetLevel() >= LOG_LV_DEBUG_B)
#define LOG_IS_INFO (LOGgetLevel() >= LOG_LV_INFO_C)
#define LOG_IS_WARNING (LOGgetLevel() >= LOG_LV_WARNING_D)
#define LOG_IS_ERROR (LOGgetLevel() >= LOG_LV_ERROR_E)
#define LOG_IS_FATAL (LOGgetLevel() >= LOG_LV_FATAL_F)

int LOGsetInfo (const char* dir, const char *prefix);
int LOGlogging (char log_type, const char *src_file, const char* func, int line_no, const char *fmt,...);
int LOGsetLevel (int log_lv);
int LOGgetLevel(void);
void OpenConfig(const char* prefix);

// 함수의 실행 시작과 종료를 Log로 생성하는 Macro 함수

#define LOG_CALL(func)\
	LOG_TRACE("%s ----starting...",#func);\
	func;\
	LOG_TRACE("%s ----end.",#func)

// Trace Log를 생성하는 macro 함수

#define LOG_TRACE(...) \
	do { \
		if (LOG_IS_TRACE) { \
			LOGlogging('T',__FILE__, __func__, __LINE__, __VA_ARGS__);\
		} \
	} while(0)

// debug Log를 생성하는 Macro 함수

#define LOG_DEBUG(...) \
	do { \
		if (LOG_IS_DEBUG) { \
			LOGlogging('D', __FILE__, __func__, __LINE__, __VA_ARGS__);\
		} \
	} while(0)

// 중요 정보 Log를 생성하는 Macro 함수

#define LOG_INFO(...) \
        do { \
                if (LOG_IS_INFO) { \
                        LOGlogging('I', __FILE__, __func__, __LINE__, __VA_ARGS__);\
                } \
        } while(0)

			
// Warning Log를 생성하는 Macro 함수

#define LOG_WARNING(...) \
        do { \
                if (LOG_IS_WARNING) { \
                        LOGlogging('W', __FILE__, __func__, __LINE__, __VA_ARGS__);\
                } \
       } while(0)

// Error Log를 생성하는 Macro 함수

#define LOG_ERROR(...) \
        do { \
                if (LOG_IS_ERROR) { \
                        LOGlogging('E', __FILE__, __func__, __LINE__, __VA_ARGS__);\
                } \
        } while(0)

// Fatal Error Log를 생성하는 Macro 함수
	
#define LOG_FATAL(...) \
        do { \
                if (LOG_IS_FATAL) { \
                        LOGlogging('F', __FILE__, __func__, __LINE__, __VA_ARGS__);\
                } \
       } while(0)

#endif // __LOG_H__























