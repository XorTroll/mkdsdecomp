
// Need this for the compiler to generate an actually valid ROM

__attribute__((weak, section(".secure")))
volatile const char __secure_area__[2048];