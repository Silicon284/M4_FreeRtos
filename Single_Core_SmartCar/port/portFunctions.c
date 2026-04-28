/*
void* memset(void *dst, int value, unsigned int size) {
    
    unsigned char *ptr = (unsigned char*) dst;
    unsigned char byte_value = (unsigned char) value;

    while (size > 0) {
        *ptr++ = byte_value;
        size--;
    }

    return dst;
}
*/

void* memcpy (void *dst, const void *src, unsigned int size) {
    unsigned char *d = (unsigned char*) dst;
    const unsigned char *s = (const unsigned char*) src;

    while (size > 0){
        *d++ = *s++;
        size--;
    }

    return dst;
}

// Add strcpy function
char* strcpy(char *dst, const char *src) {
    char *original_dst = dst;
    
    while (*src != '\0') {
        *dst++ = *src++;
    }
    *dst = '\0';  // null terminate
    
    return original_dst;
}

// Add strlen function
unsigned int strlen(const char *str) {
    unsigned int len = 0;
    
    while (*str != '\0') {
        len++;
        str++;
    }
    
    return len;
}

// Add basic snprintf function
int snprintf(char *str, unsigned int size, const char *format, ...) {
    // Simple implementation - just copy format string for now
    // In a real implementation, you'd handle format specifiers
    unsigned int i = 0;
    const char *src = format;
    
    if (size == 0) return 0;
    
    while (*src != '\0' && i < (size - 1)) {
        str[i++] = *src++;
    }
    str[i] = '\0';
    
    return i;
}