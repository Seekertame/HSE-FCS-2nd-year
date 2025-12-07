#ifndef OS_IHW_LOG_UTILS_H
#define OS_IHW_LOG_UTILS_H

/**
 * Возвращает время с начала работы процесса в миллисекундах.
 */
double now_ms(void);

/**
 * Логирование события.
 * who < 0  → "SYS"
 * who >= 0 → "P<who>"
 */
void log_event(int who, const char *fmt, ...);

/**
 * Установить 1 файловый дескриптор наблюдателя.
 * Старые дескрипторы забываются.
 */
void log_set_observer_fd(int fd);

/**
 * Добавить ещё один файловый дескриптор наблюдателя (до фиксированного лимита).
 */
void log_add_observer_fd(int fd);

/**
 * Очистить список дескрипторов наблюдателей в текущем процессе.
 * Файлы не закрываются — ответственность вызывающего.
 */
void log_clear_observers(void);

#endif // OS_IHW_LOG_UTILS_H
