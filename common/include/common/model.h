#ifndef OS_IHW_MODEL_H
#define OS_IHW_MODEL_H

typedef enum {
    PROGRAM_STATUS_ON_REVIEW,
    PROGRAM_STATUS_DONE
} ProgramStatus;

typedef enum {
    PROGRAM_RESULT_NONE,
    PROGRAM_RESULT_OK,
    PROGRAM_RESULT_FAIL
} ProgramResult;

typedef struct {
    int id;
    int author_id;
    int reviewer_id;
    int version;
    ProgramStatus status;
    ProgramResult result;
} ProgramTask;

#endif // OS_IHW_MODEL_H
