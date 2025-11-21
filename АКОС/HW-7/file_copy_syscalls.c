#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(const int argc, char *argv[]) {
    if (argc != 3) {
        const char *message = "Usage: file_copy_syscalls <source> <dest>\n";
        const ssize_t dummy = write(STDERR_FILENO, message, strlen(message));
        (void)dummy;
        return 1;
    }

    const char *source_path = argv[1];
    const char *dest_path = argv[2];

    // source_fd - source file description
    const int source_fd = open(source_path, O_RDONLY);
    if (source_fd < 0) {
        perror("open source");
        return 1;
    }

    // Получаем режим доступа исходного файла для копирования прав
    struct stat st;
    if (fstat(source_fd, &st) < 0) {
        perror("fstat");
        close(source_fd);
        return 1;
    }

    // Берём только биты прав (rwx для пользователя/группы/остальных)
    const mode_t source_mode = st.st_mode & 0777;

    // Создаём/открываем dest файл на запись, обнуляя содержимое.
    // Используем тот же режим доступа, что у исходного файла.
    // dest_fd - destination's file description
    const int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, source_mode);
    if (dest_fd < 0) {
        perror("open dest");
        close(source_fd);
        return 1;
    }

    // Буфер на 32 байта для циклического использования
    char buffer[32];

    while (1) {
        // n_read - number of bytes read
        const ssize_t n_read = read(source_fd, buffer, sizeof(buffer));
        if (n_read < 0) {
            perror("read");
            close(source_fd);
            close(dest_fd);
            return 1;
        }
        if (n_read == 0) {
            break;
        }

        // Записываем n_read байт в dest файл
        ssize_t total_written = 0;
        while (total_written < n_read) {
            const ssize_t n_written = write(
                dest_fd,
                buffer + total_written,
                (size_t)(n_read - total_written)
            );
            if (n_written < 0) {
                perror("write");
                close(source_fd);
                close(dest_fd);
                return 1;
            }
            if (n_written == 0) {
                const char *message = "write returned 0 bytes\n";
                const ssize_t dummy = write(STDERR_FILENO, message, strlen(message));
                (void)dummy;
                close(source_fd);
                close(dest_fd);
                return 1;
            }
            total_written += n_written;
        }
    }

    close(source_fd);
    close(dest_fd);

    return 0;
}
