#ifndef CALOSUB_PROCESSING_THREAD_H
#define CALOSUB_PROCESSING_THREAD_H

#include <stdint.h>
#include <stdbool.h>

struct processing_thread_msg {
    bool to_save;
    int32_t value;
};


#endif // CALOSUB_PROCESSING_THREAD_H
