void PrintInit();
void PrintString(int unused, const char *str);

void NtrMain() {
    // Dummy main meanwhile

    PrintInit();
    PrintString(0xFF, "mkds rules");

    while(1);
}